#include <stdio.h>
#include <stdlib.h>

int numeric_limit_int_max() {
    return (1 << (sizeof(int) * 8 - 1)) - 1;
}

void selection_sort(int* arr, int size) {
    int i, j;
    for (i = 0; i < size; ++i) {
        int idx = -1;
        int min = numeric_limit_int_max();
        for (j = i; j < size; ++j) {
            if (min > arr[j]) {
                min = arr[j];
                idx = j;
            }
        }

        int tmp = arr[i];
        arr[i] = arr[idx];
        arr[idx] = tmp;
    }
}

int main() {
    FILE* fp = fopen("input.txt", "r");

    int n_input = 0;
    fscanf(fp, "%d", &n_input);

    int i;
    int* arr = malloc(sizeof(int) * n_input);
    for (i = 0; i < n_input; ++i) {
        fscanf(fp, "%d", &arr[i]);
    }

    fclose(fp);

    selection_sort(arr, n_input);

    fp = fopen("output.txt", "w");
    for (i = 0; i < n_input; ++i) {
        fprintf(fp, "%d ", arr[i]);
    }
    fprintf(fp, "\n");

    fclose(fp);
    free(arr);
}