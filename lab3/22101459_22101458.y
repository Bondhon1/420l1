%{

#include "symbol_table.h"

#define YYSTYPE symbol_info*

int yyparse(void);
int yylex(void);

extern FILE *yyin;

#include<bits/stdc++.h>
using namespace std;

ofstream outlog;
ofstream errlog;

symbol_table *symtab = nullptr;

int lines = 1;
int error_count = 0;

struct VarDecl {
    string name;
    bool is_array;
    int array_size;
};

struct ParamInfo {
    string name;
    string type;
};

vector<VarDecl> collected_vars;
vector<ParamInfo> pending_params;
unordered_set<string> pending_param_names_seen;
string pending_param_details;
int pending_param_count = 0;
string pending_func_name;
string pending_func_return_type;
bool pending_func_duplicate = false;

void report_error(const string &message)
{
    errlog << "At line no: " << lines << " " << message << endl << endl;
    error_count++;
}

void yyerror(char *s)
{
    report_error(s);
}

bool is_int_type(const string &type)
{
    return type == "int";
}

bool is_float_type(const string &type)
{
    return type == "float";
}

bool is_void_type(const string &type)
{
    return type == "void";
}

bool is_error_type(const string &type)
{
    return type.empty() || type == "error";
}

symbol_info *make_node(const string &name, const string &grammar_type, const string &data_type = "")
{
    symbol_info *node = new symbol_info(name, grammar_type);
    node->set_data_type(data_type);
    return node;
}

string node_type(symbol_info *node)
{
    if (node == nullptr) return "error";
    string type = node->get_data_type();
    if (type.empty()) return "error";
    return type;
}

void declare_variable(const string &name, const string &type, bool is_array = false, int array_size = 0, bool is_parameter = false)
{
    if (type == "void") {
        report_error("variable type can not be void ");
        return;
    }

    symbol_info *symbol = new symbol_info(name, "ID");
    symbol->set_symbol_kind(is_array ? "ARRAY" : "VARIABLE");
    symbol->set_data_type(type);
    if (is_array) symbol->set_array_size(array_size);

    if (!symtab->insert(symbol)) {
        if (is_parameter) {
            report_error("Multiple declaration of variable " + name + " in parameter of " + pending_func_name);
        } else {
            report_error("Multiple declaration of variable " + name);
        }
        delete symbol;
    }
}

void declare_function(const string &name, const string &return_type, const vector<string> &param_types, const string &param_details)
{
    symbol_info *symbol = new symbol_info(name, "ID");
    symbol->set_symbol_kind("FUNCTION");
    symbol->set_data_type(return_type);
    symbol->set_param_count(static_cast<int>(param_types.size()));
    symbol->set_param_details(param_details);
    symbol->set_param_types(param_types);

    if (!symtab->insert(symbol)) {
        report_error("Multiple declaration of function " + name);
        delete symbol;
    }
}

void insert_pending_parameters()
{
    unordered_set<string> inserted_names;
    for (const auto &param : pending_params) {
        if (param.name.empty()) continue;
        if (inserted_names.count(param.name)) continue;
        inserted_names.insert(param.name);
        declare_variable(param.name, param.type, false, 0, true);
    }
}

vector<string> get_pending_param_types()
{
    vector<string> types;
    for (const auto &param : pending_params) {
        types.push_back(param.type);
    }
    return types;
}

void check_function_call(symbol_info *function_symbol, symbol_info *arguments, const string &function_name)
{
    const vector<string> &formal_types = function_symbol->get_param_types();
    const vector<string> &actual_types = arguments->get_arg_types();

    if (formal_types.size() != actual_types.size()) {
        report_error("Inconsistencies in number of arguments in function call: " + function_name);
    }

    size_t matched = min(formal_types.size(), actual_types.size());
    for (size_t index = 0; index < matched; ++index) {
        if (actual_types[index] == "error") continue;
        if (formal_types[index] != actual_types[index]) {
            report_error("argument " + to_string(index + 1) + " type mismatch in function call: " + function_name);
        }
    }
}

void check_division_or_modulus_zero(const string &operation, symbol_info *right_node)
{
    if (right_node == nullptr) return;
    const string right_name = right_node->getname();
    if (operation == "/") {
        if (right_name == "0" || right_name == "0.0") {
            report_error("Division by 0 ");
        }
    } else if (operation == "%") {
        if (right_name == "0") {
            report_error("Modulus by 0 ");
        }
    }
}

%}

