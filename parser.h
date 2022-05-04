#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include "execute.h"
#include "lexer.h"
#include "stack.h"

using namespace std;

class Parser
{
private:
    // ++*(int*)0;
    LexicalAnalyzer lexer;
    Stack stack;
    vector<string> scalars;
    vector<string> arrays;
    vector<Tree> trees;

    vector<int> type_errors;
    vector<int> assign_errors;

    map<string, vector<string>> expr_rules = {
        {"1", {"expr", "MINUS", "expr"}},
        {"2", {"expr", "PLUS", "expr"}},
        {"3", {"expr", "MULT", "expr"}},
        {"4", {"expr", "DIV", "expr"}},
        {"5", {"LPAREN", "expr", "RPAREN"}},
        {"6", {"expr", "LBRAC", "expr", "RBRAC"}},
        {"7", {"expr", "LBRAC", "DOT", "RBRAC"}},
        {"8", {"primary"}}};

    string table[12][12] = {
        // 0    1    2    3    4    5    6    7    8    9    10   11
        {">", ">", "<", "<", "<", ">", "<", "e", ">", "<", "<", ">"}, // 0
        {">", ">", "<", "<", "<", ">", "<", "e", ">", "<", "<", ">"}, // 1
        {">", ">", ">", ">", "<", ">", "<", "e", ">", "<", "<", ">"}, // 2
        {">", ">", ">", ">", "<", ">", "<", "e", ">", "<", "<", ">"}, // 3
        {"<", "<", "<", "<", "<", "=", "<", "e", "<", "<", "<", "e"}, // 4
        {">", ">", ">", ">", "e", ">", ">", "e", ">", "e", "e", ">"}, // 5
        {"<", "<", "<", "<", "<", ">", "<", "=", "=", "<", "<", "e"}, // 6
        {"e", "e", "e", "e", "e", "e", "e", "e", "=", "e", "e", "e"}, // 7
        {">", ">", ">", ">", "e", ">", ">", "e", ">", "e", "e", ">"}, // 8
        {">", ">", ">", ">", "e", ">", ">", "e", ">", "e", "e", ">"}, // 9
        {">", ">", ">", ">", "e", ">", ">", "e", ">", "e", "e", ">"}, // 10
        {"<", "<", "<", "<", "<", "e", "<", "e", "e", "<", "<", "a"}  // 11
    };

    int type_check_expr(TreeNode * node);
    int type_check_assignment(TreeNode *node);
    int type_check_var_access(TreeNode *node);

    void parse_decl_section();
    vector<string> parse_scalar_decl_section();
    vector<string> parse_array_decl_section();
    vector<string> parse_id_list();
    void parse_block();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    void parse_output_stmt();
    TreeNode *parse_variable_access();
    TreeNode *parse_expr();
    void parse_primary();
    Token expect(TokenType expected_type);
    void syntax_error();
    TokenType peek(const int);

    // void init_stack();
    int getIndex(TokenType key);
    bool rule_exists(vector<string> rhs);
    bool var_exists(vector<string> rhs);
    string getRHSLexeme(StackNode s);
    string next_symbol();
    StackNode reduce(vector<StackNode> stk, vector<string> rhs);

public:
    void parse_program();
};