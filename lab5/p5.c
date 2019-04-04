#include <stdio.h>
#include <stdlib.h>

#define MAX_TRAVERSE_DEPTH 512

typedef struct Node_ {
    int data;
    struct Node_* left;
    struct Node_* right;
} Node;

typedef struct {
    Node* parent;
    Node* child;
} NodePair;

Node* make_node(int data) {
    Node* node = malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

NodePair make_pair(Node* parent, Node* child) {
    NodePair pair;
    pair.parent = parent;
    pair.child = child;
    return pair;
}

Node* max_node(Node* node) {
    Node* prev = NULL;
    while (node) {
        prev = node;
        node = node->right;
    }
    return prev;
}

NodePair find_node(Node* node, int data) {
    Node* prev = NULL;
    while (node && node->data != data) {
        prev = node;
        if (node->data > data) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return make_pair(prev, node);
}

int insert_node(Node* node, int data) {
    NodePair pair = find_node(node, data);
    if (pair.child != NULL) {
        return 0;
    }

    if (pair.parent->data < data) {
        pair.parent->right = make_node(data);
    } else {
        pair.parent->left = make_node(data);
    }
    return 1;
}

int delete_node(Node* node, int data) {
    NodePair found = find_node(node, data);
    if (found.child == NULL) {
        return 0;
    }
    Node* exchg = max_node(found.child->left);

    if (exchg) {
        data = exchg->data;
        delete_node(found.child, data);
        found.child->data = data;
    } else {
        if (found.parent) {
            if (found.parent->left == found.child) {
                found.parent->left = NULL;
            } else {
                found.parent->right = NULL;
            }
        }
        free(found.child);
    }
    return 1;
}

void preorder_traversal(Node* node, void(*callback)(Node*)) {
    int idx = 0;
    Node* stack[MAX_TRAVERSE_DEPTH] = { node, };

    while (idx > -1) {
        node = stack[idx--];
        callback(node);

        if (node->right) {
            stack[++idx] = node->right;
        }
        if (node->left) {
            stack[++idx] = node->left;
        }
    }
}

void inorder_traversal(Node* node, void(*callback)(Node*)) {
    if (node) {
        inorder_traversal(node->left, callback);
        callback(node);
        inorder_traversal(node->right, callback);
    }
}

void postorder_traversal(Node* node, void(*callback)(Node*)) {
    if (node) {
        postorder_traversal(node->left, callback);
        postorder_traversal(node->right, callback);
        callback(node);
    }
}

void insert(Node** tree, FILE* input, FILE* output) {
    int num, res;
    fscanf(input, "%d", &num);

    if (*tree == NULL) {
        *tree = make_node(num);
    } else {
        res = insert_node(*tree, num);
        if (res == 0) {
            fprintf(output, "%d already exists.\n", num);
        }
    }
}

void delete(Node** tree, FILE* input, FILE* output) {
    int num, res;
    fscanf(input, "%d", &num);

    if (!*tree || delete_node(*tree, num) == 0) {
        fprintf(output, "Deletion failed. %d does not exist.\n", num);
    }
}

void find(Node* tree, FILE* input, FILE* output) {
    int num;
    fscanf(input, "%d", &num);

    NodePair pair = find_node(tree, num);
    if (pair.child) {
        fprintf(output, "%d is in the tree.\n", num);
    } else {
        fprintf(output, "%d is not in the tree.\n", num);
    }
}

FILE* output = NULL;
void print_callback(Node* node) {
    fprintf(output, "%d ", node->data);
}

void free_callback(Node* node) {
    free(node);
}

int main() {
    output = fopen("output.txt", "w");
    FILE* input = fopen("input.txt", "r");

    Node* tree = NULL;

    char cmd[3] = { 0, };
    while (fscanf(input, "%s", cmd) == 1) {
        switch (cmd[0]) {
        case 'i':
            insert(&tree, input, output);
            break;
        case 'd':
            delete(&tree, input, output);
            break;
        case 'f':
            find(tree, input, output);
            break;
        case 'p':
        {
            fprintf(output, "%s - ", cmd);
            switch (cmd[1]) {
            case 'i':
                inorder_traversal(tree, print_callback);
                break;
            case 'r':
                preorder_traversal(tree, print_callback);
                break;
            case 'o':
                postorder_traversal(tree, print_callback);
                break;
            default:
                break;
            }
            fprintf(output, "\n");
            break;
        }
        default:
            break;
        }
    }

    postorder_traversal(tree, free_callback);
    fclose(input);
    fclose(output);
}