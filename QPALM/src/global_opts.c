#include <global_opts.h>

void *qpalm_calloc(size_t num, size_t size) {
    return ladel_calloc(num, size);
}

void *qpalm_malloc(size_t size) {
    return ladel_malloc(size, 1);
}

void* qpalm_realloc(void *ptr, size_t size) {
    ladel_int status;
    return ladel_realloc(ptr, size, 1, &status);
}

void qpalm_free(void *ptr) {
    ladel_free(ptr);
}
