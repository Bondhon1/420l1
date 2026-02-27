#pragma once
#include <bits/stdc++.h>
using namespace std;

class symbol_info {
private:
    string name;
    string type;           // lexeme type, e.g. "ID", "INT", "FLOAT"

    // "VARIABLE", "ARRAY", or "FUNCTION"
    string symbol_kind;

    // data type / return type: "int", "float", "void"
    string data_type;

    // for arrays
    int array_size;

    // for functions
    int param_count;
    string param_details; // e.g. "int a, float b"

public:
    symbol_info(string name, string type)
        : name(name), type(type),
          symbol_kind("VARIABLE"), data_type(""),
          array_size(0), param_count(0), param_details("") {}

    // Getters / setters for name and type
    string get_name() const { return name; }
    string getname()  const { return name; }   // alias used by .y grammar actions
    string get_type() const { return type; }

    void set_name(string n) { name = n; }
    void set_type(string t) { type = t; }

    // Symbol kind
    void   set_symbol_kind(string kind) { symbol_kind = kind; }
    string get_symbol_kind() const      { return symbol_kind;  }

    // Data type / return type
    void   set_data_type(string dt) { data_type = dt; }
    string get_data_type() const    { return data_type;  }

    // Array size
    void set_array_size(int sz) { array_size = sz; }
    int  get_array_size() const { return array_size;  }

    // Function parameters
    void   set_param_count(int count)     { param_count = count; }
    int    get_param_count() const        { return param_count;  }

    void   set_param_details(string det)  { param_details = det; }
    string get_param_details() const      { return param_details; }

    ~symbol_info() {}
};
