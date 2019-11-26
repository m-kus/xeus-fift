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
        }
        
        sym::symbols = sym::SymTable<100003>{};
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

    void force_return(funC::CodeBlob* code) {
        for (auto& op : code->ops) {
            if (op.is_pure()
                && op.next
                && op.next->cl == funC::Op::_Return
                && op.next->args.empty()
                && op.next->next
                && op.next->next->cl == funC::Op::_Nop
                && op.next->next->next == nullptr)
            {
                op.show_var_list(std::cout, op.left, code->vars);
                for (const auto& var : op.left) {
                    op.next->left.push_back(var);
                }
                return;
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
            
            funC::CodeBlob* code = func_val->code;
            force_return(code);
            code->print(std::cout, 15);
            
            code->simplify_var_types();
            code->prune_unreachable_code();
            code->split_vars(true);

            for (int i = 0; i < 8; i++) {
                code->compute_used_code_vars();
                code->fwd_analyze();
                code->prune_unreachable_code();
            }

            code->mark_noreturn();

            bool inline_ref = (func_val->flags & 2);
            impl << name << " PROC" << (inline_ref ? "REF" : "") << ":<{\n";
            if (name == "main") {
                has_main = true;
                impl << "DEBUGON\n";
            }

            code->generate_code(impl, Stack::_DisableOpt, 1);
            impl << "}>\n";
        }

        if (!has_main) {
            throw src::Fatal{"`main` function is not defined"};
        }
       
        decl << impl.str() << "}END>s\n" << "runvmdict .s";
        return decl.str();
    }
}