#include "string.h"
#include "../alloc/alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;

void *string_dup(void *str) {
    char *x = (char*)str;
    uint len = strlen(x);
    char *dest = xmalloc(len + 1);
    strcpy(dest, x);

    return dest;
}

int string_cmp(char* s1, char* s2) {
    return strcmp(s1, s2);
}

string *string_create() {
    string *str = xmalloc(sizeof(string));
    str->data = xmalloc(64);
    str->length = 0;
    str->size = 64;
    str->index = 0;

    return str;
}

string *string_from(char *s) {
    int len = strlen(s);
    string *str = xmalloc(sizeof(string));
    str->data = xmalloc(len + 1);
    str->length = len;
    str->size = len;
    str->index = 0;

    strcpy(str->data, s);

    return str;
}

void string_push(string *str, const char c) {
    if(str->length == str->size) {
        str->size *= 2;
        str->data = xrealloc(str->data, str->size);
    }

    str->data[str->length] = c;
    str->length++;
}

void string_extend(string *str, const char* s2) {
    for(uint i = 0; i < strlen(s2); i++) {
        string_push(str, s2[i]);
    }
}

char *string_str(string *str) {
    if(str->length == 0) string_push(str, '\0');
    else if(str->data[str->length - 1] != '\0') string_push(str, '\0');
    return string_dup(str->data);
}

char string_peek(string *str) {
    if(str->index >= str->length) return 0;
    if(str->data[str->index] == '\0') return 0;

    return str->data[str->index];
}

char string_next(string *str) {
    char chr = string_peek(str);
    if(chr == 0) return 0;

    str->index++;

    return chr;
}

void string_free(void *str) {
    string *x = (string*)str;
    free(x->data);
    free(x);
}
