#include "string.h"
#include "../alloc/alloc.h"
#include <stdio.h>
#include <string.h>

typedef unsigned int uint;

void *string_dup(void *str) {
    char *x = (char*)str;
    uint len = strlen(x);
    // for (len = 0; x[len] != '\0'; len++);
    char *dest = xmalloc(len + 1);
    strcpy(dest, x);

    return dest;
}

int string_cmp(char* s1, char* s2) {
    return strcmp(s1, s2);
}

char *string_append(char *s1, const char *s2) {
    int l2 = strlen(s2);

    if (s1 == NULL) {
        s1 = xmalloc(1);
        s1[0] = '\0';
    }

    int l1 = strlen(s1);
    s1 = xrealloc(s1, l1 + l2 + 1);

    strcat(s1, s2);

    return s1;
}
