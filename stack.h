#include <vector>
#include <string>
#include "lexer.h"
#include "tree.h"

using namespace std;

typedef enum snodeType
{
    EXPR = 0,
    TERM = 1
} snodeType;

typedef struct StackNode
{
    snodeType type;
    TreeNode *expr;
    Token term;

    StackNode() {}

    StackNode(snodeType t, TreeNode *e, Token ter)
    {
        type = t;
        expr = e;
        term = ter;
    }
} StackNode;

class Stack
{
private:
    vector<StackNode> stack;

public:
    StackNode top();
    StackNode terminal_peek();
    StackNode pop();
    void push(StackNode &);

    void init_stack()
    {
        StackNode node;
        // next_index = 0;
        node.type = TERM;
        node.term.lexeme = "$";
        node.term.line_no = -1;
        node.term.token_type = END_OF_FILE;
        push(node);
    };
    // StackNode reduce(vector<StackNode> stk, vector<string> rhs);
};
