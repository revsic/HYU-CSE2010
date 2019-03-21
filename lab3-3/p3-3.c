#include <stdio.h>

#define MAX_BUFFER_SIZE 100

// Struct for Stack data structure implementation.
typedef struct {
    int idx;
    int buffer[MAX_BUFFER_SIZE];
} Stack;

// Generate empty stack. Initialize with start index 0.
Stack empty_stack() {
    Stack stack;
    stack.idx = 0;

    return stack;
}

// Push `data` into `stack`.
// Returns:
//     1 for success.
//     0 for failure.
// Failure:
//     Try pushing data to full stack.
int push(Stack* stack, int data) {
    if (stack->idx >= MAX_BUFFER_SIZE) {
        return 0;
    }
    stack->buffer[stack->idx++] = data;
    return 1;
}

// Pop element from `stack` and store it to `res`.
// Returns:
//     1 for success.
//     0 for failure.
// Failure:
//     Try popping from empty stack.
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

// Validate if stack is empty.
int empty(Stack* stack) {
    return stack->idx < 1;
}

// Get top element from stack.
int top(Stack* stack) {
    return stack->buffer[stack->idx - 1];
}

// Get precedence from given operator.
// Returns:
//     2 for *, /, %
//     1 for +, -
//     0 for otherwise
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

// Calculate operator with given operands.
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

// Convert infix form `input` to postfix form `output`.
// Condition:
//     #-terminated string `input`
int make_postfix(char* output, char* input) {
    int res;

    int i = -1;
    int idx = 0;

    Stack oper_stack = empty_stack();

    // #-terminated string
    while (input[++i] != '#') {
        // If digit
        if (input[i] >= '0' && input[i] <= '9') {
            output[idx++] = input[i];
        }
        // If open parenthesis
        else if (input[i] == '(') {
            push(&oper_stack, '(');
        }
        // If close parenthesis
        else if (input[i] == ')') {
            // Until finding open parenthesis
            while (top(&oper_stack) != '(') {
                output[idx++] = top(&oper_stack);
                pop(&oper_stack, NULL);
            }
            // Pop open parenthesis
            pop(&oper_stack, NULL);
        }
        // If stack is empty
        // If precedence of top operator is lower than current operator.
        else if (empty(&oper_stack) || prec(top(&oper_stack)) < prec(input[i])) {
            push(&oper_stack, input[i]);
        }
        // If precedence of top operator is higher than current operator.
        else {
            // Until lower precedence operator appears 
            while (prec(top(&oper_stack)) >= prec(input[i])) {
                pop(&oper_stack, &res);
                output[idx++] = res;
            }
            push(&oper_stack, input[i]);
        }
    }

    // Clear operator stack
    while (!empty(&oper_stack)) {
        pop(&oper_stack, &res);
        output[idx++] = res;
    }

    return idx;
}

// Calculate postfix form `input`.
int calc_postfix(char* input, int len) {
    Stack num_stack = empty_stack();

    int i, n1, n2;
    for (i = 0; i < len; ++i) {
        // If digit
        if (input[i] >= '0' && input[i] <= '9') {
            push(&num_stack, input[i] - '0');
        }
        // If operator
        else {
            pop(&num_stack, &n2);
            pop(&num_stack, &n1);
            push(&num_stack, operate(input[i], n1, n2));
        }
    }

    return top(&num_stack);
}

int main() {
    // Prepare for file I/O.
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    char infix[MAX_BUFFER_SIZE + 1] = { 0, };
    fscanf(input, "%s", infix);

    // Convert to postfix form.
    char postfix[MAX_BUFFER_SIZE + 1] = { 0, };
    int size = make_postfix(postfix, infix);

    // Calculate postfix form.
    int result = calc_postfix(postfix, size);

    // Convert #-terminated string to NULL-terminated string
    int idx = -1;
    while (infix[++idx] != '#');
    infix[idx] = 0;

    // Log
    fprintf(output, "Infix Form : %s\n", infix);
    fprintf(output, "Postfix Form : %s\n", postfix);
    fprintf(output, "Evaluation Result : %d\n", result);

    fclose(input);
    fclose(output);

    return 0;
}