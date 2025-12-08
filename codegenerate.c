#include "node.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *white_space = "        ";

void print_msg(const char *msg, char *label)
{
    if (label == NULL)
        printf(white_space);
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
        code(son);

        if (son->bro)
            code(son->bro);
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

    // ldc (value) 출력!
    print_msg(msg, NULL);
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

    else if (strcmp(ptr->kind, "NUMBER"))
    {
    }
}