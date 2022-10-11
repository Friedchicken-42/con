#include "../utils/string/string.h"
#include "../on.h"
#include "json.h"
#include <stdlib.h>

on* json_loads_object(string *str) {
    on *o = on_create_object();
    string_next(str);

    while(string_peek(str) != '}') {
        if(string_peek(str) == 0) {
            on_free(o);
            return NULL;
        }
        string_next(str);
    }

    return o;
}

on* json_loads_array(string *str) {
    on *o = on_create_array();

    return o;
}

on* json_loads_inner(string *str) {
    if(string_peek(str) == '{') return json_loads_object(str);
    if(string_peek(str) == '[') return json_loads_array(str);

    return NULL;
}

on* json_loads(char* s) {
    string* str = string_from(s);
    on* o = json_loads_inner(str);
    string_free(str);

    return o;
}
