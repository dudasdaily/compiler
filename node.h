#ifndef NODE_H
#define NODE_H

typedef struct _node
{
    char kind[10];
    union
    {
        double dv;
        char *sv;
    } value;
    struct _node *bro;
    struct _node *son;
} Node;

Node *makeNode(char *, Node *, Node *);
#endif