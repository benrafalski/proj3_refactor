#include <iostream>
#include "stack.h"
#include <vector>
#include <string>
#include "helperfuns.h"
using namespace std;

class TypeChecker
{
private:
    vector<string> scalars;
    vector<string> arrays;
    vector<int> type_errors;
    vector<int> assign_errors;

    int type_check_expr(TreeNode *node);
    int type_check_assignment(TreeNode *node);
    int type_check_var_access(TreeNode *node);

public:
    void set_declarations(vector<string> a, vector<string> s);
    void type_check_assignment_stmt(TreeNode *rhs, TreeNode *lhs, const int ln);
    void type_check_output_stmt(TreeNode *va, const int ln);
    void print_type_errors();
};