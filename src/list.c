#include <stdio.h>
#include <stdlib.h>
#include <list.h>
#include <alloc.h>

list *list_create() {
    list *l = xmalloc(sizeof(list));

    l->func_dup = NULL;
    l->func_free = NULL;
    l->func_cmp = NULL;

    l->length = 0;
    l->size = 16;
    l->data = xmalloc(sizeof(void*) * l->size);
    
    return l;
}

void list_push(list *l, void *v) {
    void *n = v;
    if(l->func_dup) n = l->func_dup(v);

    if(l->length == l->size - 1) {
        l->size *= 2;
        l->data = realloc(l->data, l->size);
    }
    printf("%d ~ %d\n", l->length, l->size);
    l->data[l->length] = n;

    l->length++;
}

void *list_pop(list *l) {
    if(l->length == 0) return NULL;
    l->length--;
    return l->data[l->length];
}

int list_remove(list *l, void *v) {
    if(l->length == 0) return -1;
    if(l->func_cmp == NULL) return -2;

    uint i = 0;
    for(i = 0; i < l->length; i++) {
        if(!l->func_cmp(v, l->data[i])) break;
    }

    if(l->func_free) l->func_free(l->data[i]);
    for(i = i + 1; i < l->length; i++) {
        l->data[i - 1] = l->data[i];
    }
    l->length--;

    return 0;
}

void *list_get(list *l, int index) {
    if (index >= l->length) return NULL;
    return l->data[index];
}

void list_free(void *l) {
    list *li = (list*)l;
    if(li->func_free) {
        for(uint i = 0; i < li->length; i++) li->func_free(li->data[i]);
    }
    free(li->data);
    free(l);
}

void list_print(list *l) {
    printf("%16p:  %d ~ %d\n", l->data, l->length, l->size);
    for(uint i = 0; i < l->length; i++) {
        printf("    %16p\n", l->data[i]);
    }
}
