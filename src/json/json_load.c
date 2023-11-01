#include <alloc.h>
#include <json.h>
#include <on.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum json_err_type {
    OK = 0,
    WRONG_CHARACTER,
    UNEXPECTED_END,
    EMPTY_KEY,
    MISSING_DIGIT,
    INTERNAL_ERROR,
    TRAILING_COMMA,
};

const char *json_err_string(enum json_err_type err) {
    const char *table[] = {
        [OK] = "Ok",
        [WRONG_CHARACTER] = "Wrong Character",
        [UNEXPECTED_END] = "Unexpected End",
        [EMPTY_KEY] = "Empty Key",
        [MISSING_DIGIT] = "Missing Digit",
        [INTERNAL_ERROR] = "Internal Error",
        [TRAILING_COMMA] = "Trailing Comma",
    };

    return table[err];
}

json_err *json_err_new(char errno, int index, const char *msg) {
    json_err *err = xmalloc(sizeof(json_err));

    err->errno = errno;
    err->index = index;
    if (msg == NULL) err->msg = json_err_string(errno);
    else err->msg = msg;

    return err;
}

void json_skip_spaces(string *str) {
    while (1) {
        char c = string_peek(str);
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') break;
        string_next(str);
    }
}

json_err *json_read_simple_string(string *str, char **output) {
    if (string_peek(str) != '\"') {
        return json_err_new(WRONG_CHARACTER, str->index, "Expected \"");
    }

    string_next(str);

    string *key_str = string_create();
    while (string_peek(str) != '\"') {
        char x = string_peek(str);
        string_next(str);
        if (x == 0) {
            string_free(key_str);
            return json_err_new(UNEXPECTED_END, str->index, NULL);
        } else if (x == '\\') {
            string_push(key_str, x);
            x = string_peek(str);
            string_next(str);
        }

        string_push(key_str, x);
    }

    string_next(str);

    char *key = string_str(key_str);
    string_free(key_str);

    *output = key;
    return NULL;
}

json_err *json_read_string(string *str, on *output) {
    on_set(output, ON_STRING);
    return json_read_simple_string(str, (char **)&(output->data));
}

json_err *json_read_key(string *str, char **output) {
    json_err *err = json_read_simple_string(str, output);
    if (err != NULL) return err;

    if (string_cmp(*output, "") == 0) {
        return json_err_new(EMPTY_KEY, str->index, NULL);
    }

    return NULL;
}

json_err *json_read_sequence(string *str, const char *sequence) {
    for (uint i = 0; sequence[i] != '\0'; i++) {
        if (string_peek(str) != sequence[i]) {
            return json_err_new(WRONG_CHARACTER, str->index, NULL);
        }
        string_next(str);
    }

    return NULL;
}

json_err *json_read_true(string *str, on *output) {
    on_set(output, ON_TRUE);
    return json_read_sequence(str, "true");
}

json_err *json_read_false(string *str, on *output) {
    on_set(output, ON_FALSE);
    return json_read_sequence(str, "false");
}

json_err *json_read_null(string *str, on *output) {
    on_set(output, ON_NULL);
    return json_read_sequence(str, "null");
}

int json_is_digit(char c) { return c >= '0' && c <= '9'; }

json_err *json_read_integer(string *str, int *number) {
    int sign = 0;

    if (string_peek(str) == '-') {
        sign = 1;
        string_next(str);
    } else if (string_peek(str) == '+') {
        string_next(str);
    }

    if (!json_is_digit(string_peek(str))) {
        return json_err_new(MISSING_DIGIT, str->index, NULL);
    }

    char start = string_peek(str);
    *number = 0;

    while (json_is_digit(string_peek(str))) {
        *number = *number * 10 + string_peek(str) - '0';
        string_next(str);
    }
    *number = (sign ? -1 : 1) * *number;

    if (start == '0' && *number != 0) {
        return json_err_new(WRONG_CHARACTER, str->index, NULL);
    }

    return NULL;
}

json_err *json_read_fraction(string *str, int *fraction) {
    *fraction = 0;

    if (string_peek(str) != '.') return NULL;
    string_next(str);

    while (json_is_digit(string_peek(str))) {
        *fraction = *fraction * 10 + string_peek(str) - '0';
        string_next(str);
    }

    return NULL;
}

json_err *json_read_exponent(string *str, int *exponent) {
    int sign = 0;
    *exponent = 0;

    if (string_peek(str) != 'e' && string_peek(str) != 'E') return NULL;
    string_next(str);

    if (string_peek(str) == '-') sign = 1;
    else if (string_peek(str) == '+') sign = 0;
    else if (!json_is_digit(string_peek(str))) {
        return json_err_new(WRONG_CHARACTER, str->index, NULL);
    }

    string_next(str);

    if (!json_is_digit(string_peek(str))) {
        return json_err_new(UNEXPECTED_END, str->index, NULL);
    }

    while (json_is_digit(string_peek(str))) {
        *exponent = *exponent * 10 + string_peek(str) - '0';
        string_next(str);
    }

    *exponent = (sign ? -1 : 1) * *exponent;

    return NULL;
}

