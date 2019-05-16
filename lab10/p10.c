#include <stdio.h>
#include <stdlib.h>

// Structure for Graph representation.
typedef struct {
    int* node;
    int** matrix;
    int size;
    int capacity;
} Graph;

// Structure for Queue representation.
typedef struct {
    int* queue;
    int size;
    int capacity;
    int front, rear;
} Queue;

// Structure for dynamic allocated array.
typedef struct {
    int* array;
    int size;
} DynArray;

// Create new empty graph with given capacity and initialize with zero.
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

// Free graph structure (free only member not object itself).
void delete_graph(Graph* graph) {
    int i;
    for (i = 0; i < graph->capacity; ++i) {
        free(graph->matrix[i]);
    }
    free(graph->matrix);
    free(graph->node);
}

// Print adjacency matrix to given stream.
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

// Create new empty queue with given capacity.
Queue make_queue(int capacity) {
    Queue queue;
    queue.queue = malloc(sizeof(int) * capacity);
    queue.size = 0;
    queue.capacity = capacity;
    queue.front = 0;
    queue.rear = 0;
    return queue;
}

// Free the member of queue, not itself.
void delete_queue(Queue* queue) {
    free(queue->queue);
}

// Create new dynamic allocated array with given size.
DynArray make_dyn_array(int size) {
    int i;
    DynArray dyn_array;
    dyn_array.size = size;
    // if size is zero
    if (size == 0) {
        dyn_array.array = NULL;
    } else {
        // if size is not zero
        dyn_array.array = malloc(sizeof(int) * size);
        // initialize with zero
        for (i = 0; i < size; ++i) {
            dyn_array.array[i] = 0;
        }
    }
    return dyn_array;
}

// Free the member of dynamic allocated array.
void delete_dyn_array(DynArray* arr) {
    free(arr->array);
}

// Return if queue is empty.
int empty(Queue* queue) {
    return queue->size <= 0;
}

// Return if queue is full.
int full(Queue* queue) {
    return queue->size >= queue->capacity;
}

// Get first element of queue.
// Returns:
//     return invalid value if queue is empty.
//     return first element if queue has elements.
int front(Queue* queue) {
    return queue->queue[queue->front];
}

// Append element to queue.
// Returns:
//     return 0 if queue is full.
//     return 1 if success.
int enqueue(Queue* queue, int elem) {
    if (full(queue)) {
        return 0;
    }
    queue->queue[queue->rear] = elem;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size += 1;
    return 1;
}

// Remove element from queue.
// Returns:
//     return 0 if queue is empty.
//     return 1 if success.
int dequeue(Queue* queue) {
    if (empty(queue)) {
        return 0;
    }
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size -= 1;
    return 1;
}

// Insert multiple nodes to graph.
// Returns:
//     number of inserted nodes (since the capacity of graph).
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

// Insert node to graph.
// Returns:
//     return 1 if success.
//     return 0 if graph is full.
int insert_node(Graph* graph, int node) {
    if (graph->size < graph->capacity) {
        graph->node[graph->size++] = node;
        return 1;
    }
    return 0;
}

// Find index of given node.
// Returns:
//     return -1 if it couldn't find given node.
//     return index if node is found.
int find_idx(Graph* graph, int a) {
    int i;
    for (i = 0; i < graph->size; ++i) {
        if (graph->node[i] == a) {
            return i;
        }
    }
    return -1;
}

// Insert edge from a to b
// Returns:
//     return 0 if insertion failure.
//     return 1 if success.
int insert_edge(Graph* graph, int a, int b) {
    int a_idx = find_idx(graph, a);
    int b_idx = find_idx(graph, b);
    if (a_idx == -1 || b_idx == -1) {
        return 0;
    }
    graph->matrix[a_idx][b_idx] = 1;
    return 1;
}

