#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 100

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

void init(Stack *s) {
    s->top = -1;
}

void push(Stack *s, int value) {
    if (s->top >= STACK_SIZE - 1) {
        printf("Stack overflow\n");
        exit(1);
    }
    s->data[++s->top] = value;
}

int pop(Stack *s) {
    if (s->top < 0) {
        printf("Stack underflow\n");
        exit(1);
    }
    return s->data[s->top--];
}

int main() {
    Stack s;
    init(&s);

    push(&s, 5);
    push(&s, 3);
    int b = pop(&s);
    int a = pop(&s);
    push(&s, a + b);
    push(&s, 2);
    b = pop(&s);
    a = pop(&s);
    push(&s, a * b);

    printf("%d\n", pop(&s));

    return 0;
}
