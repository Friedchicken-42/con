#if !defined(CON)
#define CON

typedef unsigned int uint;

typedef struct Value Value;
typedef struct Object Object;

enum V_type {
    INT,
    DOUBLE,
    STRING,
    ARRAY,
    OBJECT,
};

struct Value {
    union {
        int _int;
        double _double;
        char string[100];
        Value *array[32];
        Object *_obj;
    };
    enum V_type type;
};

struct Object {
    char key[100];
    Value value;
    Object *next;
};

#endif
