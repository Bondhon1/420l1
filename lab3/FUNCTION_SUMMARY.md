# Lab 3 Function Summary (Non-Lexical, Non-Grammar)

This summary covers unique functions/methods implemented in the semantic/parser support code and symbol table classes.
It intentionally excludes lexical analyzer internals and grammar production explanations.

## 1) Parser Support Functions in `22101459_22101458.y`

| Function | Line | Purpose |
|---|---:|---|
| `report_error(const string &message)` | 43 | Central error logger. Writes formatted error with current line number to error log and increments `error_count`. |
| `yyerror(char *s)` | 49 | Bison parser error hook; forwards parser error text to `report_error`. |
| `is_int_type(const string &type)` | 54 | Returns `true` if semantic type is `int`. |
| `is_float_type(const string &type)` | 59 | Returns `true` if semantic type is `float`. |
| `is_void_type(const string &type)` | 64 | Returns `true` if semantic type is `void`. |
| `is_error_type(const string &type)` | 69 | Returns `true` for invalid/unknown semantic type (`""` or `"error"`). |
| `make_node(const string &name, const string &grammar_type, const string &data_type = "")` | 74 | Allocates a `symbol_info` node for parse tree/semantic propagation and sets semantic data type. |
| `node_type(symbol_info *node)` | 81 | Safe semantic type accessor. Returns `"error"` for null/empty type nodes. |
| `declare_variable(const string &name, const string &type, bool is_array, int array_size, bool is_parameter)` | 89 | Declares variable/array in current scope. Rejects `void` variables, detects duplicate declarations, records kind/type/size metadata. |
| `declare_function(const string &name, const string &return_type, const vector<string> &param_types, const string &param_details)` | 111 | Declares function in symbol table with return type, parameter count/types/details; reports duplicate declarations. |
| `insert_pending_parameters()` | 126 | Inserts collected parameter identifiers into current scope once function body scope is entered; skips unnamed and duplicate parameter names. |
| `get_pending_param_types()` | 137 | Extracts parameter type list from pending parameters for function symbol insertion. |
| `check_function_call(symbol_info *function_symbol, symbol_info *arguments, const string &function_name)` | 146 | Validates function call argument count and per-position type compatibility against function signature. |
| `check_division_or_modulus_zero(const string &operation, symbol_info *right_node)` | 164 | Reports compile-time detectable division/modulus by zero for literal right operand values. |
| `main(int argc, char *argv[])` | 782 | Program entry: validates args, opens files, initializes symbol table/root scope, runs parse, writes totals, closes resources. |

## 2) Scope Table Class Methods in `scope_table.h`

| Method | Line | Purpose |
|---|---:|---|
| `hash_function(const string &name)` | 12 | Computes bucket index (sum-of-ASCII mod bucket count). |
| `scope_table()` | 19 | Default constructor; creates scope with default bucket count and no parent. |
| `scope_table(int bucket_count, int unique_id, scope_table *parent_scope)` | 23 | Constructor for nested scopes with explicit id and parent pointer. |
| `get_parent_scope()` | 28 | Returns parent scope pointer for scope-chain traversal. |
| `get_unique_id()` | 29 | Returns current scope id. |
| `lookup_in_scope(const string &name)` | 31 | Finds symbol by name only in this scope (no parent traversal). |
| `lookup_in_scope(symbol_info *symbol)` | 39 | Overload: lookup by symbol object name. |
| `insert_in_scope(symbol_info *symbol)` | 43 | Inserts symbol into this scope if not already present in same scope. |
| `delete_from_scope(const string &name)` | 50 | Removes/deletes symbol by name from this scope bucket. |
| `delete_from_scope(symbol_info *symbol)` | 63 | Overload: remove by symbol object name. |
| `print_scope_table(ofstream &outlog)` | 67 | Dumps scope content and symbol metadata (variable/array/function details). |
| `~scope_table()` | 94 | Destructor; deletes all symbols stored in this scope. |

## 3) Symbol Table Class Methods in `symbol_table.h`

| Method | Line | Purpose |
|---|---:|---|
| `symbol_table(int bucket_count, ofstream *outlog)` | 12 | Initializes symbol table manager with no active scope yet. |
| `enter_scope()` | 15 | Creates and switches to a new child/current scope and logs creation. |
| `exit_scope()` | 22 | Prints scopes, destroys current scope, restores parent scope, logs removal. |
| `insert(symbol_info *symbol)` | 32 | Inserts symbol into current scope only. |
| `remove(symbol_info *symbol)` | 37 | Removes symbol from current scope only. |
| `lookup(const string &name)` | 42 | Name lookup through current scope then parent chain (nearest match wins). |
| `lookup(symbol_info *symbol)` | 52 | Overload: chain lookup using symbol object name. |
| `print_current_scope()` | 56 | Prints only active scope content. |
| `print_all_scopes(ofstream &outlog)` | 60 | Prints active scope and all ancestors. |
| `~symbol_table()` | 71 | Destructor; deletes remaining scope chain safely. |

## 4) Symbol Metadata Methods in `symbol_info.h`

