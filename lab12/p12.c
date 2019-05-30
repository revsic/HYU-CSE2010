#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration.
struct HashTable_;

// Forward declaration.
struct Hasher_;

// Type alias for hash function.
typedef int(*hasher_t)(int, void*);
// Type alias for hash function updator.
typedef void(*update_hasher_t)(struct Hasher_*, struct HashTable_*);

// Structure for hasher.
typedef struct Hasher_ {
    hasher_t hasher;
    update_hasher_t updater;
    void* data;
} Hasher;

// Forward declaration.
struct Resolver_;

// Type alias for hash collison resolver.
typedef int(*resolver_t)(int, int, void*);

// Type alis for resolver updator.
typedef void(*update_resolver_t)(struct Resolver_*, struct HashTable_*);

// Structure for hash collison resolver.
typedef struct Resolver_ {
    resolver_t resolver;
    update_resolver_t updater;
    void* data;
} Resolver;

// Structure for hash table.
typedef struct HashTable_ {
    int size;
    int* table;
    Hasher hasher;
    Resolver resolver;
} HashTable;

// Structure for result of method find_space.
typedef struct {
    int resolve;
    int idx;
} FindResult;

// Construct FindResult with given resolve and idx value.
FindResult find_result(int resolve, int idx) {
    FindResult result;
    result.resolve = resolve;
    result.idx = idx;
    return result;
}

// Modular based hash function.
int modular_hasher_f(int value, void* mod) {
    return value % *(int*)mod;
}

// Construct modular hashing structure.
Hasher modular_hasher(int mod) {
    Hasher hasher;
    hasher.hasher = modular_hasher_f;
    hasher.updater = NULL;
    hasher.data = malloc(sizeof(int));
    *(int*)hasher.data = mod;
    return hasher;
}

// Linear collison resolver.
int linear_resolver_f(int value, int resolve, void* data) {
    return resolve;
}

// Construct linear collison resolver structure.
Resolver linear_resolver() {
    Resolver resolver;
    resolver.resolver = linear_resolver_f;
    resolver.updater = NULL;
    resolver.data = NULL;
    return resolver;
}

// Quadratic collison resolver.
int quadratic_resolver_f(int value, int resolve, void* data) {
    return resolve * resolve;
}

// Construct quadratic collison resolver structure.
Resolver quadratic_resolver() {
    Resolver resolver;
    resolver.resolver = quadratic_resolver_f;
    resolver.updater = NULL;
    resolver.data = NULL;
    return resolver;
}

// Double hashing collison resolver.
int double_resolver_f(int value, int resolve, void* data) {
    int mod = *(int*)data;
    return resolve * (mod - (value % mod));
}

// Construct double hashing collison resolver structure.
Resolver double_resolver(int mod) {
    Resolver resolver;
    resolver.resolver = double_resolver_f;
    resolver.updater = NULL;
    resolver.data = malloc(sizeof(int));
    *(int*)resolver.data = mod;
    return resolver;
}

// Fill memory with given value.
void fill(int* ptr, int size, int value) {
    int i;
    for (i = 0; i < size; ++i) {
        ptr[i] = value;
    }
}

// Construct empty hash table with given hasher and resolver.
HashTable empty(int size, Hasher hasher, Resolver resolver) {
    HashTable table;
    table.size = size;
    table.table = malloc(sizeof(int) * size);
    table.hasher = hasher;
    table.resolver = resolver;
    fill(table.table, size, 0);
    return table;
}

// Construct hashtable with modular hash function.
HashTable modular_hashtable(int size, Resolver resolver) {
    return empty(size, modular_hasher(size), resolver);
}

// Construct modular based hashtable with given c-string.
HashTable modular_hashtable_cstr(int size, const char* str) {
    if (!strcmp(str, "Quadratic")) {
        return modular_hashtable(size, quadratic_resolver());
    } else if (!strcmp(str, "Double")) {
        return modular_hashtable(size, double_resolver(7));
    }
    // default linear
    return modular_hashtable(size, linear_resolver());
}

// Free hash table structure.
void delete_hashtable(HashTable table) {
    free(table.table);
    free(table.hasher.data);
    free(table.resolver.data);
}

// Run hash function with given value.
int runhasher(HashTable* hashtable, int value) {
    Hasher* hasher = &hashtable->hasher;
    return hasher->hasher(value, hasher->data);
}

// Run collison resolver with given value.
int runresolver(HashTable* hashtable, int value, int resolve) {
    Resolver* resolver = &hashtable->resolver;
    return resolver->resolver(value, resolve, resolver->data);
}

// Hashing i-th value with given hash table.
int hash(HashTable* hashtable, int value, int i) {
    return (runhasher(hashtable, value) + runresolver(hashtable, value, i)) % hashtable->size;
}

// Find space from hash table.
FindResult find_space(HashTable* hashtable, int value) {
    int i = -1, idx;
    do {
        idx = hash(hashtable, value, ++i);
    } while (i < hashtable->size
        && hashtable->table[idx] > 0
        && hashtable->table[idx] != value);
    return find_result(i, idx);
}

// Find value from hash table.
int find(HashTable* hashtable, int value) {
    int idx = find_space(hashtable, value).idx;
    return hashtable->table[idx] == value ? idx : -1;
}

// Insert value to hash table.
int insert(HashTable* hashtable, int value) {
    int idx = find_space(hashtable, value).idx;
    // if value found
    if (hashtable->table[idx] == value) {
        return -1;
    }
    hashtable->table[idx] = value;
    return idx;
}

// Delete value from hash table.
int delete(HashTable* hashtable, int value) {
    int idx = find_space(hashtable, value).idx;
    // if value not found
    if (hashtable->table[idx] <= 0) {
        return -1;
    }
    hashtable->table[idx] = -1;
    return idx;
}

// Print hash table.
void print(HashTable* hashtable, FILE* output) {
    int i, num;
    for (i = 0; i < hashtable->size; ++i) {
        num = hashtable->table[i];
        fprintf(output, "%d ", num > 0 ? num : 0);
    }
}

int main() {
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    // input number of test
    int n_test;
    fscanf(input, "%d", &n_test);

    int i;
    for (i = 0; i < n_test; ++i) {
        // input resolver type
        int size;
        char str[1024] = { 0, };
        fscanf(input, "%s %d", str, &size);
        HashTable hashtable = modular_hashtable_cstr(size, str);

        fprintf(output, "%s\n", str);
        while (1) {
            // input option
            fscanf(input, "%s", str);
            if (str[0] == 'q') {
                break;
            }

            int num;
            switch (str[0]) {
            case 'i':
                fscanf(input, "%d", &num);
                if (insert(&hashtable, num) != -1) {
                    fprintf(output, "Inserted %d\n", num);
                } else {
                    fprintf(output, "Already exists\n");
                }
                break;
            case 'd':
                fscanf(input, "%d", &num);
                if (delete(&hashtable, num) != -1) {
                    fprintf(output, "Deleted %d\n", num);
                } else {
                    fprintf(output, "%d not exists\n", num);
                }
                break;
            case 'f':
                fscanf(input, "%d", &num);
                num = find(&hashtable, num);
                if (num != -1) {
                    fprintf(output, "%d\n", num);
                } else {
                    fprintf(output, "Not found\n");
                }
                break;
            case 'p':
                print(&hashtable, output);
                fprintf(output, "\n");
                break;
            default:
                break;
            }
        }

        delete_hashtable(hashtable);
        fprintf(output, "\n");
    }

    return 0;
}