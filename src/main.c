#include <stdio.h>

#include "on.h"

int main() {
    Object *o, *o1;
    // o = create_con();
    o = NULL;
    o1 = NULL;

    int x = 42;
    o = add_value(o, "a", INT, &x);

    int y = 100;
    o1 = add_value(o1, "d", INT, &y);

    // add_value(o, "b", OBJECT, o1);

    char str[] = "test";
    o = add_value(o, "c", STRING, &str);

    o = add_value(o, "e", ARRAY, (void *)0);
    int z = 1;
    add_item_array(get(o, "e"), OBJECT, o1);
    add_item_array(get(o, "e"), INT, &z);

    // char flag = 1;
    // add_value(o, "f", BOOLEAN, &flag);

    o1 = get(o, "a");

    char *str_dump;
    str_dump = dumps_con(o);
    printf("%s\n", str_dump);
    // // // Object* o2;
    // // Value v;
    // // v = get_value(o, "a");
    // // print_value(v);

    // FILE *f;
    // f = fopen("test.json", "w");
    // dump_con(f, o);
    // fclose(f);

    return 0;
}