// Swap to integer values.
void swap(int *a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Propagating element of max heap.
void propagate(int* key, int* value, int size, int idx) {
    int child;
    // until node doesn't have leaf
    for (; idx * 2 <= size; idx = child) {
        child = idx * 2;
        // if node have right child then get higher one
        if (child + 1 <= size && value[key[child - 1]] < value[key[child]]) {
            child += 1;
        }
        // propagate
        if (value[key[idx - 1]] < value[key[child - 1]]) {
            swap(&key[child - 1], &key[idx - 1]);
        } else {
            break;
        }
    }
}

// Sorting queue with node from graph.
void heap_sort(Graph* graph, Queue* queue) {
    int i;
    int* key = queue->queue;
    int size = queue->size;
    // make heap
    for (i = size / 2; i > 0; --i) {
        propagate(key, graph->node, size, i);
    }
    // sorting
    for (i = size - 1; i > 0; --i) {
        swap(&key[0], &key[i]);
        propagate(key, graph->node, i, 1);
    }
}

// Topological sort.
// Returns:
//     DynArray(0, NULL) if graph is not directed or acyclic.
//     proper DynArray if topological sort success.
DynArray topological_sort(Graph* graph) {
    int i, j;
    int top = 0;
    // queue for graph traversal
    Queue queue = make_queue(graph->size);
    // array for indegree memoization
    DynArray indegree = make_dyn_array(graph->size);
    // compute indegree of each node
    for (i = 0; i < graph->size; ++i) {
        for (j = 0; j < graph->size; ++j) {
            indegree.array[i] += graph->matrix[j][i];
        }
        if (indegree.array[i] == 0) {
            enqueue(&queue, i);
        }
    }

    // if graph has cycle
    if (queue.size == 0) {
        return make_dyn_array(0);
    }

    // sort queue
    heap_sort(graph, &queue);

    int elem;
    int idx = -1;
    // temporal queue
    Queue tmp = make_queue(graph->size);
    // result array
    DynArray arr = make_dyn_array(graph->size);
    // while queue is not empty
    while (!empty(&queue)) {
        // append to result array
        elem = front(&queue);
        arr.array[++idx] = graph->node[elem];
        dequeue(&queue);

        // reduce indegree and check if it is zero
        for (i = 0; i < graph->size; ++i) {
            if (graph->matrix[elem][i] == 1 && --indegree.array[i] == 0) {
                enqueue(&tmp, i);
            }
        }
        // sort queue
        heap_sort(graph, &tmp);
        for (i = tmp.size; i > 0; --i) {
            enqueue(&queue, front(&tmp));
            dequeue(&tmp);
        }
    }

    // free memory
    delete_dyn_array(&indegree);
    delete_queue(&queue);
    return arr;
}

int main() {
    // open files
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    // read line
    char line[1024] = { 0, };
    fgets(line, 1024, input);

    int n_read, num;
    char* ptr = line;

    // get node ids
    int len = 0;
    int temporal[512] = { 0, };
    while (sscanf(ptr, "%d%n", &num, &n_read) == 1) {
        temporal[len++] = num;
        ptr += n_read;
    }

    // make graph with given node ids
    Graph graph = make_graph(len);
    insert_multiple_node(&graph, temporal, len);

    // get edges
    fgets(line, 1024, input);
    ptr = line;

    char tmp;
    int start, end;
    while (sscanf(ptr, "%d%c%d%n", &start, &tmp, &end, &n_read) == 3) {
        insert_edge(&graph, start, end);
        ptr += n_read;
    }

    // print adjacency matrix
    fprintf(output, "Adjacency matrix\n");
    print_matrix(&graph, output);
    fprintf(output, "\n\n\n");

    // topological sort
    int i;
    DynArray arr = topological_sort(&graph);
    if (arr.size == 0) {
        fprintf(output, "Graph should be acyclic.\n");
    } else {
        fprintf(output, "TopSort Result : ");
        for (i = 0; i < arr.size; ++i) {
            fprintf(output, "%d ", arr.array[i]);
        }
        fprintf(output, "\n");
        delete_dyn_array(&arr);
    }

    // free memory
    delete_graph(&graph);
    return 0;
}