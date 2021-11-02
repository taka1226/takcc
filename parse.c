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
    Node *node = expr();
    expect(";");
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
            node->offset = (tok->str[0] - 'a' + 1) * 8;
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
            new_node(ND_LT, node, add());
        }else if (consume("<=")){
            new_node(ND_LE, node, add());
        }else if (consume(">")){
            new_node(ND_LT, add(), node);
        }else if (consume(">=")){
            new_node(ND_LE, add(), node);
        }else{
            return node;
        }
    }
}

Node *expr(){
    return assign();
}
