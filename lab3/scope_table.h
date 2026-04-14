#pragma once
#include "symbol_info.h"
#include <fstream>

class scope_table {
private:
    int bucket_count;
    int unique_id;
    scope_table *parent_scope;
    vector<list<symbol_info *>> table;

    int hash_function(const string &name) {
        int sum = 0;
        for (char c : name) sum += static_cast<int>(c);
        return sum % bucket_count;
    }

public:
    scope_table() : bucket_count(10), unique_id(0), parent_scope(nullptr) {
        table.resize(bucket_count);
    }

    scope_table(int bucket_count, int unique_id, scope_table *parent_scope)
        : bucket_count(bucket_count), unique_id(unique_id), parent_scope(parent_scope) {
        table.resize(bucket_count);
    }

    scope_table *get_parent_scope() { return parent_scope; }
    int get_unique_id() { return unique_id; }

    symbol_info *lookup_in_scope(const string &name) {
        int idx = hash_function(name);
        for (auto *sym : table[idx]) {
            if (sym->get_name() == name) return sym;
        }
        return nullptr;
    }

    symbol_info *lookup_in_scope(symbol_info *symbol) {
        return lookup_in_scope(symbol->get_name());
    }

    bool insert_in_scope(symbol_info *symbol) {
        if (lookup_in_scope(symbol->get_name()) != nullptr) return false;
        int idx = hash_function(symbol->get_name());
        table[idx].push_back(symbol);
        return true;
    }

    bool delete_from_scope(const string &name) {
        int idx = hash_function(name);
        auto &bucket = table[idx];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if ((*it)->get_name() == name) {
                delete *it;
                bucket.erase(it);
                return true;
            }
        }
        return false;
    }

    bool delete_from_scope(symbol_info *symbol) {
        return delete_from_scope(symbol->get_name());
    }

    void print_scope_table(ofstream &outlog) {
        outlog << "ScopeTable # " << unique_id << endl;
        for (int i = 0; i < bucket_count; i++) {
            if (!table[i].empty()) {
                outlog << i << " --> " << endl;
                for (auto *sym : table[i]) {
                    outlog << "< " << sym->get_name() << " : ID >" << endl;
                    const string &kind = sym->get_symbol_kind();
                    if (kind == "ARRAY") {
                        outlog << "Array" << endl;
                        outlog << "Type: " << sym->get_data_type() << endl;
                        outlog << "Size: " << sym->get_array_size() << endl;
                    } else if (kind == "FUNCTION") {
                        outlog << "Function Definition" << endl;
                        outlog << "Return Type: " << sym->get_data_type() << endl;
                        outlog << "Number of Parameters: " << sym->get_param_count() << endl;
                        outlog << "Parameter Details: " << sym->get_param_details() << endl;
                    } else {
                        outlog << "Variable" << endl;
                        outlog << "Type: " << sym->get_data_type() << endl;
                    }
                    outlog << endl;
                }
            }
        }
    }

    ~scope_table() {
        for (auto &bucket : table) {
            for (auto *sym : bucket) {
                delete sym;
            }
        }
    }
};
