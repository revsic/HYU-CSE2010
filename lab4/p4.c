#include <stdio.h>
#include <stdlib.h>

#define LEFT 0
#define RIGHT 1

#define GET 0
#define SET 1

typedef unsigned char byte;

typedef struct ThreadedTree_ {
    byte thread;
    char data;
    struct ThreadedTree_* left;
    struct ThreadedTree_* right;
} ThreadedTree;

typedef struct List_ {
    ThreadedTree* tree;
    struct List_* next;
} List;

typedef struct {
    List* top;
} Stack;

Stack make_stack() {
    Stack stack;
    stack.top = NULL;
    return stack;
}

void push(Stack* stack, ThreadedTree* tree) {
    List* head = malloc(sizeof(List));
    head->next = stack->top;
    head->tree = tree;
    stack->top = head;
}

ThreadedTree* pop(Stack* stack) {
    if (stack->top == NULL) {
        return NULL;
    }

    List* head = stack->top;
    stack->top = head->next;
    ThreadedTree* data = head->tree;

    free(head);
    return data;
}

ThreadedTree* top(Stack* stack) {
    return stack->top->tree;
}

int threaded(ThreadedTree* tree, int dir, int set) {
    if (set == SET) {
        tree->thread |= (1 << dir);
    }
    return tree->thread & (1 << dir);
}

ThreadedTree* make_tree(char data) {
    ThreadedTree* tree = malloc(sizeof(ThreadedTree));
    tree->thread = 0;
    tree->data = data;
    tree->left = NULL;
    tree->right = NULL;
    return tree;
}

ThreadedTree** insert_point(ThreadedTree* tree) {
    return NULL;
}

void insert_node(ThreadedTree* tree, char data) {
    ThreadedTree** node = insert_point(tree);
    *node = make_tree(data);
}

void make_inorder_threaded(ThreadedTree* node) {
    int already_pop = 0;
    ThreadedTree* prev = NULL;
    Stack stack = make_stack();
    while (1) {
        if (already_pop) {
            already_pop = 0;
        }
        else {
            for (; node; node = node->left) {
                push(&stack, node);
            }
            node = pop(&stack);
        }

        if (node == NULL) {
            break;
        }

        if (node->left == NULL) {
            threaded(node, LEFT, SET);
            node->left = prev;
        }
        if (node->right == NULL) {
            threaded(node, RIGHT, SET);
            node->right = pop(&stack);
            already_pop = 1;
        }

        prev = node;
        node = node->right;
    }
}

ThreadedTree* next_node(ThreadedTree* node) {
    ThreadedTree* tmp = node->right;
    if (!threaded(node, RIGHT, GET)) {
        while (!threaded(tmp, LEFT, GET)) {
            tmp = tmp->left;
        }
    }
    return tmp;
}

void traverse(ThreadedTree* node, void(*func)(char)) {
    while (!threaded(node, LEFT, GET)) {
        node = node->left;
    }

    while (1) {
        func(node->data);
        node = next_node(node);
        if (node == NULL) {
            break;
        }
    }
}

void print(char data) {
    printf("%c  ", data);
}

int main() {
    ThreadedTree* root = make_tree('A');
    root->left = make_tree('B');
    root->right = make_tree('C');
    root->left->left = make_tree('D');
    root->left->right = make_tree('E');
    root->right->left = make_tree('F');
    root->right->right = make_tree('G');
    root->left->left->left = make_tree('H');
    root->left->left->right = make_tree('I');

    make_inorder_threaded(root);
    traverse(root, print);

    printf("\n");
    return 0;
}
