#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* name;
    int studentID;
    char* major;
} studentT;

int main(int argc, char* argv[]) {
    FILE* fp = fopen("input.txt", "r");
    
    int i;
    int n_input = 0;
    fscanf(fp, "%d", &n_input);

    studentT* arr = malloc(sizeof(studentT) * n_input);
    for (i = 0; i < n_input; ++i) {
        arr[i].name = malloc(sizeof(char) * 31);
        arr[i].major = malloc(sizeof(char) * 31);

        fscanf(fp, "%s %d %s", arr[i].name, &arr[i].studentID, arr[i].major);
    }

    fclose(fp);

    fp = fopen("output.txt", "w");
    for (i = 0; i < n_input; ++i) {
        fprintf(fp, "%s %d %s\n", arr[i].name, arr[i].studentID, arr[i].major);
    }

    fclose(fp);

    for (i = 0; i < n_input; ++i) {
        free(arr[i].name);
        free(arr[i].major);
    }
    free(arr);

    return 0;
}