#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace std;

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const = 0;
};

// Expression node types

class ExprNode : public ASTNode {
protected:
    string node_type; // Type information (int, float, void, etc.)
public:
    ExprNode(string type) : node_type(type) {}
    virtual string get_type() const { return node_type; }
};

// Variable node (for ID references)

class VarNode : public ExprNode {
private:
    string name;
    ExprNode* index; // For array access, nullptr for simple variables

public:
    VarNode(string name, string type, ExprNode* idx = nullptr)
        : ExprNode(type), name(name), index(idx) {}
    
    ~VarNode() { if(index) delete index; }
    
    bool has_index() const { return index != nullptr; }
    
    string generate_index_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                              int& temp_count, int& label_count) const {
        if (index) {
            return index->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        if (has_index()) {
            string index_temp = generate_index_code(outcode, symbol_to_temp, temp_count, label_count);
            return name + "[" + index_temp + "]";
        }
        return name;
    }
    
    string get_name() const { return name; }
};

// Constant node

class ConstNode : public ExprNode {
private:
    string value;

public:
    ConstNode(string val, string type) : ExprNode(type), value(val) {}
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        return value;
    }
};

// Binary operation node

class BinaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* left;
    ExprNode* right;

public:
    BinaryOpNode(string op, ExprNode* left, ExprNode* right, string result_type)
        : ExprNode(result_type), op(op), left(left), right(right) {}
    
    ~BinaryOpNode() {
        delete left;
        delete right;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        string left_code = left->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string right_code = right->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string temp_var = "t" + to_string(temp_count++);
        outcode << temp_var << " = " << left_code << " " << op << " " << right_code << endl;
        return temp_var;
    }
};

// Unary operation node

class UnaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* expr;

public:
    UnaryOpNode(string op, ExprNode* expr, string result_type)
        : ExprNode(result_type), op(op), expr(expr) {}
    
    ~UnaryOpNode() { delete expr; }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        string expr_code = expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string temp_var = "t" + to_string(temp_count++);
        outcode << temp_var << " = " << op << " " << expr_code << endl;
        return temp_var;
    }
};

// Assignment node

class AssignNode : public ExprNode {
private:
    VarNode* lhs;
    ExprNode* rhs;

public:
    AssignNode(VarNode* lhs, ExprNode* rhs, string result_type)
        : ExprNode(result_type), lhs(lhs), rhs(rhs) {}
    
    ~AssignNode() {
        delete lhs;
        delete rhs;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
        string rhs_code = rhs->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string lhs_code = lhs->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << lhs_code << " = " << rhs_code << endl;
        return lhs_code generate code for assignment operations
        return "";
    }
};

// Statement node types

class StmtNode : public ASTNode {
public:
    virtual string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                                int& temp_count, int& label_count) const = 0;
};

// Expression statement node

class ExprStmtNode : public StmtNode {
private:
    ExprNode* expr;

public:
    ExprStmtNode(ExprNode* e) : expr(e) {}
    ~ExprStmtNode() { if(expr) delete expr; }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
        if (expr) expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        // Should generate code for expression statements
        return "";
    }
};

// Block (compound statement) node

class BlockNode : public StmtNode {
private:
    vector<StmtNode*> statements;

public:
    ~BlockNode() {
        for (auto stmt : statements) {
            delete stmt;
        }
    }
    
    void add_statement(StmtNode* stmt) {
        if (stmt) statements.push_back(stmt);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        for (auto stmt : statements) {
            if (stmt) stmt->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
};

// If statement node

class IfNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* then_block;
    StmtNode* else_block; // nullptr if no else part

public:
    IfNode(ExprNode* cond, StmtNode* then_stmt, StmtNode* else_stmt = nullptr)
        : condition(cond), then_block(then_stmt), else_block(else_stmt) {}
    
    ~IfNode() {
        delete condition;
        delete then_block;
        if (else_block) delete else_block;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
        string cond_code = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        int L_then = label_count++, L_else = label_count++, L_end = label_count++;
        outcode << "if (" << cond_code << ") goto L" << L_then << endl;
        outcode << "goto L" << (else_block ? L_else : L_end) << endl;
        outcode << "L" << L_then << ":" << endl;
        then_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        if (else_block) {
            outcode << "goto L" << L_end << endl << "L" << L_else << ":" << endl;
            else_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        outcode << "L" << L_end << ":" << endl;
        // Should generate code for if-else statements
        return "";
    }
};

// While statement node

class WhileNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* body;

public:
    WhileNode(ExprNode* cond, StmtNode* body_stmt)
        int L_start = label_count++, L_body = label_count++, L_end = label_count++;
        outcode << "L" << L_start << ":" << endl;
        string cond_code = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "if (" << cond_code << ") goto L" << L_body << endl;
        outcode << "goto L" << L_end << endl << "L" << L_body << ":" << endl;
        body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto L" << L_start << endl << "L" << L_end << ":" << endl;
    ~WhileNode() {
        delete condition;
        delete body;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for while loops
        return "";
    }
};

// For statement node

class ForNode : public StmtNode {
private:
    ExprNode* init;
    ExprNode* condition;
    ExprNode* update;
    StmtNode* body;
if (init) init->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        int L_start = label_count++, L_body = label_count++, L_end = label_count++;
        outcode << "L" << L_start << ":" << endl;
        if (condition) {
            string cond_code = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << "if (" << cond_code << ") goto L" << L_body << endl;
        }
        outcode << "goto L" << L_end << endl << "L" << L_body << ":" << endl;
        body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        if (update) update->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto L" << L_start << endl << "L" << L_end << ":" << endl;
    ForNode(ExprNode* init_expr, ExprNode* cond_expr, ExprNode* update_expr, StmtNode* body_stmt)
        : init(init_expr), condition(cond_expr), update(update_expr), body(body_stmt) {}
    
