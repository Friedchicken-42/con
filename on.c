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

void print_pair(Object* o) {
    printf("\"%s\": ", o->key);
    switch (o->value.type) {
        case INT:
            printf("%d", o->value._int);
            break;
        case DOUBLE:
            printf("%f", o->value._double);
            break;
        case STRING:
            printf("\"%s\"", o->value.string);
            break;
        case ARRAY:
            break;
        case OBJECT:
            _print_con(o->value._obj);
            break;
    }
}

void _print_con(Object* o) {
    Object* t = o;
    printf("{");
    while (t->next) {
        print_pair(t);
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

    while (t->next) {
        if (strcmp(t->key, key) == 0) {
            printf("Cannot add key \"%s\", already present\n", key);
            return 1;
        }
        t = t->next;
    }

    t->next = create_con();

    strcpy(t->key, key);
    t->value.type = type;
    switch (type) {
        case INT:
            t->value._int = *(int*)data;
            break;
        case DOUBLE:
            t->value._double = *(double*)data;
            break;
        case STRING:
            strcpy(t->value.string, (char*)data);
            break;
        case ARRAY:
            break;
        case OBJECT:
            t->value._obj = (Object*)data;
            break;

        default:
            break;
    }

    return 0;
}

int main() {
    Object *o, *o1;
    o = create_con();

    int x = 42;
    add_value(o, "a", INT, &x);
    char str[] = "test";
    add_value(o, "b", STRING, &str);
    print_con(o);

    int y = 100;
    o1 = create_con();
    add_value(o1, "d", INT, &y);

    add_value(o, "c", OBJECT, o1);
    print_con(o);

    return 0;
}