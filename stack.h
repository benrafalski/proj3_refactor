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
} StackNode;

class Stack
{
private:
    vector<StackNode> stack;

public:
    StackNode top();
    StackNode terminal_peek();
    StackNode pop();
    void push(StackNode&);
    // StackNode reduce(vector<StackNode> stk, vector<string> rhs);
};

