#pragma once
#include "symbol_info.h"
#include <fstream>

class scope_table {
private:
    int bucket_count;
    int unique_id;
    scope_table *parent_scope;
    vector<list<symbol_info *>> table;

    // Hash function: sum of ASCII values of each character mod bucket_count
    int hash_function(const string &name) {
        int sum = 0;
        for (char c : name) sum += static_cast<int>(c);
        return sum % bucket_count;
    }

public:
    scope_table(int bucket_count, int unique_id, scope_table *parent_scope)
        : bucket_count(bucket_count), unique_id(unique_id), parent_scope(parent_scope) {
        table.resize(bucket_count);
    }

    scope_table *get_parent_scope() { return parent_scope; }
    int          get_unique_id()    { return unique_id;    }

    // Insert symbol into this scope; reject duplicate. Returns true on success.
    bool insert_in_scope(symbol_info *symbol) {
        int idx = hash_function(symbol->get_name());
        // check for duplicate in this bucket
        for (auto *s : table[idx]) {
            if (s->get_name() == symbol->get_name()) return false;
        }
        table[idx].push_back(symbol);
        return true;
    }

    // TODO: implement lookup_in_scope
    symbol_info *lookup_in_scope(const string &name) {
        // int idx = hash_function(name);
        // iterate table[idx] and return matching symbol_info*
        return nullptr;
    }
    symbol_info *lookup_in_scope(symbol_info *symbol) {
        return lookup_in_scope(symbol->get_name());
    }

    // TODO: implement delete_from_scope
    bool delete_from_scope(const string &name) {
        // find the entry in the correct bucket and erase it
        return false;
    }
    bool delete_from_scope(symbol_info *symbol) {
        return delete_from_scope(symbol->get_name());
    }

    // TODO: complete print_scope_table to show Variable/Array/Function details
    void print_scope_table(ofstream &outlog) {
        outlog << "ScopeTable # " << unique_id << endl;
        for (int i = 0; i < bucket_count; i++) {
            if (!table[i].empty()) {
                outlog << i << " --> " << endl;
                for (auto *sym : table[i]) {
                    outlog << "< " << sym->get_name() << " : ID >" << endl;
                    // TODO: print kind (Variable/Array/Function) and related fields
                }
            }
        }
    }

    ~scope_table() {
        for (auto &bucket : table)
            for (auto *sym : bucket)
                delete sym;
    }
};