/* Tokens */
%token ID CONST_INT CONST_FLOAT
%token INT FLOAT VOID
%token IF FOR WHILE DO RETURN PRINTLN
%token CHAR DOUBLE BREAK SWITCH CASE DEFAULT CONTINUE GOTO
%token ADDOP MULOP INCOP DECOP
%token RELOP ASSIGNOP LOGICOP NOT
%token LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD
%token COMMA COLON SEMICOLON

/* Handle dangling else */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

start : program
    {
        outlog<<"At line no: "<<lines<<" start : program "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "start", node_type($1));
    }
    ;

program : program unit
    {
        outlog<<"At line no: "<<lines<<" program : program unit "<<endl<<endl;
        outlog<<$1->getname()<<"\n"<<$2->getname()<<endl<<endl;
        $$ = make_node($1->getname()+"\n"+$2->getname(), "program");
    }
    | unit
    {
        outlog<<"At line no: "<<lines<<" program : unit "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "program");
    }
    ;

unit : var_declaration
    {
        outlog<<"At line no: "<<lines<<" unit : var_declaration "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "unit", node_type($1));
    }
    | func_definition
    {
        outlog<<"At line no: "<<lines<<" unit : func_definition "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "unit");
    }
    ;

func_definition : type_specifier ID LPAREN parameter_list RPAREN
    {
        pending_func_name = $2->getname();
        pending_func_return_type = $1->getname();
        pending_func_duplicate = (symtab->lookup(pending_func_name) != nullptr);
        if (pending_func_duplicate) {
            report_error("Multiple declaration of function " + pending_func_name);
        }
    }
    compound_statement
    {
        outlog<<"At line no: "<<lines<<" func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement "<<endl<<endl;
        outlog<<$1->getname()<<" "<<$2->getname()<<"("<<$4->getname()<<")\n"<<$7->getname()<<endl<<endl;
        if (!pending_func_duplicate) {
            declare_function(pending_func_name, pending_func_return_type, get_pending_param_types(), pending_param_details);
        }
        pending_params.clear();
        pending_param_names_seen.clear();
        pending_param_details = "";
        pending_param_count = 0;
        pending_func_duplicate = false;
        $$ = make_node($1->getname()+" "+$2->getname()+"("+$4->getname()+")\n"+$7->getname(), "func_def");
    }
    | type_specifier ID LPAREN RPAREN
    {
        pending_func_name = $2->getname();
        pending_func_return_type = $1->getname();
        pending_params.clear();
        pending_param_names_seen.clear();
        pending_param_details = "";
        pending_param_count = 0;
        pending_func_duplicate = (symtab->lookup(pending_func_name) != nullptr);
        if (pending_func_duplicate) {
            report_error("Multiple declaration of function " + pending_func_name);
        }
    }
    compound_statement
    {
        outlog<<"At line no: "<<lines<<" func_definition : type_specifier ID LPAREN RPAREN compound_statement "<<endl<<endl;
        outlog<<$1->getname()<<" "<<$2->getname()<<"()\n"<<$6->getname()<<endl<<endl;
        if (!pending_func_duplicate) {
            declare_function(pending_func_name, pending_func_return_type, vector<string>(), "");
        }
        pending_func_duplicate = false;
        $$ = make_node($1->getname()+" "+$2->getname()+"()\n"+$6->getname(), "func_def");
    }
    ;

