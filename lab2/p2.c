#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char* name;
} Elem;

typedef struct Node_ {
    Elem elem;
    struct Node_* next;
} Node;

Node* empty_node() {
    Node* node = malloc(sizeof(Node));

    node->next = NULL;
    node->elem.id = 0;
    node->elem.name = NULL;

    return node;
}

void print_node(Node* node, FILE* output) {
    fprintf(output, "-----LIST-----\n");
    while (node != NULL) {
        fprintf(output, "%d %s\n", node->elem.id, node->elem.name);
        node = node->next;
    }
    fprintf(output, "--------------\n");
}

void print_current_node(Node* node, FILE* output) {
    fprintf(output, "Current List > ");

    int sep = 0;
    while (node != NULL) {
        if (sep != 0) {
            fprintf(output, "-");
        }
        fprintf(output, "%d %s", node->elem.id, node->elem.name);
        
        sep = 1;
        node = node->next;
    }

    fprintf(output, "\n");
}

Node* find_node(Node* node, int id) {
    while (node != NULL) {
        if (node->elem.id == id) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

int delete_node(Node* node, int id) {
    if (node->elem.id == id) {
        Node* new_head = node->next;
        free(node);
        return 1;
    }

    Node* prev = node;
    node = node->next;

    while (node != NULL) {
        if (node->elem.id == id) {
            prev->next = node->next;
            free(node);
            return 1;
        }

        prev = node;
        node = node->next;
    }

    return 0;
}

Node* insert_node(Node* node, Elem elem) {
    Node* new_node = empty_node();

    new_node->elem = elem;
    new_node->next = node->next;

    node->next = new_node;
    return new_node;
}

Node* insertion_point(Node* node, int id) {
    if (node == NULL || node->elem.id > id) {
        return NULL;
    }

    Node* prev = node;
    node = node->next;

    while (node != NULL) {
        if (node->elem.id > id) {
            break;
        }
        prev = node;
        node = node->next;
    }

    return prev;
}

void insert(Node* head, FILE* input, FILE* output) {
    int id = 0;
    char name[1024] = { 0, };
    fscanf(input, "%d %s", &id, name);

    int size = strlen(name);
    name[size] = ' ';

    fscanf(input, "%s", &name[size + 1]);
    size = strlen(name) + 1;

    if (find_node(head->next, id)) {
        fprintf(output, "Insertion Failed. ID %d already exists\n", id);
        return;
    }

    Elem elem;
    elem.id = id;
    elem.name = malloc(sizeof(char) * size);

    strcpy(elem.name, name);
    
    Node* point = insertion_point(head->next, id);
    if (point == NULL) {
        insert_node(head, elem);
    } else {
        insert_node(point, elem);
    }

    fprintf(output, "Insertion Success : %d\n", id);
    print_current_node(head->next, output);
}

void find(Node* head, FILE* input, FILE* output) {
    int id = 0;
    fscanf(input, "%d", &id);

    head = head->next;
    while (head != NULL) {
        if (head->elem.id == id) {
            fprintf(output, "Find Success : %d %s\n", head->elem.id, head->elem.name);
            return;
        }
        head = head->next;
    }

    fprintf(output, "Find %d Failed. There is no student ID\n", id);
}

void delete(Node* head, FILE* input, FILE* output) {
    int id = 0;
    fscanf(input, "%d", &id);

    if (delete_node(head->next, id)) {
        fprintf(output, "Deletion Success : %d\n", id);
        print_current_node(head->next, output);
    } else {
        fprintf(output, "Deletion Failed : Student ID %d is not in the list.\n", id);
    }
}

int main() {
    char opt;
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    Node* head = empty_node();
    while (fscanf(input, "%c", &opt) == 1) {
        switch (opt) {
        case 'i':
            insert(head, input, output);
            break;
        case 'd':
            delete(head, input, output);
            break;
        case 'f':
            find(head, input, output);
            break;
        case 'p':
            print_node(head->next, output);
            break;
        default:
            break;
        }
    }

    while (head != NULL) {
        Node* node = head->next;
        free(head);
        head = node;
    }

    fclose(input);
    fclose(output);

    return 0;
}