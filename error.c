#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static XprError g_xpr_error = { XPR_OK, {0} };

void xpr_error_clear(void) {
    g_xpr_error.code = XPR_OK;
    g_xpr_error.message[0] = '\0';
}

void xpr_error_set(XprErrorCode code, const char *fmt, ...) {
    g_xpr_error.code = code;
    if (!fmt) {
        g_xpr_error.message[0] = '\0';
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(g_xpr_error.message, sizeof(g_xpr_error.message), fmt, ap);
    va_end(ap);
}

int xpr_error_occurred(void) {
    return g_xpr_error.code != XPR_OK;
}

XprErrorCode xpr_error_code(void) {
    return g_xpr_error.code;
}

const char *xpr_error_message(void) {
    return g_xpr_error.message;
}

void *xpr_malloc(size_t size) {
    void *p = malloc(size);
    if (!p && size) {
        xpr_error_set(XPR_ERR_MEMORY, "memory allocation failed");
    }
    return p;
}

void *xpr_calloc(size_t count, size_t size) {
    void *p = calloc(count, size);
    if (!p && count && size) {
        xpr_error_set(XPR_ERR_MEMORY, "memory allocation failed");
    }
    return p;
}

void *xpr_realloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (!p && size) {
        xpr_error_set(XPR_ERR_MEMORY, "memory allocation failed");
    }
    return p;
}

char *xpr_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *dup = xpr_malloc(len + 1);
    if (!dup) return NULL;
    memcpy(dup, s, len + 1);
    return dup;
}
