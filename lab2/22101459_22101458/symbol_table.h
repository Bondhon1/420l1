#pragma once
#include "scope_table.h"

class symbol_table {
private:
    scope_table *current_scope;
    int bucket_count;
    int next_scope_id;   // counter that gives each new scope a unique id
    ofstream *outlog;

public:
    symbol_table(int bucket_count, ofstream *outlog)
        : bucket_count(bucket_count), current_scope(nullptr),
          next_scope_id(0), outlog(outlog) {}

    // Create a new scope on top of the stack
    void enter_scope() {
        next_scope_id++;
        scope_table *ns = new scope_table(bucket_count, next_scope_id, current_scope);
        current_scope = ns;
        *outlog << "New ScopeTable with ID " << next_scope_id
                << " created" << endl << endl;
    }

    // Print all visible scopes, then pop and report the top scope
    void exit_scope() {
        if (!current_scope) return;
        print_all_scopes(*outlog);
        int id = current_scope->get_unique_id();
        scope_table *parent = current_scope->get_parent_scope();
        delete current_scope;
        current_scope = parent;
        *outlog << "Scopetable with ID " << id << " removed" << endl << endl;
    }

    // Insert into current scope; return true on success
    bool insert(symbol_info *symbol) {
        if (!current_scope) return false;
        bool ok = current_scope->insert_in_scope(symbol);
        return ok;
    }

    // Remove from current scope; return true on success
    bool remove(symbol_info *symbol) {
        if (!current_scope) return false;
        return current_scope->delete_from_scope(symbol);
    }

    // Lookup walking up the scope stack; returns nullptr if not found
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

    // Print only the current scope table
    void print_current_scope() {
        if (current_scope)
            current_scope->print_scope_table(*outlog);
    }

    // Print all scope tables from innermost to outermost, wrapped in
    // ################################ delimiters  (matches sample output)
    void print_all_scopes(ofstream &outlog) {
        outlog << "################################" << endl << endl;
        scope_table *temp = current_scope;
        while (temp) {
            temp->print_scope_table(outlog);
            temp = temp->get_parent_scope();
            if (temp) outlog << endl; // blank line between consecutive scope tables
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