parameter_list : parameter_list COMMA type_specifier ID
    {
        outlog<<"At line no: "<<lines<<" parameter_list : parameter_list COMMA type_specifier ID "<<endl<<endl;
        outlog<<$1->getname()<<","<<$3->getname()<<" "<<$4->getname()<<endl<<endl;
        if (pending_param_names_seen.count($4->getname())) {
            report_error("Multiple declaration of variable " + $4->getname() + " in parameter of " + pending_func_name);
        }
        pending_param_names_seen.insert($4->getname());
        pending_params.push_back({$4->getname(), $3->getname()});
        if (!pending_param_details.empty()) pending_param_details += ", ";
        pending_param_details += $3->getname()+" "+$4->getname();
        pending_param_count++;
        $$ = make_node($1->getname()+","+$3->getname()+" "+$4->getname(), "param_list");
    }
    | parameter_list COMMA type_specifier
    {
        outlog<<"At line no: "<<lines<<" parameter_list : parameter_list COMMA type_specifier "<<endl<<endl;
        outlog<<$1->getname()<<","<<$3->getname()<<endl<<endl;
        pending_params.push_back({"", $3->getname()});
        if (!pending_param_details.empty()) pending_param_details += ", ";
        pending_param_details += $3->getname();
        pending_param_count++;
        $$ = make_node($1->getname()+","+$3->getname(), "param_list");
    }
    | type_specifier ID
    {
        outlog<<"At line no: "<<lines<<" parameter_list : type_specifier ID "<<endl<<endl;
        outlog<<$1->getname()<<" "<<$2->getname()<<endl<<endl;
        pending_params.clear();
        pending_param_names_seen.clear();
        pending_param_names_seen.insert($2->getname());
        pending_params.push_back({$2->getname(), $1->getname()});
        pending_param_details = $1->getname()+" "+$2->getname();
        pending_param_count = 1;
        $$ = make_node($1->getname()+" "+$2->getname(), "param_list");
    }
    | type_specifier
    {
        outlog<<"At line no: "<<lines<<" parameter_list : type_specifier "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        pending_params.clear();
        pending_params.push_back({"", $1->getname()});
        pending_param_details = $1->getname();
        pending_param_count = 1;
        $$ = make_node($1->getname(), "param_list");
    }
    ;

compound_statement : LCURL
    {
        symtab->enter_scope();
        insert_pending_parameters();
    }
    statements RCURL
    {
        outlog<<"At line no: "<<lines<<" compound_statement : LCURL statements RCURL "<<endl<<endl;
        outlog<<"{\n"<<$3->getname()<<"\n}"<<endl<<endl;
        symtab->exit_scope();
        $$ = make_node("{\n"+$3->getname()+"\n}", "compound_stmnt");
    }
    | LCURL
    {
        symtab->enter_scope();
        insert_pending_parameters();
    }
    RCURL
    {
        outlog<<"At line no: "<<lines<<" compound_statement : LCURL RCURL "<<endl<<endl;
        outlog<<"{\n}"<<endl<<endl;
        symtab->exit_scope();
        $$ = make_node("{\n}", "compound_stmnt");
    }
    ;

var_declaration : type_specifier declaration_list SEMICOLON
    {
        outlog<<"At line no: "<<lines<<" var_declaration : type_specifier declaration_list SEMICOLON "<<endl<<endl;
        outlog<<$1->getname()<<" "<<$2->getname()<<";"<<endl<<endl;
        if ($1->getname() == "void") {
            report_error("variable type can not be void ");
        } else {
            for (const auto &var : collected_vars) {
                declare_variable(var.name, $1->getname(), var.is_array, var.array_size, false);
            }
        }
        collected_vars.clear();
        $$ = make_node($1->getname()+" "+$2->getname()+";", "var_decl", $1->getname());
    }
    ;

type_specifier : INT
    {
        outlog<<"At line no: "<<lines<<" type_specifier : INT "<<endl<<endl;
        outlog<<"int"<<endl<<endl;
        $$ = make_node("int", "type_spec");
    }
    | FLOAT
    {
        outlog<<"At line no: "<<lines<<" type_specifier : FLOAT "<<endl<<endl;
        outlog<<"float"<<endl<<endl;
        $$ = make_node("float", "type_spec");
    }
    | VOID
    {
        outlog<<"At line no: "<<lines<<" type_specifier : VOID "<<endl<<endl;
        outlog<<"void"<<endl<<endl;
        $$ = make_node("void", "type_spec");
    }
    ;

