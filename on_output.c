#include <stdio.h>

#include "on.h"

void _print_con(Object* o);
void print_array(Object* o);
void print_key(char key[100]) { printf("\"%s\": ", key); }

void print_value(Value v) {
    switch (v.type) {
        case INT:
            printf("%d", v._int);
            break;
        case DOUBLE:
            printf("%.2f", v._double);
            break;
        case STRING:
            printf("\"%s\"", v._string);
            break;
        case ARRAY:
            print_array(v._obj);
            break;
        case OBJECT:
            _print_con(v._obj);
            break;
    }
}

void print_array(Object* o) {
    Object* t = o;

    printf("[");
    while (t->next) {
        print_value(t->pair.value);
        t = t->next;
        if (t->next) printf(", ");
    }
    printf("]");
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
