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

typedef struct {
    ThreadedTree* tree;
    ThreadedTree* parent;
} TreePair;

typedef struct PairList_ {
    TreePair pair;
    struct PairList_* next;
} PairList;

typedef struct {
    PairList* front;
    PairList* back;
} Queue;

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

TreePair empty_tree_pair() {
    TreePair pair;
    pair.tree = NULL;
    pair.parent = NULL;
    return pair;
}

Queue make_queue() {
    Queue queue;
    queue.back = NULL;
    queue.front = NULL;
    return queue;
}

void push_back(Queue* queue, ThreadedTree* tree, ThreadedTree* parent) {
    PairList* list = malloc(sizeof(PairList));
    list->next = NULL;
    list->pair.tree = tree;
    list->pair.parent = parent;

    if (queue->back == NULL) {
        queue->front = queue->back = list;
    } else {
        queue->back->next = list;
        queue->back = list;
    }
}

TreePair pop_front(Queue* queue) {
    PairList* front = queue->front;
    if (front == NULL) {
        return empty_tree_pair();
    }

    TreePair ret = front->pair;
    if (front == queue->back) {
        queue->back = NULL;
    }
    queue->front = front->next;

    free(front);
    return ret;
}

TreePair front(Queue* queue) {
    return queue->front->pair;
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
    Queue queue = make_queue();
    push_back(&queue, tree, NULL);
    
    TreePair pair;
    while (1) {
        pair = pop_front(&queue);
        tree = pair.tree;
        
        if (tree == NULL) {
            break;
        }
        push_back(&queue, tree->left, tree);
        push_back(&queue, tree->right, tree);
    }

    if (pair.parent->left == tree) {
        return &pair.parent->left;
    } else {
        return &pair.parent->right;
    }
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
        } else {
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

FILE* output;
void print(char data) {
    fprintf(output, "%c  ", data);
}

int main() {
    output = fopen("output.txt", "w");
    FILE* input = fopen("input.txt", "r");

    int n_list;
    fscanf(input, "%d", &n_list);
    fgetc(input);

    int i;
    char data[2];
    ThreadedTree* root;

    if (n_list > 0) {
        fscanf(input, "%s", data);
        root = make_tree(data[0]);

        for (i = 1; i < n_list; ++i) {
            fscanf(input, "%s", data);
            insert_node(root, data[0]);
        }

        make_inorder_threaded(root);
        traverse(root, print);
    }

    fprintf(output, "\n");
    return 0;
}