declaration_list : declaration_list COMMA ID
    {
        outlog<<"At line no: "<<lines<<" declaration_list : declaration_list COMMA ID "<<endl<<endl;
        outlog<<$1->getname()<<","<<$3->getname()<<endl<<endl;
        collected_vars.push_back({$3->getname(), false, 0});
        $$ = make_node($1->getname()+","+$3->getname(), "decl_list");
    }
    | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD
    {
        outlog<<"At line no: "<<lines<<" declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD "<<endl<<endl;
        outlog<<$1->getname()<<","<<$3->getname()<<"["<<$5->getname()<<"]"<<endl<<endl;
        collected_vars.push_back({$3->getname(), true, stoi($5->getname())});
        $$ = make_node($1->getname()+","+$3->getname()+"["+$5->getname()+"]", "decl_list");
    }
    | ID
    {
        outlog<<"At line no: "<<lines<<" declaration_list : ID "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        collected_vars.clear();
        collected_vars.push_back({$1->getname(), false, 0});
        $$ = make_node($1->getname(), "decl_list");
    }
    | ID LTHIRD CONST_INT RTHIRD
    {
        outlog<<"At line no: "<<lines<<" declaration_list : ID LTHIRD CONST_INT RTHIRD "<<endl<<endl;
        outlog<<$1->getname()<<"["<<$3->getname()<<"]"<<endl<<endl;
        collected_vars.clear();
        collected_vars.push_back({$1->getname(), true, stoi($3->getname())});
        $$ = make_node($1->getname()+"["+$3->getname()+"]", "decl_list");
    }
    ;

statements : statement
    {
        outlog<<"At line no: "<<lines<<" statements : statement "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "statements");
    }
    | statements statement
    {
        outlog<<"At line no: "<<lines<<" statements : statements statement "<<endl<<endl;
        outlog<<$1->getname()<<"\n"<<$2->getname()<<endl<<endl;
        $$ = make_node($1->getname()+"\n"+$2->getname(), "statements");
    }
    ;

