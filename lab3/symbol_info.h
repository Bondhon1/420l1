#pragma once
#include <bits/stdc++.h>
using namespace std;

class symbol_info {
private:
    string name;
    string type;
    string symbol_kind;
    string data_type;
    int array_size;
    int param_count;
    string param_details;

public:
    symbol_info(string name, string type)
        : name(name), type(type),
          symbol_kind("VARIABLE"), data_type(""),
          array_size(0), param_count(0), param_details("") {}

    string get_name() const { return name; }
    string getname() const { return name; }
    string get_type() const { return type; }

    void set_name(string n) { name = n; }
    void set_type(string t) { type = t; }

    void set_symbol_kind(string kind) { symbol_kind = kind; }
    string get_symbol_kind() const { return symbol_kind; }

    void set_data_type(string dt) { data_type = dt; }
    string get_data_type() const { return data_type; }

    void set_array_size(int sz) { array_size = sz; }
    int get_array_size() const { return array_size; }

    void set_param_count(int count) { param_count = count; }
    int get_param_count() const { return param_count; }

    void set_param_details(string det) { param_details = det; }
    string get_param_details() const { return param_details; }

    ~symbol_info() {}
};
