#include <stdio.h>
#include <stdlib.h>

#define order 3

#define INSERT_FAIL 0
#define INSERT_SUCCESS 1
#define INSERT_REQUIRED 2
#define ROTATION_REQUIRED 3

typedef struct Node_ {
    int n_key;
    struct Node_* child[order + 1];
    int key[order];
} Node;

typedef struct {
    Node* child;
    Node* parent;
} FindResult;

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

Node* empty_node() {
    Node* node = malloc(sizeof(Node));
    node->n_key = 0;
    
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

FindResult find(Node* node, int key) {
    return find_internal(NULL, node, key);
}

int insert_internal(Node* node, int key) {
    int i, j, k;
    int result;
    if (node == NULL) {
        return INSERT_REQUIRED;
    }

    for (i = 0; i < node->n_key; ++i) {
        if (key == node->key[i]) {
            return INSERT_FAIL;
        } else if (key < node->key[i]) {
            result = insert_internal(node->child[i], key);
            break;
        }
    }
    if (i == node->n_key) {
        result = insert_internal(node->child[i], key);
    }

    if (result == INSERT_FAIL || result == INSERT_SUCCESS) {
        return result;
    } else if (result == ROTATION_REQUIRED) {
        for (j = node->n_key; j > i; --j) {
            node->key[j] = node->key[j - 1];
        }
        int mid = order / 2;
        Node* child = node->child[i];
        node->key[i] = child->key[mid];

        for (j = order; j > i + 1; --j) {
            node->child[j] = node->child[j - 1];
        }

        Node* left = empty_node();
        left->n_key = mid;
        for (j = 0; j < mid; ++j) {
            left->key[j] = child->key[j];
            left->child[j] = child->child[j];
        }
        left->child[j] = child->child[j];

        Node* right = empty_node();
        right->n_key = order - mid - 1;
        for (j = mid + 1, k = 0; j < order; ++j, ++k) {
            right->key[k] = child->key[j];
            right->child[k] = child->child[j];
        }
        right->child[k] = child->child[j];

        delete_node(child);

        node->child[i] = left;
        node->child[i + 1] = right;

        node->n_key += 1;
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
        for (j = node->n_key; j > i; --j) {
            node->key[j] = node->key[j - 1];
        }
        node->key[i] = key;
        node->n_key += 1;

        if (node->n_key < order) {
            return INSERT_SUCCESS;
        } else {
            return ROTATION_REQUIRED;
        }
    }
    return INSERT_SUCCESS;
}

Node* insert(Node* root, int key) {
    if (root == NULL) {
        root = empty_node();
        root->n_key = 1;
        root->key[0] = key;
        return root;
    }

    int i, j, k;
    int result = insert_internal(root, key);
    if (result == ROTATION_REQUIRED) {
        Node* new_root = empty_node();
        new_root->n_key = 1;

        int mid = order / 2;
        new_root->key[0] = root->key[mid];

        Node* left = empty_node();
        left->n_key = mid;
        for (j = 0; j < mid; ++j) {
            left->key[j] = root->key[j];
            left->child[j] = root->child[j];
        }
        left->child[j] = root->child[j];

        Node* right = empty_node();
        right->n_key = order - mid - 1;
        for (j = mid + 1, k = 0; j < order; ++j, ++k) {
            right->key[k] = root->key[j];
            right->child[k] = root->child[j];
        }
        right->child[k] = root->child[j];

        delete_node(root);

        new_root->child[0] = left;
        new_root->child[1] = right;
        root = new_root;
    }
    return root;
}

void inorder(Node* node, FILE* fp) {
    if (node) {
        int i;
        for (i = 0; i < node->n_key; ++i) {
            inorder(node->child[i], fp);
            fprintf(fp, "%d ", node->key[i]);
        }
        inorder(node->child[i], fp);
    }
}

void delete_btree(Node* node) {
    if (node) {
        int i;
        for (i = 0; i <= node->n_key; ++i) {
            delete_btree(node->child[i]);
        }
        free(node);
    }
}

int main() {
    FILE* input = fopen("./input.txt", "r");
    FILE* output = fopen("./output.txt", "w");

    int num;
    char opt[2];
    Node* root = NULL;
    while (fscanf(input, "%s", opt) == 1) {
        switch (opt[0]) {
        case 'i':
            fscanf(input, "%d", &num);
            root = insert(root, num);
            break;
        case 'p':
            inorder(root, output);
            fprintf(output, "\n");
            break;
        default:
            break;
        }
    }

    delete_btree(root);
    fclose(output);
    fclose(input);

    return 0;
}
