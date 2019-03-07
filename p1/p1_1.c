#include <stdio.h>
#include <stdlib.h>

int numeric_limit_int_max() {
    return (1 << (sizeof(int) * 8 - 1)) - 1;
}

void selection_sort(int* arr, int size) {
    for (int i = 0; i < size; ++i) {
        int idx = -1;
        int min = numeric_limit_int_max();
        for (int j = i; j < size; ++j) {
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

int main(int argc, char* argv[]) {
    FILE* fp = fopen(argv[1], "r");

    int n_input = 0;
    fscanf(fp, "%d", &n_input);

    int* arr = malloc(sizeof(int) * n_input);
    for (int i = 0; i < n_input; ++i) {
        fscanf(fp, "%d", &arr[i]);
    }

    fclose(fp);

    selection_sort(arr, n_input);
    for (int i = 0; i < n_input; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
}