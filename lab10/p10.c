#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int* node;
    int** matrix;
    int size;
    int capacity;
} Graph;

typedef struct {
    int* queue;
    int size;
    int capacity;
    int front, rear;
} Queue;

typedef struct {
    int* array;
    int size;
} DynArray;

Graph make_graph(int capacity) {
    int i, j;
    Graph graph;
    graph.size = 0;
    graph.capacity = capacity;
    graph.node = malloc(sizeof(int) * capacity);
    for (i = 0; i < capacity; ++i) {
        graph.node[i] = 0;
    }
    graph.matrix = malloc(sizeof(int*) * capacity);
    for (i = 0; i < capacity; ++i) {
        graph.matrix[i] = malloc(sizeof(int) * capacity);
        for (j = 0; j < capacity; ++j) {
            graph.matrix[i][j] = 0;
        }
    }
    return graph;
}

void delete_graph(Graph* graph) {
    int i;
    for (i = 0; i < graph->capacity; ++i) {
        free(graph->matrix[i]);
    }
    free(graph->matrix);
    free(graph->node);
}

void print_matrix(Graph* graph, FILE* fp) {
    int i, j;
    fprintf(fp, "  ");
    for (i = 0; i < graph->size; ++i) {
        fprintf(fp, "%d ", graph->node[i]);
    }
    fprintf(fp, "\n");

    for (i = 0; i < graph->size; ++i) {
        fprintf(fp, "%d ", graph->node[i]);
        for (j = 0; j < graph->size; ++j) {
            fprintf(fp, "%d ", graph->matrix[i][j]);
        }
        fprintf(fp, "\n");
    }
}

Queue make_queue(int capacity) {
    Queue queue;
    queue.queue = malloc(sizeof(int) * capacity);
    queue.size = 0;
    queue.capacity = capacity;
    queue.front = 0;
    queue.rear = 0;
    return queue;
}

void delete_queue(Queue* queue) {
    free(queue->queue);
}

DynArray make_dyn_array(int size) {
    int i;
    DynArray dyn_array;
    dyn_array.size = size;
    if (size == 0) {
        dyn_array.array = NULL;
    } else {
        dyn_array.array = malloc(sizeof(int) * size);
        for (i = 0; i < size; ++i) {
            dyn_array.array[i] = 0;
        }
    }
    return dyn_array;
}

void delete_dyn_array(DynArray* arr) {
    free(arr->array);
}

int empty(Queue* queue) {
    return queue->size <= 0;
}

int full(Queue* queue) {
    return queue->size >= queue->capacity;
}

int front(Queue* queue) {
    return queue->queue[queue->front];
}

int enqueue(Queue* queue, int elem) {
    if (full(queue)) {
        return 0;
    }
    queue->queue[queue->rear] = elem;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size += 1;
    return 1;
}

int dequeue(Queue* queue) {
    if (empty(queue)) {
        return 0;
    }
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size -= 1;
    return 1;
}

int insert_multiple_node(Graph* graph, int* nodes, int n_node) {
    int i;
    int rest = graph->capacity - graph->size;
    int n_iter = n_node < rest ? n_node : rest;
    for (i = 0; i < n_iter; ++i) {
        graph->node[graph->size + i] = nodes[i];
    }
    graph->size += n_iter;
    return n_iter;
}

int insert_node(Graph* graph, int node) {
    if (graph->size < graph->capacity) {
        graph->node[graph->size++] = node;
        return 1;
    }
    return 0;
}

int find_idx(Graph* graph, int a) {
    int i;
    for (i = 0; i < graph->size; ++i) {
        if (graph->node[i] == a) {
            return i;
        }
    }
    return -1;
}

int insert_edge(Graph* graph, int a, int b) {
    int a_idx = find_idx(graph, a);
    int b_idx = find_idx(graph, b);
    if (a_idx == -1 || b_idx == -1) {
        return 0;
    }
    graph->matrix[a_idx][b_idx] = 1;
    return 1;
}

void swap(int *a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort_node(Graph* graph, Queue* queue) {
    int i, j;
    int* node = graph->node;
    int* indices = queue->queue;
    for (i = 1; i <= queue->size; ++i) {
        for (j = 0; j < queue->size - i; ++j) {
            if (node[indices[j]] > node[indices[j + 1]]) {
                swap(&indices[j], &indices[j + 1]);
            }
        }
    }
}

DynArray topological_sort(Graph* graph) {
    int i, j;
    int top = 0;
    Queue queue = make_queue(graph->size);
    DynArray indegree = make_dyn_array(graph->size);
    for (i = 0; i < graph->size; ++i) {
        for (j = 0; j < graph->size; ++j) {
            indegree.array[i] += graph->matrix[j][i];
        }
        if (indegree.array[i] == 0) {
            enqueue(&queue, i);
        }
    }

    if (queue.size == 0) {
        return make_dyn_array(0);
    }

    sort_node(graph, &queue);

    int elem;
    int idx = -1;
    Queue tmp = make_queue(graph->size);
    DynArray arr = make_dyn_array(graph->size);
    while (!empty(&queue)) {
        elem = front(&queue);
        arr.array[++idx] = graph->node[elem];
        dequeue(&queue);

        for (i = 0; i < graph->size; ++i) {
            if (graph->matrix[elem][i] == 1 && --indegree.array[i] == 0) {
                enqueue(&tmp, i);
            }
        }
        sort_node(graph, &tmp);
        for (i = tmp.size; i > 0; --i) {
            enqueue(&queue, front(&tmp));
            dequeue(&tmp);
        }
    }

    delete_dyn_array(&indegree);
    delete_queue(&queue);
    return arr;
}

int main() {
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    char line[1024] = { 0, };
    fgets(line, 1024, input);

    int n_read, num;
    char* ptr = line;

    int len = 0;
    int temporal[512] = { 0, };
    while (sscanf(ptr, "%d%n", &num, &n_read) == 1) {
        temporal[len++] = num;
        ptr += n_read;
    }

    Graph graph = make_graph(len);
    insert_multiple_node(&graph, temporal, len);

    fgets(line, 1024, input);
    ptr = line;

    char tmp;
    int start, end;
    while (sscanf(ptr, "%d%c%d%n", &start, &tmp, &end, &n_read) == 3) {
        insert_edge(&graph, start, end);
        ptr += n_read;
    }

    fprintf(output, "Adjacency matrix\n");
    print_matrix(&graph, output);
    fprintf(output, "\n\n\n");

    DynArray arr = topological_sort(&graph);
    if (arr.size == 0) {
        fprintf(output, "Graph should be acyclic.\n");
    } else {
        fprintf(output, "TopSort Result : ");
        for (int i = 0; i < arr.size; ++i) {
            fprintf(output, "%d ", arr.array[i]);
        }
        fprintf(output, "\n");
        delete_dyn_array(&arr);
    }

    delete_graph(&graph);
    return 0;
}