#include "stack.h"

StackNode Stack::pop()
{
    StackNode top = stack.back();
    stack.pop_back();
    return top;
}

void Stack::push(StackNode &node)
{
    stack.push_back(node);
}

StackNode Stack::top()
{
    return stack.back();
}

StackNode Stack::terminal_peek()
{
    vector<StackNode> s = stack;
    StackNode node = s.back();
    if (node.type == EXPR)
    {
        s.pop_back();
        node = s.back();
    }
    return node;
}
