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
    OVERFLOW_DETECTED = 3,
};

// Enums for deletion status.
enum DeleteStatus {
    DELETE_FAIL = 0,
    DELETE_SUCCESS = 1,
    UNDERFLOW_DETECTED = 2,
};

// Swap two integers.
void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Return lower bound for number of keys.
int lower_bound(int order) {
    return order & 1 == 0 ? order / 2 - 1 : order / 2;
}

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

// Delete key from node with given deletion point.
void delete_key(Node* node, int deletion_point) {
    int i;
    // pull all keys after deletion point to compact list
    for (i = deletion_point; i < node->n_key - 1; ++i) {
        node->key[i] = node->key[i + 1];
    }
    // decrease number of keys and fill zero
    node->key[--node->n_key] = 0;
}

// Insert subtree to node with given insertion point.
void insert_child(Node* node, int insertion_point, Node* subtree) {
    int i;
    // push all subtrees after insertion point to get blank
    for (i = node->n_key + 1; i > insertion_point; --i) {
        node->child[i] = node->child[i - 1];
    }
    // assign subtree
    node->child[insertion_point] = subtree;
}

// Delete subtree from node with given deletion point.
void delete_child(Node* node, int deletion_point) {
    int i;
    // pull all subtrees after deletion point to compact list
    for (i = deletion_point; i < node->n_key; ++i) {
        node->child[i] = node->child[i + 1];
    }
    // fill with NULL
    node->child[node->n_key] = NULL;
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

// Return non-zero value if current child is rotatable.
int insert_rotatable(Node* node, int pos, int order) {
    int left, right;
    // check left rotatable
    for (left = pos - 1; left >= 0; --left) {
        if (node->child[left]->n_key + 1 < order) {
            break;
        }
    }
    // check right rotatable
    for (right = pos + 1; right <= node->n_key; ++right) {
        if (node->child[right]->n_key + 1 < order) {
            break;
        }
    }
    // calculate relative position
    int rel_left = left - pos;
    int rel_right = right - pos;
    // if left and right both rotatable
    if (left >= 0 && right <= node->n_key) {
        return -rel_left < rel_right ? rel_left : rel_right;
    // if left only rotatable
    } else if (left >= 0) {
        return rel_left;
    // if right only rotatable
    } else if (right <= node->n_key) {
        return rel_right;
    // if both are not
    } else {
        return 0;
    }
}

// Rotate child to left.
void rotate_left(Node* node, int start, int end) {
    int i, j;
    // until rotation finish
    for (i = start; i > end; --i) {
        Node* child = node->child[i];
        Node* left_child = node->child[i - 1];

        // backup key and subtree for rotation
        int key = child->key[0];
        Node* subtree = child->child[0];
        
        // delete key and subtree from child node
        delete_child(child, 0);
        delete_key(child, 0);  // side-effect: child->n_key -=1;

        // swap with proper key of parent node
        swap(&key, &node->key[i - 1]);
        // append key and subtree to left child
        left_child->key[left_child->n_key] = key;
        left_child->child[left_child->n_key + 1] = subtree;
        left_child->n_key += 1;
    }    
}

// Rotate child to right.
void rotate_right(Node* node, int start, int end) {
    int i, j;
    // until rotation finish
    for (i = start; i < end; ++i) {
        Node* child = node->child[i];
        Node* right_child = node->child[i + 1];

        // backup key and subtree for rotation
        int key = child->key[child->n_key - 1];
        Node* subtree = child->child[child->n_key];

        // delete key and subtree from child node
        child->key[child->n_key - 1] = 0;
        child->child[child->n_key] = NULL;
        child->n_key -= 1;

        // swap with proper key of parent node
        swap(&key, &node->key[i]);
        // append key and subtree to right child
        insert_child(right_child, 0, subtree);
        insert_key(right_child, 0, key);  // side-effect: right_child->n_key += 1;
    }
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
    // if number of key overflow
    } else if (result == OVERFLOW_DETECTED) {
        int pos = insert_rotatable(node, i, order);
        // if rotation available
        if (pos != 0) {
            // if left rotation available
            if (pos < 0) {
                rotate_left(node, i, i + pos);
            // if right rotation available
            } else {
                rotate_right(node, i, i + pos);
            }
            return INSERT_SUCCESS;
        } else {
            Node* child = node->child[i];
            // split child on half and assign to parent
            split_to(node, child, i, order);
            delete_node(child);

            // if insertion success
            if (node->n_key < order) {
                return INSERT_SUCCESS;
            // if current node overflow
            } else {
                return OVERFLOW_DETECTED;
            }
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
            return OVERFLOW_DETECTED;
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
    if (result == OVERFLOW_DETECTED) {
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

// Return right most node from child nodes.
// Exception:
//     if node is null -> segmentation fault
Node* right_most_node(Node* node) {
    Node* largest_child = node->child[node->n_key];
    if (largest_child != NULL) {
        return right_most_node(largest_child);
    }
    return node;
}

// Return left most node from child nodes.
// Exception:
//     if node is null -> segmentation fault
Node* left_most_node(Node* node) {
    Node* smallest_child = node->child[0];
    if (smallest_child != NULL) {
        return left_most_node(smallest_child);
    }
    return node;
}

// Merge two nodes on each side of the base key.
Node* merge_node(Node* left, Node* right, int base_key, int order) {
    int i;
    Node* merged = empty_node(order);
    merged->n_key = left->n_key + right->n_key + 1;

    // copy left key
    for (i = 0; i < left->n_key; ++i) {
        merged->key[i] = left->key[i];
        merged->child[i] = left->child[i];
    }
    merged->child[i] = left->child[i];

    // copy base key
    merged->key[i] = base_key;

    // copy right key
    int base = i + 1;
    for (i = 0; i < right->n_key; ++i) {
        merged->key[base + i] = right->key[i];
        merged->child[base + i] = right->child[i];
    }
    merged->child[base + i] = right->child[i];

    return merged;
}

// Merge child with given base key index.
// Returns:
//     index where base key is inserted
int merge(Node* node, int idx, int order) {
    int key = node->key[idx];
    Node* left = node->child[idx];
    Node* right = node->child[idx + 1];
    // index where base key is inserted
    int res = left->n_key;
    // merge left and right node
    Node* merged = merge_node(left, right, key, order);
    // free memory
    delete_node(left);
    delete_node(right);
    // delete proper child and key
    delete_child(node, idx);
    delete_key(node, idx);
    // assign new merged child
    node->child[idx] = merged;
    return res;
}

// forward declaration
int delete_internal(Node* node, int key, int order);

// Remove key from given node.
int remove_key(Node* node, int key_idx, int order) {
    int bound = lower_bound(order);
    // if given node is leaf
    if (node->child[key_idx] == NULL) {
        delete_key(node, key_idx);
        // if number of key satisfy lower bound
        if (node->n_key >= bound) {
            return DELETE_SUCCESS;
        } else {
            return UNDERFLOW_DETECTED;
        }
    // if given is internal node
    } else {
        Node* left = node->child[key_idx];
        Node* right = node->child[key_idx + 1];

        // if left node has sufficient keys to pull largest key up
        if (left->n_key > bound) {
            // find largest key
            Node* right_most = right_most_node(left);
            int largest_key = right_most->key[right_most->n_key - 1];

            // swap it and remove key
            node->key[key_idx] = largest_key;
            return delete_internal(left, largest_key, order);
        // if right node has sufficient keys to pull smallest key up 
        } else if (right->n_key > bound) {
            // find smallest key
            Node* left_most = left_most_node(right);
            int smallest_key = left_most->key[0];

            // swap it and remove key
            node->key[key_idx] = smallest_key;
            return delete_internal(right, smallest_key, order);
        // merge child
        } else {
            int idx = merge(node, key_idx, order);
            // remove key from merged node
            remove_key(node->child[key_idx], idx, order);
            if (node->n_key < bound) {
                return UNDERFLOW_DETECTED;
            } else {
                return DELETE_SUCCESS;   
            }
        }
    }
}

// Internal implementation of method delete.
int delete_internal(Node* node, int key, int order) {
    int i, result;
    // if given key couldn't be found
    if (node == NULL) {
        return DELETE_FAIL;
    }
    // searching key
    for (i = 0; i < node->n_key; ++i) {
        // if key found
        if (key == node->key[i]) {
            return remove_key(node, i, order);
        // if child could have key
        } else if (key < node->key[i]) {
            result = delete_internal(node->child[i], key, order);
            break;
        }
    }
    if (i == node->n_key) {
        result = delete_internal(node->child[i], key, order);
    }

    // if underflow occured in i-th child
    if (result == UNDERFLOW_DETECTED) {
        int bound = lower_bound(order);
        // if left rotatable
        if (i > 0 && node->child[i - 1]->n_key > bound) {
            rotate_right(node, i - 1, i);
        // if right rotatable
        } else if (i < node->n_key && node->child[i + 1]->n_key > bound) {
            rotate_left(node, i + 1, i);
        // if merge operation required
        } else {
            // for non-zero index
            if (i > 0) {
                merge(node, i - 1, order);
            // for zero
            } else {
                merge(node, i, order);
            }
        }
        result = DELETE_SUCCESS;
    }
    return result;
}

// Remove given key from tree.
int delete(BTree* tree, int key) {
    int res = delete_internal(tree->root, key, tree->order);
    if (tree->root != NULL && tree->root->n_key < 1) {
        tree->root = tree->root->child[0];
    }
    return res;
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
        case 'f':
            fscanf(input, "%d", &num);
            if (find(tree, num).child != NULL) {
                fprintf(output, "key exist\n");
            } else {
                fprintf(output, "couldn't find key\n");
            }
            break;
        case 'd':
            fscanf(input, "%d", &num);
            delete(tree, num);
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
