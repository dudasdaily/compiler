#include "codegenerate.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *white_space = "        ";
int label_count = 0;

void print_msg(const char *msg, char *label)
{
    if (label == NULL)
        printf("%s %s;\n", white_space, msg);
    else
        printf("%s %s\n", msg, label);
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
        while (son != NULL)
        {
            if (strcmp(son->kind, "VARDECL") == 0 ||
                strcmp(son->kind, "COMP") == 0)
            {
                int local_var_count = 0;
                Node *body = son;

                if (strcmp(son->kind, "VARDECL") == 0)
                {
                    local_var_count = son->value.dv;
                    body = son->bro;
                }

                char msg[50];
                snprintf(msg, sizeof(msg), "ssp %d", local_var_count + 2);
                print_msg(msg, NULL);

                code(body);
                print_msg("stp", NULL);
                break;
            }

            code(son);
            son = son->bro;
        }
    }
    else if (strcmp(ptr->kind, "ASSIGN") == 0)
    {
        if (son == NULL)
            return;

        code_L(son);
        code_R(son->bro);
        print_msg("sto", NULL);
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
    else if (strcmp(ptr->kind, "PROC") == 0)
    {
        if (son == NULL)
            return;

        char *procName = son->value.sv;
        int param_count = 0;
        int local_var_count = 0;
        Node *body = NULL;

        // son is Name. son->bro is first param (if any) or VarDecl or Comp
        Node *curr = son->bro;
        while (curr != NULL)
        {
            if (strcmp(curr->kind, "DECL") == 0)
            {
                param_count += curr->value.dv;
            }
            else if (strcmp(curr->kind, "VARDECL") == 0)
            {
                local_var_count += curr->value.dv;
            }
            else if (strcmp(curr->kind, "COMP") == 0)
            {
                body = curr;
                break;
            }
            curr = curr->bro;
        }

        char label[100];
        snprintf(label, sizeof(label), "l_%s:", procName);
        int label_len = strlen(label);
        int ws_len = strlen(white_space);
        int spaces = ws_len - label_len;
        if (spaces < 0)
            spaces = 0;

        printf("%s", label);
        for (int i = 0; i < spaces; i++)
            printf(" ");
        printf("ssp %d;\n", param_count + local_var_count + 2);

        if (body != NULL)
            code(body);

        print_msg("retp", NULL);
    }
    else if (strcmp(ptr->kind, "FUNC") == 0)
    {
        if (son == NULL)
            return;

        char *procName = son->value.sv;
        int param_count = 0;
        int local_var_count = 0;
        Node *body = NULL;

        Node *curr = son->bro;
        while (curr != NULL)
        {
            if (strcmp(curr->kind, "DECL") == 0)
            {
                param_count += curr->value.dv;
            }
            else if (strcmp(curr->kind, "VARDECL") == 0)
            {
                local_var_count += curr->value.dv;
            }
            else if (strcmp(curr->kind, "COMP") == 0)
            {
                body = curr;
                break;
            }
            curr = curr->bro;
        }

        char label[100];
        snprintf(label, sizeof(label), "l_%s:", procName);
        int label_len = strlen(label);
        int ws_len = strlen(white_space);
        int spaces = ws_len - label_len;
        if (spaces < 0)
            spaces = 0;

        printf("%s", label);
        for (int i = 0; i < spaces; i++)
            printf(" ");
        printf("ssp %d;\n", param_count + local_var_count + 2);

        if (body != NULL)
            code(body);

        print_msg("retf", NULL);
    }
    else if (strcmp(ptr->kind, "RETURN") == 0)
    {
        if (son == NULL)
            return;

        code_R(son);
        print_msg("str 0", NULL);
        print_msg("retf", NULL);
    }
    else if (strcmp(ptr->kind, "IF") == 0)
    {
        if (son == NULL)
            return;

        if (son->bro->bro)
        {
        }

        else
        {
        }
    }
    else if (strcmp(ptr->kind, "CALL") == 0)
    {
        if (son == NULL)
            return;

        print_msg("mst", NULL);

        char *procName = son->value.sv;
        int arg_count = 0;
        Node *arg = son->bro;

        while (arg != NULL)
        {
            code_R(arg);
            arg = arg->bro;
            arg_count++;
        }

        char msg[100];
        snprintf(msg, sizeof(msg), "cup %d l_%s", arg_count, procName);
        print_msg(msg, NULL);
    }
    else if (strcmp(ptr->kind, "FOR") == 0)
    {
    }
    else if (strcmp(ptr->kind, "WHILE") == 0)
    {
        if (son == NULL)
            return;
        if (son->bro == NULL)
            return;

        // 라벨 2개 생성
        char Lbegin[32];
        char Lend[32];
        snprintf(Lbegin, sizeof(Lbegin), "L%d", label_count++);
        snprintf(Lend, sizeof(Lend), "L%d", label_count++);

        // loop 시작 라벨
        print_msg("lab", Lbegin);

        // condition 평가 (결과가 스택에 올라온다고 가정)
        code_R(son);

        // cond가 false면 루프 종료 라벨로 점프
        char msg[64];
        snprintf(msg, sizeof(msg), "fjp %s", Lend);
        print_msg(msg, NULL);

        code(son->bro);

        // 다시 조건 검사로 점프
        snprintf(msg, sizeof(msg), "ujp %s", Lbegin);
        print_msg(msg, NULL);

        // loop 종료 라벨
        print_msg("lab", Lend);
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
    else if (strcmp(ptr->kind, "FUNC_CALL") == 0)
    {
        print_msg("mst", NULL);

        char *funcName = son->value.sv;
        int arg_count = 0;
        Node *arg = son->bro;

        while (arg != NULL)
        {
            code_R(arg);
            arg = arg->bro;
            arg_count++;
        }

        char msg[100];
        snprintf(msg, sizeof(msg), "cup %d l_%s", arg_count, funcName);
        print_msg(msg, NULL);
    }

    // 식 산술연산 식
    else if (strcmp(ptr->kind, "PLUS") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "add";
        print_msg(msg, NULL);
    }
    else if (strcmp(ptr->kind, "MINUS") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "minus";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "MUL") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "mul";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "DIV") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "div";
        print_msg(msg, NULL);
    }

    // 식 비교연산 식
    else if (strcmp(ptr->kind, "EQ") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "equ";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "LT") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "lt";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "LE") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "leq";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "GT") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "gt";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "GE") == 0)
    {
        code_R(son);
        code_R(son->bro);

        char *msg = "geq";
        print_msg(msg, NULL);
    }

    else if (strcmp(ptr->kind, "NE") == 0)
    {
        code_R(son);
        code_R(son->bro);

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