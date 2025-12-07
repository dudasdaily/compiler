typedef struct
{
    int capacity; // 스택의 최대 용량
    int size;     // 현재 스택의 사이즈 [0 ~ capacity]
    Stack *top;   // 스택의 탑
} Stack;

void push(int value);
void pop();
void get_value();