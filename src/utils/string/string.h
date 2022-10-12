#ifndef STRING_H

typedef unsigned int uint;

void *string_dup(void *str);
int string_cmp(char* s1, char* s2);

typedef struct string_t {
    char *data;
    uint length;
    uint size;
    uint index;
} string;

string *string_create();
string *string_from(char *s);
void string_push(string *str, const char c);
void string_extend(string *str, const char* s2);
char *string_str(string *str);
char string_peek(string *str);
void string_next(string *str);
void string_free(void *str);

#endif // !STRING_H
