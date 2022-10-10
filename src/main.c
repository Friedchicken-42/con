#include "utils/alloc/alloc.h"
#include "utils/list/list.h"
#include "utils/hashmap/hashmap.h"
#include "on.h"
#include "json/json.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/string/string.h"

int main (int argc, char *argv[]) {
    int x = 42;
    float y = 12.3;

    on *obj = on_create_object();
    on_add(obj, "a", "KEK", ON_STRING);
    on_add(obj, "b", &x, ON_INTEGER);
    on_add(obj, "c", &y, ON_FLOAT);
    on_add(obj, "d", NULL, ON_OBJECT);
    on_add(obj, "e", NULL, ON_ARRAY);

    on *test = on_create_object();
    on_add(test, "qwer", "QWER", ON_STRING);
    on_add(obj, "f", test, ON_OBJECT);

    on *ooo = on_get(obj, "d");
    on_add(ooo, "asdf", "ASDF", ON_STRING);

    on *arr = on_get(obj, "e");
    on_add(arr, NULL, "test1", ON_STRING);
    on_add(arr, NULL, "test2", ON_STRING);
    on_add(arr, NULL, "test3", ON_STRING);

    char *s = json_dumps(obj);
    printf("%s\n", s);
    free(s);

    on_free(obj);

    return 0;
}
