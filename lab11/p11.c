#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int idx;
    int* dist;
    int* heap_idx;
} HeapNode;

typedef struct {
    int capacity;
    int size;
    HeapNode* elements;
} Heap;

typedef struct {
    int* node;
    int** matrix;
    int size;
    int capacity;
} Graph;

typedef struct {
    int* array;
    int size;
} DynArray;

int inf() {
    // return 1 << (sizeof(int) * 8 - 1) - 1;
    return INT_MAX;
}

void swap(HeapNode* vec, int a, int b) {
    HeapNode tmp = vec[a];
    vec[a] = vec[b];
    vec[b] = tmp;

    *vec[a].heap_idx = a;
    *vec[b].heap_idx = b;
}

HeapNode make_node(int idx, int* dist, int* heap_idx) {
    HeapNode node;
    node.idx = idx;
    node.dist = dist;
    node.heap_idx = heap_idx;
    return node;
}

Heap make_heap(int capacity) {
    Heap heap;
    heap.capacity = capacity;
    heap.size = 0;
    heap.elements = malloc(sizeof(HeapNode) * capacity);
    return heap;
}

void delete_heap(Heap* heap) {
    free(heap->elements);
}

HeapNode top(Heap* heap) {
    return heap->elements[1];
}

void extend_heap(Heap* heap) {
    int new_capacity = heap->capacity * 2;
    HeapNode* new_elements = malloc(sizeof(HeapNode) * new_capacity);

    int i;
    for (i = 1; i <= heap->size; ++i) {
        new_elements[i] = heap->elements[i];
    }

    free(heap->elements);

    heap->capacity = new_capacity;
    heap->elements = new_elements;
}

void propagate(Heap* heap, int idx) {
    int child, size = heap->size;
    HeapNode* vec = heap->elements;
    for (; idx * 2 <= size; idx = child) {
        child = idx * 2;
        if (child + 1 <= size && *vec[child + 1].dist < *vec[child].dist) {
            child += 1;
        }
        if (*vec[child].dist < *vec[idx].dist) {
            swap(vec, child, idx);
        } else {
            break;
        }
    }
}

void back_propagate(Heap* heap, int idx) {
    HeapNode* vec = heap->elements;
    for(; idx > 1 && *vec[idx].dist < *vec[idx / 2].dist; idx /= 2) {
        swap(vec, idx, idx / 2);
    }
}

void push(Heap* heap, HeapNode elem) {
    if (heap->capacity == 1 + heap->size) {
        extend_heap(heap);
    }
    heap->size += 1;
    *elem.heap_idx = heap->size;
    heap->elements[heap->size] = elem;
    back_propagate(heap, heap->size);
}

void pop(Heap* heap) {
    if (heap->size == 0) {
        return;
    }
    heap->elements[1] = heap->elements[heap->size--];
    propagate(heap, 1);
}

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

int insert_edge(Graph* graph, int a, int b, int weight) {
    int a_idx = find_idx(graph, a);
    int b_idx = find_idx(graph, b);
    if (a_idx == -1 || b_idx == -1) {
        return 0;
    }
    graph->matrix[a_idx][b_idx] = weight;
    return 1;
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

DynArray dijkstra_search(Graph* graph, int start) {
    int n_node = graph->size;
    Heap heap = make_heap(n_node);

    int** adjacency = graph->matrix;
    DynArray pred = make_dyn_array(n_node);
    DynArray distances = make_dyn_array(n_node);
    DynArray heap_index = make_dyn_array(n_node);

    int i, dist;
    for (i = 0; i < n_node; ++i) {
        if (adjacency[start][i] == 0) {
            dist = inf();
            pred.array[i] = -1;
        } else {
            dist = adjacency[start][i];
            pred.array[i] = start;
        }
        distances.array[i] = dist;
        push(&heap, make_node(i, &distances.array[i], &heap_index.array[i]));
    }

    HeapNode now;
    while (heap.size > 0) {
        now = top(&heap);
        pop(&heap);

        for (i = 0; i < n_node; ++i) {
            int weight = adjacency[now.idx][i];
            if (weight != 0) {
                int new_dist = *now.dist + weight;
                if (new_dist < distances.array[i]) {
                    pred.array[i] = now.idx;
                    distances.array[i] = new_dist;
                    propagate(&heap, heap_index.array[i]);
                }
            }
        }
    }

    delete_dyn_array(&heap_index);
    delete_dyn_array(&distances);
    delete_heap(&heap);

    return pred;
}

int main() {
    int i;
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    char line[1024] = { 0, };
    fgets(line, 1024, input);

    int idx = 0;
    int nodes[512] = { 0, };

    int n_read, num;
    char* ptr = line;
    while (sscanf(ptr, "%d%n", &num, &n_read) == 1) {
        ptr += n_read;
        nodes[idx++] = num;
    }

    Graph graph = make_graph(idx);
    for (i = 0; i < idx; ++i) {
        insert_node(&graph, nodes[i]);
    }

    fgets(line, 1024, input);
    ptr = line;

    int a, b, weight;
    while (sscanf(ptr, "%d-%d-%d%n", &a, &b, &weight, &n_read) == 3) {
        ptr += n_read;
        insert_edge(&graph, a, b, weight);
    }

    int start, end;
    fscanf(input, "%d %d", &start, &end);

    start = find_idx(&graph, start);
    end = find_idx(&graph, end);
    DynArray pred = dijkstra_search(&graph, start);

    idx = 0;
    DynArray succ = make_dyn_array(graph.size);

    succ.array[idx++] = end;
    while (end != start && idx < graph.size) {
        succ.array[idx++] = pred.array[end];
        end = pred.array[end];
    }

    if (succ.array[idx - 1] != start) {
        fprintf(output, "path doesn't exist\n");
    } else {
        for (i = idx - 1; i >= 0; --i) {
            fprintf(output, "%d ", graph.node[succ.array[i]]);
        }
        fprintf(output, "\n");
    }

    delete_dyn_array(&succ);
    delete_dyn_array(&pred);
    delete_graph(&graph);
    fclose(input);
    fclose(output);
}