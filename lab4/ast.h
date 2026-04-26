#ifndef AST_H
#define AST_H

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const = 0;
};

class ExprNode : public ASTNode {
protected:
    string node_type;

public:
    ExprNode(string type) : node_type(type) {}
    virtual ~ExprNode() {}

    virtual string get_type() const { return node_type; }
    virtual ExprNode* clone() const = 0;
};

class VarNode : public ExprNode {
private:
    string name;
    ExprNode* index;

public:
    VarNode(string var_name, string type, ExprNode* idx = nullptr)
        : ExprNode(std::move(type)), name(std::move(var_name)), index(idx) {}

    ~VarNode() override { delete index; }

    bool has_index() const { return index != nullptr; }
    string get_name() const { return name; }

    string generate_index_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const {
        if (!index) return "";
        return index->generate_code(outcode, symbol_to_temp, temp_count, label_count);
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        if (!index) return name;
        string index_code = generate_index_code(outcode, symbol_to_temp, temp_count, label_count);
        return name + "[" + index_code + "]";
    }

    ExprNode* clone() const override {
        return new VarNode(name, node_type, index ? index->clone() : nullptr);
    }
};

class ConstNode : public ExprNode {
private:
    string value;

public:
    ConstNode(string val, string type) : ExprNode(std::move(type)), value(std::move(val)) {}

    string generate_code(ofstream&, map<string, string>&, int&, int&) const override {
        return value;
    }

    ExprNode* clone() const override {
        return new ConstNode(value, node_type);
    }
};

class BinaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* left;
    ExprNode* right;

public:
    BinaryOpNode(string operation, ExprNode* lhs, ExprNode* rhs, string result_type)
        : ExprNode(std::move(result_type)), op(std::move(operation)), left(lhs), right(rhs) {}

    ~BinaryOpNode() override {
        delete left;
        delete right;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        string left_code = left ? left->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string right_code = right ? right->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string temp_var = "t" + to_string(temp_count++);
        outcode << temp_var << " = " << left_code << " " << op << " " << right_code << endl;
        return temp_var;
    }

    ExprNode* clone() const override {
        return new BinaryOpNode(op, left ? left->clone() : nullptr, right ? right->clone() : nullptr, node_type);
    }
};

class UnaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* expr;

public:
    UnaryOpNode(string operation, ExprNode* operand, string result_type)
        : ExprNode(std::move(result_type)), op(std::move(operation)), expr(operand) {}

    ~UnaryOpNode() override { delete expr; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        string expr_code = expr ? expr->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string temp_var = "t" + to_string(temp_count++);
        outcode << temp_var << " = " << op << " " << expr_code << endl;
        return temp_var;
    }

    ExprNode* clone() const override {
        return new UnaryOpNode(op, expr ? expr->clone() : nullptr, node_type);
    }
};

class AssignNode : public ExprNode {
private:
    VarNode* lhs;
    ExprNode* rhs;

public:
    AssignNode(VarNode* left, ExprNode* right, string result_type)
        : ExprNode(std::move(result_type)), lhs(left), rhs(right) {}

    ~AssignNode() override {
        delete lhs;
        delete rhs;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        string rhs_code = rhs ? rhs->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string lhs_code = lhs ? lhs->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        outcode << lhs_code << " = " << rhs_code << endl;
        return lhs_code;
    }

    ExprNode* clone() const override {
        return new AssignNode(lhs ? dynamic_cast<VarNode*>(lhs->clone()) : nullptr, rhs ? rhs->clone() : nullptr, node_type);
    }
};

class StmtNode : public ASTNode {
public:
    virtual ~StmtNode() {}
};

class ExprStmtNode : public StmtNode {
private:
    ExprNode* expr;

public:
    ExprStmtNode(ExprNode* e) : expr(e) {}
    ~ExprStmtNode() override { delete expr; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        if (expr) expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        return "";
    }
};

class BlockNode : public StmtNode {
private:
    vector<StmtNode*> statements;

public:
    ~BlockNode() override {
        for (StmtNode* stmt : statements) delete stmt;
    }

    void add_statement(StmtNode* stmt) {
        if (stmt) statements.push_back(stmt);
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        for (StmtNode* stmt : statements) {
            if (stmt) stmt->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
};

class IfNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* then_block;
    StmtNode* else_block;

public:
    IfNode(ExprNode* cond, StmtNode* then_stmt, StmtNode* else_stmt = nullptr)
        : condition(cond), then_block(then_stmt), else_block(else_stmt) {}

    ~IfNode() override {
        delete condition;
        delete then_block;
        delete else_block;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        int then_label = label_count++;
        int else_label = label_count++;
        int end_label = label_count++;
        string cond_code = condition ? condition->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";

        outcode << "if " << cond_code << " goto L" << then_label << endl;
        outcode << "goto L" << else_label << endl;
        outcode << "L" << then_label << ":" << endl;
        if (then_block) then_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto L" << end_label << endl;
        outcode << "L" << else_label << ":" << endl;
        if (else_block) else_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "L" << end_label << ":" << endl;
        return "";
    }
};

class WhileNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* body;

public:
    WhileNode(ExprNode* cond, StmtNode* body_stmt)
        : condition(cond), body(body_stmt) {}

