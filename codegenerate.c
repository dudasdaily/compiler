#include "codegenerate.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *white_space = "        ";

void print_msg(const char *msg, char *label)
{
    if (label == NULL)
        printf("%s", white_space);
    else
        return; // 아직 label 구현 안함!

    printf("%s;\n", msg);
}

// Statement를 처리하는 함수
void code(Node *ptr)
{
    if (ptr == NULL)
        return;

    Node *son = ptr->son; // 왼쪽 자식
    Node *bro = ptr->bro; // 오른쪽 자식

    if (strcmp(ptr->kind, "PROGRAM") == 0)
    {
        if (strcmp(son->kind, "VARDECL") == 0)
            son = son->bro; // VARDECL은 코드를 생성하지 않으므로 pass!

        code(son); // 다음 문장을 넘김
    }
    else if (strcmp(ptr->kind, "ASSIGN") == 0)
    {
        if (son == NULL)
            return;

        code_L(son);
        code_R(son->bro);
    }
    else if (strcmp(ptr->kind, "COMP") == 0)
    {
        if (son == NULL)
            return;

        code(son);
        while (son->bro != NULL)
        {
            son = son->bro;
            code(son);
        }
    }
    else if (strcmp(ptr->kind, "RETURN") == 0)
    {
        if (son == NULL)
            return;

        code_R(son);
        print_msg("str", NULL);
        print_msg("retf", NULL);
    }
    else if (strcmp(ptr->kind, "IF") == 0)
    {
    }
    else if (strcmp(ptr->kind, "PCALL") == 0)
    {
    }
    else if (strcmp(ptr->kind, "FOR") == 0)
    {
    }
    else if (strcmp(ptr->kind, "WHILE") == 0)
    {
    }
}

// l-value
void code_L(Node *ptr)
{
    if (ptr == NULL)
        return;

    // NAME이 아니면 pass
    if (strcmp(ptr->kind, "NAME") == 1)
        return;

    /* 출력 메시지 포멧팅 시작 */
    int length = strlen(ptr->value.sv) + 6 + 1; // 6: ldc (), 1: NULL문자
    char *msg = (char *)malloc(sizeof(char) * length);
    if (msg == NULL)
        return;
    sprintf(msg, "ldc (%s)", ptr->value.sv);
    /* 출력 메시지 포멧팅 끝 */

    print_msg(msg, NULL); // ldc (value) 출력!
    free(msg);
}

// r-value
void code_R(Node *ptr)
{
    if (ptr == NULL)
        return;

    Node *son = ptr->son;
    Node *bro = ptr->bro;

    if (strcmp(ptr->kind, "NAME") == 0)
    {
        code_L(ptr);
        print_msg("ind", NULL);
    }

    else if (strcmp(ptr->kind, "NUMBER") == 0)
    {
        char *msg = malloc(sizeof(char) * 100);
        double value = ptr->value.dv;
        sprintf(msg, "ldc (%g)", value);
        print_msg(msg, NULL);
    }

    // 식 산술연산 식
    else if (strcmp(ptr->kind, "PLUS") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "add";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "MINUS") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "minus";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "MUL") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "mul";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "DIV") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "div";
        print_msg(msg, NULL);
    }

    // 식 비교연산 식
    else if (strcmp(ptr->kind, "EQ") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "equ";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "LT") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "lt";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "LE") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "leq";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "GT") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "gt";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "GE") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "geq";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "NE") == 0)
    {
        code_R(son);
        code_R(bro);

        char *msg = "neq";
        print_msg(msg, NULL);
    }

    // !식
    else if (strcmp(ptr->kind, "NEG") == 0)
    {
        code_R(son);
        char *msg = "neg";
        print_msg(msg, NULL);
    }
}