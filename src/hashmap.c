#include <hashmap.h>
#include <alloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    return string_cmp(a, b);
}

int hashmap_hash_str(void *key) {
    uint hash = 0;
    char *k = (char*)key;
    while(*k != '\0') {
        hash += *k;
        k++;
    }

    return hash;
}

hashmap *hashmap_create_str(uint size) {
    hashmap *h = hashmap_create(size);
    h->func_cmp = hashmap_cmp_str;
    h->func_hash = hashmap_hash_str;
    h->func_dup = string_dup;
    h->func_free_key = free;
    h->func_free_value = NULL;

    h->keys->func_cmp = hashmap_cmp_str;

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
    entry *e;

    if(node == NULL) {
        h->entries[hash] = entry_create(key, value, h->func_dup);
        e = h->entries[hash];
    } else {
        entry *curr = node;
        entry *prev = NULL;
        while(curr != NULL) {
            if(!h->func_cmp(key, curr->key)) {
                curr->value = value;
                return 1;
            }
            prev = curr;
            curr = curr->next;
        }

        prev->next = entry_create(key, value, h->func_dup);
        e = prev->next;
    }

    list_push(h->keys, e->key);
    return 0;
}

entry *hashmap_get_entry(hashmap *h, void *key) {
    int hash = h->func_hash(key) % h->size;
    entry *curr = h->entries[hash];

    while(curr != NULL) {
        if(h->func_cmp(key, curr->key) == 0) return curr;
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
    int hash = h->func_hash(key) % h->size;
    entry *curr = hashmap_get_entry(h, key);
    if(curr == NULL) return 1;

    if(curr->prev) curr->prev->next = curr->next;
    if(curr->next) curr->next->prev = curr->prev;
    if(curr == h->entries[hash]) h->entries[hash] = curr->next;

    list_remove(h->keys, key);
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
