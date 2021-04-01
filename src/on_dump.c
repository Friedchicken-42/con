#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "on.h"

uint dump_value(Value v, char *buffer, uint size);
uint dump_objects(Object *o, char *buffer, uint size);

uint string_append(char *buffer, uint size, const char *string) {
    if (buffer != NULL) {
        strcat(buffer, string);
    }
    return size + strlen(string);
}

uint dump_string(char str[100], char *buffer, uint size) {
    uint n;
    n = string_append(buffer, size, "\"");
    n += string_append(buffer, size, str);
    n += string_append(buffer, size, "\"");
    return n;
}

uint dump_int(int _int, char *buffer, uint size) {
    uint l, n;
    char *str;

    l = snprintf(NULL, 0, "%d", _int);
    str = malloc(l + 1);
    snprintf(str, l, "%d", _int);
    str[l] = '\0';

    n = string_append(buffer, size, str);
    free(str);
    return n;
}

uint dump_double(double _double, char *buffer, uint size) {
    uint l, n;
    char *str;

    l = snprintf(NULL, 0, "%.2f", _double);
    str = malloc(l + 1);
    snprintf(str, l, "%.2f", _double);
    str[l] = '\0';

    n = string_append(buffer, size, str);
    free(str);
    return n;
}

uint dump_boolean(char _boolean, char *buffer, uint size) {
    if (_boolean == 0) return string_append(buffer, size, "false");
    return string_append(buffer, size, "true");
}

uint dump_array(Object *o, char *buffer, uint size) {
    uint n;
    Object *t;
    t = o;

    n = string_append(buffer, size, "[");

    while (t->next != NULL) {
        n += dump_value(t->pair.value, buffer, size);
        t = t->next;
        if (t->next != NULL) n += string_append(buffer, size, ", ");
    }

    n += string_append(buffer, size, "]");
    return n;
}

uint dump_value(Value v, char *buffer, uint size) {
    uint n;
    n = 0;

    switch (v.type) {
        case INT:
            n += dump_int(v._int, buffer, size);
            break;
        case DOUBLE:
            n += dump_double(v._double, buffer, size);
            break;
        case BOOLEAN:
            n += dump_boolean(v._boolean, buffer, size);
            break;
        case STRING:
            n += dump_string(v._string, buffer, size);
            break;
        case ARRAY:
            n += dump_array(v._obj, buffer, size);
            break;
        case OBJECT:
            n += dump_objects(v._obj, buffer, size);
            break;
        default:
            n += string_append(buffer, size, "WIP");
            break;
    }

    return n;
}

uint dump_object(Object *o, char *buffer, uint size) {
    uint s;
    s = dump_string(o->pair.key, buffer, size);
    s += string_append(buffer, size, ": ");
    s += dump_value(o->pair.value, buffer, size);
    return s;
}

uint dump_objects(Object *o, char *buffer, uint size) {
    uint n;
    Object *t;
    t = o;

    n = string_append(buffer, size, "{");

    while (t->next != NULL) {
        n += dump_object(t, buffer, size);
        t = t->next;
        if (t->next != NULL) n += string_append(buffer, size, ", ");
    }

    n += string_append(buffer, size, "}");

    return n;
}

char *dumps_con(Object *o) {
    char *string;
    uint size;

    size = dump_objects(o, NULL, 0);

    string = malloc(size);
    string[0] = '\0';

    dump_objects(o, string, size);

    return string;
}

void dump_con(FILE *f, Object *o) {
    ;
    fprintf(f, dumps_con(o));
}