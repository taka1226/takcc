#ifndef TAKCC_H
#define TAKCC_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN, // =
    ND_LVAR, // ローカル変数
    ND_NUM, // 整数
    ND_RETURN, // return
    ND_IF,  // if
    ND_WHILE, // while
    ND_BLOCK, //{}
    ND_FOR, // for
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *elh;  // else, for のときだけ使う
    Node *forh1; //for 1
    Node *forh2; // for 2
    Node *forh3; //for3
    Node **block;
    int block_len; // block の行数
    int val;  //kindがND_NUMの場合のみ使う
    int offset; // kindがND_LVARの場合のみ使う
};

typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,  // 識別子
    TK_NUM, // 整数トークン
    TK_RETURN, // return トークン
    TK_IF,  // if トークン
    TK_ELSE,  // else トークン
    TK_WHILE, // while トークン
    TK_FOR, // for トークン
    TK_BLOCK,  //{}
    TK_EOF, // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

typedef struct LVar LVar;

//ローカル変数の型
struct LVar{
    LVar *next; //次の変数がNULL
    char *name;
    int len;
    int offset;
};

//ローカル変数
extern LVar *locals;

extern Token *token;
extern char *user_input;
extern Node *code[100];
extern int LelseNum;
extern int LendNum;
extern int LbeginNum;

Node *assign();
Node *stmt();
void program();
Node *primary();
Node *add();
Node *mul();
Node *unary();
Node *equality();
Node *relational();
Node *expr();
void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void gen(Node *node);
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);
Token *consume_ident();
LVar *find_lvar(Token *tok);
int is_alnum(char c);

#endif
