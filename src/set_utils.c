#include "set_utils.h"

Set *set_create(int capacity) {
    Set *s = malloc(sizeof(Set));
    s->vertices = malloc(capacity * sizeof(int));
    s->size = 0;
    s->capacity = capacity;
    return s;
}

void set_add(Set *s, int vertex) {
    if (s->size < s->capacity) {
        s->vertices[s->size++] = vertex;
    }
}

void set_remove(Set *s) {
    if (s->size > 0) {
        s->size--;
    }
}

void set_destroy(Set *s) {
    free(s->vertices);
    free(s);
}