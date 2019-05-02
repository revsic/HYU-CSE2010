#include <stdio.h>
#include <stdlib.h>

// Structure for heap implementation.
typedef struct {
    int capacity;
    int size;
    int* elements;
    int(*compare)(int, int);
} Heap;

// Return min value.
int min(int a, int b) {
    return a > b ? b : a;
}

// Return max value.
int max(int a, int b) {
    return a > b ? a : b;
}

// Swap two integer.
void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Generate heap with given capacity and comparing method.
Heap make_heap(int capacity, int(*compare)(int, int)) {
    Heap heap;
    heap.capacity = capacity;
    heap.size = 0;
    heap.elements = malloc(sizeof(int) * capacity);
    heap.compare = compare;
    return heap;
}

// Get top of heap.
// Exception:
//     segmentation fault: if heap capacity is smaller than 2.
//     invalid value: if heap size is smaller than 1.
int top(Heap* heap) {
    return heap->elements[1];
}

// Free heap structure.
void delete_heap(Heap* heap) {
    free(heap->elements);
}

// Extend heap capacity with double of exist capacity.
void extend_heap(Heap* heap) {
    // calculate new capacity
    int new_capacity = heap->capacity * 2;
    int* new_elements = malloc(sizeof(int) * new_capacity);

    // copy array
    int i;
    for (i = 1; i <= heap->size; ++i) {
        new_elements[i] = heap->elements[i];
    }

    // free previous vector
    free(heap->elements);

    // assign new vector
    heap->capacity = new_capacity;
    heap->elements = new_elements;
}

// Propagate forward (root -> leaf) elements in given index.
void propagate(Heap* heap, int idx) {
    int child, size = heap->size;
    int* vec = heap->elements;
    int(*cmp)(int, int) = heap->compare;
    // until node doesn't have leaf
    for (; idx * 2 <= size; idx = child) {
        child = idx * 2;
        // if node have right child then get higher one
        if (child + 1 <= size && vec[child + 1] == cmp(vec[child + 1], vec[child])) {
            child += 1;
        }
        // propagate
        if (vec[child] == cmp(vec[idx], vec[child])) {
            swap(&vec[child], &vec[idx]);
        } else {
            break;
        }
    }
}

// Propagate backward (leaf -> root) elements in given index.
void back_propagate(Heap* heap, int idx) {
    int* vec = heap->elements;
    int(*cmp)(int, int) = heap->compare;
    // until node is root
    for(; idx > 1 && vec[idx] == cmp(vec[idx], vec[idx / 2]); idx /= 2) {
        // propagate
        swap(&vec[idx], &vec[idx / 2]);
    }
}

// Push element to heap.
void push(Heap* heap, int elem) {
    // if size overflow the capacity
    if (heap->capacity == 1 + heap->size) {
        // extend capacity
        extend_heap(heap);
    }
    // add element to back
    heap->elements[++heap->size] = elem;
    // back propagate
    back_propagate(heap, heap->size);
}

// Pop top element from heap
void pop(Heap* heap) {
    // if heap is empty
    if (heap->size == 0) {
        return;
    }
    // copy last element to top
    heap->elements[1] = heap->elements[heap->size--];
    // propagate
    propagate(heap, 1);
}

// find element from heap
int find_elem(Heap* heap, int elem, int index) {
    // if index overflow the size
    if (index > heap->size) {
        return -1;
    }
    // if element is in current index
    if (heap->elements[index] == elem) {
        return index;
    }

    // if priority of current node is lower than given element
    if (elem == heap->compare(elem, heap->elements[index])) {
        return -1;
    }

    // find elements from child nodes
    int res = find_elem(heap, elem, index * 2);
    if (res != -1) {
        return res;
    } else {
        return find_elem(heap, elem, index * 2 + 1);
    }
}

// Build heap with given sequence
Heap heapify(int* list, int n, int(*compare)(int, int)) {
    Heap heap = make_heap(n + 1, compare);
    heap.size = n;
    for (int i = 0; i < n; ++i) {
        heap.elements[i + 1] = list[i];
    }
    for (int i = n / 2; i > 0; --i) {
        propagate(&heap, i);
    }
    return heap;
}

// Heap sort
void heap_sort(int* list, int n, int(*compare)(int, int)) {
    Heap heap = heapify(list, n, compare);
    for (int i = 0; i < n; ++i) {
        list[i] = top(&heap);
        pop(&heap);
    }
    delete_heap(&heap);
}

// Read number and insert it to heap.
void insert(Heap* heap, FILE* input, FILE* output) {
    int num;
    fscanf(input, "%d", &num);

    // if given element is already existing in heap.
    if (find_elem(heap, num, 1) != -1) {
        fprintf(output, "%d is already in heap.\n", num);
    } else {
        push(heap, num);
        fprintf(output, "insert %d\n", num);
    }
}

// Delete top element.
void delete(Heap* heap, FILE* output) {
    // if heap is empty
    if (heap->size < 1) {
        fprintf(output, "heap is empty.\n");
    } else {
        int num = top(heap);
        pop(heap);
        fprintf(output, "delete %d\n", num);
    }
}

// Read number and find it from heap.
void find(Heap* heap, FILE* input, FILE* output) {
    int num;
    fscanf(input, "%d", &num);

    if (find_elem(heap, num, 1) == -1) {
        fprintf(output, "%d is not in the heap.\n", num);
    } else {
        fprintf(output, "%d is in the heap.\n", num);
    }
}

// Print heap in index-order.
void print(Heap* heap, FILE* output) {
    int i;
    for (i = 1; i <= heap->size; ++i) {
        fprintf(output, "%d ", heap->elements[i]);
    }
    fprintf(output, "\n");
}

int main() {
    // open file IO
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    // input capacity of heap.
    int capacity;
    fscanf(input, "%d", &capacity);

    // generate heap.
    char option;
    Heap heap = make_heap(capacity, max);

    // input option.
    while (fscanf(input, "%c", &option) == 1) {
        switch (option) {
        case 'i':
            insert(&heap, input, output);
            break;
        case 'd':
            delete(&heap, output);
            break;
        case 'f':
            find(&heap, input, output);
            break;
        case 'p':
            print(&heap, output);
            break;
        default:
            break;
        }
    }

    // free memory
    delete_heap(&heap);
    fclose(input);
    fclose(output);
}