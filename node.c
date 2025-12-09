#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *makeNode(char *kind, Node *bro, Node *son)
{
    Node *temp;

    temp = (Node *)malloc(sizeof(Node));
    strcpy(temp->kind, kind);
    temp->bro = bro;
    temp->son = son;

    return temp;
}