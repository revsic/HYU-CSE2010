#include <stdio.h>
#include <stdlib.h>

// Structure for b-tree node.
typedef struct Node_ {
    int n_key;
    struct Node_** child;
    int* key;
} Node;

// Structure for b-tree.
typedef struct {
    Node* root;
    int order;
} BTree;

// Result of method `find`.
typedef struct {
    Node* child;
    Node* parent;
} FindResult;

// Enums for insertion status.
enum InsertStatus {
    INSERT_FAIL = 0,
    INSERT_SUCCESS = 1,
    INSERT_REQUIRED = 2,
    ROTATION_REQUIRED = 3,
};

// Create empty node with given order.
Node* empty_node(int order) {
    Node* node = malloc(sizeof(Node));
    node->n_key = 0;
    node->child = malloc(sizeof(Node*) * (order + 1));
    node->key = malloc(sizeof(int) * order);
    
    // initialize to zero
    int i;
    for (i = 0; i < order + 1; ++i) {
        node->child[i] = NULL;
    }
    for (i = 0; i < order; ++i) {
        node->key[i] = 0;
    }
    return node;
}

// Free dynamic allocated node.
void delete_node(Node* node) {
    free(node->child);
    free(node->key);
    free(node);
}

// Create structure FindResult with given pointers.
FindResult find_result(Node* child, Node* parent) {
    FindResult result;
    result.child = child;
    result.parent = parent;
    return result;
}

// Internal implementation of method find.
FindResult find_internal(Node* parent, Node* node, int key) {
    int i;
    // if method couldn't find such key
    if (node == NULL) {
        return find_result(NULL, parent);
    }
    // iterating key list
    for (i = 0; i < node->n_key; ++i) {
        // if key found
        if (key == node->key[i]) {
            return find_result(node, parent);
        // if key value is smaller than node's key
        } else if (key < node->key[i]) {
            return find_internal(node, node->child[i], key);
        }
    }
    // if key value is bigger than all keys
    return find_internal(node, node->child[i], key);
}

// Find key from b-tree.
// Returns:
//     FindResult(NULL, _): couldn't find key.
//     FindResult(node, parent): node with such key and its parent.
FindResult find(BTree* tree, int key) {
    return find_internal(NULL, tree->root, key);
}

// Insert key value to node with given insertion point.
void insert_key(Node* node, int insertion_point, int key) {
    int i;
    // push all keys after insertion point to get blank
    for (i = node->n_key; i > insertion_point; --i) {
        node->key[i] = node->key[i - 1];
    }
    // assign given key
    node->key[insertion_point] = key;
    // increase number of keys
    node->n_key += 1;
}

// Split child on half and assign median value to insertion point of parent node.
void split_to(Node* parent, Node* child, int insertion_point, int order) {
    int i, j;
    int mid = order / 2;
    // insert median value of child to parent
    insert_key(parent, insertion_point, child->key[mid]);

    // split child: left half
    Node* left = empty_node(order);
    left->n_key = mid;
    // copy key and child of left half
    for (i = 0; i < mid; ++i) {
        left->key[i] = child->key[i];
        left->child[i] = child->child[i];
    }
    left->child[i] = child->child[i];

    // split child: right half
    Node* right = empty_node(order);
    right->n_key = order - mid - 1;
    // copy key and child of right half
    for (i = mid + 1, j = 0; i < order; ++i, ++j) {
        right->key[j] = child->key[i];
        right->child[j] = child->child[i];
    }
    right->child[j] = child->child[i];

    // push all children after insertion point to get two blanks, for left and right
    for (i = order; i > insertion_point + 1; --i) {
        parent->child[i] = parent->child[i - 1];
    }

    // assign left half and right half
    parent->child[insertion_point] = left;
    parent->child[insertion_point + 1] = right;
}

// Internal implementation of method insert.
int insert_internal(Node* node, int key, int order) {
    int i, j;
    int result;
    // if current node is null
    if (node == NULL) {
        return INSERT_REQUIRED;
    }

    // iterating list of key
    for (i = 0; i < node->n_key; ++i) {
        // if key found, stop insertion
        if (key == node->key[i]) {
            return INSERT_FAIL;
        // if key value is smaller than node's key
        } else if (key < node->key[i]) {
            result = insert_internal(node->child[i], key, order);
            break;
        }
    }
    // if all keys are smaller than given key
    if (i == node->n_key) {
        result = insert_internal(node->child[i], key, order);
    }

    // if insertion end
    if (result == INSERT_FAIL || result == INSERT_SUCCESS) {
        return result;
    // if rotation is required
    } else if (result == ROTATION_REQUIRED) {
        Node* child = node->child[i];
        // split child on half and assign to parent
        split_to(node, child, i, order);
        delete_node(child);

        // if insertion success
        if (node->n_key < order) {
            return INSERT_SUCCESS;
        //if current node overflow
        } else {
            return ROTATION_REQUIRED;
        }
    // if insertion required in this node
    } else if (result == INSERT_REQUIRED) {
        // find insertion point
        for (i = 0; i < node->n_key; ++i) {
            if (node->key[i] > key) {
                break;
            }
        }
        // insert key
        insert_key(node, i, key);

        // if insertion success
        if (node->n_key < order) {
            return INSERT_SUCCESS;
        // if current node overflow
        } else {
            return ROTATION_REQUIRED;
        }
    }
    return INSERT_SUCCESS;
}

// Insert given key to b-tree.
// Returns:
//     INSERT_SUCCESS: successfully inserted
//     INSERT_FAIL: failed on insertion (duplicated element)
int insert(BTree* tree, int key) {
    // if tree is empty
    if (tree->root == NULL) {
        tree->root = empty_node(tree->order);
        tree->root->n_key = 1;
        tree->root->key[0] = key;
        return INSERT_SUCCESS;
    }

    // try insert key
    int result = insert_internal(tree->root, key, tree->order);
    // if root node overflow
    if (result == ROTATION_REQUIRED) {
        Node* new_root = empty_node(tree->order);
        // split current root and assign to new root
        split_to(new_root, tree->root, 0, tree->order);
        delete_node(tree->root);

        // assign new root
        tree->root = new_root;
        result = INSERT_SUCCESS;
    }
    return result;
}

// Inorder traversal with given callback.
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

// Postorder traversal with given callback.
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

// Create empty b-tree with given order.
BTree* empty_tree(int order) {
    BTree* tree = malloc(sizeof(BTree));
    tree->root = NULL;
    tree->order = order;
    return tree;
}

// Free dynamic allocated b-tree.
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
