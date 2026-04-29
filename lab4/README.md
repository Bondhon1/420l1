# Lab 4: Intermediate Code Generation - Comprehensive Documentation

## Table of Contents
1. [Introduction](#introduction)
2. [Assignment Overview](#assignment-overview)
3. [Task 2.1: Abstract Syntax Tree (AST) Implementation](#task-21-abstract-syntax-tree-ast-implementation)
4. [Task 2.2: AST Construction During Parsing](#task-22-ast-construction-during-parsing)
5. [Task 2.3: Three-Address Code Generation](#task-23-three-address-code-generation)
6. [Files Structure and Purposes](#files-structure-and-purposes)
7. [Key Data Structures](#key-data-structures)
8. [Compilation and Execution](#compilation-and-execution)
9. [Output Files](#output-files)
10. [Example Walkthrough](#example-walkthrough)

---

## Introduction

Lab 4 is the final phase of building a two-pass compiler for a subset of the C programming language. This phase focuses on:

- **Intermediate Code Generation (ICG)**: Converting high-level C code into three-address code (3AC)
- **Abstract Syntax Tree (AST)**: Building a tree representation of the parsed program
- **Code Traversal**: Generating intermediate code by traversing the AST

The purpose of intermediate code is to:
- Bridge the gap between high-level source code and low-level machine code
- Enable code optimizations
- Support multiple target architectures
- Simplify the final code generation phase

---

## Assignment Overview

### What is Three-Address Code?

Three-Address Code is an intermediate representation where:
- Each instruction has at most **3 operands**
- Instructions follow the format: `result = operand1 OP operand2`
- Temporary variables are used: `t0, t1, t2, ...`
- Labels are used for control flow: `L0, L1, L2, ...`

### Example Conversions:

**Input C Code:**
```c
int a = 5;
int b = 10;
int c = a + b * 2;
if (c > 20) {
    printf("Large");
}
```

**Three-Address Code:**
```
t0 = b * 2       // Multiplication happens first
t1 = a + t0      // Then addition
c = t1           // Assignment
t2 = c > 20      // Comparison
if t2 goto L0    // Conditional jump
goto L1          // Unconditional jump
L0:              // Label for true branch
// printf code here
L1:              // Label for false branch
```

---

## Task 2.1: Abstract Syntax Tree (AST) Implementation

### Overview

The AST is a hierarchical tree structure that represents the syntactic structure of the source code. Each node in the tree represents a construct in the source program.

### AST Node Class Hierarchy

```
ASTNode (Abstract Base Class)
├── ExprNode (Expression Nodes)
│   ├── VarNode
│   ├── ConstNode
│   ├── BinaryOpNode
│   ├── UnaryOpNode
│   ├── AssignNode
│   └── FuncCallNode
├── StmtNode (Statement Nodes)
│   ├── ExprStmtNode
│   ├── BlockNode
│   ├── IfNode
│   ├── WhileNode
│   ├── ForNode
│   ├── ReturnNode
│   ├── BreakNode
│   ├── ContinueNode
│   └── DeclNode
└── ProgramNode (Root Node)
```

### Detailed AST Node Classes

#### 1. **VarNode** - Variable and Array References

**Purpose:** Represents variable references or array accesses

**Code:**
```cpp
class VarNode : public ExprNode {
private:
    string name;           // Variable name
    ExprNode* index;       // Index expression (for arrays)
public:
    VarNode(string var_name, string type, ExprNode* idx = nullptr)
    bool has_index() const;
    string get_name() const;
    string generate_code(...) const override;
};
```

**Generated Code Examples:**
- Simple variable: `x` → outputs `"x"`
- Array access: `arr[i]` → outputs `"arr[index_result]"` where `index_result` is the evaluated index

---

#### 2. **ConstNode** - Constants

**Purpose:** Represents constant values (integers and floats)

**Code:**
```cpp
class ConstNode : public ExprNode {
private:
    string value;          // Constant value
public:
    ConstNode(string val, string type)
    string generate_code(...) const override;
};
```

**Generated Code Examples:**
- Integer constant: `5` → outputs `"5"`
- Float constant: `3.14` → outputs `"3.14"`

---

#### 3. **BinaryOpNode** - Binary Operations

**Purpose:** Represents operations with two operands

**Supported Operations:**
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Logical: `&&`, `||`
- Relational: `<`, `>`, `==`, `!=`, `<=`, `>=`

**Code:**
```cpp
class BinaryOpNode : public ExprNode {
private:
    string op;             // Operator symbol
    ExprNode* left;        // Left operand
    ExprNode* right;       // Right operand
public:
    BinaryOpNode(string operation, ExprNode* lhs, ExprNode* rhs, string result_type)
    string generate_code(...) const override;
};
```

**Generated Code Pattern:**
```
t_n = left_result OP right_result
```

**Example:**
- Expression: `a + b` 
- Generated: `t0 = a + b`

---

#### 4. **UnaryOpNode** - Unary Operations

**Purpose:** Represents operations with one operand

**Supported Operations:**
- Negation: `-expr`
- Logical NOT: `!expr`
- Positive: `+expr` (identity)

**Code:**
```cpp
class UnaryOpNode : public ExprNode {
private:
    string op;             // Operator symbol
    ExprNode* operand;     // Operand expression
public:
    UnaryOpNode(string operation, ExprNode* expr, string result_type)
    string generate_code(...) const override;
};
```

**Generated Code Pattern:**
```
t_n = OP operand_result
```

**Example:**
- Expression: `-x` 
- Generated: `t0 = -x`

---

#### 5. **AssignNode** - Assignment Operations

**Purpose:** Represents variable assignments

**Code:**
```cpp
class AssignNode : public ExprNode {
private:
    ExprNode* left;        // Left-hand side (variable)
    ExprNode* right;       // Right-hand side (expression)
public:
    AssignNode(ExprNode* lhs, ExprNode* rhs, string result_type)
    string generate_code(...) const override;
};
```

**Generated Code Pattern:**
```
variable = expression_result
```

**Example:**
- Assignment: `x = a + b` 
- Generated:
  ```
  t0 = a + b
  x = t0
  ```

---

#### 6. **ExprStmtNode** - Expression Statements

**Purpose:** Wraps an expression to make it a statement

**Code:**
```cpp
class ExprStmtNode : public StmtNode {
private:
    ExprNode* expr;        // Expression to execute
public:
    ExprStmtNode(ExprNode* e)
    string generate_code(...) const override;
};
```

**Purpose:** Allows expressions to be used as statements (e.g., function calls, assignments)

---

#### 7. **BlockNode** - Code Blocks

**Purpose:** Represents compound statements or blocks of code

**Code:**
```cpp
class BlockNode : public StmtNode {
private:
    vector<StmtNode*> statements;  // List of statements
public:
    BlockNode()
    void add_statement(StmtNode* stmt)
    string generate_code(...) const override;
};
```

**Generated Code:** Generates code for each statement in sequence

---

#### 8. **IfNode** - If-Else Statements

**Purpose:** Represents conditional branching

**Code:**
```cpp
class IfNode : public StmtNode {
private:
    ExprNode* condition;   // Condition expression
    StmtNode* then_stmt;   // Statement when true
    StmtNode* else_stmt;   // Statement when false (optional)
public:
    IfNode(ExprNode* cond, StmtNode* then_s, StmtNode* else_s = nullptr)
    string generate_code(...) const override;
};
```

**Generated Code Pattern:**
```
condition_result = evaluate_condition
if condition_result goto L_then
goto L_else
L_then:
  // then_stmt code
  goto L_end
L_else:
  // else_stmt code (if present)
L_end:
```

---

#### 9. **WhileNode** - While Loops

**Purpose:** Represents while loop constructs

**Code:**
```cpp
class WhileNode : public StmtNode {
private:
    ExprNode* condition;   // Loop condition
    StmtNode* body;        // Loop body
public:
    WhileNode(ExprNode* cond, StmtNode* loop_body)
    string generate_code(...) const override;
};
```

**Generated Code Pattern:**
```
L_start:
condition_result = evaluate_condition
if condition_result goto L_body
goto L_end
L_body:
  // body code
  goto L_start
L_end:
```

---

#### 10. **ForNode** - For Loops

**Purpose:** Represents for loop constructs

**Code:**
```cpp
class ForNode : public StmtNode {
private:
    ExprNode* init;        // Initialization expression
    ExprNode* condition;   // Loop condition
    ExprNode* increment;   // Increment expression
    StmtNode* body;        // Loop body
public:
    ForNode(ExprNode* i, ExprNode* c, ExprNode* inc, StmtNode* b)
    string generate_code(...) const override;
};
```

**Generated Code Pattern:**
```
// initialization
init_code
L_start:
condition_result = evaluate_condition
if condition_result goto L_body
goto L_end
L_body:
  // body code
  // increment
  increment_code
  goto L_start
L_end:
```

---

#### 11. **ReturnNode** - Return Statements

**Purpose:** Represents function return statements

**Code:**
```cpp
class ReturnNode : public StmtNode {
private:
    ExprNode* expr;        // Expression to return (can be null)
public:
    ReturnNode(ExprNode* e = nullptr)
    string generate_code(...) const override;
};
```

**Generated Code:**
```
return expression_result
```

---

#### 12. **BreakNode** - Break Statements

**Purpose:** Represents break statement for exiting loops

**Generated Code:**
```
goto L_break  // Jump to end of loop
```

---

#### 13. **ContinueNode** - Continue Statements

**Purpose:** Represents continue statement for loop continuation

**Generated Code:**
```
goto L_start  // Jump to loop start
```

---

#### 14. **DeclNode** - Variable Declarations

**Purpose:** Represents variable declarations with optional initialization

**Code:**
```cpp
class DeclNode : public StmtNode {
private:
    string var_name;       // Variable name
    string var_type;       // Data type
    ExprNode* init_expr;   // Initialization expression (optional)
public:
    DeclNode(string name, string type, ExprNode* init = nullptr)
    string generate_code(...) const override;
};
```

**Generated Code:**
- Declaration only: `// Declaration: int x`
- With initialization: 
  ```
  t0 = initial_value
  x = t0
  ```

---

#### 15. **FuncCallNode** - Function Calls

**Purpose:** Represents function call expressions

**Code:**
```cpp
class FuncCallNode : public ExprNode {
private:
    string func_name;      // Function name
    vector<ExprNode*> args; // Function arguments
public:
    FuncCallNode(string name, vector<ExprNode*> a, string result_type)
    string generate_code(...) const override;
};
```

**Generated Code Pattern:**
```
// Evaluate each argument
arg_result0 = evaluate_arg0
arg_result1 = evaluate_arg1
...
t_n = call func_name(arg_result0, arg_result1, ...)
```

---

#### 16. **ProgramNode** - Program Root

**Purpose:** Root node representing the entire program

**Code:**
```cpp
class ProgramNode : public ASTNode {
private:
    vector<ASTNode*> units;  // Program units (declarations, functions, etc.)
public:
    ProgramNode()
    void add_unit(ASTNode* unit)
    string generate_code(...) const override;
};
```

**Generated Code:** Generates code for each unit in the program

---

## Task 2.2: AST Construction During Parsing

### How AST Construction Works

During parsing, the Yacc parser builds AST nodes and attaches them to symbol_info objects. Each grammar rule creates the appropriate AST node type.

### Symbol Info Extension for AST

The `symbol_info` class has been extended with AST support:

```cpp
class symbol_info {
private:
    // ... existing members ...
    ASTNode* ast_node;  // Pointer to corresponding AST node
public:
    ASTNode* get_ast_node() { return ast_node; }
    void set_ast_node(ASTNode* node) { ast_node = node; }
};
```

### Grammar Rule Modifications

#### Example 1: Expression Rule

**Before (No AST):**
```yacc
expression : logic_expression
    { $$ = $1; }
    | variable ASSIGNOP logic_expression
    { $$ = new symbol_info(...); }
    ;
```

**After (With AST):**
```yacc
expression : logic_expression
    {
        $$ = $1;  // Pass symbol info
        // AST node already in $1
    }
    | variable ASSIGNOP logic_expression
    {
        // Create assignment AST node
        AssignNode* assignNode = new AssignNode(
            (VarNode*)$1->get_ast_node(),
            (ExprNode*)$3->get_ast_node(),
            $3->getvartype()
        );
        
        $$ = new symbol_info($1->getname() + "=" + $3->getname(), "assignment");
        $$->set_ast_node(assignNode);  // Attach AST node
    }
    ;
```

#### Example 2: Binary Operation Rule

```yacc
additive_expression : multiplicative_expression
    { $$ = $1; }
    | additive_expression ADDOP multiplicative_expression
    {
        // Create binary operation AST node
        BinaryOpNode* binNode = new BinaryOpNode(
            $2->getname(),  // Operator (+, -)
            (ExprNode*)$1->get_ast_node(),  // Left operand
            (ExprNode*)$3->get_ast_node(),  // Right operand
            $3->getvartype()  // Result type
        );
        
        $$ = new symbol_info($1->getname() + $2->getname() + $3->getname(), "additive");
        $$->set_ast_node(binNode);
    }
    ;
```

#### Example 3: Statement Rule

```yacc
statement : expression SEMICOLON
    {
        // Create expression statement node
        ExprStmtNode* exprStmt = new ExprStmtNode(
            (ExprNode*)$1->get_ast_node()
        );
        
        $$ = new symbol_info($1->getname(), "statement");
        $$->set_ast_node(exprStmt);
    }
    | compound_statement
    { $$ = $1; }
    ;
```

#### Example 4: If Statement Rule

```yacc
statement : IF LPAREN expression RPAREN statement
    {
        // Create if node (without else)
        IfNode* ifNode = new IfNode(
            (ExprNode*)$3->get_ast_node(),  // Condition
            (StmtNode*)$5->get_ast_node(),  // Then statement
            nullptr  // No else statement
        );
        
        $$ = new symbol_info("if statement", "statement");
        $$->set_ast_node(ifNode);
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    {
        // Create if node (with else)
        IfNode* ifNode = new IfNode(
            (ExprNode*)$3->get_ast_node(),  // Condition
            (StmtNode*)$5->get_ast_node(),  // Then statement
            (StmtNode*)$7->get_ast_node()   // Else statement
        );
        
        $$ = new symbol_info("if-else statement", "statement");
        $$->set_ast_node(ifNode);
    }
    ;
```

### Key Points

1. **Each grammar rule creates appropriate AST nodes**
2. **Child nodes are obtained via `get_ast_node()` from child symbols**
3. **AST nodes are attached using `set_ast_node()`**
4. **The root AST is obtained from the `start` rule's result**

---

## Task 2.3: Three-Address Code Generation

### Overview

Three-address code generation traverses the AST and generates intermediate code. Each AST node's `generate_code()` method:

1. **Recursively generates code for child nodes**
2. **Creates temporary variables as needed**
3. **Outputs instructions to the code.txt file**
4. **Returns the result identifier (variable/temporary/constant)**

### Code Generation Process

#### State Variables

```cpp
class ThreeAddrCodeGenerator {
private:
    ProgramNode* ast_root;           // Root of AST
    ofstream& outcode;               // Output file stream
    map<string, string> symbol_to_temp;  // Symbol to temporary mapping
    int temp_count;                  // Counter for temporary variables
    int label_count;                 // Counter for labels
};
```

### Generation Methods for Each Node

#### 1. VarNode - Variable Reference

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    if (!index) 
        return name;  // Simple variable
    
    // Array access: generate index code
    string index_code = index->generate_code(...);
    return name + "[" + index_code + "]";
}
```

**Output:**
- Simple: `x` (no output, returns "x")
- Array: `arr[i]` → generates index, returns `"arr[i_result]"`

---

#### 2. ConstNode - Constant

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    return value;  // Just return the constant
}
```

**Output:** `5` (no output, returns "5")

---

#### 3. BinaryOpNode - Binary Operation

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    string left_code = left->generate_code(...);
    string right_code = right->generate_code(...);
    string temp_var = "t" + to_string(temp_count++);
    outcode << temp_var << " = " << left_code << " " << op << " " << right_code << endl;
    return temp_var;
}
```

**Output Example:**
```
// For expression: a + b * 2
t0 = b * 2      // Right side evaluated first (precedence)
t1 = a + t0     // Then addition
// Returns "t1"
```

---

#### 4. UnaryOpNode - Unary Operation

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    string operand_code = operand->generate_code(...);
    string temp_var = "t" + to_string(temp_count++);
    outcode << temp_var << " = " << op << operand_code << endl;
    return temp_var;
}
```

**Output Example:**
```
// For expression: -x
t0 = -x
// Returns "t0"
```

---

#### 5. AssignNode - Assignment

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    string right_code = right->generate_code(...);
    string left_name = ((VarNode*)left)->get_name();
    outcode << left_name << " = " << right_code << endl;
    return left_name;
}
```

**Output Example:**
```
// For assignment: x = a + b
t0 = a + b
x = t0
// Returns "x"
```

---

#### 6. ExprStmtNode - Expression Statement

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    return expr->generate_code(...);  // Just delegate
}
```

**Output:** Same as the wrapped expression

---

#### 7. BlockNode - Code Block

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    for (auto stmt : statements) {
        stmt->generate_code(...);
    }
    return "";
}
```

**Output:** All statements in the block, one after another

---

#### 8. IfNode - If-Else Statement

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    string cond_result = condition->generate_code(...);
    int label_then = label_count++;
    int label_else = label_count++;
    int label_end = label_count++;
    
    outcode << "if " << cond_result << " goto L" << label_then << endl;
    outcode << "goto L" << label_else << endl;
    
    outcode << "L" << label_then << ":" << endl;
    then_stmt->generate_code(...);
    outcode << "goto L" << label_end << endl;
    
    outcode << "L" << label_else << ":" << endl;
    if (else_stmt) {
        else_stmt->generate_code(...);
    }
    
    outcode << "L" << label_end << ":" << endl;
    return "";
}
```

**Output Example:**
```
t0 = x > 5       // Condition
if t0 goto L0    // Jump if true
goto L1          // Jump if false
L0:              // True branch
// then code
goto L2          // Skip else
L1:              // False branch
// else code
L2:              // End
```

---

#### 9. WhileNode - While Loop

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    int label_start = label_count++;
    int label_body = label_count++;
    int label_end = label_count++;
    
    outcode << "L" << label_start << ":" << endl;
    string cond_result = condition->generate_code(...);
    outcode << "if " << cond_result << " goto L" << label_body << endl;
    outcode << "goto L" << label_end << endl;
    
    outcode << "L" << label_body << ":" << endl;
    body->generate_code(...);
    outcode << "goto L" << label_start << endl;
    
    outcode << "L" << label_end << ":" << endl;
    return "";
}
```

**Output Example:**
```
L0:           // Loop start
t0 = i < 10   // Condition
if t0 goto L1 // Jump if true
goto L2       // Exit loop
L1:           // Loop body
// body code
goto L0       // Jump back
L2:           // End
```

---

#### 10. ForNode - For Loop

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    // Generate initialization
    init->generate_code(...);
    
    int label_start = label_count++;
    int label_body = label_count++;
    int label_end = label_count++;
    
    outcode << "L" << label_start << ":" << endl;
    string cond_result = condition->generate_code(...);
    outcode << "if " << cond_result << " goto L" << label_body << endl;
    outcode << "goto L" << label_end << endl;
    
    outcode << "L" << label_body << ":" << endl;
    body->generate_code(...);
    increment->generate_code(...);  // Increment
    outcode << "goto L" << label_start << endl;
    
    outcode << "L" << label_end << ":" << endl;
    return "";
}
```

**Output Example:**
```
i = 0           // Initialization
L0:             // Loop start
t0 = i < 10     // Condition
if t0 goto L1   // Jump if true
goto L2         // Exit
L1:             // Body
// loop body
t1 = i + 1      // Increment
i = t1
goto L0         // Repeat
L2:             // End
```

---

#### 11. ReturnNode - Return Statement

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    if (expr) {
        string expr_result = expr->generate_code(...);
        outcode << "return " << expr_result << endl;
    } else {
        outcode << "return" << endl;
    }
    return "";
}
```

**Output:**
```
return expression_result
```

---

#### 12. DeclNode - Variable Declaration

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    outcode << "// Declaration: " << var_type << " " << var_name << endl;
    if (init_expr) {
        string init_result = init_expr->generate_code(...);
        string temp_var = "t" + to_string(temp_count++);
        outcode << temp_var << " = " << init_result << endl;
        outcode << var_name << " = " << temp_var << endl;
    }
    return "";
}
```

**Output:**
```
// Declaration: int x
```
or with initialization:
```
// Declaration: int x = 5
t0 = 5
x = t0
```

---

#### 13. ProgramNode - Program Root

**generate_code() Logic:**
```cpp
string generate_code(...) const override {
    outcode << "//========== THREE ADDRESS CODE ==========\n\n";
    outcode << "// Format: t0, t1 = temps; L0, L1 = labels\n\n";
    
    for (auto unit : units) {
        unit->generate_code(...);
    }
    
    outcode << "\n//========== END OF CODE ==========\n";
    return "";
}
```

---

### ThreeAddrCodeGenerator Class

**Purpose:** Main class responsible for orchestrating three-address code generation

**Key Methods:**

```cpp
class ThreeAddrCodeGenerator {
public:
    // Constructor
    ThreeAddrCodeGenerator(ProgramNode* root, ofstream& out)
        : ast_root(root), outcode(out), temp_count(0), label_count(0) {}
    
    // Main generation method
    void generate() {
        if (!ast_root) {
            outcode << "// Error: No AST root" << endl;
            return;
        }
        // Output header
        outcode << "//========== THREE ADDRESS CODE ==========\n\n";
        outcode << "// Format: \n";
        outcode << "// - t0, t1, etc. are temporary variables\n";
        outcode << "// - L0, L1, etc. are labels for jumps\n";
        outcode << "// - Operations follow the three-address code format\n\n";
        
        // Generate code from AST
        ast_root->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        
        outcode << "\n//========== END OF CODE ==========\n";
    }
};
```

---

## Files Structure and Purposes

### Main Files

#### 1. **ast.h**
- **Purpose:** Header file containing all AST node class definitions
- **Content:**
  - Base `ASTNode` class
  - `ExprNode` classes: VarNode, ConstNode, BinaryOpNode, UnaryOpNode, AssignNode, FuncCallNode
  - `StmtNode` classes: ExprStmtNode, BlockNode, IfNode, WhileNode, ForNode, ReturnNode, BreakNode, ContinueNode, DeclNode
  - `ProgramNode` root class
  - `clone()` methods for deep copying nodes
  - `generate_code()` abstract method implementations

#### 2. **three_addr_code.h**
- **Purpose:** Header file for three-address code generation
- **Content:**
  - `ThreeAddrCodeGenerator` class
  - State variables: `temp_count`, `label_count`, `symbol_to_temp`
  - `generate()` method to orchestrate code generation

#### 3. **lex_analyzer.l**
- **Purpose:** Flex lexer specification
- **Content:**
  - Token definitions for C keywords, operators, identifiers, constants
  - Regex patterns for tokenization
  - Integration with Yacc parser

#### 4. **syntax_analyzer.y**
- **Purpose:** Yacc parser specification
- **Content:**
  - Grammar rules for C subset
  - AST node construction in action parts
  - Symbol table management
  - Variable declarations for global parsing context

#### 5. **symbol_info.h**
- **Purpose:** Information container for symbols during parsing
- **Content:**
  - Symbol name and type
  - Variable type (int, float, void, etc.)
  - Array size information
  - Parameter lists for functions
  - AST node pointer: `ast_node`
  - Methods to get/set AST nodes

#### 6. **symbol_table.h**
- **Purpose:** Symbol table management
- **Content:**
  - `symbol_table` class for scope management
  - Methods: `enter_scope()`, `exit_scope()`, `Insert_in_table()`, `Lookup_in_table()`
  - Manages nested scopes for variable declarations

#### 7. **scope_table.h**
- **Purpose:** Individual scope table implementation
- **Content:**
  - Hash table for symbols in a specific scope
  - Scope stack for nested scopes
  - Lookup operations with scope chaining

#### 8. **script.sh**
- **Purpose:** Build and execution script
- **Content:**
  - Yacc compilation: `yacc -d -y --debug --verbose syntax_analyzer.y`
  - Flex compilation: `flex lex_analyzer.l`
  - GCC compilation: `g++ -w -c y.tab.c` and `g++ -w -c lex.yy.c`
  - Linking: `g++ y.o l.o -o two_pass_compiler`
  - Execution: `./two_pass_compiler input.c`

---

## Key Data Structures

### 1. Symbol Info Structure

```cpp
class symbol_info {
    string sym_name;              // Symbol name
    string sym_type;              // Token type
    string ID_type;               // var, array, func_dec, func_def
    string var_type;              // int, float, void, char, double
    int array_size;               // For array variables
    vector<string> param_list;    // Parameter types for functions
    vector<string> param_name;    // Parameter names
    ASTNode* ast_node;            // Attached AST node
};
```

### 2. AST Node Hierarchy

```
ASTNode (pure virtual)
├── ExprNode (expressions that return values)
│   ├── VarNode
│   ├── ConstNode
│   ├── BinaryOpNode
│   ├── UnaryOpNode
│   ├── AssignNode
│   └── FuncCallNode
├── StmtNode (statements)
│   ├── ExprStmtNode
│   ├── BlockNode
│   ├── IfNode
│   ├── WhileNode
│   ├── ForNode
│   ├── ReturnNode
│   ├── BreakNode
│   ├── ContinueNode
│   └── DeclNode
└── ProgramNode (root, contains all units)
```

### 3. Code Generation Context

```cpp
struct CodeGenContext {
    ofstream& outcode;                  // Output file
    map<string, string> symbol_to_temp; // Symbol to temp mapping
    int& temp_count;                    // Current temp variable count
    int& label_count;                   // Current label count
};
```

---

## Compilation and Execution

### Build Process

1. **Generate Parser with Yacc:**
   ```bash
   yacc -d -y --debug --verbose syntax_analyzer.y
   ```
   Outputs:
   - `y.tab.c`: C parser source
   - `y.tab.h`: Parser header with token definitions

2. **Generate Scanner with Flex:**
   ```bash
   flex lex_analyzer.l
   ```
   Outputs:
   - `lex.yy.c`: C lexer source

3. **Compile Parser Object:**
   ```bash
   g++ -fpermissive -w -c -o y.o y.tab.c
   ```

4. **Compile Lexer Object:**
   ```bash
   g++ -w -c -o l.o lex.yy.c
   ```

5. **Link into Executable:**
   ```bash
   g++ y.o l.o -o two_pass_compiler
   ```

### Execution

```bash
./two_pass_compiler input.c
```

This produces:
- `log.txt`: Parsing trace and symbol table
- `error.txt`: Syntax and semantic errors
- `code.txt`: Generated three-address code

### Running the Build Script

```bash
bash script.sh
```

---

## Output Files

### 1. **log.txt** - Parsing Log

**Purpose:** Records the parsing process and symbol table

**Content:**
```
At line no: 1 start : program 

At line no: 1 program : unit 

Symbol Table

################################

Scope Table ID: 1

...symbol entries...

################################

Line count: 25
Error count: 0
```

**Key Information:**
- Grammar rules matched
- Source code segments
- Complete symbol table with all declarations
- Scope information
- Line and error counts

### 2. **error.txt** - Error Report

**Purpose:** Lists all compilation errors found

**Content:**
```
At line 5: Undeclared variable 'x'
At line 10: Type mismatch in assignment
Error count: 2
```

**Possible Errors:**
- Undeclared variables
- Type mismatches
- Duplicate declarations
- Missing semicolons
- Syntax errors

### 3. **code.txt** - Three-Address Code

**Purpose:** Contains generated three-address code

**Content:**
```
//========== THREE ADDRESS CODE ==========

// This code was generated by a two-pass compiler
// Format: 
// - t0, t1, etc. are temporary variables
// - L0, L1, etc. are labels for jumps
// - Operations follow the three-address code format

// Three Address Code

// Declaration: int x
// Declaration: int y
t0 = 5
x = t0
t1 = 10
y = t1
t2 = x + y
t3 = t2 * 2
// if statement
t4 = t3 > 20
if t4 goto L0
goto L1
L0:
// then part
goto L2
L1:
// else part
L2:

//========== END OF CODE ==========
```

---

## Example Walkthrough

### Input C Code (input.c)

```c
int main() {
    int a = 5;
    int b = 10;
    int c;
    c = a + b;
    
    if (c > 10) {
        c = c * 2;
    } else {
        c = c / 2;
    }
    
    while (a < c) {
        a = a + 1;
    }
    
    return c;
}
```

### Parsing Phase

The parser reads tokens from the lexer and applies grammar rules:

1. **Function Declaration:** `int main() { ... }`
   - Creates `FunctionDeclNode`

2. **Variable Declarations:**
   - `int a = 5;` → `DeclNode(a, int, ConstNode(5))`
   - `int b = 10;` → `DeclNode(b, int, ConstNode(10))`
   - `int c;` → `DeclNode(c, int, null)`

3. **Assignment:** `c = a + b;`
   - Creates `AssignNode(VarNode(c), BinaryOpNode(+, VarNode(a), VarNode(b)))`

4. **If Statement:** 
   - Condition: `c > 10` → `BinaryOpNode(>, VarNode(c), ConstNode(10))`
   - Then: `c = c * 2` → `AssignNode`
   - Else: `c = c / 2` → `AssignNode`

5. **While Loop:**
   - Condition: `a < c` → `BinaryOpNode(<, VarNode(a), VarNode(c))`
   - Body: `a = a + 1` → `AssignNode`

6. **Return:** `return c;` → `ReturnNode(VarNode(c))`

### AST Structure

```
ProgramNode
└── FunctionDeclNode(main)
    └── BlockNode
        ├── DeclNode(a, int, 5)
        ├── DeclNode(b, int, 10)
        ├── DeclNode(c, int)
        ├── AssignNode
        │   ├── VarNode(c)
        │   └── BinaryOpNode(+)
        │       ├── VarNode(a)
        │       └── VarNode(b)
        ├── IfNode
        │   ├── BinaryOpNode(>)
        │   │   ├── VarNode(c)
        │   │   └── ConstNode(10)
        │   ├── BlockNode (then)
        │   │   └── AssignNode(c = c * 2)
        │   └── BlockNode (else)
        │       └── AssignNode(c = c / 2)
        ├── WhileNode
        │   ├── BinaryOpNode(<)
        │   │   ├── VarNode(a)
        │   │   └── VarNode(c)
        │   └── BlockNode (body)
        │       └── AssignNode(a = a + 1)
        └── ReturnNode(VarNode(c))
```

### Code Generation Phase

Traversing the AST and generating code:

```
// Declaration: int a
t0 = 5
a = t0

// Declaration: int b
t1 = 10
b = t1

// Declaration: int c

// Assignment: c = a + b
t2 = a + b
c = t2

// If statement
t3 = c > 10
if t3 goto L0
goto L1
L0:
// c = c * 2
t4 = c * 2
c = t4
goto L2
L1:
// c = c / 2
t5 = c / 2
c = t5
L2:

// While loop
L3:
t6 = a < c
if t6 goto L4
goto L5
L4:
// a = a + 1
t7 = a + 1
a = t7
goto L3
L5:

// Return
return c

//========== END OF CODE ==========
```

---

## Summary

Lab 4 implements a complete **Intermediate Code Generation** compiler phase:

### Key Achievements:

1. ✅ **16 AST Node Types** - Complete representation of C subset
2. ✅ **AST Construction** - During parsing, creating tree structure
3. ✅ **Three-Address Code** - Generating intermediate code for all language constructs
4. ✅ **Proper Output Format** - Three output files with all required information
5. ✅ **Complete Symbol Management** - Scope-aware symbol table
6. ✅ **Error Handling** - Syntax and semantic error reporting

### What's Generated:

- **log.txt**: Parse tree and symbol table information
- **error.txt**: All compilation errors found
- **code.txt**: Three-address code ready for next compilation phase

This compiler foundation can now be extended to:
- Code optimization
- Machine code generation
- Target-specific code generation
- Performance analysis

