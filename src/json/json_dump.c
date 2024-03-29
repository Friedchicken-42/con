#include "list.h"
#include <alloc.h>
#include <hashmap.h>
#include <json.h>
#include <on.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void json_dumps_inner(on *o, string *str);

void json_dumps_object(on *o, string *str) {
    string_extend(str, "{");

    hashmap *map = (hashmap *)o->data;
    list *keys = map->keys;
    for (uint i = 0; i < keys->length; i++) {
        char *key = list_get(keys, i);
        string_extend(str, "\"");
        string_extend(str, key);
        string_extend(str, "\"");
        string_extend(str, ": ");
        json_dumps_inner((on *)(hashmap_get(map, key)), str);

        if (i != keys->length - 1) string_extend(str, ", ");
    }

    string_extend(str, "}");
}

void json_dumps_array(on *o, string *str) {
    string_extend(str, "[");

    list *l = (list *)o->data;
    for (uint i = 0; i < l->length; i++) {
        json_dumps_inner((on *)list_get(l, i), str);

        if (i != l->length - 1) string_extend(str, ", ");
    }

    string_extend(str, "]");
}

void json_dumps_integer(string *str, int n) {
    int len = snprintf(NULL, 0, "%d", n);
    char *s = xmalloc(len + 1);
    snprintf(s, len + 1, "%d", n);
    string_extend(str, s);
    free(s);
}

void json_dumps_double(string *str, double n) {
    char *format = "";
    int limit = 1000000;

    if (n > limit || n < 1.0f / limit) format = "%e";
    else format = "%lf";

    int len = snprintf(NULL, 0, format, n);
    char *s = xmalloc(len + 1);
    snprintf(s, len + 1, format, n);
    string_extend(str, s);
    free(s);
}

void json_dumps_inner(on *o, string *str) {
    switch (o->type) {
    case ON_EMPTY:
        break;
    case ON_NULL:
        string_extend(str, "null");
        break;
    case ON_STRING:
        string_extend(str, "\"");
        string_extend(str, (char *)o->data);
        string_extend(str, "\"");
        break;
    case ON_INTEGER:
        json_dumps_integer(str, o->number);
        break;
    case ON_DOUBLE:
        json_dumps_double(str, o->number);
        break;
    case ON_TRUE:
        string_extend(str, "true");
        break;
    case ON_FALSE:
        string_extend(str, "false");
        break;
    case ON_OBJECT:
        json_dumps_object(o, str);
        break;
    case ON_ARRAY:
        json_dumps_array(o, str);
        break;
    }
}

char *json_dumps(on *o) {
    string *str = string_create();

    json_dumps_inner(o, str);

    char *x = string_str(str);
    string_free(str);
    return x;
}

int json_dump(on *o, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) return -1;

    char *string = json_dumps(o);
    if (string == NULL) {
        fclose(f);
        return -1;
    }

    fputs(string, f);
    free(string);
    fclose(f);

    return 0;
}
