#include "xfunc.hpp"

namespace xfunc {

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