#ifndef XPR_ERROR_H
#define XPR_ERROR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    XPR_OK = 0,
    XPR_ERR_PARSE,
    XPR_ERR_INVALID_ARG,
    XPR_ERR_FILE_IO,
    XPR_ERR_MEMORY,
    XPR_ERR_STATE,
    XPR_ERR_UNKNOWN
} XprErrorCode;

typedef struct {
    XprErrorCode code;
    char message[256];
} XprError;

void xpr_error_clear(void);
void xpr_error_set(XprErrorCode code, const char *fmt, ...);
int xpr_error_occurred(void);
XprErrorCode xpr_error_code(void);
const char *xpr_error_message(void);

void *xpr_malloc(size_t size);
void *xpr_calloc(size_t count, size_t size);
void *xpr_realloc(void *ptr, size_t size);
char *xpr_strdup(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* XPR_ERROR_H */
