#include <stdio.h>

#include "on.h"

int main() {
    Object *o, *o1;
    o = create_con();

    int x = 42;
    add_value(o, "a", INT, &x);

    int y = 100;
    o1 = create_con();
    add_value(o1, "d", INT, &y);

    // add_value(o, "b", OBJECT, o1);

    char str[] = "test";
    add_value(o, "c", STRING, &str);

    add_value(o, "e", ARRAY, (void *)0);
    int z = 1;
    add_item_array(get(o, "e"), INT, &z);
    add_item_array(get(o, "e"), OBJECT, o1);

    char *str_dump;
    str_dump = dump_con(o);
    printf("%s\n", str_dump);
    // // Object* o2;
    // Value v;
    // v = get_value(o, "a");
    // print_value(v);
    return 0;
}