    ~WhileNode() override {
        delete condition;
        delete body;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        int start_label = label_count++;
        int body_label = label_count++;
        int end_label = label_count++;

        outcode << "L" << start_label << ":" << endl;
        string cond_code = condition ? condition->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        outcode << "if " << cond_code << " goto L" << body_label << endl;
        outcode << "goto L" << end_label << endl;
        outcode << "L" << body_label << ":" << endl;
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto L" << start_label << endl;
        outcode << "L" << end_label << ":" << endl;
        return "";
    }
};

class ForNode : public StmtNode {
private:
    ASTNode* init;
    ASTNode* condition;
    ASTNode* update;
    StmtNode* body;

public:
    ForNode(ASTNode* init_expr, ASTNode* cond_expr, ASTNode* update_expr, StmtNode* body_stmt)
        : init(init_expr), condition(cond_expr), update(update_expr), body(body_stmt) {}

    ~ForNode() override {
        delete init;
        delete condition;
        delete update;
        delete body;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        int start_label = label_count++;
        int body_label = label_count++;
        int end_label = label_count++;

        if (init) init->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "L" << start_label << ":" << endl;

        if (condition) {
            string cond_code = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << "if " << cond_code << " goto L" << body_label << endl;
            outcode << "goto L" << end_label << endl;
        } else {
            outcode << "goto L" << body_label << endl;
        }

        outcode << "L" << body_label << ":" << endl;
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        if (update) update->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto L" << start_label << endl;
        outcode << "L" << end_label << ":" << endl;
        return "";
    }
};

class ReturnNode : public StmtNode {
private:
    ExprNode* expr;

public:
    ReturnNode(ExprNode* e) : expr(e) {}
    ~ReturnNode() override { delete expr; }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        if (expr) {
            string result = expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << "return " << result << endl;
        } else {
            outcode << "return" << endl;
        }
        return "";
    }
};

class DeclNode : public StmtNode {
private:
    string type;
    vector<pair<string, int>> vars;

public:
    DeclNode(string t) : type(std::move(t)) {}

    void add_var(string name, int array_size = 0) {
        vars.emplace_back(std::move(name), array_size);
    }

    string generate_code(ofstream& outcode, map<string, string>&, int&, int&) const override {
        for (const auto& var : vars) {
            outcode << "// Declaration: " << type << " " << var.first;
            if (var.second > 0) outcode << "[" << var.second << "]";
            outcode << endl;
        }
        return "";
    }
};

class ArgumentsNode : public ASTNode {
private:
    vector<ExprNode*> args;

public:
    ~ArgumentsNode() override = default;

    void add_argument(ExprNode* arg) {
        if (arg) args.push_back(arg);
    }

    ExprNode* get_argument(int index) const {
        if (index < 0 || index >= static_cast<int>(args.size())) return nullptr;
        return args[index];
    }

    size_t size() const { return args.size(); }

    const vector<ExprNode*>& get_arguments() const { return args; }

    string generate_code(ofstream&, map<string, string>&, int&, int&) const override {
        return "";
    }
};

class FuncCallNode : public ExprNode {
private:
    string func_name;
    vector<ExprNode*> arguments;

public:
    FuncCallNode(string name, string result_type)
        : ExprNode(std::move(result_type)), func_name(std::move(name)) {}

    ~FuncCallNode() override {
        for (ExprNode* arg : arguments) delete arg;
    }

    void add_argument(ExprNode* arg) {
        if (arg) arguments.push_back(arg);
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        for (ExprNode* arg : arguments) {
            string arg_code = arg ? arg->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
            outcode << "param " << arg_code << endl;
        }

        string temp_var = "t" + to_string(temp_count++);
        outcode << temp_var << " = call " << func_name << ", " << arguments.size() << endl;
        return temp_var;
    }

    ExprNode* clone() const override {
        FuncCallNode* copy = new FuncCallNode(func_name, node_type);
        for (ExprNode* arg : arguments) {
            copy->add_argument(arg ? arg->clone() : nullptr);
        }
        return copy;
    }
};

class FuncDeclNode : public ASTNode {
private:
    string return_type;
    string name;
    vector<pair<string, string>> params;
    BlockNode* body;

public:
    FuncDeclNode(string ret_type, string n) : return_type(std::move(ret_type)), name(std::move(n)), body(nullptr) {}
    ~FuncDeclNode() override { delete body; }

    void add_param(string type, string param_name) {
        params.emplace_back(std::move(type), std::move(param_name));
    }

    void set_body(BlockNode* b) {
        body = b;
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        outcode << "\n// Function: " << return_type << " " << name << "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) outcode << ", ";
            outcode << params[i].first << " " << params[i].second;
        }
        outcode << ")" << endl;
        outcode << name << ":" << endl;
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << endl;
        return "";
    }
};

class ProgramNode : public ASTNode {
private:
    vector<ASTNode*> units;

public:
    ~ProgramNode() override {
        for (ASTNode* unit : units) delete unit;
    }

    void add_unit(ASTNode* unit) {
        if (unit) units.push_back(unit);
    }

    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const override {
        outcode << "// Three Address Code" << endl << endl;
        for (ASTNode* unit : units) {
            if (unit) unit->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
};

#endif // AST_H