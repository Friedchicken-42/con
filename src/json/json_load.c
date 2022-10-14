#include "../utils/string/string.h"
#include "../utils/alloc/alloc.h"
#include "../on.h"
#include "json.h"
#include <stdio.h>
#include <stdlib.h>

enum json_err_type {
    OK = 0,
    WRONG_CHARACTER,
    UNEXPECTED_END,
    EMPTY_KEY,
    MISSING_DIGIT,
};

const char* json_err_string(enum json_err_type err) {
    const char* table[] = {
        [OK] = "Ok",
        [WRONG_CHARACTER] = "Wrong Character",
        [UNEXPECTED_END] = "Unexpected End",
        [EMPTY_KEY] = "Empty Key",
        [MISSING_DIGIT] = "Missing Digit",
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
    while(1) {
        char c = string_peek(str);
        if(c != ' ' && c != '\t' && c != '\n' && c != '\r') break;
        string_next(str);
    }
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

int json_is_digit(char c) {
    return c >= '0' && c <= '9';
}

json_result json_read_integer(string *str) {
    json_result result = json_result_create();
    int sign = 0;

    if(string_peek(str) == '-') {
        sign = 1;
        string_next(str);
    } else if(string_peek(str) == '+') {
        string_next(str);
    }

    if(!json_is_digit(string_peek(str))) {
        result.err.type = MISSING_DIGIT;
        result.err.index = str->index;
        return result;
    }

    int* number = xmalloc(sizeof(int));
    *number = 0;
    result.ok.value = number;

    while(json_is_digit(string_peek(str))) {
        *number = *number * 10 + string_peek(str) - '0';
        printf("number: %d\n", *number);
        string_next(str);
    }
    *number = (sign ? -1 : 1) * *number;

    return result;
}

json_result json_read_fraction(string *str) {
    json_result result = json_result_create();

    int *fraction = xmalloc(sizeof(int));
    *fraction = 0;
    result.ok.value = fraction;

    if(string_peek(str) != '.') return result;
    string_next(str);

    while(json_is_digit(string_peek(str))) {
        *fraction = *fraction * 10 + string_peek(str) - '0';
        printf("fraction: %d\n", *fraction);
        string_next(str);
    }

    return result;
}

json_result json_read_exponent(string *str) {
    json_result result = json_result_create();
    int sign = 0;

    int *exponent = xmalloc(sizeof(int));
    *exponent = 0;
    result.ok.value = exponent;

    if(string_peek(str) != 'e' && string_peek(str) != 'E') return result;
    string_next(str);

    if(string_peek(str) == '-') sign = 1;
    else if(string_peek(str) == '+') sign = 0;
    else if(!json_is_digit(string_peek(str))) {
        result.err.type = WRONG_CHARACTER;
        result.err.index = str->index;
        return result;
    }

    string_next(str);

    while(json_is_digit(string_peek(str))) {
        *exponent = *exponent * 10 + string_peek(str) - '0';
        printf("exponent: %d\n", *exponent);
        string_next(str);
    }

    *exponent = (sign ? -1 : 1) * *exponent;

    return result;
}

json_result json_read_number(string *str) {
    json_result result = json_result_create();

    json_result int_res = json_read_integer(str);
    json_result fra_res = json_read_fraction(str);
    json_result exp_res = json_read_exponent(str);

    if(int_res.err.type != OK) return int_res;
    if(fra_res.err.type != OK) return fra_res;
    if(exp_res.err.type != OK) return exp_res;

    int num = *(int*)(int_res.ok.value);
    int fra = *(int*)(fra_res.ok.value);
    int exp = *(int*)(exp_res.ok.value);

    printf("%d %d %d\n", num, fra, exp);

    double fraction = fra;
    while(fraction > 1) {
        fraction /= 10;
    }
    printf("%f\n", fraction);

    if(fra == 0 && exp == 0) {
        int *number = xmalloc(sizeof(int));
        *number = num;
        result.ok.value = number;
        result.ok.type = ON_INTEGER;
    } else {
        double *number = xmalloc(sizeof(double));
        *number = num + fraction;
        int exp_abs = exp >= 0 ? exp : -exp;
        for(uint i = 0; i < exp_abs; i++) {
            if(exp > 0) *number = *number * 10;
            else *number = *number / 10;
        }

        printf("%f\n", *number);

        result.ok.value = number;
        result.ok.type = ON_DOUBLE;
    }

    free(int_res.ok.value);
    free(fra_res.ok.value);
    free(exp_res.ok.value);

    return result;
}

json_result json_read_inner(string *str);

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
        case '{':
        case '[':
            return json_read_inner(str);
        default:
            return json_read_number(str);
    }

    result.err.type = UNEXPECTED_END;
    result.err.index = str->index;

    return result;
}

json_result json_read_object(string *str) {
    json_result result = json_result_create();
    if(string_peek(str) != '{') {
        result.err.type = WRONG_CHARACTER;
        result.err.index = str->index;
        return result;
    }

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

        // printf("err type: %s\n", json_err_string(value_res.err.type));
        printf("obj type: %s\n", on_type_string(value_res.ok.type));

        on_add(o, key, value_res.ok.value, value_res.ok.type);

        free(key);
        if (value_res.ok.value != NULL) {
            if(value_res.ok.type != ON_ARRAY && value_res.ok.type != ON_OBJECT) {
                free(value_res.ok.value);
            }
        }

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

    string_next(str);

    result.ok.value = o;
    result.ok.type = ON_OBJECT;
    return result;
}

json_result json_read_array(string *str) {
    json_result result = json_result_create();

    if(string_peek(str) != '[') {
        result.err.type = WRONG_CHARACTER;
        result.err.index = str->index;
        return result;
    }

    string_next(str);
    on *o = on_create_array();

    while(1) {
        if(string_peek(str) == 0) {
            on_free(o);
            result.err.type = UNEXPECTED_END;
            result.err.index = str->index;
            return result;
        }

        json_skip_spaces(str);

        json_result value_res = json_read_value(str);
        if(value_res.err.type != OK) {
            on_free(o);
            return value_res;
        }

        on_add(o, NULL, value_res.ok.value, value_res.ok.type);

        if(value_res.ok.value != NULL) {
            if(value_res.ok.type != ON_OBJECT && value_res.ok.type != ON_ARRAY) {
                free(value_res.ok.value);
            }
        }

        json_skip_spaces(str);
        if(string_peek(str) != ',') break;
        string_next(str);
        json_skip_spaces(str);

    }

    if(string_peek(str) != ']') {
        on_free(o);
        result.err.type = WRONG_CHARACTER;
        result.err.index = str->index;
        return result;
    }


    string_next(str);

    result.ok.value = o;
    result.ok.type = ON_ARRAY;
    return result;
}

json_result json_read_inner(string *str) {
    if(string_peek(str) == '{') return json_read_object(str);
    if(string_peek(str) == '[') return json_read_array(str);

    json_result result = json_result_create();
    result.err.type = WRONG_CHARACTER;
    result.err.index = str->index;
    return result;
}

on* json_loads(char* s) {
    on *o = NULL;
    string* str = string_from(s);
    json_result result = json_read_inner(str);

    if(result.err.type == OK) o = result.ok.value;
    else printf("err: %s, %c\n", json_err_string(result.err.type), s[result.err.index]);
    string_free(str);

    return o;
}
