#ifndef ON_H
#define ON_H

enum on_type {
    ON_EMPTY,
    ON_NULL,
    ON_STRING,
    ON_INTEGER,
    ON_FLOAT,
    ON_TRUE,
    ON_FALSE,
    ON_OBJECT,
    ON_ARRAY,
};

typedef void (*on_func_free)(void *);

typedef struct on_t {
    void *data;
    enum on_type type;
    on_func_free func_free;
} on;

on *on_create();
void on_free(void* o);
void on_print(on *o);

on *on_create_object();
on *on_create_array();
int on_add(on *o, char *key, void *value, enum on_type type);
void *on_get(on *o, void *key);

const char* on_type_string(on *o);

#endif // !ON_H
