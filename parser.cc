#include "parser.h"


instNode *Append(instNode *head, instNode *newNode)
{
    if (head == NULL)
    {
        // cout << "here";
        head = newNode;
    }
    else
    {
        instNode *node = head;
        while (node->next)
        {
            node = node->next;
        }

        node->next = newNode;
    }
    return head;
}


TreeNode *Parser::makeInst(TreeNode *node)
{
    TreeNode *rt_me = new TreeNode();
    instNode *inst = new instNode();

    if (node->left == NULL && node->right == NULL)
    {
        if (node->type == _SCALAR || node->type == _ARRAY)
        {
            inst->lhs = loc(node->rawValue);
            inst->lhsat = DIRECT;
            rt_me->inst = inst;
            return rt_me;
        }
        else if (node->is_num)
        {
            inst->lhs = stoi(node->rawValue);
            inst->lhsat = IMMEDIATE;
            rt_me->inst = inst;
            return rt_me;
        }
    }
    else
    {
        TreeNode *right = makeInst(node->right);
        TreeNode *left = makeInst(node->left);
        if ((right->type == _SCALAR && left->type == _SCALAR) && (node->op == _PLUS || node->op == _MINUS || node->op == _MULT || node->op == _DIV))
        {
            inst->lhsat = DIRECT;
            inst->lhs = next_index++;
            inst->iType = ASSIGN_INST;
            inst->op1at = left->inst->lhsat;
            inst->op1 = left->inst->lhs;
            inst->op2at = right->inst->lhsat;
            inst->op2 = right->inst->lhs;
            switch (node->op)
            {
            case _PLUS:
                inst->oper = OP_PLUS;
                break;
            case _MINUS:
                inst->oper = OP_MINUS;
                break;
            case _MULT:
                inst->oper = OP_MULT;
                break;
            case _DIV:
                inst->oper = OP_DIV;
                break;
            default:
                cout << "invalid operation type !!!!\n";
                break;
            }
            rt_me->inst = inst;
            return rt_me;
        }else if(left->type == _ARRAY && right->type == _SCALAR && (node->op == _EXPR)){
            // add undefined part
            inst->lhsat = DIRECT;
            inst->lhs = next_index++;
            inst->iType = ASSIGN_INST;
            inst->op1at = IMMEDIATE;
            inst->op1 = left->inst->lhs;
            inst->oper = OP_PLUS;
            inst->op2at = right->inst->lhsat;
            inst->op2 = right->inst->lhs;
            rt_me->inst = inst;
            return rt_me;
        }else if((right->op == _DOT && left->op == _DOT) && (left->type == _ARRAY && right->type == _ARRAY) && (node->op == _PLUS || node->op == _MINUS || node->op == _MULT)){
            instNode * head = new instNode(), *prev = new instNode();
            switch (node->op)
            {
            case _PLUS:
                inst->oper = OP_PLUS;

                

                for(int i = 0; i < 10; i++){
                    if(i == 0){
                        head->lhsat = DIRECT;
                        head->lhs = next_index++; // c[i]
                        head->iType = ASSIGN_INST;
                        head->op1at = IMMEDIATE;
                        head->op1 = left->inst->lhs;
                        head->oper = OP_PLUS;
                        head->op2at = IMMEDIATE;
                        head->op2 = i;
                    }else{
                        prev->lhsat = DIRECT;
                        prev->lhs = next_index++; // c[i]
                        prev->iType = ASSIGN_INST;
                        prev->op1at = IMMEDIATE;
                        prev->op1 = left->inst->lhs;
                        prev->oper = OP_PLUS;
                        prev->op2at = IMMEDIATE;
                        prev->op2 = i;
                        head = Append(head, prev);
                    }
                    inst = head;


                }

                break;
            case _MINUS:
                inst->oper = OP_MINUS;
                break;
            case _MULT:
                inst->oper = OP_MULT;
                break;
            default:
                cout << "invalid operation type !!!?!\n";
                break;
            }

            rt_me->inst = inst;
            return rt_me;


        }
    }
}

string nodeAt(AddrType type)
{
    string t = "NA";
    switch (type)
    {
    case 0:
    {
        t = "IMMEDIATE";
        break;
    }
    case 1:
    {
        t = "DIRECT";
        break;
    }
    case 2:
    {
        t = "INDIRECT";
        break;
    }
    case 3:
    {
        t = "NOAT";
        break;
    }
    }

    return t;
}