statement : var_declaration
    {
        outlog<<"At line no: "<<lines<<" statement : var_declaration "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "stmnt");
    }
    | expression_statement
    {
        outlog<<"At line no: "<<lines<<" statement : expression_statement "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "stmnt");
    }
    | compound_statement
    {
        outlog<<"At line no: "<<lines<<" statement : compound_statement "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "stmnt");
    }
    | FOR LPAREN expression_statement expression_statement expression RPAREN statement
    {
        outlog<<"At line no: "<<lines<<" statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement "<<endl<<endl;
        outlog<<"for("<<$3->getname()<<$4->getname()<<$5->getname()<<")\n"<<$7->getname()<<endl<<endl;
        $$ = make_node("for("+$3->getname()+$4->getname()+$5->getname()+")\n"+$7->getname(), "stmnt");
    }
    | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
    {
        outlog<<"At line no: "<<lines<<" statement : IF LPAREN expression RPAREN statement "<<endl<<endl;
        outlog<<"if("<<$3->getname()<<")\n"<<$5->getname()<<endl<<endl;
        $$ = make_node("if("+$3->getname()+")\n"+$5->getname(), "stmnt");
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    {
        outlog<<"At line no: "<<lines<<" statement : IF LPAREN expression RPAREN statement ELSE statement "<<endl<<endl;
        outlog<<"if("<<$3->getname()<<")\n"<<$5->getname()<<"\nelse\n"<<$7->getname()<<endl<<endl;
        $$ = make_node("if("+$3->getname()+")\n"+$5->getname()+"\nelse\n"+$7->getname(), "stmnt");
    }
    | WHILE LPAREN expression RPAREN statement
    {
        outlog<<"At line no: "<<lines<<" statement : WHILE LPAREN expression RPAREN statement "<<endl<<endl;
        outlog<<"while("<<$3->getname()<<")\n"<<$5->getname()<<endl<<endl;
        $$ = make_node("while("+$3->getname()+")\n"+$5->getname(), "stmnt");
    }
    | PRINTLN LPAREN ID RPAREN SEMICOLON
    {
        outlog<<"At line no: "<<lines<<" statement : PRINTLN LPAREN ID RPAREN SEMICOLON "<<endl<<endl;
        outlog<<"printf("<<$3->getname()<<");"<<endl<<endl;
        symbol_info *symbol = symtab->lookup($3->getname());
        if (symbol == nullptr || symbol->get_symbol_kind() == "FUNCTION") {
            report_error("Undeclared variable " + $3->getname());
        } else if (symbol->get_symbol_kind() == "ARRAY") {
            report_error("variable is of array type : " + $3->getname());
        }
        $$ = make_node("printf("+$3->getname()+");", "stmnt");
    }
    | RETURN expression SEMICOLON
    {
        outlog<<"At line no: "<<lines<<" statement : RETURN expression SEMICOLON "<<endl<<endl;
        outlog<<"return "<<$2->getname()<<";"<<endl<<endl;
        $$ = make_node("return "+$2->getname()+";", "stmnt");
    }
    ;

expression_statement : SEMICOLON
    {
        outlog<<"At line no: "<<lines<<" expression_statement : SEMICOLON "<<endl<<endl;
        outlog<<";"<<endl<<endl;
        $$ = make_node(";", "expr_stmnt");
    }
    | expression SEMICOLON
    {
        outlog<<"At line no: "<<lines<<" expression_statement : expression SEMICOLON "<<endl<<endl;
        outlog<<$1->getname()<<";"<<endl<<endl;
        $$ = make_node($1->getname()+";", "expr_stmnt", node_type($1));
    }
    ;

variable : ID
    {
        outlog<<"At line no: "<<lines<<" variable : ID "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        symbol_info *symbol = symtab->lookup($1->getname());
        string data_type = "error";
        if (symbol == nullptr || symbol->get_symbol_kind() == "FUNCTION") {
            report_error("Undeclared variable " + $1->getname());
        } else {
            if (symbol->get_symbol_kind() == "ARRAY") {
                report_error("variable is of array type : " + $1->getname());
                data_type = "error";
            } else {
                data_type = symbol->get_data_type();
            }
        }
        $$ = make_node($1->getname(), "variable", data_type);
    }
    | ID LTHIRD expression RTHIRD
    {
        outlog<<"At line no: "<<lines<<" variable : ID LTHIRD expression RTHIRD "<<endl<<endl;
        outlog<<$1->getname()<<"["<<$3->getname()<<"]"<<endl<<endl;
        symbol_info *symbol = symtab->lookup($1->getname());
        string data_type = "error";
        if (symbol == nullptr || symbol->get_symbol_kind() == "FUNCTION") {
            report_error("Undeclared variable " + $1->getname());
        } else if (symbol->get_symbol_kind() != "ARRAY") {
            report_error("variable is not of array type : " + $1->getname());
        } else {
            if (node_type($3) != "int") {
                report_error("array index is not of integer type : " + $1->getname());
            }
            data_type = symbol->get_data_type();
        }
        $$ = make_node($1->getname()+"["+$3->getname()+"]", "variable", data_type);
    }
    ;

expression : logic_expression
    {
        outlog<<"At line no: "<<lines<<" expression : logic_expression "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "expression", node_type($1));
    }
    | variable ASSIGNOP logic_expression
    {
        outlog<<"At line no: "<<lines<<" expression : variable ASSIGNOP logic_expression "<<endl<<endl;
        outlog<<$1->getname()<<"="<<$3->getname()<<endl<<endl;
        string left_type = node_type($1);
        string right_type = node_type($3);
        if (is_void_type(left_type) || is_void_type(right_type)) {
            report_error("operation on void type ");
        } else if (is_int_type(left_type) && is_float_type(right_type)) {
            report_error("Warning: Assignment of float value into variable of integer type ");
        }
        $$ = make_node($1->getname()+"="+$3->getname(), "expression", left_type);
    }
    ;

