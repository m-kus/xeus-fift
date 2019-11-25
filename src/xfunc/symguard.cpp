#include "xfunc.hpp"


namespace xfunc {

    global_sym_guard::global_sym_guard(const std::string& expr) {
        /*
            Parse all function names that occur in the given code block;
            Save the according symbol definitions and remove them from the global symbol table;
            This will prevent function override failures.
        */
        std::regex func_name_re("\\s~?([_\\w]+)\??\\([_,\\w\\s]*\\)\\s?(?:[a-z]|\\{)");
       
        for (std::sregex_iterator it = std::sregex_iterator(expr.begin(), expr.end(), func_name_re);
                                  it != std::sregex_iterator();
                                ++it)
        {
            std::smatch match = *it;
            assert(match.size() == 2);
            std::string func_name = match.str(1);
            func_names_.push_back(func_name);

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

    bool global_sym_guard::not_a_func() const {
        return func_names_.empty();
    }
}