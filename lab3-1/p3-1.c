#include <stdio.h>
#include <string.h>

#define MAX_STACK_SIZE 100

// Struct for Stack data structure implementation.
typedef struct {
    int idx;
    int buffer[MAX_STACK_SIZE];
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
//     Push to full stack.
int push(Stack* stack, int data) {
    if (stack->idx >= MAX_STACK_SIZE) {
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
//     Try to pop from empty stack.
int pop(Stack* stack, int* res) {
    if (stack->idx < 1) {
        return 0;
    }
    *res = stack->buffer[--stack->idx];
    return 1;
}

int main() {
    // Prepare for file I/O.
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    int n_query;
    fscanf(input, "%d", &n_query);

    int num, res;
    char option[10] = { 0, };

    // Generate empty stack.
    Stack stack = empty_stack();

    int i;
    for (i = 0; i < n_query; ++i) {
        fscanf(input, "%s", option);

        // Push proper data to stack.
        if (!strcmp(option, "push")) {
            fscanf(input, "%d", &num);
            res = push(&stack, num);

            // If failure (Full of stack)
            if (res == 0) {
                fprintf(output, "Full\n");
            }
        }
        // Pop proper data from stack.
        else if (!strcmp(option, "pop")) {
            res = pop(&stack, &num);

            // If failure (Empty of stack)
            if (res == 0) {
                fprintf(output, "Empty\n");
            }
            // If Success
            else {
                fprintf(output, "%d\n", num);
            }
        }
    }

    fclose(input);
    fclose(output);

    return 0;
}
