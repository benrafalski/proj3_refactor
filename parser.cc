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
    // ++*(int *)0;
    cout << "SNYATX EORRR !!!\n";
    exit(1);
}

TokenType Parser::peek(const int far)
{
    return lexer.peek(far).token_type;
}

// not done
void Parser::parse_program(const int task)
{
    stack.init_stack();
    parse_decl_section();
    parse_block();
    expect(END_OF_FILE);

    switch (task)
    {
    case 1:
        trees[0].printLevelOrder();
        break;
    case 2:
        checker.print_type_errors();
        break;
    case 3:
        cout << "3\n";
    }
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
    checker.set_declarations(arrays, scalars);
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
    if (peek(1) == ID)
    {
        ids = parse_id_list();
        ids.push_back(id);
    }
    else if (peek(1) == ARRAY || peek(1) == LBRACE)
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
    if (peek(1) == ID || peek(1) == OUTPUT)
        parse_stmt_list();
    else if (peek(1) == RBRACE)
        return;
    else
        syntax_error();
}

// not done
void Parser::parse_stmt()
{
    // cout << "stmt\n";
    if (peek(1) == ID)
        parse_assign_stmt();
    else if (peek(1) == OUTPUT)
        parse_output_stmt();
    else
        syntax_error();
}

// not done
void Parser::parse_assign_stmt()
{
    TreeNode *lhs = parse_variable_access();
    int ln = expect(EQUAL).line_no;
    TreeNode *rhs = parse_expr();
    trees.push_back(Tree(new TreeNode(rhs, lhs, "="), expect(SEMICOLON).line_no));
    checker.type_check_assignment_stmt(rhs, lhs, ln);
}

void Parser::parse_output_stmt()
{
    int ln = expect(OUTPUT).line_no;
    TreeNode *va = parse_variable_access();
    checker.type_check_output_stmt(va, ln);
    expect(SEMICOLON);
}

TreeNode *Parser::parse_variable_access()
{
    StackNode E = reduce({StackNode(TERM, NULL, expect(ID))}, {"primary"});
    if (peek(1) == LBRAC)
    {
        stack.push(E);
        return parse_expr();
    }
    else if (peek(1) == EQUAL)
        return E.expr;
    else
        syntax_error();
    return NULL;
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
            int b = getIndex(t.token_type), a = getIndex(stack.terminal_peek().term.token_type);
            if (table[a][b] == "<" || table[a][b] == "=")
            {

                t = lexer.GetToken();

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
                    if (s.type == TERM)
                    {
                        lpt_set = true;
                        lpt = s;
                    }
                    reduce_me.push_back(s);
                    RHS.push_back(getRHSLexeme(s));
                    if (lpt_set && stack.top().type == TERM && table[getIndex(stack.terminal_peek().term.token_type)][getIndex(lpt.term.token_type)] == "<")
                        break;
                }
                reverse(RHS.begin(), RHS.end());
                reverse(reduce_me.begin(), reduce_me.end());

                if (rule_exists(RHS))
                {
                    StackNode E = reduce(reduce_me, RHS);
                    stack.push(E);
                }
                else
                    syntax_error();
            }
            else
                syntax_error();
        }
    }
}

// done
bool Parser::rule_exists(vector<string> rhs)
{
    for (auto &expr : expr_rules)
    {
        if (expr.second == rhs)
            return true;
    }
    return false;
}

// done
string Parser::getRHSLexeme(StackNode s)
{
    string lexeme;
    if (s.type == TERM)
    {
        if (s.term.token_type == NUM || s.term.token_type == ID)
            lexeme = "primary";
        else if (s.term.token_type == PLUS)
            lexeme = "PLUS";
        else if (s.term.token_type == MINUS)
            lexeme = "MINUS";
        else if (s.term.token_type == MULT)
            lexeme = "MULT";
        else if (s.term.token_type == DIV)
            lexeme = "DIV";
        else if (s.term.token_type == LPAREN)
            lexeme = "LPAREN";
        else if (s.term.token_type == RPAREN)
            lexeme = "RPAREN";
        else if (s.term.token_type == RBRAC)
            lexeme = "RBRAC";
        else if (s.term.token_type == LBRAC)
            lexeme = "LBRAC";
        else if (s.term.token_type == DOT)
            lexeme = "DOT";
    }
    else
        lexeme = "expr";
    return lexeme;
}

// done
string Parser::next_symbol()
{
    string symbol;
    if (peek(1) == SEMICOLON || peek(1) == EQUAL)
    {
        symbol = "$";
    }
    else if (peek(1) == RBRAC)
    {
        if (peek(2) == EQUAL)
        {
            symbol = "$";
        }
    }
    else
    {
        symbol = lexer.peek(1).lexeme;
    }
    return symbol;
}

// done
int Parser::getIndex(TokenType key)
{
    switch (key)
    {
    case PLUS:
        return 0;
    case MINUS:
        return 1;
    case MULT:
        return 2;
    case DIV:
        return 3;
    case LPAREN:
        return 4;
    case RPAREN:
        return 5;
    case LBRAC:
        return 6;
    case DOT:
        return 7;
    case RBRAC:
        return 8;
    case NUM:
        return 9;
    case ID:
        return 10;
    default:
        return 11;
    }
}

StackNode Parser::reduce(vector<StackNode> stk, vector<string> rhs)
{
    StackNode node;
    node.type = EXPR;
    TreeNode *tnode;
    if (rhs == vector<string>({"expr", "MINUS", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _MINUS, "-", "-");
    }
    else if (rhs == vector<string>({"expr", "PLUS", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _PLUS, "+", "+");
    }
    else if (rhs == vector<string>({"expr", "MULT", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _MULT, "*", "*");
    }
    else if (rhs == vector<string>({"expr", "DIV", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _DIV, "/", "/");
    }
    else if (rhs == vector<string>({"LPAREN", "expr", "RPAREN"}))
    {
        tnode = stk[1].expr;
        tnode->wrapped = true;
    }
    else if (rhs == vector<string>({"expr", "LBRAC", "expr", "RBRAC"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _EXPR, "[]", "[]");
    }
    else if (rhs == vector<string>({"expr", "LBRAC", "DOT", "RBRAC"}))
    {
        tnode = new TreeNode(NULL, stk[0].expr, _ARRAY, _DOT, "[.]", "[.]");
    }
    else
    { // primary
        tnode = new TreeNode(NULL, NULL, _ARRAY, _NOOP, ((stk[0].term.token_type == NUM) ? ("NUM \"" + stk[0].term.lexeme + "\"") : ("ID \"" + stk[0].term.lexeme + "\"")), stk[0].term.lexeme);
        if (stk[0].term.token_type == NUM)
            tnode->is_num = true;
    }
    node.expr = tnode;
    return node;
}
