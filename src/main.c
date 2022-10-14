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
    /*
    int x = 42;
    double y = 12.3;

    on *obj = on_create_object();
    on_add(obj, "a", "KEK", ON_STRING);
    on_add(obj, "b", &x, ON_INTEGER);
    on_add(obj, "c", &y, ON_DOUBLE);
    on_add(obj, "d", NULL, ON_OBJECT);
    on_add(obj, "e", NULL, ON_ARRAY);

    on *test = on_create_object();
    on_add(obj, "f", test, ON_OBJECT);
    on_add(test, "qwer", "QWER", ON_STRING);

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
    */
    char *input = "{\"asdf\": \"zxcv\", \"a\": true, \"b\": false, \"c\": null, \"d\": {\"aa\": \"bb\"}, \"e\": [true, false], \"f\": -123456, \"g\": 12.34, \"h\": 12.3e-5}";
    printf("input: %s\n", input);
    on *o = json_loads(input);
    if(o == NULL) return 1;
    printf("---------\n");
    on_print(o);
    printf("---------\n");
    char *str = json_dumps(o);
    printf("output: %s\n", str);
    free(str);
    on_free(o);

    return 0;
}
