#include <string>
#include <iostream>
#include "execute.h"


using namespace std;

typedef enum tnodeType
{
    _ARRAY = 0,
    _SCALAR = 1,
    _ERROR = 2,
    _ARRAYDECL = 3
} tnodeType;

typedef enum Optr
{
    _MINUS = 0,
    _PLUS,
    _MULT,
    _DIV,
    _LPAREN,
    _EXPR,
    _DOT,
    _NOOP
} Optr;

typedef struct TreeNode
{
    TreeNode *right;
    TreeNode *left;
    tnodeType type;
    Optr op;
    string value;
    string rawValue;
    bool wrapped;
    bool is_num;

    instNode * inst;

} TreeNode;

class Tree
{
    public:
    TreeNode *root;
    int line_no;

    void init_root(TreeNode *lhs, TreeNode *rhs, int line);
    int height(TreeNode *node);
    void printCurrentLevel(TreeNode *root, int level);
    void printLevelOrder();
};