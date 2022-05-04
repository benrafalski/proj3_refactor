#include "tree.h"

void Tree::init_root(TreeNode *lhs, TreeNode *rhs, int line)
{
    root = new TreeNode();
    TreeNode *root = new TreeNode();
    root->value = "=";
    root->left = lhs;
    root->right = rhs;

    line_no = line;
}

int Tree::height(TreeNode *node)
{
    if (node == NULL)
    {
        return 0;
    }
    else
    {
        int lh = height(node->left);
        int rh = height(node->right);

        if (lh > rh)
        {
            return (lh + 1);
        }
        else
        {
            return (rh + 1);
        }
    }
}

void Tree::printCurrentLevel(TreeNode *root, int level)
{
    if (root == NULL)
    {
        return;
    }

    if (level == 1)
    {
        cout << root->value << " ";
    }
    else if (level > 1)
    {
        printCurrentLevel(root->left, level - 1);
        printCurrentLevel(root->right, level - 1);
    }
}

void Tree::printLevelOrder()
{
    int h = height(root);
    for (int i = 1; i <= h; i++)
    {
        printCurrentLevel(root, i);
        // cout << "\n";
    }
}