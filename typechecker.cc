#include "typechecker.h"


int TypeChecker::type_check_expr(TreeNode *node)
{
    // 1 = good , 2 = type error, 3 = assign error
    int result = 1;
    if (node->left == NULL && node->right == NULL)
    {
        if (in_vector(scalars, node->rawValue)) // rule 1
        {
            node->type = _SCALAR;
        }
        else if (in_vector(arrays, node->rawValue))
        {
            node->type = _ARRAYDECL;
        }
        else if (node->is_num) // rule 2
        {

            node->type = _SCALAR;
        }
        else if (!in_vector(scalars, node->rawValue) && !in_vector(arrays, node->rawValue)) // rule 14
        {
            node->type = _ERROR;
            result = 2;
        }
        else // rule 15
        {
            node->type = _ERROR;
            result = 2;
        }
    }
    else
    {
        int ls = type_check_expr(node->left);
        int rs = (node->value != "[.]" ? type_check_expr(node->right) : 1);
        if (node->value == "[]" && node->left->type == _ARRAYDECL && node->right->type == _SCALAR) // rule 4
        {
            node->type = _SCALAR;
        }
        else if (node->value == "[.]" && node->left->type == _ARRAYDECL && node->right == NULL) // rule 3
        {
            node->type = _ARRAY;
            // cout << "hi\n";
        }
        else if ((node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/") && node->left->type == _SCALAR && node->right->type == _SCALAR)
        {
            node->type = _SCALAR; // rule 5
        }
        else if ((node->value == "+" || node->value == "-") && node->left->type == _ARRAY && node->right->type == _ARRAY) // rule 6
        {
            node->type = _ARRAY;
        }
        else if (node->value == "*" && node->left->type == _ARRAY && node->right->type == _ARRAY) // rule 7
        {
            node->type = _SCALAR;
        }
        else if (node->value == "[]" && node->left->type == _ARRAY && node->right->type == _SCALAR) // rule 8
        {
            node->type = _SCALAR;
        }
        else if (node->value == "[.]" && node->left->type == _SCALAR && node->right == NULL) // rule 9
        {
            node->type = _ARRAY;
        }
        else if (node->value == "[]" && ((node->left->type == _SCALAR && node->left->type == _ERROR) || (node->right->type != _SCALAR)))
        { // rule 10 and 11
            node->type = _ERROR;
            result = 2;
        }
        else if ((node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/") && (node->left->type != node->right->type))
        { // rule 12
            node->type = _ERROR;
            result = 2;
        }
        else if (node->value == "/" && node->left->type == _ARRAY && node->right->type == _ARRAY)
        { // rule 13
            node->type = _ERROR;
            result = 2;
        }
        else // rule 15
        {
            node->type = _ERROR;
            result = 2;
        }

        result = ((ls == 2 || rs == 2) ? 2 : result);
    }

    return result;
}

int TypeChecker::type_check_assignment(TreeNode *node)
{
    // 1 = good , 2 = type error, 3 = assign error
    int result = 1;
    if (!(node->left == NULL && node->right == NULL))
    {
        if (node->left->type == node->right->type)
        {
            node->type = node->left->type;
        }
        else if (node->left->type == _ARRAY && node->right->type == _SCALAR)
        {
            node->type = _ARRAY;
        }
        else
        {
            node->type = _ERROR;
            result = 2;
        }
    }
    return result;
}

int TypeChecker::type_check_var_access(TreeNode *node)
{
    // 1 = good , 2 = type error, 3 = assign error
    int result = 1;
    if (node->left == NULL && node->right == NULL)
    {
        if (in_vector(scalars, node->rawValue)) // rule 1
        {
            node->type = _SCALAR;
        }
        else // rule 4
        {
            node->type = _ERROR;
            result = 2;
        }
    }
    else
    {
        int expr = 1;
        if (node->value != "[.]")
        {
            expr = type_check_expr(node->right);
        }

        if (node->value == "[]" && in_vector(arrays, node->left->rawValue) && node->right->type == _SCALAR) // rule 2
        {
            node->type = _SCALAR;
        }
        else if (node->value == "[.]" && in_vector(arrays, node->left->rawValue)) // rule 3
        {
            node->type = _ARRAY;
        }
        else if (node->value == "[.]" && in_vector(arrays, node->left->rawValue)) // rule 5
        {
            node->type = _ERROR;
            result = 2;
        }
        else if (node->value == "[]" && in_vector(arrays, node->left->rawValue)) // rule 6
        {
            node->type = _ERROR;
            result = 2;
        }
        else
        {
            node->type = _ERROR;
            result = 2;
        }

        result = ((expr == 2) ? 2 : result);
    }

    return result;
}

void TypeChecker::set_declarations(vector<string> a, vector<string> s)
{
    arrays = a;
    scalars = s;
}

void TypeChecker::type_check_assignment_stmt(TreeNode *rhs, TreeNode *lhs, const int ln)
{
    int e = type_check_expr(rhs);
    int v = type_check_var_access(lhs);

    if (e == 2)
    {
        type_errors.push_back(ln);
    }
    else if (v == 2)
    {
        type_errors.push_back(ln);
    }

    int assn = type_check_assignment(new TreeNode(rhs, lhs, "="));
    if (assn == 2)
    {
        assign_errors.push_back(ln);
    }
}

void TypeChecker::type_check_output_stmt(TreeNode *va, const int ln)
{
    if (type_check_var_access(va) == 2)
    {
        type_errors.push_back(ln);
    }
}

void TypeChecker::print_type_errors()
{
    if (type_errors.empty())
    {
        if (assign_errors.empty())
        {
            cout << "No type errors here :)\n";
        }
        else
        {
            cout << "Invalid assignment :(\n\n";
            for (auto line : assign_errors)
            {
                cout << "Line " << line << "\n";
            }
        }
    }
    else
    {
        cout << "Expression type error :(\n\n";
        for (auto line : type_errors)
        {
            cout << "Line " << line << "\n";
        }
    }
}