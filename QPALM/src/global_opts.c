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

calloc_sig *qpalm_set_alloc_config_calloc(calloc_sig *calloc) {
    return ladel_set_alloc_config_calloc(calloc);
}
malloc_sig *qpalm_set_alloc_config_malloc(malloc_sig *malloc) {
    return ladel_set_alloc_config_malloc(malloc);
}
realloc_sig *qpalm_set_alloc_config_realloc(realloc_sig *realloc) {
    return ladel_set_alloc_config_realloc(realloc);
}
free_sig *qpalm_set_alloc_config_free(free_sig *free) {
    return ladel_set_alloc_config_free(free);
}
printf_sig *qpalm_set_print_config_printf(printf_sig *printf) {
    return ladel_set_print_config_printf(printf);
}
