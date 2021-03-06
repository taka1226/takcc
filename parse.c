#include "takcc.h"

Node *code[100];


// 次のトークンが期待している記号のときには、トークンを一つ読み進めて
// 真を返す。それ以外の場合には偽を返す
bool consume(char *op){
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}


bool consume_return(char *op){
    if (token->kind != TK_RETURN || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

bool consume_if(char *op){
    if (token->kind != TK_IF || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

bool consume_else(char *op){
    if (token->kind != TK_ELSE || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

bool consume_while(char *op){
    if (token->kind != TK_WHILE || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

bool consume_for(char *op){
    if (token->kind != TK_FOR || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

Token *consume_ident(){
    if (token->kind != TK_IDENT)
        error("変数ではありません");

    Token *res = token;
    token = token->next;
    return res;
}

// 次のトークンが期待している記号のときには、トークンを一つ読み進める
// それ以外の場合にはエラーを報告する
void expect(char *op){
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        error("'%c'ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する
int expect_number(){
    if (token->kind != TK_NUM){
        error("数ではありません。");
    }

    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *assign(){
    Node *node = equality();
    if (consume("=")){
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node *stmt(){
    Node *node;

    if (consume_return("return")){
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
    }else if (consume_if("if")){
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        if (consume_else("else")){
            node->elh = stmt();
        }
    }else if (consume_while("while")){
        expect("(");
        node = expr();
        expect(")");
        node = new_node(ND_WHILE, node, stmt());
    }else if (consume_for("for")){
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect("(");
        if (!consume(";")){
            node->forh1 = expr();
            expect(";");
        }
        if (!consume(";")){
            node->forh2 = expr();
            expect(";");
        }
        if (!consume(")")){
            node->forh3 = expr();
            expect(")");
        }
        node->elh = stmt();

    }else if (consume("{")){  // nd_block
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        Node **block;
        block = calloc(100, sizeof(Node*));
        int i = 0;
        while (!consume("}")){
            block[i] = stmt();
            i++;
        }
        node->block = block;
        node->block_len = i;

    }else{
        node = expr();
        expect(";");
    }
    return node;
}

void program(){
    int i = 0;
    while (!at_eof()){
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node *primary(){
    if (token->kind == TK_IDENT){
        Token *tok = consume_ident();
        if (tok){
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_LVAR;
            LVar *lvar = find_lvar(tok);
            if (lvar){
                node->offset = lvar->offset;
            }else{
                lvar = calloc(1, sizeof(LVar));
                lvar->next = locals;
                lvar->name = tok->str;
                lvar->len = tok->len;
                lvar->offset = locals->offset + 8;
                node->offset = lvar->offset;
                locals = lvar;
            }
            return node;
        }

    }

    //次のトークンが"("なら、"(" expr ")" のはず
    if (consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    //そうでなければ数値のはず
    return new_node_num(expect_number());
}

Node *add(){
    Node *node = mul();
    for (;;){
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul(){
    Node *node = unary();

    for (;;){
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary(){
    if (consume("+")){
        return primary();
    }
    if (consume("-")){
        return new_node(ND_SUB, new_node_num(0), primary());
    }

    return primary();
}

Node *equality(){
    Node *node = relational();

    for (;;){
        if (consume("==")){
            node = new_node(ND_EQ, node, relational());
        }else if (consume("!=")){
            node = new_node(ND_NE, node, relational());
        }else{
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();
    for (;;){
        if (consume("<")){
            node = new_node(ND_LT, node, add());
        }else if (consume("<=")){
            node = new_node(ND_LE, node, add());
        }else if (consume(">")){
            node = new_node(ND_LT, add(), node);
        }else if (consume(">=")){
            node = new_node(ND_LE, add(), node);
        }else{
            return node;
        }
    }
}

Node *expr(){
    return assign();
}
