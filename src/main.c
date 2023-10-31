#include "json.h"
#include <alloc.h>
#include <assert.h>
#include <hashmap.h>
#include <list.h>
#include <on.h>
#include <string.h>
// #include <json.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // char *input = "{\"asdf\": \"zxcv\", \"a\": true, \"b\": false, \"c\":
    // null, \"d\": {\"aa\": \"bb\"}, \"e\": [true, false], \"f\": -123456,
    // \"g\": 12.34, \"h\": 12.3e-5}"; printf("input: %s\n", input); on *o =
    // json_loads(input); if(o == NULL) return 1; printf("---------\n");
    // on_print(o);
    // printf("---------\n");
    // char *str = json_dumps(o);
    // printf("output: %s\n", str);
    // free(str);
    // on_free(o);

    /*
    on *o = on_create_object();
    on_add(o, "a", "qwer", ON_STRING);
    on_add(o, "b", "asdf", ON_STRING);

    on *c = on_create_object();
    on_add(c, "a", "a", ON_STRING);
    on_add(c, "b", "b", ON_STRING);
    on_add(o, "c", c, ON_OBJECT);

    on *d = on_create_array();
    on_add(d, NULL, "a", ON_STRING);
    on_add(d, NULL, "b", ON_STRING);
    on_add(o, "d", d, ON_ARRAY);

    on_print(o);
    on_free(o);
    */

    on *o = NULL;
    json_err *err =
        json_loads("{\"a\": \"qwer\", \"b\": \"asdf\", \"c\": {\"a\": "
                   "\"b\"},\"d\":[1, 4.2, true, false, null]}",
                   &o);
    assert(err == NULL);
    on_print(o);
    on_free(o);

    err = json_loads("what", &o);
    if (err != NULL) free(err);
}
