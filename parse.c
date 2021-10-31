#include "takcc.h"
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>


// 次のトークンが期待している記号のときには、トークンを一つ読み進めて
// 真を返す。それ以外の場合には偽を返す
bool consume(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    token = token->next;
    return true;
}

// 次のトークンが期待している記号のときには、トークンを一つ読み進める
// それ以外の場合にはエラーを報告する
void expect(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op){
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

Node *primary(){
    //次のトークンが"("なら、"(" expr ")" のはず
    if (consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }

    //そうでなければ数値のはず
    return new_node_num(expect_number());
}

Node *mul(){
    Node *node = primary();

    for (;;){
        if (consume('*')){
            node = new_node(ND_MUL, node, primary());
        }else if (consume('/')){
            node = new_node(ND_DIV, node, primary());
        }else{
            return node;
        }
    }
}

Node *expr(){
    Node *node = mul();

    for (;;){
        if (consume('+')){
            node = new_node(ND_ADD, node, mul());
        }else if (consume('-')){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p){
        //空白文字をスキップ
        if (isspace(*p)){
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("tokenizeできません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}
