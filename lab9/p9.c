#include <stdio.h>
#include <stdlib.h>

#define INSERT_FAIL 0
#define INSERT_SUCCESS 1
#define INSERT_REQUIRED 2
#define ROTATION_REQUIRED 3

typedef struct Node_ {
    int n_key;
    struct Node_** child;
    int* key;
} Node;

typedef struct {
    Node* root;
    int order;
} BTree;

typedef struct {
    Node* child;
    Node* parent;
} FindResult;

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

Node* empty_node(int order) {
    Node* node = malloc(sizeof(Node));
    node->n_key = 0;
    node->child = malloc(sizeof(Node*) * (order + 1));
    node->key = malloc(sizeof(int) * order);
    
    int i;
    for (i = 0; i < order + 1; ++i) {
        node->child[i] = NULL;
    }
    for (i = 0; i < order; ++i) {
        node->key[i] = 0;
    }
    return node;
}

void delete_node(Node* node) {
    free(node->child);
    free(node->key);
    free(node);
}

FindResult find_result(Node* child, Node* parent) {
    FindResult result;
    result.child = child;
    result.parent = parent;
    return result;
}

FindResult find_internal(Node* parent, Node* node, int key) {
    int i;
    if (node == NULL) {
        return find_result(NULL, parent);
    }
    for (i = 0; i < node->n_key; ++i) {
        if (key == node->key[i]) {
            return find_result(node, parent);
        } else if (key < node->key[i]) {
            return find_internal(node, node->child[i], key);
        }
    }
    return find_internal(node, node->child[i], key);
}

FindResult find(BTree* tree, int key) {
    return find_internal(NULL, tree->root, key);
}

void insert_key(Node* node, int insertion_point, int key) {
    int i;
    for (i = node->n_key; i > insertion_point; --i) {
        node->key[i] = node->key[i - 1];
    }
    node->key[insertion_point] = key;
    node->n_key += 1;
}

void split_to(Node* parent, Node* child, int insertion_point, int order) {
    int i, j;
    int mid = order / 2;
    insert_key(parent, insertion_point, child->key[mid]);

    Node* left = empty_node(order);
    left->n_key = mid;
    for (i = 0; i < mid; ++i) {
        left->key[i] = child->key[i];
        left->child[i] = child->child[i];
    }
    left->child[i] = child->child[i];

    Node* right = empty_node(order);
    right->n_key = order - mid - 1;
    for (i = mid + 1, j = 0; i < order; ++i, ++j) {
        right->key[j] = child->key[i];
        right->child[j] = child->child[i];
    }
    right->child[j] = child->child[i];

    for (i = order; i > insertion_point + 1; --i) {
        parent->child[i] = parent->child[i - 1];
    }

    parent->child[insertion_point] = left;
    parent->child[insertion_point + 1] = right;
}

int insert_internal(Node* node, int key, int order) {
    int i, j;
    int result;
    if (node == NULL) {
        return INSERT_REQUIRED;
    }

    for (i = 0; i < node->n_key; ++i) {
        if (key == node->key[i]) {
            return INSERT_FAIL;
        } else if (key < node->key[i]) {
            result = insert_internal(node->child[i], key, order);
            break;
        }
    }
    if (i == node->n_key) {
        result = insert_internal(node->child[i], key, order);
    }

    if (result == INSERT_FAIL || result == INSERT_SUCCESS) {
        return result;
    } else if (result == ROTATION_REQUIRED) {
        Node* child = node->child[i];
        split_to(node, child, i, order);
        delete_node(child);

        if (node->n_key < order) {
            return INSERT_SUCCESS;
        } else {
            return ROTATION_REQUIRED;
        }
    } else if (result == INSERT_REQUIRED) {
        for (i = 0; i < node->n_key; ++i) {
            if (node->key[i] > key) {
                break;
            }
        }
        insert_key(node, i, key);

        if (node->n_key < order) {
            return INSERT_SUCCESS;
        } else {
            return ROTATION_REQUIRED;
        }
    }
    return INSERT_SUCCESS;
}

void insert(BTree* tree, int key) {
    if (tree->root == NULL) {
        tree->root = empty_node(tree->order);
        tree->root->n_key = 1;
        tree->root->key[0] = key;
        return;
    }

    int result = insert_internal(tree->root, key, tree->order);
    if (result == ROTATION_REQUIRED) {
        Node* new_root = empty_node(tree->order);
        split_to(new_root, tree->root, 0, tree->order);
        delete_node(tree->root);

        tree->root = new_root;
    }
}

void inorder(Node* node, void(*callback)(int key)) {
    if (node) {
        int i;
        for (i = 0; i < node->n_key; ++i) {
            inorder(node->child[i], callback);
            callback(node->key[i]);
        }
        inorder(node->child[i], callback);
    }
}

void postorder(Node* node, void(*callback)(Node*)) {
    if (node) {
        int i;
        for (i = 0; i <= node->n_key; ++i) {
            postorder(node->child[i], callback);
        }
        callback(node);
    }
}

FILE* output;
void print_key(int key) {
    fprintf(output, "%d ", key);
}

BTree* empty_tree(int order) {
    BTree* tree = malloc(sizeof(BTree));
    tree->root = NULL;
    tree->order = order;
    return tree;
}

void delete_tree(BTree* tree) {
    postorder(tree->root, delete_node);
    free(tree);
}

int main() {
    output = fopen("./output.txt", "w");
    FILE* input = fopen("./input.txt", "r");

    BTree* tree = empty_tree(3);

    int num;
    char opt[2];
    while (fscanf(input, "%s", opt) == 1) {
        switch (opt[0]) {
        case 'i':
            fscanf(input, "%d", &num);
            insert(tree, num);
            break;
        case 'p':
            inorder(tree->root, print_key);
            fprintf(output, "\n");
            break;
        default:
            break;
        }
    }

    delete_tree(tree);
    fclose(output);
    fclose(input);

    return 0;
}
