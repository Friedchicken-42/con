#include "on.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _print_con(Object* o);

Object* create_con() {
    Object* o;
    o = malloc(sizeof(Object));
    o->next = NULL;
    return o;
}

void print_key(char key[100]) { printf("\"%s\": ", key); }

void print_value(Value v) {
    switch (v.type) {
        case INT:
            printf("%d", v._int);
            break;
        case DOUBLE:
            printf("%f", v._double);
            break;
        case STRING:
            printf("\"%s\"", v._string);
            break;
        // case ARRAY:
        //     break;
        case OBJECT:
            _print_con(v._obj);
            break;
    }
}

void print_pair(KeyValue* pair) {
    print_key(pair->key);
    print_value(pair->value);
}

void _print_con(Object* o) {
    Object* t = o;
    printf("{");
    while (t->next != NULL) {
        print_pair(&t->pair);
        t = t->next;
        if (t->next) printf(", ");
    }
    printf("}");
}

void print_con(Object* o) {
    _print_con(o);
    printf("\n");
}

uint add_value(Object* o, char key[100], enum V_type type, void* data) {
    Object* t = o;

    while (t->next != NULL) {
        if (strcmp(t->pair.key, key) == 0) {
            printf("Cannot add key \"%s\", already present\n", key);
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
        case STRING:
            strcpy(t->pair.value._string, (char*)data);
            break;
        case ARRAY:
            break;
        case OBJECT:
            t->pair.value._obj = (Object*)data;
            break;
        default:
            break;
    }
    return 0;
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

int main() {
    Object *o, *o1;
    o = create_con();

    int x = 42;
    add_value(o, "a", INT, &x);

    int y = 100;
    o1 = create_con();
    add_value(o1, "d", INT, &y);

    add_value(o, "b", OBJECT, o1);

    char str[] = "test";
    add_value(o, "c", STRING, &str);
    print_con(o);

    // Object* o2;
    Value v;
    v = get_value(o, "b");
    print_value(v);

    return 0;
}