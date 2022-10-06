#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include "../list/list.h"

typedef struct entry_t {
    void *key;
    void *value;
    struct entry_t *prev;
    struct entry_t *next;
} entry;

typedef int   (*hashmap_func_cmp)  (void *, void *);
typedef int   (*hashmap_func_hash) (void *);
typedef void  (*hashmap_func_free) (void *);
typedef void* (*hashmap_func_dup)  (void *);

typedef struct hashmap_t {
    hashmap_func_cmp func_cmp;
    hashmap_func_hash func_hash;
    hashmap_func_dup func_dup;
    hashmap_func_free func_free_key;
    hashmap_func_free func_free_value;

    entry **entries;
    size_t size;
} hashmap;

hashmap *hashmap_create_str(size_t size);
int hashmap_set(hashmap *h, void *key, void *value);
entry *hashmap_get(hashmap *h, void *key);
int hashmap_remove(hashmap *h, void *key);
int hashmap_contains(hashmap *h, void *key);
list *hashmap_keys(hashmap *h);
void hashmap_free(void *h);

void hashmap_print(hashmap *h);

#endif // !HASHMAP_H