json_err *json_read_number(string *str, on *output) {
    int num;
    int fra;
    int exp;

    json_err *err;
    err = json_read_integer(str, &num);
    if (err != NULL) return err;

    err = json_read_fraction(str, &fra);
    if (err != NULL) return err;

    err = json_read_exponent(str, &exp);
    if (err != NULL) return err;

    double fraction = fra;
    while (fraction > 1) {
        fraction /= 10;
    }

    if (fra == 0 && exp == 0) {
        on_set(output, ON_INTEGER);
        output->number = num;
    } else {
        on_set(output, ON_DOUBLE);

        double number = num + fraction;
        int exp_abs = exp >= 0 ? exp : -exp;
        for (uint i = 0; i < exp_abs; i++) {
            if (exp > 0) number = number * 10;
            else number = number / 10;
        }

        output->number = number;
    }

    return NULL;
}

json_err *json_read_inner(string *str, on *output);

json_err *json_read_value(string *str, on *output) {
    switch (string_peek(str)) {
    case '"':
        return json_read_string(str, output);
    case 't':
        return json_read_true(str, output);
    case 'f':
        return json_read_false(str, output);
    case 'n':
        return json_read_null(str, output);
    case '{':
    case '[':
        return json_read_inner(str, output);
    default:
        return json_read_number(str, output);
    }

    return json_err_new(UNEXPECTED_END, str->index, NULL);
}

json_err *json_read_object(string *str, on *output) {
    if (string_peek(str) != '{') {
        return json_err_new(WRONG_CHARACTER, str->index, NULL);
    }

    string_next(str);

    on_set(output, ON_OBJECT);

    while (1) {
        if (string_peek(str) == 0) {
            return json_err_new(WRONG_CHARACTER, str->index, NULL);
        }

        json_skip_spaces(str);

        if (string_peek(str) == '}') {
            string_next(str);
            return NULL;
        }

        char *key = NULL;
        json_err *err = NULL;

        err = json_read_key(str, &key);
        if (err != NULL) return err;
        json_skip_spaces(str);

        if (string_peek(str) != ':') {
            free(key);
            return json_err_new(WRONG_CHARACTER, str->index, "Expected ':'");
        }

        string_next(str);
        json_skip_spaces(str);

        on *value = on_create();
        err = json_read_value(str, value);

        if (err != NULL) {
            free(key);
            free(value);
            return err;
        }

        int status = on_add_value(output, key, value);

        // mabye remove string dups from hashmap
        free(key);
        if (status != 0) return json_err_new(INTERNAL_ERROR, str->index, NULL);

        json_skip_spaces(str);
        if (string_peek(str) != ',') break;
        string_next(str);
        json_skip_spaces(str);

        if (string_peek(str) == '}') {
            return json_err_new(TRAILING_COMMA, str->index, NULL);
        }
    }

    if (string_peek(str) != '}') {
        return json_err_new(WRONG_CHARACTER, str->index, NULL);
    }

    string_next(str);

    return NULL;
}

json_err *json_read_array(string *str, on *output) {
    if (string_peek(str) != '[') {
        return json_err_new(WRONG_CHARACTER, str->index, NULL);
    }

    string_next(str);

    on_set(output, ON_ARRAY);

    while (1) {
        if (string_peek(str) == 0) {
            return json_err_new(UNEXPECTED_END, str->index, NULL);
        }

        json_skip_spaces(str);

        if (string_peek(str) == ']') {
            string_next(str);
            return NULL;
        }

        on *value = on_create();
        json_err *err = json_read_value(str, value);
        if (err != NULL) {
            on_free(value);
            return err;
        }

        int status = on_add_value(output, NULL, value);
        if (status != 0) return json_err_new(INTERNAL_ERROR, str->index, NULL);

        json_skip_spaces(str);
        if (string_peek(str) != ',') break;
        string_next(str);
        json_skip_spaces(str);

        if (string_peek(str) == ']') {
            return json_err_new(TRAILING_COMMA, str->index, NULL);
        }
    }

    if (string_peek(str) != ']') {
        return json_err_new(WRONG_CHARACTER, str->index, NULL);
    }

    string_next(str);

    return NULL;
}

json_err *json_read_inner(string *str, on *output) {
    if (string_peek(str) == '{') return json_read_object(str, output);
    if (string_peek(str) == '[') return json_read_array(str, output);

    return json_err_new(WRONG_CHARACTER, str->index, "Expected '{' or '['");
}

json_err *json_loads(char *s, on **output) {
    *output = on_create();

    string *str = string_from(s);
    json_err *err = json_read_inner(str, *output);
    if (err != NULL) {
        string_free(str);
        on_free(*output);
        return err;
    }

    if (str->index != str->length && string_peek(str) != '\n') {
        return json_err_new(UNEXPECTED_END, str->index, NULL);
    }

    string_free(str);

    return NULL;
}

json_err *json_load(const char *filename, on **output) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) return NULL;

    fseek(f, 0, SEEK_END);
    uint length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = xmalloc(length + 1);
    fread(buffer, 1, length, f);
    fclose(f);

    buffer[length] = '\0';

    json_err *err = json_loads(buffer, output);

    free(buffer);

    return err;
}