logic_expression : rel_expression
    {
        outlog<<"At line no: "<<lines<<" logic_expression : rel_expression "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "logic_expr", node_type($1));
    }
    | rel_expression LOGICOP rel_expression
    {
        outlog<<"At line no: "<<lines<<" logic_expression : rel_expression LOGICOP rel_expression "<<endl<<endl;
        outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
        if (is_void_type(node_type($1)) || is_void_type(node_type($3))) {
            report_error("operation on void type ");
        }
        $$ = make_node($1->getname()+$2->getname()+$3->getname(), "logic_expr", "int");
    }
    ;

rel_expression : simple_expression
    {
        outlog<<"At line no: "<<lines<<" rel_expression : simple_expression "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "rel_expr", node_type($1));
    }
    | simple_expression RELOP simple_expression
    {
        outlog<<"At line no: "<<lines<<" rel_expression : simple_expression RELOP simple_expression "<<endl<<endl;
        outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
        if (is_void_type(node_type($1)) || is_void_type(node_type($3))) {
            report_error("operation on void type ");
        }
        $$ = make_node($1->getname()+$2->getname()+$3->getname(), "rel_expr", "int");
    }
    ;

simple_expression : term
    {
        outlog<<"At line no: "<<lines<<" simple_expression : term "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "simple_expr", node_type($1));
    }
    | simple_expression ADDOP term
    {
        outlog<<"At line no: "<<lines<<" simple_expression : simple_expression ADDOP term "<<endl<<endl;
        outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
        string left_type = node_type($1);
        string right_type = node_type($3);
        if (is_void_type(left_type) || is_void_type(right_type)) {
            report_error("operation on void type ");
            $$ = make_node($1->getname()+$2->getname()+$3->getname(), "simple_expr", "error");
        } else if (is_float_type(left_type) || is_float_type(right_type)) {
            $$ = make_node($1->getname()+$2->getname()+$3->getname(), "simple_expr", "float");
        } else {
            $$ = make_node($1->getname()+$2->getname()+$3->getname(), "simple_expr", "int");
        }
    }
    ;

term : unary_expression
    {
        outlog<<"At line no: "<<lines<<" term : unary_expression "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "term", node_type($1));
    }
    | term MULOP unary_expression
    {
        outlog<<"At line no: "<<lines<<" term : term MULOP unary_expression "<<endl<<endl;
        outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
        string operation = $2->getname();
        string left_type = node_type($1);
        string right_type = node_type($3);
        if (operation == "/" || operation == "%") {
            check_division_or_modulus_zero(operation, $3);
        }
        if (is_void_type(left_type) || is_void_type(right_type)) {
            report_error("operation on void type ");
            $$ = make_node($1->getname()+$2->getname()+$3->getname(), "term", "error");
        } else if (operation == "%") {
            if (!is_int_type(left_type) || !is_int_type(right_type)) {
                report_error("Modulus operator on non integer type ");
                $$ = make_node($1->getname()+$2->getname()+$3->getname(), "term", "error");
            } else {
                $$ = make_node($1->getname()+$2->getname()+$3->getname(), "term", "int");
            }
        } else if (is_float_type(left_type) || is_float_type(right_type)) {
            $$ = make_node($1->getname()+$2->getname()+$3->getname(), "term", "float");
        } else {
            $$ = make_node($1->getname()+$2->getname()+$3->getname(), "term", "int");
        }
    }
    ;

unary_expression : ADDOP unary_expression
    {
        outlog<<"At line no: "<<lines<<" unary_expression : ADDOP unary_expression "<<endl<<endl;
        outlog<<$1->getname()<<$2->getname()<<endl<<endl;
        string type = node_type($2);
        if (is_void_type(type)) {
            report_error("operation on void type ");
            $$ = make_node($1->getname()+$2->getname(), "unary_expr", "error");
        } else {
            $$ = make_node($1->getname()+$2->getname(), "unary_expr", type);
        }
    }
    | NOT unary_expression
    {
        outlog<<"At line no: "<<lines<<" unary_expression : NOT unary_expression "<<endl<<endl;
        outlog<<"!"<<$2->getname()<<endl<<endl;
        if (is_void_type(node_type($2))) {
            report_error("operation on void type ");
            $$ = make_node("!"+$2->getname(), "unary_expr", "error");
        } else {
            $$ = make_node("!"+$2->getname(), "unary_expr", "int");
        }
    }
    | factor
    {
        outlog<<"At line no: "<<lines<<" unary_expression : factor "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "unary_expr", node_type($1));
    }
    ;

