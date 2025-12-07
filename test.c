#include <stdio.h>
#include <stdlib.h>

int main()
{
    int size = 10;
    int *stack = malloc(sizeof(int) * size);

    for (int i = 0; i < size; i++)
    {
        if (stack[i] == NULL)
            printf("%d\n", stack[i]);
    }
    return 0;
}