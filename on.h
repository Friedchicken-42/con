#if !defined(CON)
#define CON

typedef unsigned int uint;

typedef struct Value Value;
typedef struct KeyValue KeyValue;
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
        char _string[100];
        Object* _obj;
    };
    enum V_type type;
};

struct KeyValue {
    char key[100];
    Value value;
};

struct Object {
    KeyValue pair;
    Object* next;
};

Object* create_con();
void add_value(Object* o, char key[100], enum V_type type, void* data);
void add_item_array(Object* o, enum V_type type, void* data);
Object* get(Object* o, char key[100]);
Value get_value(Object* o, char key[100]);

void print_con(Object* o);

char* dump_con(Object* o);

#endif
