#include <stdio.h>

#define MAX_BUFFER_SIZE 100

typedef struct {
    int idx;
    int buffer[MAX_BUFFER_SIZE];
} Stack;

Stack empty_stack() {
    Stack stack;
    stack.idx = 0;

    return stack;
}

int push(Stack* stack, int data) {
    if (stack->idx >= MAX_BUFFER_SIZE) {
        return 0;
    }
    stack->buffer[stack->idx++] = data;
    return 1;
}

int pop(Stack* stack, int* res) {
    if (stack->idx < 1) {
        return 0;
    }
    
    --stack->idx;
    if (res != NULL) {
        *res = stack->buffer[stack->idx];
    }

    return 1;
}

int empty(Stack* stack) {
    return stack->idx < 1;
}

int top(Stack* stack) {
    return stack->buffer[stack->idx - 1];
}

int prec(char oper) {
    switch (oper) {
    case '*':
    case '/':
    case '%':
        return 2;
    case '+':
    case '-':
        return 1;
    default:
        return 0;
    }
}

int operate(char oper, int n1, int n2) {
    switch (oper) {
    case '*':
        return n1 * n2;
    case '/':
        return n1 / n2;
    case '%':
        return n1 % n2;
    case '+':
        return n1 + n2;
    case '-':
        return n1 - n2;
    default:
        return 0;
    }
}

int make_postfix(char* output, char* input) {
    int res;

    int i = -1;
    int idx = 0;

    Stack oper_stack = empty_stack();

    while (input[++i] != '#') {
        if (input[i] >= '0' && input[i] <= '9') {
            output[idx++] = input[i];
        }
        else if (input[i] == '(') {
            push(&oper_stack, '(');
        }
        else if (input[i] == ')') {
            while (top(&oper_stack) != '(') {
                output[idx++] = top(&oper_stack);
                pop(&oper_stack, NULL);
            }
            pop(&oper_stack, NULL);
        }
        else if (empty(&oper_stack) || prec(top(&oper_stack)) < prec(input[i])) {
            push(&oper_stack, input[i]);
        }
        else {
            while (prec(top(&oper_stack)) >= prec(input[i])) {
                pop(&oper_stack, &res);
                output[idx++] = res;
            }
            push(&oper_stack, input[i]);
        }
    }

    while (!empty(&oper_stack)) {
        pop(&oper_stack, &res);
        output[idx++] = res;
    }

    return idx;
}

int calc_postfix(char* input, int len) {
    Stack num_stack = empty_stack();

    int i, n1, n2;
    for (i = 0; i < len; ++i) {
        if (input[i] >= '0' && input[i] <= '9') {
            push(&num_stack, input[i] - '0');
        }
        else {
            pop(&num_stack, &n2);
            pop(&num_stack, &n1);
            push(&num_stack, operate(input[i], n1, n2));
        }
    }

    return top(&num_stack);
}

int main() {
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    char infix[MAX_BUFFER_SIZE + 1] = { 0, };
    fscanf(input, "%s", infix);

    char postfix[MAX_BUFFER_SIZE + 1] = { 0, };
    int size = make_postfix(postfix, infix);

    int result = calc_postfix(postfix, size);

    int idx = -1;
    while (infix[++idx] != '#');
    infix[idx] = 0;

    fprintf(output, "Infix Form : %s\n", infix);
    fprintf(output, "Postfix Form : %s\n", postfix);
    fprintf(output, "Evaluation Result : %d\n", result);

    return 0;
}