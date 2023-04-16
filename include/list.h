#ifndef LIST_H
#define LIST_H

typedef unsigned int uint;

typedef void  (*list_func_free) (void *);
typedef void* (*list_func_dup)  (void *);
typedef int   (*list_func_cmp)  (void *, void *);

typedef struct list_t {
    list_func_dup func_dup;
    list_func_free func_free;
    list_func_cmp func_cmp;

    uint length;
    uint size;
    void **data;
} list;

list *list_create();
void list_push(list *l, void *v);
void *list_pop(list *l);
int list_remove(list *l, void *v);
void *list_get(list *l, int index);

void list_free(void *l);
void list_print(list *l);

#endif // !LIST_H
