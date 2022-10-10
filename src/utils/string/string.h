#ifndef STRING_H

typedef unsigned int uint;

void *string_dup(void *str);
int string_cmp(char* s1, char* s2);

typedef struct string_t {
    char *data;
    uint length;
    uint size;
} string;

string *string_create();
void string_extend(string *str, const char* s2);
char *string_str(string *str);
void string_free(void *str);

#endif // !STRING_H