    ~ForNode() {
        if (init) delete init;
        if (condition) delete condition;
        if (expr) {
            string result = expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << "return " << result << endl;
        } else {
            outcode << "return" << endl;
        }
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for for loops
        return "";
    }
};

// Return statement node

class ReturnNode : public StmtNode {
private:
    ExprNode* expr;

public:
    ReturnNode(ExprNode* e) : expr(e) {}
    ~ReturnNode() { if (expr) delete expr; }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for return statements
        return "";
    }
};

// Declaration node

class DeclNode : public StmtNode {
private:
    string type;
    vector<pair<string, int>> vars; // Variable name and array size (0 for regular vars)

public:
    DeclNode(string t) : type(t) {}
    
    void add_var(string name, int array_size = 0) {
        vars.push_back(make_pair(name, array_size));
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for variable declarations
        return "";
    }
    
    strioutcode << "\n// Function: " << name << "\n" << name << ":" << endl;
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);eturn vars; }
};

// Function declaration node

class FuncDeclNode : public ASTNode {
private:
    string return_type;
    string name;
    vector<pair<string, string>> params; // Parameter type and name
    BlockNode* body;

public:
    FuncDeclNode(string ret_type, string n) : return_type(ret_type), name(n), body(nullptr) {}
    ~FuncDeclNode() { if (body) delete body; }
    
    void add_param(string type, string name) {
        params.push_back(make_pair(type, name));
    }
    
    void set_body(BlockNode* b) {
        body = b;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for function declarations
        return "";
    }
};

// Helper class for function arguments

class ArgumentsNode : public ASTNode {
private:
    vector<ExprNode*> args;

public:
    ~ArgumentsNode() {
        // Don't delete args here - they'll be transferred to FuncCallNode
    }
    
    void add_argument(ExprNode* arg) {
        if (arg) args.push_back(arg);
    }
    
    ExprNode* get_argument(int index) const {
        if (index >= 0 && index < args.size()) {
            return args[index];
        }
        return nullptr;
    }
    
    size_t size() const {
        return args.size();
    }
    
    const vector<ExprNode*>& get_arguments() const {
        return args;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // This node doesn't generate code directly
        string arg_code = "";
        for (int i = 0; i < arguments.size(); i++) {
            if (i > 0) arg_code += ", ";
            arg_code += arguments[i]->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        string temp_var = "t" + to_string(temp_count++);
        outcode << temp_var << " = call " << func_name << "(" << arg_code << ")" << endl;
        return temp_var

// Function call node

class FuncCallNode : public ExprNode {
private:
    string func_name;
    vector<ExprNode*> arguments;

public:
    FuncCallNode(string name, string result_type)
        : ExprNode(result_type), func_name(name) {}
    
    ~FuncCallNode() {
        for (auto arg : arguments) {
            delete arg;
        }
    }
    outcode << "// Three-Address Code\n" << endl;
        for (auto unit : units) {
            if (unit) unit->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        if (arg) arguments.push_back(arg);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for function calls
        return "";
    }
};

// Program node (root of AST)

class ProgramNode : public ASTNode {
private:
    vector<ASTNode*> units;

public:
    ~ProgramNode() {
        for (auto unit : units) {
            delete unit;
        }
    }
    
    void add_unit(ASTNode* unit) {
        if (unit) units.push_back(unit);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for the entire program
        return "";
    }
};

#endif // AST_H