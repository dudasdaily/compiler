#ifndef CODEGENERATE_H
#define CODEGENERATE_H
#include "node.h"

void print_msg(const char *msg, char *label);
void code(Node *ptr);
void code_L(Node *ptr);
void code_R(Node *ptr);
#endif