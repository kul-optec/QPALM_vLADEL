#include <global_opts.h>

void *c_calloc(size_t num, size_t size) {
    return ladel_calloc(num, size);
}

void *c_malloc(size_t size) {
    return ladel_malloc(size, 1);
}

void* c_realloc(void *ptr, size_t size) {
    ladel_int status;
    return ladel_realloc(ptr, size, 1, &status);
}

void c_free(void *ptr) {
    ladel_free(ptr);
}
