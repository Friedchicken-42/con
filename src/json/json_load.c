#include "../utils/string/string.h"
#include "../on.h"
#include "json.h"
#include <stdio.h>
#include <stdlib.h>

enum json_err {
    OK = 0,
    WRONG_CHARACTER,
    UNEXPECTED_END,
    EMPTY_KEY,
};

const char* json_err_string(enum json_err err) {
    const char* table[] = {
        [OK] = "Ok",
        [WRONG_CHARACTER] = "Wrong Character",
        [UNEXPECTED_END] = "Unexpected End",
        [EMPTY_KEY] = "Empty Key",
    };

    return table[err];
}

typedef struct json_result_t {
    void *data;
    uint index;
    enum json_err err;
} json_result;

void json_skip_spaces(string *str) {
    while(string_peek(str) == ' ') string_next(str);
}

json_result json_read_string(string *str) {
    json_result result = {NULL, 0, OK};

    if(string_peek(str) != '\"') {
        result.err = WRONG_CHARACTER;
        result.index = str->index;
        return result;
    }
    string_next(str);

    string *key_str = string_create();
    while(string_peek(str) != '\"') {
        char x = string_next(str);
        if(x == 0) {
            string_free(key_str);
            result.err = UNEXPECTED_END;
            result.index = str->index;
            return result;
        }

        string_push(key_str, x);
    }

    string_next(str);

    char *key = string_str(key_str);
    string_free(key_str);

    result.data = key;
    return result;
}

json_result json_read_key(string *str) {
    json_result result = json_read_string(str);
    if(result.err != OK) return result;

    if(string_cmp(result.data, "") == 0) {
        result.err = EMPTY_KEY;
        result.index = str->index;
        free(result.data);
        result.data = NULL;
    }

    return result;
}

json_result json_read_value(string *str) {
    //temp
    return json_read_string(str);
}

json_result json_loads_object(string *str) {
    json_result result = {NULL, 0, OK};

    on *o = on_create_object();
    string_next(str);

    while(1) {
        if(string_peek(str) == 0) {
            on_free(o);
            result.err = WRONG_CHARACTER;
            result.index = str->index;
            return result;
        }

        json_result key_res = json_read_key(str);
        if(key_res.err != OK) {
            on_free(o);
            return key_res;
        }

        char *key = key_res.data;
        printf("key: %s\n", key);

        json_skip_spaces(str);
        if(string_peek(str) != ':') {
            on_free(o);
            result.err = WRONG_CHARACTER;
            result.index = str->index;
            return result;
        }
        string_next(str);
        json_skip_spaces(str);

        json_result value_res = json_read_value(str);
        if(value_res.err != OK) {
            free(key);
            on_free(o);
            return value_res;
        }

        char *value = value_res.data;
        printf("value: %s\n", (char*)value);
        on_add(o, key, value, ON_STRING);

        free(key);
        free(value);

        json_skip_spaces(str);
        if(string_peek(str) != ',') break;
        string_next(str);
        json_skip_spaces(str);
    }

    if(string_peek(str) != '}') {
        on_free(o);
        result.err = WRONG_CHARACTER;
        result.index = str->index;
        return result;
    }

    result.data = o;
    return result;
}

json_result json_loads_array(string *str) {
    json_result result = {NULL, 0, OK};
    on *o = on_create_array();

    result.data = o;
    return result;
}

json_result json_loads_inner(string *str) {
    if(string_peek(str) == '{') return json_loads_object(str);
    if(string_peek(str) == '[') return json_loads_array(str);

    json_result result = {NULL, str->index, WRONG_CHARACTER};
    return result;
}

on* json_loads(char* s) {
    on *o = NULL;
    string* str = string_from(s);
    json_result result = json_loads_inner(str);

    if(result.err == OK) o = result.data;
    else printf("err: %s, %c\n", json_err_string(result.err), s[result.index]);
    string_free(str);

    return o;
}
