#include <stdio.h>
#include <stdlib.h>
#include <list.h>
#include <alloc.h>

list *list_create() {
    list *l = xmalloc(sizeof(list));

    l->head = NULL;
    l->tail = NULL;
    l->length = 0;
    l->func_cmp = NULL;
    l->func_dup = NULL;
    l->func_free = NULL;

    return l;
}

node *node_create(void *v) {
    node *n = xmalloc(sizeof(node));
    n->data = v;
    n->prev = NULL;
    n->next = NULL;

    return n;
}

void list_push(list *l, void *v) {
    node *n = NULL;

    if(l->func_dup) n = node_create(l->func_dup(v));
    else n = node_create(v);

    if(l->tail == NULL) {
        l->tail = l->head = n;
    } else {
        l->tail->next = n;
        n->prev = l->tail;
        l->tail = n;
    }

    l->length++;
}

node *list_pop(list *l) {
    if(l->length == 0) return NULL;
    node *n = l->tail;
    l->length--;

    if(l->tail == l->head) {
        l->head = NULL;
        l->tail = NULL;
    } else {
        l->tail->prev->next = NULL;
        n->prev = NULL;
    }

    return n;
}

void *list_get(list *l, int index) {
    if (index >= l->length) return NULL;
    node *n = l->head;

    for(uint i = 0; i < l->length; i++) {
        if (i == index) {
            return n;
        }
        n = n->next;
    }
    return NULL;
}

void list_free(void *l) {
    list *li = (list*)l;
    node *curr = li->head;
    while(curr != NULL) {
        node *next = curr->next;
        if(li->func_free) li->func_free(curr->data);
        free(curr);
        curr = next;
    }
    free(l);
}

void list_print(list *l) {
    printf("%16p << %d >> %16p\n", l->head, l->length, l->tail);
    node *curr = l->head;
    while(curr != NULL) {
        printf("%16p <- %16p -> %16p\n", curr->prev, curr->data, curr->next);
        curr = curr->next;
    }
}