| Method | Line | Purpose |
|---|---:|---|
| `symbol_info(string name, string type)` | 18 | Constructs symbol with defaults (kind variable, no semantic type/params). |
| `get_name()` | 23 | Returns symbol name. |
| `getname()` | 24 | Alias getter for symbol name (used by parser actions). |
| `get_type()` | 25 | Returns parser token/category type. |
| `set_name(string n)` | 27 | Sets symbol name. |
| `set_type(string t)` | 28 | Sets parser token/category type. |
| `set_symbol_kind(string kind)` | 30 | Sets semantic kind (`VARIABLE`, `ARRAY`, `FUNCTION`). |
| `get_symbol_kind()` | 31 | Returns semantic kind. |
| `set_data_type(string dt)` | 33 | Sets semantic data type (`int`, `float`, `void`, etc.). |
| `get_data_type()` | 34 | Returns semantic data type. |
| `set_array_size(int sz)` | 36 | Stores declared array length. |
| `get_array_size()` | 37 | Returns stored array length. |
| `set_param_count(int count)` | 39 | Stores function parameter count. |
| `get_param_count()` | 40 | Returns parameter count. |
| `set_param_details(string det)` | 42 | Stores textual parameter detail string. |
| `get_param_details()` | 43 | Returns textual parameter detail string. |
| `set_param_types(const vector<string> &types)` | 45 | Stores canonical parameter type list for semantic checks. |
| `get_param_types()` | 46 | Returns parameter type list. |
| `set_arg_types(const vector<string> &types)` | 48 | Stores actual argument type list on expression nodes. |
| `get_arg_types()` | 49 | Returns stored argument type list. |
| `~symbol_info()` | 51 | Destructor (no custom cleanup beyond defaults). |

## 5) Strict Scope-Table-Related Line References

### In `22101459_22101458.y`
- Line 101: `symtab->insert(symbol)` inside variable declaration path.
- Line 120: `symtab->insert(symbol)` inside function declaration path.
- Line 237: duplicate function check via `symtab->lookup(pending_func_name)`.
- Line 264: duplicate function check via `symtab->lookup(pending_func_name)`.
- Line 331: `symtab->enter_scope()` for compound statement start.
- Line 338: `symtab->exit_scope()` for compound statement end.
- Line 343: `symtab->enter_scope()` for empty compound statement start.
- Line 350: `symtab->exit_scope()` for empty compound statement end.
- Line 483: `symtab->lookup($3->getname())` in `PRINTLN` statement semantic check.
- Line 517: `symtab->lookup($1->getname())` in scalar variable use check.
- Line 535: `symtab->lookup($1->getname())` in array variable use check.
- Line 704: `symtab->lookup($1->getname())` in function call semantic check.
- Line 801: initial `symtab->enter_scope()` before parsing.

### In `scope_table.h`
- Lines 12-16: hash index computation.
- Lines 31-37: in-scope lookup by name.
- Lines 43-48: in-scope insert with duplicate prevention.
- Lines 50-61: in-scope deletion by name.
- Lines 67-92: scope printing with symbol category-specific metadata.
- Lines 94-100: scope-owned symbol cleanup in destructor.

### In `symbol_table.h`
- Lines 15-20: new scope creation and activation.
- Lines 22-29: scope exit workflow (print, unlink, destroy).
- Lines 32-34: insertion delegated to current scope.
- Lines 37-39: removal delegated to current scope.
- Lines 42-49: hierarchical lookup across scope chain.
- Lines 56-57: print current scope.
- Lines 60-68: print all active ancestor scopes.
- Lines 71-77: cleanup of entire scope chain.

## 6) Strict Error-Handling-Related Line References

### Core Error Infrastructure (`22101459_22101458.y`)
- Line 43: `report_error` function starts.
- Line 45: writes formatted error message with line number to error log.
- Line 46: increments `error_count`.
- Line 49: `yyerror` function starts.
- Line 51: `yyerror` forwards parser errors to `report_error`.

### All Semantic/Compile-Time Error Reports (`report_error` call sites)
- Line 92: void variable type rejection.
- Line 103: duplicate parameter variable declaration.
- Line 105: duplicate variable declaration.
- Line 121: duplicate function declaration.
- Line 152: function argument count mismatch.
- Line 159: function argument type mismatch.
- Line 170: division by zero.
- Line 174: modulus by zero.
- Line 239: duplicate function definition (parameterized form).
- Line 266: duplicate function definition (no-parameter form).
- Line 286: duplicate parameter name while collecting parameter list.
- Line 360: void variable type rejection from `var_declaration` action.
- Line 485: undeclared variable in `PRINTLN`.
- Line 487: array variable used where scalar expected in `PRINTLN`.
- Line 520: undeclared scalar variable usage.
- Line 523: array variable used as scalar.
- Line 538: undeclared array variable usage.
- Line 540: non-array used with index operator.
- Line 543: non-integer array index.
- Line 564: invalid assignment involving void type.
- Line 566: warning for float-to-int assignment.
- Line 583: logic operation with void operand.
- Line 600: relational operation with void operand.
- Line 619: additive operation with void operand.
- Line 646: multiplicative operation with void operand.
- Line 650: modulus with non-integer operands.
- Line 669: unary plus/minus on void type.
- Line 680: logical NOT on void type.
- Line 707: call to undeclared/non-function identifier.

### Runtime/Execution Guard and Final Error Totals
- Line 784: command-line argument count check (`argc != 2`).
- Line 796: input file open failure message (`Couldn't open file`).
- Line 806: total error count written to parser log.
- Line 807: total error count written to error log.
