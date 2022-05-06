#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include "execute.h"
#include "lexer.h"
#include "typechecker.h"

using namespace std;

typedef enum lType
{
    SCLR = 0,
    ARR = 1
} lType;

typedef struct location
{
    int mem_addr;
    string lexeme;
    lType type;

    location(int m, string l, lType t)
    {
        mem_addr = m;
        lexeme = l;
        type = t;
    };
} location;

class Parser
{
private:
    LexicalAnalyzer lexer;
    Stack stack;

    vector<string> scalars;
    vector<string> arrays;
    vector<Tree> trees;

    TypeChecker checker;

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

    int next_index = 0;
    vector<location> location_table;

    int loc(const string);

    void parse_decl_section();
    vector<string> parse_scalar_decl_section();
    vector<string> parse_array_decl_section();
    vector<string> parse_id_list();
    void parse_block();
    instNode * parse_stmt_list();
    instNode * parse_stmt();
    instNode * parse_assign_stmt();
    instNode * parse_output_stmt();
    TreeNode *parse_variable_access();
    TreeNode *parse_expr();
    void parse_primary();
    Token expect(TokenType expected_type);
    void syntax_error();
    TokenType peek(const int);

    int getIndex(TokenType key);
    bool rule_exists(vector<string> rhs);
    bool var_exists(vector<string> rhs);
    string getRHSLexeme(StackNode s);
    string next_symbol();
    StackNode reduce(vector<StackNode> stk, vector<string> rhs);

public:
    void parse_program(const int);
};