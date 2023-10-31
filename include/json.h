#ifndef JSON_H
#define JSON_H

#include <on.h>

// will remove this
typedef struct json_err_t {
    char errno;
    int index;
    const char *msg;
} json_err;

char *json_dumps(on *o);
int json_dump(on *o, const char *filename);

json_err *json_loads(char *s, on **output);
json_err *json_load(const char *filename, on **output);

#endif // !JSON_H
