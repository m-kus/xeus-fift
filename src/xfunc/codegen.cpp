#include "xfunc.hpp"

namespace xfift {

    void force_return(funC::CodeBlob* code) {
        for (auto& op : code->ops) {
            if (op.next && op.next->cl == funC::Op::_Return && op.next->left.empty()
                && op.next->next && op.next->next->cl == funC::Op::_Nop
                && op.next->next->next == nullptr)
            {
                if (op.cl == funC::Op::_Call 
                    || op.cl == funC::Op::_CallInd
                    || op.cl == funC::Op::_IntConst
                    || op.cl == funC::Op::_Let
                    || op.cl == funC::Op::_GlobVar)
                {
                    std::copy(op.left.begin(), op.left.end(), std::back_inserter(op.next->left));
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
            if (!func_val->code) {
                std::cout << "function `" << name << "` declared but has no body\n";
                continue;
            }

            assert(func_val);
            if (func_val->method_id.is_null()) {
                decl << "DECLPROC " << name << "\n";
            } else {
                decl << func_val->method_id << " DECLMETHOD " << name << "\n";
            }
            
            funC::CodeBlob* code = func_val->code;
            if (name == "main") {
                has_main = true;
                force_return(code);
                //code->print(std::cout, 15);
            }
            
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
                impl << "  DEBUGON\n";
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