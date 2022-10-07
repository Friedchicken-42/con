#include "hashmap.h"
#include "../alloc/alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;

entry *entry_create(void *key, void *value, hashmap_func_dup dup) {
    entry *e = xmalloc(sizeof(entry));
    e->key = dup(key);
    e->value = (void*)value;
    e->prev = NULL;
    e->next = NULL;

    return e;
}

hashmap *hashmap_create(uint size) {
    hashmap *h = xmalloc(sizeof(hashmap));

    h->size = size;
    h->entries = xmalloc(sizeof(entry*) * size);
    h->func_cmp = NULL;
    h->func_hash = NULL;
    h->func_dup = NULL;
    h->func_free_key = NULL;
    h->func_free_value = NULL;
    h->keys = list_create();
    for(int i = 0; i < size; i++) h->entries[i] = NULL;

    return h;
}

int hashmap_cmp_str(void *a, void *b) {
    return strcmp(a, b);
}

int hashmap_hash_str(void *key) {
    //will fix this

    uint hash = 0;
    char *k = (char*)key;
    while(*k != '\0') {
        hash += *k;
        k++;
    }

    return hash;
}

void *hashmap_dup_str(void *str) {
    char *x = (char*)str;
    uint len = 0;
    for (len = 0; x[len] != '\0'; len++);
    char *dest = xmalloc(len + 1);
    strcpy(dest, x);

    return dest;
}

hashmap *hashmap_create_str(uint size) {
    hashmap *h = hashmap_create(size);
    h->func_cmp = hashmap_cmp_str;
    h->func_hash = hashmap_hash_str;
    h->func_dup = hashmap_dup_str;
    h->func_free_key = free;
    h->func_free_value = NULL;

    h->keys->func_free = free;
    h->keys->func_dup = hashmap_dup_str;

    return h;
}

int hashmap_contains(hashmap *h, void *key) {
    int hash = h->func_hash(key) % h->size;
    entry *curr = h->entries[hash];

    while(curr != NULL) {
        if(!h->func_cmp(key, curr->key)) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

int hashmap_set(hashmap *h, void *key, void *value) {
    int hash = h->func_hash(key) % h->size;
    entry *node = h->entries[hash];

    if(node == NULL) {
        h->entries[hash] = entry_create(key, value, h->func_dup);
    } else {
        entry *curr = node;
        while(curr->next != NULL) {
            if(!h->func_cmp(key, curr->key)) return 1;
            curr = curr->next;
        }

        curr->next = entry_create(key, value, h->func_dup);
    }

    list_push(h->keys, key);
    return 0;
}

entry *hashmap_get_entry(hashmap *h, void *key) {
    int hash = h->func_hash(key) % h->size;
    entry *curr = h->entries[hash];

    while(curr != NULL) {
        if(!h->func_cmp(key, curr->key)) return curr;
        curr = curr->next;
    }

    return NULL;
}

void *hashmap_get(hashmap *h, void *key) {
    entry *e = hashmap_get_entry(h, key);
    if(e == NULL) return NULL;
    return e->value;
}

void hashmap_free_entry(hashmap *h, entry *e) {
    h->func_free_key(e->key);
    if (h->func_free_value) h->func_free_value(e->value);
    free(e);
}

int hashmap_remove(hashmap *h, void *key) {
    entry *curr = hashmap_get(h, key);
    if(curr == NULL) return 1;

    if(curr->prev) curr->prev->next = curr->next;
    if(curr->next) curr->next->prev = curr->prev;

    hashmap_free_entry(h, curr);

    return 0;
}

void hashmap_free(void *h) {
    hashmap *map = (hashmap*)h;
    for(int i = 0; i < map->size; i++) {
        entry *curr = map->entries[i];
        entry *next = NULL;
        while(curr != NULL) {
            next = curr->next;
            hashmap_free_entry(h, curr);
            curr = next;
        }
    }
    free(map->entries);
    list_free(map->keys);
    free(h);
}

list *hashmap_keys(hashmap *h) {
    return h->keys;
}

void hashmap_print(hashmap *h) {
    printf(" -- %d -- \n", h->size);
    list_print(h->keys);
    for(int i = 0; i < h->size; i++) {
        entry *node = h->entries[i];
        printf("[%02d] ", i);
        if(node) {
            while(node) {
                printf("%s %p", (char*)(node->key), node->value);
                node = node->next;
                if(node) printf("  ->  ");
            }
        }
        printf("\n");
    }
}