factor : variable
    {
        outlog<<"At line no: "<<lines<<" factor : variable "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "factor", node_type($1));
    }
    | ID LPAREN argument_list RPAREN
    {
        outlog<<"At line no: "<<lines<<" factor : ID LPAREN argument_list RPAREN "<<endl<<endl;
        outlog<<$1->getname()<<"("<<$3->getname()<<")"<<endl<<endl;
        symbol_info *symbol = symtab->lookup($1->getname());
        string data_type = "error";
        if (symbol == nullptr || symbol->get_symbol_kind() != "FUNCTION") {
            report_error("Undeclared function: " + $1->getname());
        } else {
            check_function_call(symbol, $3, $1->getname());
            data_type = symbol->get_data_type();
        }
        $$ = make_node($1->getname()+"("+$3->getname()+")", "factor", data_type);
    }
    | LPAREN expression RPAREN
    {
        outlog<<"At line no: "<<lines<<" factor : LPAREN expression RPAREN "<<endl<<endl;
        outlog<<"("<<$2->getname()<<")"<<endl<<endl;
        $$ = make_node("("+$2->getname()+")", "factor", node_type($2));
    }
    | CONST_INT
    {
        outlog<<"At line no: "<<lines<<" factor : CONST_INT "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "factor", "int");
    }
    | CONST_FLOAT
    {
        outlog<<"At line no: "<<lines<<" factor : CONST_FLOAT "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "factor", "float");
    }
    | variable INCOP
    {
        outlog<<"At line no: "<<lines<<" factor : variable INCOP "<<endl<<endl;
        outlog<<$1->getname()<<"++"<<endl<<endl;
        $$ = make_node($1->getname()+"++", "factor", node_type($1));
    }
    | variable DECOP
    {
        outlog<<"At line no: "<<lines<<" factor : variable DECOP "<<endl<<endl;
        outlog<<$1->getname()<<"--"<<endl<<endl;
        $$ = make_node($1->getname()+"--", "factor", node_type($1));
    }
    ;

argument_list : arguments
    {
        outlog<<"At line no: "<<lines<<" argument_list : arguments "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "arg_list");
        $$->set_arg_types($1->get_arg_types());
    }
    |
    {
        $$ = make_node("", "arg_list");
        $$->set_arg_types(vector<string>());
    }
    ;

arguments : arguments COMMA logic_expression
    {
        outlog<<"At line no: "<<lines<<" arguments : arguments COMMA logic_expression "<<endl<<endl;
        outlog<<$1->getname()<<","<<$3->getname()<<endl<<endl;
        vector<string> arg_types = $1->get_arg_types();
        arg_types.push_back(node_type($3));
        $$ = make_node($1->getname()+","+$3->getname(), "arguments");
        $$->set_arg_types(arg_types);
    }
    | logic_expression
    {
        outlog<<"At line no: "<<lines<<" arguments : logic_expression "<<endl<<endl;
        outlog<<$1->getname()<<endl<<endl;
        $$ = make_node($1->getname(), "arguments");
        vector<string> arg_types;
        arg_types.push_back(node_type($1));
        $$->set_arg_types(arg_types);
    }
    ;

%%

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout<<"Usage: "<<argv[0]<<" <input_file>"<<endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    outlog.open("22101459_22101458_log.txt", ios::trunc);
    errlog.open("22101459_22101458_error.txt", ios::trunc);

    if(yyin == NULL)
    {
        cout<<"Couldn't open file"<<endl;
        return 0;
    }

    symtab = new symbol_table(10, &outlog);
    symtab->enter_scope();

    yyparse();

    outlog<<"Total lines: "<<lines<<endl;
    outlog<<"Total errors: "<<error_count<<endl;
    errlog<<"Total errors: "<<error_count<<endl;

    outlog.close();
    errlog.close();
    fclose(yyin);

    delete symtab;

    return 0;
}
