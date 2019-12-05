#include "xfunc.hpp"

namespace xfift {

    void clear_sym_def(SymDef* definitions[], sym::sym_idx_t idx) {
        sym::SymDef* sym_def = definitions[idx];
        if (sym_def) {
            if (sym_def->value) {
                delete sym_def->value;
                sym_def->value = nullptr;
            }
            delete sym_def;
            definitions[idx] = nullptr;
        }
    }

    void reset_global_state() {
        sym::scope_level = 0;
        sym::symbol_stack.clear();
        sym::scope_opened_at.clear();

        funC::glob_func.clear();
        funC::glob_func_cnt = 0;
        funC::undef_func_cnt = 0;

        funC::TypeExpr::holes = 0;
        funC::TypeExpr::type_vars = 0;

        for (int i = 0; i < sym::symbols.hprime; i++) {
            clear_sym_def(sym::sym_def, i);
            clear_sym_def(sym::global_sym_def, i);
            auto symbol = sym::symbols[i];
            if (symbol) {
                symbol->str = "";
                symbol->idx = 0;
            }
        }

        std::clog << "Global state is clear\n";
    }

    global_sym_guard::global_sym_guard(const std::vector<std::string>& func_names) {
        /*
            Receive all function names that occur in the given code block;
            Save the according symbol definitions and remove them from the global symbol table;
            This will prevent function override failures.
        */
        for (auto& func_name : func_names)
        {
            sym::SymDef* sym_def = sym::lookup_symbol(func_name, 2);
            if (sym_def) {
                backup_.emplace(sym_def->sym_idx, sym_def);
                sym::global_sym_def[sym_def->sym_idx] = nullptr;
            }
        }
    }

    global_sym_guard::~global_sym_guard() {
        /*
            In case of parser failures we need to restore original symbol definitions;
            Also we need to destroy old ones that are no longer in use.
        */
        for (auto& it : backup_) {
            sym::SymDef* sym_def = sym::global_sym_def[it.first];
            if (sym_def) {                
                auto glob_func_it = std::find(funC::glob_func.begin(), funC::glob_func.end(), it.second);
                if (glob_func_it != funC::glob_func.end()) {
                    funC::glob_func.erase(glob_func_it);
                    funC::glob_func_cnt--;
                }
                if (it.second->value) {
                    delete it.second->value;
                }
                delete it.second;
            } else {
                sym::global_sym_def[it.first] = it.second;
            }
        }
    }
}