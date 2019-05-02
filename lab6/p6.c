#include <stdio.h>
#include <stdlib.h>

// AVL Tree node.
typedef struct AVLNode_ {
    int elem;
    struct AVLNode_* left;
    struct AVLNode_* right;
    int height;
} AVLNode;

// Structure for insertion result.
typedef struct {
    AVLNode* node;
    int success;
} InsertResult;

// Structure for deletion result.
typedef struct {
    AVLNode* node;
    int success;
} RemoveResult;

// Return larger value.
int max(int a, int b) {
    return a > b ? a : b;
}

// Generate empty avl node with given element.
AVLNode* make_node(int elem) {
    AVLNode* node = malloc(sizeof(AVLNode));
    node->elem = elem;
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    return node;
}

// Get height of node.
int height(AVLNode* node) {
    if (node == NULL) {
        return -1;
    } else {
        return node->height;
    }
}

// Update height with child nodes.
void update_height(AVLNode* node) {
    node->height = max(
        height(node->left),
        height(node->right)
    ) + 1;
}

// Rotation for inserting element into left subtree of left child.
AVLNode* rotate_left(AVLNode* node) {
    AVLNode* left_node = node->left;
    node->left = left_node->right;
    left_node->right = node;

    update_height(node);
    update_height(left_node);

    return left_node;
}

// Rotation for inserting element into right subtree of right child.
AVLNode* rotate_right(AVLNode* node) {
    AVLNode* right_node = node->right;
    node->right = right_node->left;
    right_node->left = node;

    update_height(node);
    update_height(right_node);

    return right_node;
}

// Rotation for inserting element into right subtree of left child.
AVLNode* rotate_left_right(AVLNode* node) {
    node->left = rotate_right(node->left);
    node = rotate_left(node);
    return node;
}

// Rotation for inserting element into left subtree of right child.
AVLNode* rotate_right_left(AVLNode* node) {
    node->right = rotate_left(node->right);
    node = rotate_right(node);
    return node;
}

// Inserting element into AVL tree.
// Returns:
//     result: InsertResult, insertion result
//     result.success is 1 if insertion success.
//     result.success is 0 if innsertion fail.
InsertResult insert(int elem, AVLNode* node) {
    InsertResult result;
    // if empty tree
    if (node == NULL) {
        result.node = make_node(elem);
        result.success = 1;
        return result;
    }

    if (node->elem > elem) {
        result = insert(elem, node->left);
        // if insertion fail
        if (!result.success) {
            result.node = node;
            return result;
        }

        node->left = result.node;
        // if tree unbalanced
        if (height(node->left) - height(node->right) >= 2) {
            if (node->left->elem > elem) {
                node = rotate_left(node);
            } else {
                node = rotate_left_right(node);
            }
        }
    } else if (node->elem < elem) {
        result = insert(elem, node->right);
        // if insertion fail
        if (!result.success) {
            result.node = node;
            return result;
        }

        node->right = result.node;
        // if tree unbalanced
        if (height(node->right) - height(node->left) >= 2) {
            if (node->right->elem < elem) {
                node = rotate_right(node);
            } else {
                node = rotate_right_left(node);
            }
        }
    } else {
        // if element already exists
        result.node = node;
        result.success = 0;
        return result;
    }

    result.node = node;
    result.success = 1;

    update_height(node);
    return result;
}

RemoveResult delete(int elem, AVLNode* node) {
    RemoveResult result;
    if (node == NULL) {
        result.node = NULL;
        result.success = 0;
        return result;
    }
    if (node->elem > elem) {
        result = delete(elem, node->left);
        if (!result.success) {
            result.node = node;
            return result;
        }
        node->left = result.node;
    } else if (node->elem < elem) {
        result = delete(elem, node->right);
        if (!result.success) {
            result.node = node;
            return result;
        }
        node->right = result.node;
    } else if (node->left != NULL && node->right != NULL) {
        AVLNode* temp = node->right;
        AVLNode* prev = temp;
        while (temp->left) {
            prev = temp;
            temp = temp->left;
        }
        node->elem = prev->elem;
        result = delete(node->elem, node->right);
        node->right = result.node;
    } else {
        AVLNode* temp = node;
        if (node->left == NULL) {
            node = node->right;
        } else if (node->right == NULL) {
            node = node->left;
        }
        free(temp);
    }

    result.success = 1;
    result.node = node;
    if (node != NULL) {
        update_height(node);

        int hleft = height(node->left);
        int hright = height(node->right);
        if (hleft >= hright + 2) {
            if (height(node->left->left) >= height(node->left->right)) {
                node = rotate_left(node);
            } else {
                node = rotate_left_right(node);
            }
        } else if (hright >= hleft + 2) {
            if (height(node->right->right) >= height(node->right->left)) {
                node = rotate_right(node);
            } else {
                node = rotate_right_left(node);
            }
        }
    }

    return result;
}

// Traverse tree in in-order.
void inorder_traversal(AVLNode* node, void(*callback)(AVLNode*)) {
    if (node != NULL) {
        inorder_traversal(node->left, callback);
        callback(node);
        inorder_traversal(node->right, callback);
    }
}

// Traverse tree in post-order.
void postorder_traversal(AVLNode* node, void(*callback)(AVLNode*)) {
    if (node != NULL) {
        postorder_traversal(node->left, callback);
        postorder_traversal(node->right, callback);
        callback(node);
    }
}

FILE* output;
void print_callback(AVLNode* node) {
    fprintf(output, "%d(%d) ", node->elem, height(node));
}

void free_callback(AVLNode* node) {
    free(node);
}

int main() {
    output = fopen("output.txt", "w");
    FILE* input = fopen("input.txt", "r");

    int elem;
    AVLNode* tree = NULL;

    InsertResult result;
    while (fscanf(input, "%d", &elem) == 1) {
        // insert value
        result = insert(elem, tree);
        // insertion result validation
        if (result.success) {
            tree = result.node;
            inorder_traversal(tree, print_callback);
            fprintf(output, "\n");
        } else {
            fprintf(output, "%d already in the tree!\n", elem);
        }
    }

    postorder_traversal(tree, free_callback);
    fclose(input);
    fclose(output);

    return 0;
}
