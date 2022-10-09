#include "../on.h"
#include <stdio.h>

char *json_dumps(on *o) {
    char *string = NULL;

    string_append(string, "{");

    return string;
}

int json_dump(on *o, const char* filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) return -1;

    char *string = json_dumps(o);
    if (string == NULL) return -1;

    fputs(string, f);

    return 0;
}
