#include <alloc.h>
#include <hashmap.h>
#include <list.h>
#include <on.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

on *on_create() {
    on *o = xmalloc(sizeof(on));
    o->data = NULL;
    o->type = ON_EMPTY;
    o->func_free = NULL;
    return o;
}

void on_set_string(on *o, char *value) {
    o->type = ON_STRING;
    if (value != NULL) o->data = string_dup(value);
    o->func_free = free;
}

void on_set_object(on *o) {
    o->type = ON_OBJECT;
    o->func_free = hashmap_free;

    hashmap *map = hashmap_create_str(5);
    map->func_free_value = on_free;

    o->data = map;
}

void on_set_array(on *o) {
    o->type = ON_ARRAY;
    o->func_free = list_free;

    list *l = list_create();
    l->func_free = on_free;

    o->data = l;
}

void on_clear(on *o) {
    if (o->func_free) o->func_free(o->data);
    o->data = NULL;
    o->type = ON_EMPTY;
    o->func_free = NULL;
}

void on_set(on *o, void *value, enum on_type type) {
    on_clear(o);
    o->type = type;

    switch (type) {
    case ON_EMPTY:
    case ON_NULL:
    case ON_TRUE:
    case ON_FALSE:
        break;
    case ON_INTEGER:
        o->number = *(int *)value;
        break;
    case ON_DOUBLE:
        o->number = *(double *)value;
        break;
    case ON_STRING:
        on_set_string(o, (char *)value);
        break;
    case ON_OBJECT:
        on_set_object(o);
        break;
    case ON_ARRAY:
        on_set_array(o);
        break;
    }
}

on *on_create_object() {
    on *o = on_create();
    on_set(o, NULL, ON_OBJECT);
    return o;
}

on *on_create_array() {
    on *o = on_create();
    on_set(o, NULL, ON_ARRAY);
    return o;
}

int on_add_object(on *o, char *key, void *value) {
    hashmap *map = (hashmap *)o->data;

    void *prev = hashmap_get(map, key);
    int status = hashmap_set(map, key, value);
    if (status != 0 && prev == 0) return 1;
    if (status != 0) on_free(prev);

    return 0;
}

int on_add_array(on *o, void *value) {
    list *arr = (list *)o->data;
    list_push(arr, value);
    return 0;
}

int on_add_value(on *o, char *key, on *value) {
    if (o->type != ON_OBJECT && o->type != ON_ARRAY) return -1;
    if (o->type == ON_OBJECT && key == NULL) return -2;
    if (o->type == ON_ARRAY && key != NULL) return -3;

    if (o->type == ON_OBJECT) return on_add_object(o, key, value);
    if (o->type == ON_ARRAY) return on_add_array(o, value);
    return 0;
}

int on_add(on *o, char *key, void *value, enum on_type type) {
    on *obj = value;
    if (type != ON_OBJECT && type != ON_ARRAY) {
        obj = on_create();
        on_set(obj, value, type);
    }

    int status = on_add_value(o, key, obj);
    if (status != 0) on_free(obj);

    return status;
}

void *on_get_on(on *o, void *key) {
    if (o->type == ON_OBJECT) return hashmap_get(o->data, key);
    if (o->type == ON_ARRAY) return list_get(o->data, (size_t)key);
    return NULL;
}

void *on_get(on *o, void *key) {
    on *value = on_get_on(o, key);
    if (value == NULL) return NULL;

    if (o->type == ON_INTEGER || o->type == ON_DOUBLE) {
        return &o->number;
    } else {
        return value->data;
    }
}

const char *on_type_string(enum on_type type) {
    const char *table[9] = {
        [ON_EMPTY] = "Empty",   [ON_NULL] = "Null",
        [ON_STRING] = "String", [ON_INTEGER] = "Integer",
        [ON_DOUBLE] = "Double", [ON_TRUE] = "True",
        [ON_FALSE] = "False",   [ON_OBJECT] = "Object",
        [ON_ARRAY] = "Array",
    };

    return table[type];
}

void on_print_(on *o, int tabs) {
    for (int i = 0; i < tabs; i++)
        printf("\t");

    printf("type: %s\n", on_type_string(o->type));

    if (o->type != ON_EMPTY) {
        for (int i = 0; i < tabs; i++)
            printf("\t");
    }

    switch (o->type) {
    case ON_EMPTY:
        break;
    case ON_NULL:
        printf("Null\n");
        break;
    case ON_INTEGER:
        printf("%d\n", (int)o->number);
        break;
    case ON_STRING:
        printf("%s\n", (char *)o->data);
        break;
    case ON_DOUBLE:
        printf("%f\n", (double)o->number);
        break;
    case ON_TRUE:
        printf("True\n");
        break;
    case ON_FALSE:
        printf("False\n");
        break;
    case ON_OBJECT:
        printf("{\n");
        hashmap *map = (hashmap *)o->data;
        list *keys = map->keys;
        for (uint i = 0; i < keys->length; i++) {
            char *key = (char *)list_get(keys, i);

            for (int i = 0; i < tabs + 1; i++)
                printf("\t");

            printf("key: %s\n", key);

            on_print_((on *)(hashmap_get(map, key)), tabs + 2);
        }

        for (int i = 0; i < tabs; i++)
            printf("\t");

        printf("}\n");
        break;
    case ON_ARRAY:
        printf("[\n");
        list *l = (list *)o->data;
        for (uint i = 0; i < l->length; i++) {
            for (int i = 0; i < tabs + 1; i++)
                printf("\t");

            printf("index: %d\n", i);

            on_print_((on *)list_get(l, i), tabs + 2);
        }

        for (int i = 0; i < tabs; i++)
            printf("\t");

        printf("]\n");
        break;
    }
}

void on_print(on *o) { on_print_(o, 0); }

void on_free(void *o) {
    on *obj = (on *)o;
    if (obj->func_free) obj->func_free(obj->data);
    free(obj);
}
