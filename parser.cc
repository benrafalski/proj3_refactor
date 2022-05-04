#include "parser.h"

Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)

        syntax_error();
    return t;
}

void Parser::syntax_error()
{
    ++*(int *)0;
    cout << "SNYATX EORRR !!!\n";
    exit(1);
}

TokenType Parser::peek()
{
    return lexer.peek(1).token_type;
}

// not done
void Parser::init_stack()
{
    StackNode node;
    // next_index = 0;

    node.type = TERM;
    node.term.lexeme = "$";
    node.term.line_no = -1;
    node.term.token_type = END_OF_FILE;

    stack.push(node);
}

// not done
void Parser::parse_program()
{
    init_stack();
    parse_decl_section();
    parse_block();
    expect(END_OF_FILE);
}

// not done
void Parser::parse_block()
{
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}

// done
void Parser::parse_decl_section()
{
    scalars = parse_scalar_decl_section();
    arrays = parse_array_decl_section();
    reverse(scalars.begin(), scalars.end());
    reverse(arrays.begin(), arrays.end());
    // for(auto it = scalars.begin(); it != scalars.end(); it++){
    //     location_table.push_back(location(next_index, *it, SCLR));
    //     next_index++;
    // }
    // for(auto it = arrays.begin(); it != arrays.end(); it++){
    //     location_table.push_back(location(next_index, *it, ARR));
    //     next_index += 10;
    // }
}

// done
vector<string> Parser::parse_id_list()
{
    vector<string> ids;
    string id = expect(ID).lexeme;
    if (peek() == ID)
    {
        ids = parse_id_list();
        ids.push_back(id);
    }
    else if (peek() == ARRAY || peek() == LBRACE)
        ids.push_back(id);
    else
        syntax_error();
    return ids;
}

// done
vector<string> Parser::parse_scalar_decl_section()
{
    expect(SCALAR);
    vector<string> idList = parse_id_list();
    return idList;
}

// done
vector<string> Parser::parse_array_decl_section()
{
    expect(ARRAY);
    vector<string> idList = parse_id_list();
    return idList;
}

// not done
void Parser::parse_stmt_list()
{
    parse_stmt();
    if (peek() == ID || peek() == OUTPUT)
        parse_stmt_list();
    else if (peek() == RBRACE)
        return;
    else
        syntax_error();
}

// not done
void Parser::parse_stmt()
{
    cout << "stmt\n";
    if (peek() == ID)
        parse_assign_stmt();
    else if (peek() == OUTPUT)
        parse_output_stmt();
    else
        syntax_error();
}

// not done
void Parser::parse_assign_stmt()
{
    parse_variable_access();
    expect(EQUAL);
    parse_expr();
    expect(SEMICOLON);
}

void Parser::parse_output_stmt()
{
    expect(OUTPUT);
    parse_variable_access();
    expect(SEMICOLON);
}

TreeNode *Parser::parse_variable_access()
{
    parse_expr();
    // StackNode E = reduce({StackNode(TERM, NULL, expect(ID))}, {"primary"});
    // if (peek() == LBRAC)
    // {
    //     stack.push(E);
    //     return parse_expr();
    // }
    // else if (peek() == EQUAL)
    // {
    //     return E.expr;
    // }
    // else
    // {
    //     syntax_error();
    // }
    // return NULL;
}

TreeNode *Parser::parse_expr()
{
    while (1)
    {
        if (stack.terminal_peek().term.lexeme == "$" && next_symbol() == "$")
        {
            StackNode s = stack.pop();
            return s.expr;
        }
        else
        {
            Token t = lexer.peek(1);
            cout << t.lexeme << " lexeme \n";
            int b = getIndex(t.token_type), a = getIndex(stack.terminal_peek().term.token_type);
            cout << a << " " << b << endl;
            if (table[a][b] == "<" || table[a][b] == "=")
            {

                t = lexer.GetToken();
                // cout << t.lexeme << " lexeme \n";

                StackNode s_node;
                s_node.type = TERM;
                s_node.expr = NULL;
                s_node.term = t;

                stack.push(s_node);
            }
            else if (table[a][b] == ">")
            {
                vector<string> RHS = {};
                vector<StackNode> reduce_me = {};
                StackNode lpt = {};
                bool lpt_set = false;
                while (1)
                {
                    StackNode s = stack.pop();
                    cout << s.type;
                    if (s.type == TERM)
                    {
                        cout << "lpt is being set\n";
                        lpt_set = true;
                        lpt = s;
                    }
                    reduce_me.push_back(s);
                    RHS.push_back(getRHSLexeme(s));
                    if (lpt_set && stack.top().type == TERM && table[getIndex(stack.terminal_peek().term.token_type)][getIndex(lpt.term.token_type)] == "<")
                    {
                        cout << "\n\n";
                        lpt.term.Print();
                        cout << (getIndex(lpt.term.token_type)) << "condition\n";
                        cout << getIndex(stack.terminal_peek().term.token_type) << "cond2\n";
                        break;
                    }
                }
                cout << "\n\n";
                reverse(RHS.begin(), RHS.end());
                reverse(reduce_me.begin(), reduce_me.end());


                if (rule_exists(RHS))
                {
                    StackNode E = reduce(reduce_me, RHS);
                    stack.push(E);
                }
                else
                {
                    cout << "esyn1\n";
                    syntax_error();
                }
            }
            else
            {
                cout << "esyn2\n";
                syntax_error();
            }
        }
    }
}

