#include "takcc.h"

Token *token;
char *user_input;
LVar *locals;
int LelseNum;
int LendNum;
int LbeginNum;

int main(int argc, char **argv){
    if (argc != 2){
        error("引数の個数が正しくありません");
        return 1;
    }

    locals = calloc(1, sizeof(LVar));
    locals->next = NULL;

    //トークナイズしてパースする
    user_input = argv[1];
    token = tokenize(user_input);
    //Node *node = program();
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    //変数26個分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    //先頭の式から順にコード生成
    for (int i=0;code[i];i++){
        gen(code[i]);

        //式の評価結果としてスタックに一つの値が残っている
        //はずなので、スタックが溢れないようにポップしておく
        printf("  pop rax\n");
    }

    //最後の式の結果がraxに残っているのでそれが返り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

    return 0;
}
