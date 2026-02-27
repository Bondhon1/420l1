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
        : bucket_count(bucket_count), current_scope(nullptr),
          next_scope_id(0), outlog(outlog) {}

    // Create a new scope and push it onto the stack
    void enter_scope() {
        next_scope_id++;
        scope_table *ns = new scope_table(bucket_count, next_scope_id, current_scope);
        current_scope = ns;
        *outlog << "New ScopeTable with ID " << next_scope_id
                << " created" << endl << endl;
    }

    // TODO: implement exit_scope
    //   - print all scope tables
    //   - delete the current scope
    //   - log "Scopetable with ID N removed"
    void exit_scope() {
        if (!current_scope) return;
        // TODO: call print_all_scopes(...)
        int id = current_scope->get_unique_id();
        scope_table *parent = current_scope->get_parent_scope();
        delete current_scope;
        current_scope = parent;
        *outlog << "Scopetable with ID " << id << " removed" << endl << endl;
    }

    // TODO: implement insert (delegate to current scope)
    bool insert(symbol_info *symbol) {
        if (!current_scope) return false;
        return current_scope->insert_in_scope(symbol);
    }

    // TODO: implement lookup walking up the scope stack
    symbol_info *lookup(const string &name) {
        // walk from current_scope upward via get_parent_scope()
        return nullptr;
    }
    symbol_info *lookup(symbol_info *symbol) {
        return lookup(symbol->get_name());
    }

    void print_current_scope() {
        if (current_scope)
            current_scope->print_scope_table(*outlog);
    }

    // TODO: implement print_all_scopes with ##### delimiters
    void print_all_scopes(ofstream &outlog) {
        outlog << "################################" << endl << endl;
        // TODO: iterate scope stack and call print_scope_table on each
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
