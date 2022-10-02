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
void on_print(on *o, int tab);
