#ifndef SET_UTILS_H
#define SET_UTILS_H

#include "structs.h"

Set *set_create(int capacity);
void set_add(Set *s, int vertex);
void set_remove(Set *s);
void set_destroy(Set *s);

#endif // SET_UTILS_H