string nodeOp(OpType type)
{
    string t = "OP_NOOPT";
    switch (type)
    {
    case 0:
    {
        t = "OP_NOOP";
        break;
    }
    case 1:
    {
        t = "OP_PLUS";
        break;
    }
    case 2:
    {
        t = "OP_MINUS";
        break;
    }
    case 3:
    {
        t = "OP_MULT";
        break;
    }
    case 4:
    {
        t = "OP_DIV";
        break;
    }
    }

    return t;
}

string nodeType(InstType type)
{
    string t = "NOIT";
    switch (type)
    {
    case 0:
    {
        t = "OUTPUT_INST";
        break;
    }
    case 1:
    {
        t = "ASSIGN_INST";
        break;
    }
    case 2:
    {
        t = "CJMP_INST";
        break;
    }
    }

    return t;
}

void printInstNode(instNode *node)
{
    cout << "InstNode * = \n";
    cout << "lhsat: " << nodeAt(node->lhsat) << "\n";
    cout << "lhs: " << node->lhs << "\n";
    cout << "type: " << nodeType(node->iType) << "\n";
    cout << "op1at: " << nodeAt(node->op1at) << "\n";
    cout << "op1: " << node->op1 << "\n";
    cout << "oper: " << nodeOp(node->oper) << "\n";
    cout << "op2at: " << nodeAt(node->op2at) << "\n";
    cout << "op2: " << node->op2 << "\n";
    cout << "\n";
}

void printList(instNode *list)
{
    if (list == nullptr)
    {
        return;
    }
    else
    {
        printInstNode(list);
        printList(list->next);
    }
}

int Parser::loc(const string lex)
{
    for (const auto &l : location_table)
    {
        if (l.lexeme == lex)
        {
            return l.mem_addr;
        }
    }
    return -1;
}



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

TokenType Parser::peek(const int far)
{
    return lexer.peek(far).token_type;
}

// not done
instNode *Parser::parse_program(const int task)
{
    stack.init_stack();
    parse_decl_section();
    instNode *i = parse_block();
    expect(END_OF_FILE);
    switch (task)
    {
    case 1:
        trees[0].printLevelOrder();
        break;
    case 2:
        checker.print_type_errors();
        break;
    }
    return i;
}

// not done
instNode *Parser::parse_block()
{
    expect(LBRACE);
    instNode *i = parse_stmt_list();
    expect(RBRACE);

    // while (i->next)
    // {
    //     cout << i->lhs << endl;
    //     cout << i->op1 << endl;
    //     cout << "\n\n";

    //     i = i->next;
    // }

    return i;
}

