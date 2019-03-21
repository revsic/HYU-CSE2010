#include <stdio.h>
#include <string.h>

#define MAX_STACK_SIZE 100

typedef struct {
    int idx;
    int buffer[MAX_STACK_SIZE];
} Stack;

Stack empty_stack() {
    Stack stack;
    stack.idx = 0;

    return stack;
}

int push(Stack* stack, int data) {
    if (stack->idx >= MAX_STACK_SIZE) {
        return 0;
    }
    stack->buffer[stack->idx++] = data;
    return 1;
}

int pop(Stack* stack, int* res) {
    if (stack->idx < 1) {
        return 0;
    }
    *res = stack->buffer[--stack->idx];
    return 1;
}

int main() {
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    int n_query;
    fscanf(input, "%d", &n_query);

    int num, res;
    char option[10] = { 0, };

    Stack stack = empty_stack();

    for (int i = 0; i < n_query; ++i) {
        fscanf(input, "%s", option);

        if (!strcmp(option, "push")) {
            fscanf(input, "%d", &num);
            res = push(&stack, num);
            if (res == 0) {
                fprintf(output, "Full\n");
            }
        }
        else if (!strcmp(option, "pop")) {
            res = pop(&stack, &num);
            if (res == 0) {
                fprintf(output, "Empty\n");
            }
            else {
                fprintf(output, "%d\n", num);
            }
        }
    }

    return 0;
}