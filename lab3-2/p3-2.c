#include <stdio.h>
#include <string.h>

#define MAX_QUEUE_SIZE 100

typedef struct {
    int size;
    int head, tail;
    int buffer[MAX_QUEUE_SIZE];
} Queue;

Queue empty_queue() {
    Queue queue;
    queue.size = 0;
    queue.head = 0;
    queue.tail = 0;

    return queue;
}

int enqueue(Queue* queue, int data) {
    if (queue->size > MAX_QUEUE_SIZE) {
        return 0;
    }
    queue->buffer[queue->head] = data;
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    queue->size += 1;

    return 1;
}

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
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    int n_query;
    fscanf(input, "%d", &n_query);

    int num, res;
    char option[10] = { 0, };

    Queue queue = empty_queue();

    for (int i = 0; i < n_query; ++i) {
        fscanf(input, "%s", option);

        if (!strcmp(option, "enQ")) {
            fscanf(input, "%d", &num);
            res = enqueue(&queue, num);
            if (res == 0) {
                fprintf(output, "Full\n");
            }
        }
        else if (!strcmp(option, "deQ")) {
            res = dequeue(&queue, &num);
            if (res == 0) {
                fprintf(output, "Empty\n");
            }
            else {
                fprintf(output, "%d\n", num);
            }
        }
    }

    return 0;
}