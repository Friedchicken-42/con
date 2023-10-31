#include "json.h"
#include <stdio.h>

#define true 1
#define false 0

typedef struct status_t {
    int pass;
    int fail;
} status;

int json_check(char *filename) {
    on *o = NULL;
    return json_load(filename, &o) == NULL;
}

void test(const char *folder, const char *name, int size, int expected,
          status *s) {
    char filename[100];
    status local = {0, 0};

    printf("%s\n", name);

    for (int i = 0; i < size; i++) {
        snprintf(filename, 100, "tests/%s/%s%02d.%s", folder, name, i + 1,
                 folder);
        if (json_check(filename) == expected) {
            printf("\t[ Pass ] %s\n", filename);
            local.pass++;
        } else {
            printf("\t[ Fail ] %s\n", filename);
            local.fail++;
        }
    }

    printf("%d passed, %d failed\n\n", local.pass, local.fail);

    s->pass += local.pass;
    s->fail += local.fail;
}

int main() {
    status s = {0, 0};

    test("json", "pass", 26, true, &s);
    test("json", "fail", 26, false, &s);
    test("json", "complete", 1, true, &s);

    printf("Total: %d passed, %d failed\n", s.pass, s.fail);
}
