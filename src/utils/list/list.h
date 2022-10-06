#ifndef LIST_H
#define LIST_H

#include "stdlib.h"
#include <stddef.h>

typedef struct node_t {
    struct node_t *prev;
    struct node_t *next;
    void * data;
} node;

typedef void  (*list_func_free) (void *);
typedef void* (*list_func_dup)  (void *);

typedef struct list_t {
    node *head;
    node *tail;
    list_func_dup func_dup;
    list_func_free func_free;
    size_t length;
} list;

list *list_create();
void list_push(list *l, void *v);
node *list_pop(list *l);
void list_free(void *l);
void list_print(list *l);

#endif // !LIST_H
