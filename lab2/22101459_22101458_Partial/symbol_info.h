#pragma once
#include <bits/stdc++.h>
using namespace std;

class symbol_info {
private:
    string name;
    string type;

    // TODO: add symbol_kind field  (VARIABLE / ARRAY / FUNCTION)
    // TODO: add data_type / return_type field  (int / float / void)
    // TODO: add array_size  field
    // TODO: add param_count field
    // TODO: add param_details field (e.g. "int a, float b")

public:
    symbol_info(string name, string type) {
        this->name = name;
        this->type = type;
    }

    string get_name() const { return name; }
    string getname()  const { return name; }   // alias used in grammar actions
    string get_type() const { return type; }

    void set_name(string n) { name = n; }
    void set_type(string t) { type = t; }

    // TODO: add setters / getters for kind, data_type, array_size, params

    ~symbol_info() {}
};
