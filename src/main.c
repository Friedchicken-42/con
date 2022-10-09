#include "utils/list/list.h"
#include "utils/hashmap/hashmap.h"
#include "on.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    int x = 42;
    float y = 12.3;

    on *obj = on_create_object();
    on_add(obj, "a", "KEK", ON_STRING);
    on_add(obj, "b", &x, ON_INTEGER);
    on_add(obj, "c", &y, ON_FLOAT);
    on_add(obj, "d", NULL, ON_OBJECT);
    on_add(obj, "e", NULL, ON_ARRAY);

    on_print(obj);

    printf("-----------------\n");

    on *ooo = on_get(obj, "d");
    on_add(ooo, "asdf", "ASDF", ON_STRING);
    on_print(ooo);

    printf("-----------------\n");

    on_print(obj);

    on_free(obj);

    return 0;
}
