#include "xfunc.hpp"

namespace xfunc {

    std::string generate_fift_script() {
        std::stringstream decl;
        std::stringstream impl;
        bool has_main = false;

        decl << "PROGRAM{\n";

        for (sym::SymDef* func_sym : glob_func) {
            std::string name = sym::symbols.get_name(func_sym->sym_idx);
            if (name == "main") {
                has_main = true;
            }

            funC::SymValCodeFunc* func_val = dynamic_cast<SymValCodeFunc*>(func_sym->value);
            if (!func_val->code) {
                throw src::Fatal{"function `" + name + "` has no body"};
            }
            
            if (func_val->method_id.is_null()) {
                decl << "DECLPROC " << name << "\n";
            } else {
                decl << func_val->method_id << " DECLMETHOD " << name << "\n";
            }

            CodeBlob& code = *(func_val->code);
            bool inline_ref = (func_val->flags & 2);

            impl << name << " PROC" << (inline_ref ? "REF" : "") << ":<{\n";
            code.generate_code(impl, Stack::_DisableOpt, 1);
            impl << "}>\n";
        }

        if (!has_main) {
            throw src::Fatal{"`main` function is not defined"};
        }
       
        decl << impl.str() << "}END>s\n" << "runvmdict .s";
        return decl.str();
    }
}