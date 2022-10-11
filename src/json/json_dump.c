#include "../on.h"
#include "../utils/string/string.h"
#include "../utils/hashmap/hashmap.h"
#include "json.h"
#include <stdio.h>

void json_dumps_inner(on *o, string *str);

void json_dumps_object(on *o, string *str) {
    string_extend(str, "{");

    hashmap *map = (hashmap*)o->data;
    list *keys = map->keys;
    for(node *k = keys->head; k != NULL; k = k->next) {
        string_extend(str, "\"");
        string_extend(str, (char*)k->data);
        string_extend(str, "\"");
        string_extend(str, ": ");
        json_dumps_inner((on*)(hashmap_get(map, k->data)), str);

        if(k->next != NULL) string_extend(str, ", ");
    }

    string_extend(str, "}");
}

void json_dumps_array(on *o, string *str) {
    string_extend(str, "[");

    list *l = (list*)o->data;
    int i = 0;
    for(node *n = l->head; n != NULL; n = n->next) {
        json_dumps_inner((on*)n->data, str);

        if(n->next != NULL) string_extend(str, ", ");
        i++;
    }

    string_extend(str, "]");
}


void json_dumps_integer(string *str, int n) {
    int len = snprintf(NULL, 0, "%d", n);
    char *s = malloc(len + 1);
    snprintf(s, len + 1, "%d", n);
    string_extend(str, s);
    free(s);
}

void json_dumps_float(string *str, float n) {
    int len = snprintf(NULL, 0, "%f", n);
    char *s = malloc(len + 1);
    snprintf(s, len + 1, "%f", n);
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
            string_extend(str, (char*)o->data);
            string_extend(str, "\"");
            break;
        case ON_INTEGER:
            json_dumps_integer(str, *(int*)o->data);
            break;
        case ON_FLOAT:
            json_dumps_float(str, *(float*)o->data);
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

int json_dump(on *o, const char* filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) return -1;

    char *string = json_dumps(o);
    if (string == NULL) return -1;

    fputs(string, f);

    return 0;
}