#include "on.h"
#include "utils/alloc/alloc.h"
#include "utils/hashmap/hashmap.h"
#include "utils/list/list.h"
#include "utils/string/string.h"
#include <stdio.h>
#include <stdlib.h>

on *on_create() {
    on *o = xmalloc(sizeof(on));

    o->data = NULL;
    o->type = ON_EMPTY;
    o->func_free = free;

    return o;
}

on *on_create_object() {
    on* obj = on_create();
    obj->type = ON_OBJECT;
    obj->func_free = hashmap_free;

    hashmap *map = hashmap_create_str(5);
    map->func_free_value = on_free;

    obj->data = map;

    return obj;
}

on *on_create_array() {
    on* arr = on_create();
    arr->type = ON_ARRAY;
    arr->func_free = list_free;

    list* l = list_create();
    l->func_free = on_free;

    arr->data = l;

    return arr;
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

void on_add_string(on *o, char* value) {
    o->type = ON_STRING;
    o->data = string_dup(value);
    o->func_free = free;
}

void on_add_integer(on *o, int value) {
    int *x = malloc(sizeof(int));
    *x = value;

    o->type = ON_INTEGER;
    o->data = x;
    o->func_free = free;
}

void on_add_float(on *o, float value) {
    float *x = malloc(sizeof(float));
    *x = value;

    o->type = ON_FLOAT;
    o->data = x;
    o->func_free = free;
}

on *on_create_type(void *value, enum on_type type) {
    on* o = NULL;

    if (type != ON_OBJECT && type != ON_ARRAY) o = on_create();

    switch (type) {
        case ON_EMPTY:
        case ON_NULL:
            o->type = ON_NULL;
            break;
        case ON_STRING:
            on_add_string(o, (char*)value);
            break;
        case ON_INTEGER:
            on_add_integer(o, *(int*)value);
            break;
        case ON_FLOAT:
            on_add_float(o, *(float*)value);
            break;
        case ON_TRUE:
            o->type = ON_TRUE;
            break;
        case ON_FALSE:
            o->type = ON_FALSE;
            break;
        case ON_OBJECT:
            if (value != NULL) o = value;
            else o = on_create_object();
            break;
        case ON_ARRAY:
            if (value != NULL) o = value;
            else o = on_create_array();
            break;
    }

    return o;
}

int on_add_object(on *o, char *key, void *value, enum on_type type) {
    hashmap *map = (hashmap*)o->data;
    on *obj = on_create_type(value, type);

    int status = hashmap_set(map, key, obj);
    if(status != 0) on_free(obj);

    return 0;
}

int on_add_array(on *o, void *value, enum on_type type) {
    list *arr = (list*)o->data;
    on *obj = on_create_type(value, type);

    list_push(arr, obj);
    return 0;
}

int on_add(on *o, char *key, void *value, enum on_type type) {
    if(o->type != ON_OBJECT && o->type != ON_ARRAY) return -1;
    if(o->type == ON_OBJECT && key == NULL) return -2;
    if(o->type == ON_ARRAY && key != NULL) return -3;

    if(o->type == ON_OBJECT) on_add_object(o, key, value, type);
    if(o->type == ON_ARRAY) on_add_array(o, value, type);

    return 0;
}

void *on_get(on *o, void *key) {
    if(o->type == ON_OBJECT) return hashmap_get(o->data, key);
    if(o->type == ON_ARRAY)  return list_get(o->data, (size_t)key);
    return NULL;
}

void on_print(on *o) {
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
                on_print((on*)(hashmap_get(map, k->data)));
            }
            printf("}\n");
            break;
        case ON_ARRAY:
            printf("[\n");
            list *l = (list*)o->data;
            int i = 0;
            for(node *n = l->head; n != NULL; n = n->next) {
                printf("index: %d\n", i);
                on_print((on*)n->data);
                i++;
            }

            printf("]\n");
          break;
    }
}
