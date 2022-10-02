#include "utils/hashmap/hashmap.h"
#include "on.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    /*
    hashmap *h = hashmap_create_str(5);

    hashmap_set(h, "a", "aa");
    hashmap_set(h, "b", "bb");
    hashmap_set(h, "c", "cc");
    hashmap_set(h, "d", "dd");
    hashmap_set(h, "e", "ee");
    hashmap_set(h, "f", "ff");
    hashmap_set(h, "k", "kk");
    hashmap_print(h);

    list *l = hashmap_keys(h);
    list_print(l);

    node *n = list_pop(l);
    printf("%s\n", (char*)n->data);
    list_print(l);

    l->func_free(n);
    list_free(l);
    hashmap_print(h);
    hashmap_free(h);
    */

    on *obj = on_create();
    obj->type = ON_OBJECT;
    obj->func_free = hashmap_free;
    hashmap *map = hashmap_create_str(3);
    map->func_free_value = on_free;

    on *a = on_create();
    a->data = "KEK";
    a->type = ON_STRING;
    a->func_free = NULL;
    hashmap_set(map, "a", a);

    hashmap_set(map, "b", on_create());
    on *ooo = hashmap_get(map, "b")->value;
    ooo->type = ON_OBJECT;
    ooo->func_free = hashmap_free;
    ooo->data = hashmap_create_str(5);

    hashmap_print(((on*)(hashmap_get(map, "b")->value))->data);

    obj->data = map;

    hashmap_print(obj->data);

    on_print(obj, 0);
    on_free(obj);

    return 0;
}
