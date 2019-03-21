#include <stdio.h>
#include <string.h>

#define MAX_QUEUE_SIZE 100

// Struct for Queue data structure implementation.
typedef struct {
    int size;
    int head, tail;
    int buffer[MAX_QUEUE_SIZE];
} Queue;

// Generate empty queue. Initialize with size 0.
Queue empty_queue() {
    Queue queue;
    queue.size = 0;
    queue.head = 0;
    queue.tail = 0;

    return queue;
}

// Enqueue `data` into `queue`.
// Returns:
//     1 for success.
//     0 for failure.
// Failure:
//     Try appending data to full queue.
int enqueue(Queue* queue, int data) {
    if (queue->size > MAX_QUEUE_SIZE) {
        return 0;
    }
    queue->buffer[queue->head] = data;
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    queue->size += 1;

    return 1;
}

// Dequeue element from `queue` and store it to `res`.
// Returns:
//     1 for success.
//     0 for failure.
// Failure:
//     Try dequeueing data from empty queue.
int dequeue(Queue* queue, int* res) {
    if (queue->size < 1) {
        return 0;
    }
    *res = queue->buffer[queue->tail];
    queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
    queue->size -= 1;

    return 1;
}

int main() {
    // Prepare for file I/O.
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    int n_query;
    fscanf(input, "%d", &n_query);

    int num, res;
    char option[10] = { 0, };

    // Generate empty queue.
    Queue queue = empty_queue();

    for (int i = 0; i < n_query; ++i) {
        fscanf(input, "%s", option);

        // Enqueue proper data to queue.
        if (!strcmp(option, "enQ")) {
            fscanf(input, "%d", &num);
            res = enqueue(&queue, num);

            // If failure (Full of queue)
            if (res == 0) {
                fprintf(output, "Full\n");
            }
        }
        // Dequeue proper data from queue.
        else if (!strcmp(option, "deQ")) {
            res = dequeue(&queue, &num);

            // If failure (Empty of queue)
            if (res == 0) {
                fprintf(output, "Empty\n");
            }
            // If success
            else {
                fprintf(output, "%d\n", num);
            }
        }
    }

    fclose(input);
    fclose(output);

    return 0;
}