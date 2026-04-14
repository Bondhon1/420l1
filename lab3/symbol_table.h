#pragma once
#include "scope_table.h"

class symbol_table {
private:
    scope_table *current_scope;
    int bucket_count;
    int next_scope_id;
    ofstream *outlog;

public:
    symbol_table(int bucket_count, ofstream *outlog)
        : current_scope(nullptr), bucket_count(bucket_count), next_scope_id(0), outlog(outlog) {}

    void enter_scope() {
        next_scope_id++;
        scope_table *ns = new scope_table(bucket_count, next_scope_id, current_scope);
        current_scope = ns;
        *outlog << "New ScopeTable with ID " << next_scope_id << " created" << endl << endl;
    }

    void exit_scope() {
        if (!current_scope) return;
        print_all_scopes(*outlog);
        int id = current_scope->get_unique_id();
        scope_table *parent = current_scope->get_parent_scope();
        delete current_scope;
        current_scope = parent;
        *outlog << "Scopetable with ID " << id << " removed" << endl << endl;
    }

    bool insert(symbol_info *symbol) {
        if (!current_scope) return false;
        return current_scope->insert_in_scope(symbol);
    }

    bool remove(symbol_info *symbol) {
        if (!current_scope) return false;
        return current_scope->delete_from_scope(symbol);
    }

    symbol_info *lookup(const string &name) {
        scope_table *scope = current_scope;
        while (scope) {
            symbol_info *found = scope->lookup_in_scope(name);
            if (found) return found;
            scope = scope->get_parent_scope();
        }
        return nullptr;
    }

    symbol_info *lookup(symbol_info *symbol) {
        return lookup(symbol->get_name());
    }

    void print_current_scope() {
        if (current_scope) current_scope->print_scope_table(*outlog);
    }

    void print_all_scopes(ofstream &outlog) {
        outlog << "################################" << endl << endl;
        scope_table *temp = current_scope;
        while (temp) {
            temp->print_scope_table(outlog);
            temp = temp->get_parent_scope();
            if (temp) outlog << endl;
        }
        outlog << "################################" << endl << endl;
    }

    ~symbol_table() {
        while (current_scope) {
            scope_table *parent = current_scope->get_parent_scope();
            delete current_scope;
            current_scope = parent;
        }
    }
};
