#include "on.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Object* create_con() {
    Object* o;
    o = malloc(sizeof(Object));
    o->next = NULL;
    return o;
}

uint _add_value(Object* o, char key[100], char key_check, enum V_type type, void* data) {
    Object* t = o;

    while (t->next != NULL) {
        if (key_check == 1 && strcmp(t->pair.key, key) == 0) {
            return 1;
        }
        t = t->next;
    }

    t->next = create_con();
    strcpy(t->pair.key, key);
    t->pair.value.type = type;
    switch (type) {
        case INT:
            t->pair.value._int = *(int*)data;
            break;
        case DOUBLE:
            t->pair.value._double = *(double*)data;
            break;
        case BOOLEAN:
            t->pair.value._boolean = *(char*)data;
            break;
        case STRING:
            strcpy(t->pair.value._string, (char*)data);
            break;
        case ARRAY:
            t->pair.value._obj = create_con();
            break;
        case OBJECT:
            t->pair.value._obj = (Object*)data;
            break;
        default:
            break;
    }
    return 0;
}

void add_value(Object* o, char key[100], enum V_type type, void* data) {
    uint res;
    res = _add_value(o, key, 1, type, data);
    if (res == 1) {
        printf("Cannot add key \"%s\", already present\n", key);
    }
}

void add_item_array(Object* o, enum V_type type, void* data) {
    ;
    _add_value(o->pair.value._obj, "", 0, type, data);
}

Object* get(Object* o, char key[100]) {
    Object *t, *n;
    t = o;
    n = create_con();
    n->next = create_con();

    while (t->next != NULL) {
        if (strcmp(t->pair.key, key) == 0) {
            n->pair = t->pair;
            return n;
        }
        t = t->next;
    }

    return NULL;
}

Value get_value(Object* o, char key[100]) {
    Object* t;
    t = get(o, key);
    return t->pair.value;
}