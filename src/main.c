#include "utils/alloc/alloc.h"
#include "utils/list/list.h"
#include "utils/hashmap/hashmap.h"
#include "utils/string/string.h"
#include "on.h"
#include "json/json.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
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
}
