#include "utils/list/list.h"
#include "utils/hashmap/hashmap.h"
#include "on.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    on *obj = on_create();
    obj->type = ON_OBJECT;
    obj->func_free = hashmap_free;
    hashmap *map = hashmap_create_str(3);
    map->func_free_value = on_free;

    on *a = on_create();
    hashmap_set(map, "a", a);
    a->data = "KEK";
    a->type = ON_STRING;
    a->func_free = NULL;

    hashmap_set(map, "b", on_create());
    on *ooo = hashmap_get(map, "b");
    ooo->type = ON_OBJECT;
    ooo->func_free = hashmap_free;
    ooo->data = hashmap_create_str(5);

    on* asdf = on_create();
    hashmap_set(map, "c", asdf);
    asdf->type = ON_ARRAY;
    asdf->func_free = list_free;

    list* l = list_create();
    asdf->data = l;
    l->func_free = on_free;

    on* asdf1 = on_create();
    asdf1->type = ON_STRING;
    asdf1->func_free = NULL;
    asdf1->data = "item 1";
    list_push(l, asdf1);

    on* asdf2 = on_create();
    asdf2->type = ON_STRING;
    asdf2->func_free = NULL;
    asdf2->data = "item 2";
    list_push(l, asdf2);

    hashmap_print(((on*)(hashmap_get(map, "b")))->data);
    list_print(((on*)(hashmap_get(map, "c")))->data);

    obj->data = map;

    hashmap_print(obj->data);

    on_print(obj, 0);
    on_free(obj);
    return 0;
}