// done
void Parser::parse_decl_section()
{
    scalars = parse_scalar_decl_section();
    arrays = parse_array_decl_section();
    reverse(scalars.begin(), scalars.end());
    reverse(arrays.begin(), arrays.end());
    checker.set_declarations(arrays, scalars);
    for (const auto s : scalars)
    {
        location_table.push_back(location(next_index, s, SCLR));
        next_index++;
    }
    for (const auto a : arrays)
    {
        location_table.push_back(location(next_index, a, ARR));
        next_index += 10;
    }

    last_index = next_index;

    // cout << next_index << endl;
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
instNode *Parser::parse_stmt_list()
{
    instNode *i = parse_stmt();
    if (peek(1) == ID || peek(1) == OUTPUT)
    {
        Append(i, parse_stmt_list());

        // cout << "VA LIST:\n";
        // cout << "oooooooo\n";
        // // cout << head->lhs << " " << head->op1 << endl;
        // instNode *n = i;
        // while (n)
        // {
        //     cout << n->lhs << endl;
        //     cout << n->op1 << endl;
        //     cout << endl;
        //     n = n->next;
        // }
        // cout << "oooooooo\n";
        return i;
    }
    else if (peek(1) == RBRACE)
        return i;
    else
        syntax_error();

    return i;
}

// not done
instNode *Parser::parse_stmt()
{
    // cout << "smtt\n";
    instNode *i;
    // cout << "stmt\n";
    if (peek(1) == ID)
        i = parse_assign_stmt();
    else if (peek(1) == OUTPUT)
        i = parse_output_stmt();
    else
        syntax_error();

    return i;
}

// not done
instNode *Parser::parse_assign_stmt()
{
    TreeNode *lhs = parse_variable_access();
    int ln = expect(EQUAL).line_no;
    TreeNode *rhs = parse_expr();
    trees.push_back(Tree(new TreeNode(rhs, lhs, "="), expect(SEMICOLON).line_no));
    checker.type_check_assignment_stmt(rhs, lhs, ln);

    instNode *node = rhs->inst;
    while (node->next != nullptr)
    {
        node = node->next;
    }

    instNode *node2 = lhs->inst;
    while (node2->next != nullptr)
    {
        node2 = node2->next;
    }

    instNode *i = new instNode();
    i->lhsat = (node2->lhs > last_index) ? INDIRECT : DIRECT;
    i->lhs = node2->lhs;
    i->iType = ASSIGN_INST;
    i->op1at = node->lhsat;
    i->op1 = node->lhs;
    i->oper = OP_NOOP;

    instNode * l = makeInst(lhs)->inst;
    instNode * r = makeInst(rhs)->inst;

    printList(l);
    printList(r);

    instNode *rt_me = i;

    if (lhs->inst->op1 != -1 && rhs->inst->op1 != -1)
    {
        Append(lhs->inst, rhs->inst);
        Append(lhs->inst, i);
        rt_me = lhs->inst;
    }
    else if (rhs->inst->op1 == -1 && lhs->inst->op1 != -1)
    {
        Append(lhs->inst, i);
        rt_me = lhs->inst;
    }
    else if (rhs->inst->op1 != -1 && lhs->inst->op1 == -1)
    {
        Append(rhs->inst, i);
        rt_me = rhs->inst;
    }

    return rt_me;
}

instNode *Parser::parse_output_stmt()
{
    int ln = expect(OUTPUT).line_no;
    TreeNode *va = parse_variable_access();
    checker.type_check_output_stmt(va, ln);
    expect(SEMICOLON);

    instNode *node = va->inst;
    while (node->next != nullptr)
    {
        node = node->next;
    }

    instNode *i = new instNode();
    i->iType = OUTPUT_INST;
    i->op1at = (node->lhs > last_index) ? INDIRECT : DIRECT;
    i->op1 = node->lhs;

    if (va->inst->op1 != -1)
    {
        va->inst = Append(va->inst, i);
        return va->inst;
    }
    else
    {
        return i;
    }
}

TreeNode *Parser::parse_variable_access()
{
    StackNode E = reduce({StackNode(TERM, NULL, expect(ID))}, {"primary"});
    if (peek(1) == LBRAC)
    {
        stack.push(E);
        return parse_expr();
    }
    else if (peek(1) == EQUAL || peek(1) == SEMICOLON)
        return E.expr;
    else
        syntax_error();
    return NULL;
}

TreeNode *Parser::parse_expr()
{

    instNode *head = NULL;
    while (1)
    {
        if (stack.terminal_peek().term.lexeme == "$" && next_symbol() == "$")
        {
            StackNode s = stack.pop();
            if (head != NULL)
            {

                instNode *node = head;
                while (node->next != nullptr)
                {
                    node = node->next;
                }

                s.expr->inst = head;
            }

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

                    if ((E.expr->inst->oper == OP_PLUS || E.expr->inst->oper == OP_MULT || E.expr->inst->oper == OP_DIV || E.expr->inst->oper == OP_MINUS) && E.expr->wrapped == false)
                    {

                        // cout << "_________\n";
                        // cout << E.expr->inst->lhs << endl;
                        // cout << E.expr->inst->op1 << endl;

                        // cout << "_________\n";
                        head = Append(head, E.expr->inst);

                        // if (head == NULL)
                        // {
                        //     cout << "null\n";
                        // }

                        // cout << "CURRENT LIST:\n";
                        // cout << "+++++++\n";
                        // // cout << head->lhs << " " << head->op1 << endl;
                        // instNode *n = head;
                        // while (n)
                        // {
                        //     cout << n->lhs << endl;
                        //     cout << n->op1 << endl;
                        //     cout << endl;
                        //     n = n->next;
                        // }
                        // cout << "+++++++\n";
                    }
                }
                else
                {
                    cout << t.line_no << endl;
                    syntax_error();
                }
            }
            else
            {
                cout << t.line_no << endl;
                syntax_error();
            }
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
    instNode *inst = new instNode();
    if (rhs == vector<string>({"expr", "MINUS", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _MINUS, "-", "-");

        inst->lhsat = DIRECT;
        inst->lhs = next_index++;
        inst->iType = ASSIGN_INST;
        inst->op1at = stk[0].expr->inst->lhsat;
        inst->op1 = stk[0].expr->inst->lhs;
        inst->oper = OP_MINUS;
        inst->op2at = stk[2].expr->inst->lhsat;
        inst->op2 = stk[2].expr->inst->lhs;
    }
    else if (rhs == vector<string>({"expr", "PLUS", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _PLUS, "+", "+");

        inst->lhsat = DIRECT;
        inst->lhs = next_index++;
        inst->iType = ASSIGN_INST;
        inst->op1at = stk[0].expr->inst->lhsat;
        inst->op1 = stk[0].expr->inst->lhs;
        inst->oper = OP_PLUS;
        inst->op2at = stk[2].expr->inst->lhsat;
        inst->op2 = stk[2].expr->inst->lhs;
    }
    else if (rhs == vector<string>({"expr", "MULT", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _MULT, "*", "*");

        inst->lhsat = DIRECT;
        inst->lhs = next_index++;
        inst->iType = ASSIGN_INST;
        inst->op1at = stk[0].expr->inst->lhsat;
        inst->op1 = stk[0].expr->inst->lhs;
        inst->oper = OP_MULT;
        inst->op2at = stk[2].expr->inst->lhsat;
        inst->op2 = stk[2].expr->inst->lhs;
    }
    else if (rhs == vector<string>({"expr", "DIV", "expr"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _DIV, "/", "/");

        inst->lhsat = DIRECT;
        inst->lhs = next_index++;
        inst->iType = ASSIGN_INST;
        inst->op1at = stk[0].expr->inst->lhsat;
        inst->op1 = stk[0].expr->inst->lhs;
        inst->oper = OP_DIV;
        inst->op2at = stk[2].expr->inst->lhsat;
        inst->op2 = stk[2].expr->inst->lhs;
    }
    else if (rhs == vector<string>({"LPAREN", "expr", "RPAREN"}))
    {
        tnode = stk[1].expr;
        tnode->wrapped = true;

        inst = stk[1].expr->inst;
    }
    else if (rhs == vector<string>({"expr", "LBRAC", "expr", "RBRAC"}))
    {
        tnode = new TreeNode(stk[2].expr, stk[0].expr, _ARRAY, _EXPR, "[]", "[]");

        inst->lhsat = DIRECT;
        inst->lhs = next_index++;
        inst->iType = ASSIGN_INST;
        inst->op1at = IMMEDIATE;
        inst->op1 = stk[0].expr->inst->lhs;
        inst->oper = OP_PLUS;
        inst->op2at = stk[2].expr->inst->lhsat;
        inst->op2 = stk[2].expr->inst->lhs;
    }
    else if (rhs == vector<string>({"expr", "LBRAC", "DOT", "RBRAC"}))
    {
        tnode = new TreeNode(NULL, stk[0].expr, _ARRAY, _DOT, "[.]", "[.]");

        inst->lhsat = DIRECT;
        inst->lhs = next_index++;
        inst->iType = ASSIGN_INST;
        inst->op1at = IMMEDIATE;
        inst->op1 = stk[0].expr->inst->lhs;
        inst->oper = OP_NOOP;
    }
    else
    { // primary
        tnode = new TreeNode(NULL, NULL, _ARRAY, _NOOP, ((stk[0].term.token_type == NUM) ? ("NUM \"" + stk[0].term.lexeme + "\"") : ("ID \"" + stk[0].term.lexeme + "\"")), stk[0].term.lexeme);
        if (stk[0].term.token_type == NUM)
        {
            tnode->is_num = true;
            inst->lhsat = IMMEDIATE;
            inst->lhs = stoi(stk[0].term.lexeme);
        }
        else
        {
            inst->lhsat = DIRECT;
            inst->lhs = loc(stk[0].term.lexeme);
        }
    }
    tnode->inst = inst;
    node.expr = tnode;
    return node;
}