bool Parser::var_exists(vector<string> rhs)
{
    vector<vector<string>> rules = {
        {"expr", "LBRAC", "expr", "RBRAC"},
        {"primary"},
        {"expr", "LBRAC", "DOT", "RBRAC"}};

    for (auto r : rhs)
    {
        cout << r << endl;
    }
    for (const auto &rule : rules)
    {
        if (rule == rhs)
        {
            return true;
        }
    }
    return false;
}

bool Parser::rule_exists(vector<string> rhs)
{
    cout << "---------------------\n";
    for (auto r : rhs)
    {
        cout << r << endl;
    }
    cout << "---------------------\n";
    for (auto expr : expr_rules)
    {
        if (expr.second == rhs)
        {
            return true;
        }
    }
    return false;
}

string Parser::getRHSLexeme(StackNode s)
{
    string lexeme;
    if (s.type == TERM)
    {
        if (s.term.token_type == NUM || s.term.token_type == ID)
        {
            lexeme = "primary";
        }
        else if (s.term.token_type == PLUS)
        {
            lexeme = "PLUS";
        }
        else if (s.term.token_type == MINUS)
        {
            lexeme = "MINUS";
        }
        else if (s.term.token_type == MULT)
        {
            lexeme = "MULT";
        }
        else if (s.term.token_type == DIV)
        {
            lexeme = "DIV";
        }
        else if (s.term.token_type == LPAREN)
        {
            lexeme = "LPAREN";
        }
        else if (s.term.token_type == RPAREN)
        {
            lexeme = "RPAREN";
        }
        else if (s.term.token_type == RBRAC)
        {
            lexeme = "RBRAC";
        }
        else if (s.term.token_type == LBRAC)
        {
            lexeme = "LBRAC";
        }
        else if (s.term.token_type == DOT)
        {
            lexeme = "DOT";
        }
    }
    else
    {
        lexeme = "expr";
    }

    return lexeme;
}

string Parser::next_symbol()
{
    Token t = lexer.peek(1);
    string symbol;
    if (t.token_type == SEMICOLON || t.token_type == EQUAL)
    {
        symbol = "$";
    }
    else if (t.token_type == RBRAC)
    {
        t = lexer.peek(2);
        if (t.token_type == EQUAL)
        {
            symbol = "$";
        }
    }
    else
    {
        symbol = t.lexeme;
    }
    return symbol;
}

int Parser::getIndex(TokenType key)
{
    switch (key)
    {
    case PLUS:
    {
        return 0;
    }
    case MINUS:
    {
        return 1;
    }
    case MULT:
    {
        return 2;
    }
    case DIV:
    {
        return 3;
    }
    case LPAREN:
    {
        return 4;
    }
    case RPAREN:
    {
        return 5;
    }
    case LBRAC:
    {
        return 6;
    }
    case DOT:
    {
        return 7;
    }
    case RBRAC:
    {
        return 8;
    }
    case NUM:
    {
        return 9;
    }
    case ID:
    {
        return 10;
    }
    default:
    {
        return 11;
    }
    }
}

StackNode Parser::reduce(vector<StackNode> stk, vector<string> rhs)
{
    StackNode node;
    node.type = EXPR;

    TreeNode *tnode = new TreeNode();
    tnode->wrapped = false;
    tnode->is_num = false;
    // if (rhs == vector<string>({"expr", "MINUS", "expr"}))
    if (rhs == vector<string>({"expr", "MINUS", "expr"}))
    {
        tnode->op = _MINUS;
        tnode->right = stk[2].expr;
        tnode->left = stk[0].expr;
        tnode->value = "-";
    }
    else if (rhs == vector<string>({"expr", "PLUS", "expr"}))
    {
        tnode->op = _PLUS;
        tnode->right = stk[2].expr;
        tnode->left = stk[0].expr;
        tnode->value = "+";
    }
    else if (rhs == vector<string>({"expr", "MULT", "expr"}))
    {
        tnode->op = _MULT;
        tnode->right = stk[2].expr;
        tnode->left = stk[0].expr;
        tnode->value = "*";
    }
    else if (rhs == vector<string>({"expr", "DIV", "expr"}))
    {
        tnode->op = _DIV;
        tnode->right = stk[2].expr;
        tnode->left = stk[0].expr;
        tnode->value = "/";
    }
    else if (rhs == vector<string>({"LPAREN", "expr", "RPAREN"}))
    {
        tnode = stk[1].expr;
        tnode->wrapped = true;
    }
    else if (rhs == vector<string>({"expr", "LBRAC", "expr", "RBRAC"}))
    {
        tnode->op = _EXPR;
        tnode->right = stk[2].expr;
        tnode->left = stk[0].expr;
        tnode->value = "[]";
    }
    else if (rhs == vector<string>({"expr", "LBRAC", "DOT", "RBRAC"}))
    {
        tnode->op = _DOT;
        tnode->right = NULL;
        tnode->left = stk[0].expr;
        tnode->value = "[.]";
    }
    else
    { // primary
        tnode->right = NULL;
        tnode->left = NULL;
        tnode->op = _NOOP;
        if (stk[0].term.token_type == NUM)
        {
            tnode->is_num = true;
            tnode->value = "NUM \"" + stk[0].term.lexeme + "\"";
            tnode->rawValue = stk[0].term.lexeme;
        }
        else
        {
            tnode->value = "ID \"" + stk[0].term.lexeme + "\"";
            tnode->rawValue = stk[0].term.lexeme;
        }
    }
    node.expr = tnode;
    return node;
}
