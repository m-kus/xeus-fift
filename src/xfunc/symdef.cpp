#include "xfunc.hpp"

namespace xfift {

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

    std::string generate_fift_script() {
        std::stringstream decl;
        std::stringstream impl;
        bool has_main = false;

        decl << "PROGRAM{\n";

        for (sym::SymDef* func_sym : glob_func) {
            std::string name = sym::symbols.get_name(func_sym->sym_idx);

            assert(func_sym->value);
            assert(func_sym->value->type == sym::SymValBase::_Func);
            funC::SymValCodeFunc* func_val = dynamic_cast<funC::SymValCodeFunc*>(func_sym->value);

            assert(func_val);
            if (func_val->method_id.is_null()) {
                decl << "DECLPROC " << name << "\n";
            } else {
                decl << func_val->method_id << " DECLMETHOD " << name << "\n";
            }
            
            if (!func_val->code) {
                throw src::Fatal{"function `" + name + "` declared but has no body"};
            }
            
            // TODO: remove this
            func_val->code->print(std::clog, 15);

            func_val->code->simplify_var_types();
            func_val->code->prune_unreachable_code();
            func_val->code->split_vars(true);

            for (int i = 0; i < 1; i++) {
                func_val->code->compute_used_code_vars();
                func_val->code->fwd_analyze();
                func_val->code->prune_unreachable_code();
            }

            func_val->code->mark_noreturn();

            bool inline_ref = (func_val->flags & 2);
            impl << name << " PROC" << (inline_ref ? "REF" : "") << ":<{\n";
            if (name == "main") {
                has_main = true;
                impl << "DEBUGON\n";
            }

            func_val->code->generate_code(impl, Stack::_DisableOpt, 1);
            impl << "}>\n";
        }

        if (!has_main) {
            throw src::Fatal{"`main` function is not defined"};
        }
       
        decl << impl.str() << "}END>s\n" << "runvmdict .s";
        return decl.str();
    }
}