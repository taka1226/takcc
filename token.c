#include "takcc.h"

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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';'){
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])){
            cur = new_token(TK_RETURN, cur, p);
            cur->len = 6;
            p += 6;
            continue;
        }

        //複数の文字列をもつ変数を扱うため
        int ident_len = 0;
        char *tmp_p;
        while ('a' <= *p && *p <= 'z'){
            if (ident_len == 0) tmp_p = p;
            p++;
            ident_len++;
        }
        if (ident_len > 0){
            cur = new_token(TK_IDENT, cur, tmp_p);
            cur->len = ident_len;
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


LVar *find_lvar(Token *tok){
    for (LVar *var = locals; var; var=var->next){
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
}

int is_alnum(char c){
    return ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z') ||
        ('0' <= c && c <= '9') ||
        (c == '_');
}
