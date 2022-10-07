#include "on.h"
#include "utils/alloc/alloc.h"
#include "utils/hashmap/hashmap.h"
#include "utils/list/list.h"
#include <stdio.h>
#include <stdlib.h>

on *on_create() {
    on *o = xmalloc(sizeof(on));

    o->data = NULL;
    o->type = ON_EMPTY;
    o->func_free = free;

    return o;
}

void on_free(void* o) {
    on *obj = (on*)o;
    if(obj->func_free) obj->func_free(obj->data);
    free(obj);
}

const char* on_type_string(on *o) {
    const char* table[9] = {
        [ON_EMPTY] = "Empty",
        [ON_NULL] = "Null",
        [ON_STRING] = "String",
        [ON_INTEGER] = "Integer",
        [ON_FLOAT] = "Float",
        [ON_TRUE] = "True",
        [ON_FALSE] = "False",
        [ON_OBJECT] = "Object",
        [ON_ARRAY] = "Array",
    };

    return table[o->type];
}

void on_print(on *o, int tab) {
    printf("type: %s\n", on_type_string(o));
    switch (o->type) {
        case ON_EMPTY:
            break;
        case ON_NULL:
            printf("Null\n");
            break;
        case ON_INTEGER:
            printf("%d\n", *(int*)o->data);
            break;
        case ON_STRING:
            printf("%s\n", (char*)o->data);
            break;
        case ON_FLOAT:
            printf("%f\n", *(float*)o->data);
            break;
        case ON_TRUE:
            printf("True\n");
            break;
        case ON_FALSE:
            printf("False\n");
            break;
        case ON_OBJECT:
            printf("{\n");
            hashmap *map = (hashmap*)o->data;
            list *keys = map->keys;
            for(node *k = keys->head; k != NULL; k = k->next) {
                printf("key: %s\n", (char*)k->data);
                on_print((on*)(hashmap_get(map, k->data)), tab);
            }
            printf("}\n");
            break;
        case ON_ARRAY:
            printf("[\n");
            list *l = (list*)o->data;
            int i = 0;
            for(node *n = l->head; n != NULL; n = n->next) {
                printf("index: %d\n", i);
                on_print((on*)n->data, tab);
                i++;
            }

            printf("]\n");
          break;
    }
}
