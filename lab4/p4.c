#include <stdio.h>
#include <stdlib.h>

// Macro for left or right direction of binary tree
#define LEFT 0
#define RIGHT 1

// Macro for getter option or setter option for function `threaded`
#define GET 0
#define SET 1

// type alias for byte type
typedef unsigned char byte;

// Threaded Tree
typedef struct ThreadedTree_ {
    byte thread;
    char data;
    struct ThreadedTree_* left;
    struct ThreadedTree_* right;
} ThreadedTree;

// Linked list for structure Stack
typedef struct List_ {
    ThreadedTree* tree;
    struct List_* next;
} List;

// Stack structure with linked list
typedef struct {
    List* top;
} Stack;

// Pair object for finding insertion point
typedef struct {
    ThreadedTree* tree;
    ThreadedTree* parent;
} TreePair;

// Linked list for structure Queue
typedef struct PairList_ {
    TreePair pair;
    struct PairList_* next;
} PairList;

// Queue structure with linked list
typedef struct {
    PairList* front;
    PairList* back;
} Queue;

// Generate empty stack
Stack make_stack() {
    Stack stack;
    stack.top = NULL;
    return stack;
}

// Delete stack
void delete_stack(Stack* stack) {
    List* next;
    while (stack->top) {
        next = stack->top->next;
        free(stack->top);
        stack->top = next;
    }
}

// Push object to stack
void push(Stack* stack, ThreadedTree* tree) {
    List* head = malloc(sizeof(List));
    head->next = stack->top;
    head->tree = tree;
    stack->top = head;
}

// Pop object from stack
// Returns:
//     NULL, if stack is empty
//     ThreadedTree*, if otherwise
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

// Get top object of stack.
// Exceptions:
//     if stack is empty
ThreadedTree* top(Stack* stack) {
    return stack->top->tree;
}

// Generate empty TreePair
TreePair empty_tree_pair() {
    TreePair pair;
    pair.tree = NULL;
    pair.parent = NULL;
    return pair;
}

// Generate empty queue
Queue make_queue() {
    Queue queue;
    queue.back = NULL;
    queue.front = NULL;
    return queue;
}

// Delete queue
void delete_queue(Queue* queue) {
    PairList* pair;
    while (queue->front != queue->back) {
        pair = queue->front->next;
        free(queue->front);
        queue->front = pair;
    }

    if (queue->front != NULL) {
        free(queue->front);
    }
}

// Enqueue object to queue
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

// Dequeue object from queue
// Returns:
//     empty_tree_pair(), if queue is empty
//     TreePair, if otherwise
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

// Get front object of queue
// Exceptions:
//     if queue is empty
TreePair front(Queue* queue) {
    return queue->front->pair;
}

// Querying thread pointer from given tree
// Args:
//     tree: ThreadedTree*, node
//     dir: LEFT or RIGHT, querying direction
//     set: GET or SET, querying method
// Returns:
//     1, if LEFT is set
//     2, if RIGHT is set
//     0, if given node doesn't have thread pointer
int threaded(ThreadedTree* tree, int dir, int set) {
    if (set == SET) {
        tree->thread |= (1 << dir);
    }
    return tree->thread & (1 << dir);
}

// Generate empty tree
ThreadedTree* make_tree(char data) {
    ThreadedTree* tree = malloc(sizeof(ThreadedTree));
    tree->thread = 0;
    tree->data = data;
    tree->left = NULL;
    tree->right = NULL;
    return tree;
}

// Delete all tree nodes
void delete_tree(ThreadedTree* root) {
    Stack stack = make_stack();
    push(&stack, root);

    ThreadedTree* tree;
    while ((tree = pop(&stack))) {
        if (!threaded(tree, LEFT, GET) && tree->left) {
            push(&stack, tree->left);
        }
        if (!threaded(tree, RIGHT, GET) && tree->right) {
            push(&stack, tree->right);
        }

        free(tree);
    }

    delete_stack(&stack);
}

// Find insertion point, targeting full-complete binary tree
ThreadedTree** insert_point(ThreadedTree* tree) {
    Queue queue = make_queue();
    push_back(&queue, tree, NULL);

    // Level order traverse to find insertion point
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
    delete_queue(&queue);

    if (pair.parent->left == tree) {
        return &pair.parent->left;
    } else {
        return &pair.parent->right;
    }
}

// Insert node with given data
void insert_node(ThreadedTree* tree, char data) {
    ThreadedTree** node = insert_point(tree);
    *node = make_tree(data);
}

// Generate inorder thread pointer
void make_inorder_threaded(ThreadedTree* node) {
    int already_pop = 0;
    ThreadedTree* prev = NULL;
    Stack stack = make_stack();

    // In-order traverse
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

    delete_stack(&stack);
}

// Get next node based on thread pointer
ThreadedTree* next_node(ThreadedTree* node) {
    ThreadedTree* tmp = node->right;
    if (!threaded(node, RIGHT, GET)) {
        while (!threaded(tmp, LEFT, GET)) {
            tmp = tmp->left;
        }
    }
    return tmp;
}

// Traverse tree in in-order and run given callback with proper data
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

// Printer callback
FILE* output;
void print(char data) {
    fprintf(output, "%c  ", data);
}

int main() {
    // Preparing File I/O
    output = fopen("output.txt", "w");
    FILE* input = fopen("input.txt", "r");

    // Get size of list
    int n_list;
    fscanf(input, "%d", &n_list);

    int i;
    char data[2];
    ThreadedTree* root;

    if (n_list > 0) {
        // Generate root node
        fscanf(input, "%s", data);
        root = make_tree(data[0]);

        // Insert node
        for (i = 1; i < n_list; ++i) {
            fscanf(input, "%s", data);
            insert_node(root, data[0]);
        }

        // Make thread pointer in in-order
        make_inorder_threaded(root);
        // Traverse tree with callback `print`
        traverse(root, print);

        // Delete tree
        delete_tree(root);
    }

    // Delete file objects
    fclose(input);
    fclose(output);
    return 0;
}
