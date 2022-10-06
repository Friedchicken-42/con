#include "alloc.h"
#include <stdlib.h>

void *checked(void *v) {
    if(v == NULL) exit(EXIT_FAILURE);
    return v;
}

void *xmalloc(unsigned long n) {
    return checked(malloc(n));
}
