#include "../utils/string/string.h"
#include "../on.h"
#include "json.h"
#include <stdio.h>
#include <stdlib.h>

enum json_err_type {
    OK = 0,
    WRONG_CHARACTER,
    UNEXPECTED_END,
    EMPTY_KEY,
};

const char* json_err_string(enum json_err_type err) {
    const char* table[] = {
        [OK] = "Ok",
        [WRONG_CHARACTER] = "Wrong Character",
        [UNEXPECTED_END] = "Unexpected End",
        [EMPTY_KEY] = "Empty Key",
    };

    return table[err];
}

typedef struct json_result_t {
    struct ok_t {
        void *value;
        enum on_type type;
    } ok;
    struct err_t {
        uint index;
        enum json_err_type type;
    } err;
} json_result;

json_result json_result_create() {
    json_result result = {
        .ok = {
            .type = ON_EMPTY,
            .value = NULL,
        },
        .err = {
            .index = 0,
            .type= OK,
        },
    };
    return result;
}

void json_skip_spaces(string *str) {
    while(string_peek(str) == ' ') string_next(str);
}

json_result json_read_string(string *str) {
    json_result result = json_result_create();

    if(string_peek(str) != '\"') {
        result.err.type = WRONG_CHARACTER;
        result.err.index = str->index;
        return result;
    }
    string_next(str);

    string *key_str = string_create();
    while(string_peek(str) != '\"') {
        char x = string_peek(str);
        string_next(str);
        if(x == 0) {
            string_free(key_str);
            result.err.type = UNEXPECTED_END;
            result.err.index = str->index;
            return result;
        }

        string_push(key_str, x);
    }

    string_next(str);

    char *key = string_str(key_str);
    string_free(key_str);

    result.ok.value = key;
    result.ok.type = ON_STRING;
    return result;
}

json_result json_read_key(string *str) {
    json_result result = json_read_string(str);
    if(result.err.type != OK) return result;

    if(string_cmp(result.ok.value, "") == 0) {
        result.err.type = EMPTY_KEY;
        result.err.index = str->index;
        free(result.ok.value);
        result.ok.value = NULL;
    }

    return result;
}

json_result json_read_sequence(string *str, const char* sequence) {
    json_result result = json_result_create();

    for(uint i = 0; sequence[i] != '\0'; i++) {
        if(string_peek(str) != sequence[i]) {
            result.err.type = WRONG_CHARACTER;
            result.err.index = str->index;
        }
        string_next(str);
    }

    return result;
}

json_result json_read_true(string *str) {
    json_result result = json_read_sequence(str, "true");

    if(result.err.type != OK) return result;

    result.ok.type = ON_TRUE;
    result.ok.value = NULL;

    return result;
}

json_result json_read_false(string *str) {
    json_result result = json_read_sequence(str, "false");

    if(result.err.type != OK) return result;

    result.ok.type = ON_FALSE;
    result.ok.value = NULL;

    return result;
}

json_result json_read_null(string *str) {
    json_result result = json_read_sequence(str, "null");

    if(result.err.type != OK) return result;

    result.ok.type = ON_NULL;
    result.ok.value = NULL;

    return result;
}

json_result json_read_value(string *str) {
    json_result result = json_result_create();

    switch(string_peek(str)) {
        case '"':
            return json_read_string(str);
        case 't':
            return json_read_true(str);
        case 'f':
            return json_read_false(str);
        case 'n':
            return json_read_null(str);
    }

    result.err.type = UNEXPECTED_END;
    result.err.index = str->index;

    return result;
}

json_result json_loads_object(string *str) {
    json_result result = json_result_create();

    on *o = on_create_object();
    string_next(str);

    while(1) {
        if(string_peek(str) == 0) {
            on_free(o);
            result.err.type = WRONG_CHARACTER;
            result.err.index = str->index;
            return result;
        }

        json_result key_res = json_read_key(str);
        if(key_res.err.type != OK) {
            on_free(o);
            return key_res;
        }

        char *key = key_res.ok.value;
        printf("key: %s\n", key);

        json_skip_spaces(str);
        if(string_peek(str) != ':') {
            on_free(o);
            result.err.type = WRONG_CHARACTER;
            result.err.index = str->index;
            return result;
        }
        string_next(str);
        json_skip_spaces(str);

        json_result value_res = json_read_value(str);
        if(value_res.err.type != OK) {
            free(key);
            on_free(o);
            return value_res;
        }

        on_add(o, key, value_res.ok.value, value_res.ok.type);

        free(key);
        if (value_res.ok.value != NULL) free(value_res.ok.value);

        json_skip_spaces(str);
        if(string_peek(str) != ',') break;
        string_next(str);
        json_skip_spaces(str);
    }

    if(string_peek(str) != '}') {
        on_free(o);
        result.err.type = WRONG_CHARACTER;
        result.err.index = str->index;
        return result;
    }

    result.ok.value = o;
    result.ok.type = ON_OBJECT;
    return result;
}

json_result json_loads_array(string *str) {
    json_result result = json_result_create();
    on *o = on_create_array();

    result.ok.value = o;
    result.ok.type = ON_ARRAY;
    return result;
}

json_result json_loads_inner(string *str) {
    if(string_peek(str) == '{') return json_loads_object(str);
    if(string_peek(str) == '[') return json_loads_array(str);

    json_result result = json_result_create();
    result.err.type = WRONG_CHARACTER;
    result.err.index = str->index;
    return result;
}

on* json_loads(char* s) {
    on *o = NULL;
    string* str = string_from(s);
    json_result result = json_loads_inner(str);

    if(result.err.type == OK) o = result.ok.value;
    else printf("err: %s, %c\n", json_err_string(result.err.type), s[result.err.index]);
    string_free(str);

    return o;
}
