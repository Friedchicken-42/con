#include "on.h"
#include "utils/hashmap/hashmap.h"
#include <stdio.h>
#include <stdlib.h>

on *on_create() {
    on *o = malloc(sizeof(on));

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

/*

   ON_EMPTY,
   ON_NULL,
   ON_STRING,
   ON_INTEGER,
   ON_FLOAT,
   ON_TRUE,
   ON_FALSE,
   ON_OBJECT,
   ON_ARRAY,
   */

void on_print(on *o, int tab) {
    const char* table[9] = {
        "Empty", "Null", "String", "Integer", "Float", "True", "False", "Object", "Array"
    };
    printf("type: %s\n", table[o->type]);
    switch (o->type) {
        case ON_EMPTY:
            break;
        case ON_NULL:
            printf("null\n");
            break;
        case ON_INTEGER:
            printf("%d\n", *(int*)o->data);
            break;
        case ON_STRING:
            printf("%s\n", (char*)o->data);
            break;
        case ON_FLOAT:
        case ON_TRUE:
        case ON_FALSE:
        case ON_OBJECT:
            printf("{\n");
            tab++;
            hashmap *map = (hashmap*)o->data;
            for(int i = 0; i < map->size; i++) {
                entry *node = map->entries[i];
                while(node) {
                    printf("key: %s\n", (char*)node->key);
                    on_print((on*)node->value, tab + 1);
                    node = node->next;
                }
            }
            printf("}\n");
        case ON_ARRAY:
          break;
    }
}
