#include "xfunc.hpp"

#include <ton/crypto/func/func.h>

namespace xfunc {

    bool parse_func_name(const std::string& line, std::string& func_name) {
        /*
            Note, that this regex-based matching can produce false positives (will cause parser errors later);
            But that's fine :D
        */
        static std::regex forall_re("^forall\s(?:[_\w]+|,\s?)+\s->\s"),
                          type_re("^(?:_|\((?:[_\w]+|,\s?|(?R))*\)\s?"),
                          name_re("^~?([_\w]+)\??\(");

        auto it = line.begin();

        std::smatch forall_match;
        if (std::regex_match(it, line.end(), forall_match, forall_re)) {
            if (forall_match.size() != 1) {
                return false;
            }
            it += forall_match[0].length();
        } else {
            ;; // ok
        }

        std::smatch type_match;
        if (std::regex_match(it, line.end(), type_match, type_re)) {
            if (type_match.size() != 1) {
                return false;
            }
            it += type_match[0].length();
        } else {
            return false;
        }

        std::smatch name_match;
        if (std::regex_match(it, line.end(), name_match, name_re)) {
            if (name_match.size() != 2) {
                return false;
            }
            func_name = name_match[1];
        } else {
            return false;
        }

        return true;
    }

    global_sym_guard::global_sym_guard(const std::string& expr) {
        /*
            Parse all function names that occur in the given code block;
            Save the according symbol definitions and remove them from the global symbol table;
            This will prevent function override failures.
        */
        std::istringstream iss(expr);
        for (std::string line; std::getline(iss, line);)
        {
            std::string func_name;
            if (parse_func_name(line, func_name)) {
                func_names_.push_back(func_name);
                sym::sym_idx_t name_idx = sym::symbols.lookup(func_name);
                if (name_idx) {
                    sym::SymDef* sym_def = sym::global_sym_def[name_idx];
                    if (sym_def) {
                        backup_.emplace(name_idx, sym_def);
                        sym::global_sym_def[name_idx] = nullptr;
                    }
                }
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
                auto old_ptr = std::find(funC::glob_func.begin(), funC::glob_func.end(), it.second);
                if (old_ptr != funC::glob_func.end()) {
                    funC::glob_func.erase(old_ptr);
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

    bool global_sym_guard::empty() const {
        return func_names_.empty();
    }

    bool global_sym_guard::has(const std::string& name) const {
        return func_names_.end() != std::find(func_names_.begin(), func_names_.end(), name);
    }

    void global_sym_guard::analyze_new(int cycles) {
        for (auto& name : func_names_) {
            sym::SymDef* sym_def = sym::lookup_symbol(name, 2);
            if (!sym_def) {
                throw src::Fatal{"global symbol `" + name + "` is undefined"};
            }
            
            funC::SymValCodeFunc* func_val = dynamic_cast<funC::SymValCodeFunc*>(sym_def->value);
            if (func_val->code) {          
                funC::CodeBlob& code = *(func_val->code);
                code.simplify_var_types();
                code.prune_unreachable_code();
                code.split_vars(true);

                for (int i = 0; i < cycles; i++) {
                    code.compute_used_code_vars();
                    code.fwd_analyze();
                    code.prune_unreachable_code();
                }

                code.mark_noreturn();
            }
        }
    }
}