#ifndef JSON_H
#define JSON_H

#include "../on.h"

char *json_dumps(on *o);
int json_dump(on *o, const char* filename);

on* json_loads(char* s);
on* json_load(const char* filename);

#endif // !JSON_H
