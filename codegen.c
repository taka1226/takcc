#include "takcc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Node **in_block;

// エラーを報告するための関数
// printf と同じ引数をとる
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void gen_lval(Node *node){
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node){
    if (node->kind == ND_RETURN){
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }
    switch (node->kind){
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            return;
        case ND_EQ:
            gen(node->lhs);
            gen(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  cmp rdi, rax\n");
            printf("  sete al\n");
            printf("  push rax\n");
            return;

        case ND_NE:
            gen(node->lhs);
            gen(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  push rax\n");
            return;
        case ND_LT:
            gen(node->lhs);
            gen(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  push rax\n");
            return;

        case ND_LE:
            gen(node->lhs);
            gen(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  push rax\n");
            return;

        case ND_IF:
            // else があれば、
            if (node->elh){
                gen(node->lhs);
                printf("  pop rax\n");
                printf("  cmp rax, 0\n");
                printf("  je .Lelse%d\n", LelseNum);
                gen(node->rhs);
                printf("  jmp .Lend%d\n", LendNum);
                printf(".Lelse%d:\n", LelseNum);
                gen(node->elh);
                printf(".Lend%d:\n", LendNum);
                LelseNum++;
                LendNum++;
                return;

            }else{
                gen(node->lhs);
                printf("  pop rax\n");
                printf("  cmp rax, 0\n");
                printf("  je .Lend%d\n", LendNum);
                gen(node->rhs);
                printf(".Lend%d:\n", LendNum);
                LendNum++;
                return;
            }

        case ND_WHILE:
            printf(".Lbegin%d:\n", LbeginNum);
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", LendNum);
            gen(node->rhs);
            printf("  jmp .Lbegin%d\n", LbeginNum);
            printf(".Lend%d\n", LendNum);
            LbeginNum++;
            LendNum++;
            return;
        case ND_FOR:
            gen(node->forh1);
            printf(".Lbegin%d:\n", LbeginNum);
            gen(node->forh2);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", LendNum);
            gen(node->elh);
            gen(node->forh3);
            printf("  jmp .Lbegin%d\n", LbeginNum);
            printf(".Lend%d:\n", LendNum);
            LbeginNum++;
            LendNum++;
            return;

        case ND_BLOCK:
            in_block = node->block;
            for (int i=0;i < node->block_len;i++){
                gen(in_block[i]);
            }
            return;

    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind){
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;

    }

    printf("  push rax\n");
}
