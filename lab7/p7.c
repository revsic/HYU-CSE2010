#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Matrix for storing edge information. 
typedef struct {
    int height;
    int width;
    char** ptr;
} Matrix;

// Array based disjoint set union find implementation.
typedef struct {
    int size;
    int* table;
} DisjointSet;

// Int tuple type.
typedef struct {
    int fst;
    int snd;
} Tuple;

// Nested int tuple type.
typedef struct {
    Tuple fst;
    Tuple snd;
} CoordTuple;

// Return max value.
int max(int a, int b) {
    return a > b ? a : b;
}

// Generate matrix with given height and width, initialize with zero.
Matrix make_matrix(int height, int width) {
    Matrix matrix;
    matrix.height = height;
    matrix.width = width;
    matrix.ptr = malloc(sizeof(char*) * height);

    int i, j;
    for (i = 0; i < height; ++i) {
        matrix.ptr[i] = malloc(sizeof(char) * width);
        for (j = 0; j < width; ++j) {
            matrix.ptr[i][j] = 0;
        }
    }
    return matrix;
}

// Free matrix structure.
void delete_matrix(Matrix matrix) {
    int i;
    for (i = 0; i < matrix.height; ++i) {
        free(matrix.ptr[i]);
    }
    free(matrix.ptr);
}

// Generate disjoint set with square of given size.
DisjointSet make_disjoint_set(int size) {
    int i;
    DisjointSet set;
    set.size = size * size;
    set.table = malloc(sizeof(int) * (set.size + 1));
    for (i = 0; i <= set.size; ++i) {
        set.table[i] = 0;
    }
    return set;
}

// Free disjoint set structure.
void delete_disjoint_set(DisjointSet set) {
    free(set.table);
}

// Find root of union.
int find_root(DisjointSet* disjoint_set, int id) {
    while (disjoint_set->table[id] > 0) {
        id = disjoint_set->table[id];
    }
    return id;
}

// Append child subtree to root.
int append(DisjointSet* disjoint_set, int root, int child) {
    int rank_root = -disjoint_set->table[root];
    int rank_child = -disjoint_set->table[child];
    disjoint_set->table[root] = -max(rank_root, rank_child + 1);
    disjoint_set->table[child] = root;
    return 1;
}

// Union two disjoint set.
int union_set(DisjointSet* disjoint_set, int id1, int id2) {
    int root1 = find_root(disjoint_set, id1);
    int root2 = find_root(disjoint_set, id2);
    if (root1 == root2) {
        return 0;
    }

    int rank1 = -disjoint_set->table[root1];
    int rank2 = -disjoint_set->table[root2];
    if (rank1 > rank2) {
        append(disjoint_set, root1, root2);
    } else {
        append(disjoint_set, root2, root1);
    }
    return 1;
}

// Return 1 if all elements are in one disjoint set.
int all_union(DisjointSet* disjoint_set) {
    int i, flag = 0;
    for (i = 1; i <= disjoint_set->size; ++i) {
        if (disjoint_set->table[i] <= 0) {
            if (++flag > 1) {
                return 0;
            }
        }
    }
    return 1;
}

// Generate tuple with given numbers.
Tuple make_tuple(int fst, int snd) {
    Tuple tuple;
    tuple.fst = fst;
    tuple.snd = snd;
    return tuple;
}

// Generate nested tuple with given tuples.
CoordTuple make_coordtuple(Tuple fst, Tuple snd) {
    CoordTuple coord;
    coord.fst = fst;
    coord.snd = snd;
    return coord;
}

// Generate random number in range [0, n).
int rand_num(int n) {
    return rand() % n;
}

// Generate random index for pointing specific edge.
Tuple random_edge(int n) {
    Tuple tuple;
    tuple.fst = rand_num(2 * n - 1);
    if (tuple.fst % 2 == 0) {
        tuple.snd = rand_num(n - 1);
    } else {
        tuple.snd = rand_num(n);
    }
    return tuple;
}

// Get adjacent cells of given edge.
CoordTuple get_adjacent(Tuple edge, int size) {
    if (edge.fst % 2 == 0) {
        return make_coordtuple(
            make_tuple(edge.fst / 2, edge.snd),
            make_tuple(edge.fst / 2, edge.snd + 1));
    } else {
        return make_coordtuple(
            make_tuple(edge.fst / 2, edge.snd),
            make_tuple(edge.fst / 2 + 1, edge.snd));
    }
}

// Generate disjoint set id from cell index.
int tuple_to_id(Tuple tuple, int size) {
    return tuple.fst * size + tuple.snd + 1;
}

// Generate disjoint set id from nested tuples.
Tuple coordtuple_to_id(CoordTuple coords, int size) {
    Tuple tuple;
    tuple.fst = tuple_to_id(coords.fst, size);
    tuple.snd = tuple_to_id(coords.snd, size);
    return tuple;
}

// Make maze with random edge selection and disjoint set.
Matrix make_maze(int size) {
    // generate edge matrix and disjoint sets.
    Matrix matrix = make_matrix(2 * size - 1, size);
    DisjointSet disjoint_set = make_disjoint_set(size);

    // until all elements are in one disjoint set.
    while (!all_union(&disjoint_set)) {
        // get random edge.
        Tuple edge = random_edge(size);
        // if edge already deleted.
        if (matrix.ptr[edge.fst][edge.snd] == 1) {
            continue;
        }

        // get ids of adjacent cells.
        CoordTuple coords = get_adjacent(edge, size);
        Tuple ids = coordtuple_to_id(coords, size);

        // try to union and set edge matrix to represent deletion if union success.
        int res = union_set(&disjoint_set, ids.fst, ids.snd);
        if (res == 1) {
            matrix.ptr[edge.fst][edge.snd] = 1;
        }
    }

    // free disjoint set.
    delete_disjoint_set(disjoint_set);
    return matrix;
}

// Print maze to given output stream.
void print_maze(Matrix maze, int size, FILE* output) {
    char chr;
    int i, j;
    fprintf(output, "+");
    for (i = 0; i < size; ++i) {
        fprintf(output, "-+");
    }
    fprintf(output, "\n");

    for (i = 0; i < 2 * size - 1; ++i) {
        if (i % 2 == 0) {
            fprintf(output, "|");
            for (j = 0; j < size - 1; ++j) {
                if (!maze.ptr[i][j]) {
                    chr = '|';
                } else {
                    chr = ' ';
                }
                fprintf(output, " %c", chr);

            }
            fprintf(output, " |\n");
        } else {
            fprintf(output, "+");
            for (j = 0; j < size; ++j) {
                if (!maze.ptr[i][j]) {
                    chr = '-';
                } else {
                    chr = ' ';
                }
                fprintf(output, "%c+", chr);
            }
            fprintf(output, "\n");
        }
    }
    fprintf(output, "+");
    for (i = 0; i < size; ++i) {
        fprintf(output, "-+");
    }
    fprintf(output, "\n");
}

int main() {
    srand(time(NULL));

    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");
    
    int size;
    fscanf(input, "%d", &size);

    Matrix maze = make_maze(size);
    print_maze(maze, size, output);

    delete_matrix(maze);
    fclose(input);
    fclose(output);
    return 0;
}