/**
 * @file error.c
 * @brief Implementation of the centralized error handling system.
 *
 * This file contains the internal logic for error creation, storage,
 * formatting and output. All public APIs are declared in error.h.
 *
 * @ingroup error
 * @author Joshua (Codeshark37) � labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


const char* error_type_name(ErrorType type) {
    const ErrorDescriptor* d = error_desc_get(type);
    return d ? d->name : "UNKNOWN_ERROR";
}

const char* error_severity_name(ErrorSeverity sev) {
    switch (sev) {
        case SEV_INFO: return "INFO";
        case SEV_WARNING: return "WARNING";
        case SEV_ERROR: return "ERROR";
        case SEV_FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

bool error_init(ErrorContext* ctx, const ErrorConfig* config){
    if (ctx->initialized) return true;
    
    list_init(&ctx->errors.errors);
    ctx->config = (ErrorConfig){
        .max_errors = config ? config->max_errors : ERROR_MAX_DEFAULT,
        .show_context = config ? config->show_context : true,
        .format = config ? config->format : FORMAT_PLAIN,
        .verbosity = config ? config->verbosity : 1,
    };

    ctx->errors.count = 0;
    ctx->config.shared_source = NULL;
    ctx->initialized = true;
    return true;
}

void error_clear(ErrorContext* ctx){
    if (!ctx->initialized) return;
    list_for_each_safe(it,tmp,&ctx->errors.errors) {
        Error* err = container_of(it, Error, node);
        list_remove(it);
        free(err);
    }
    ctx->errors.count = 0;
}

void errors_destroy(ErrorContext* ctx) {
    error_clear(ctx);
}

/**
 * @brief Calculate absolute source position from line and column.
 *
 * Handles LF and CRLF line endings.
 *
 * @param err Error descriptor
 * @return Absolute idx in source buffer
 */
static size_t error_calc_pos(const Error* err) {
    if (!err->source ||
        !err->position.line ||
        !err->position.col) return 0;

    size_t pos = 0;
    size_t cur_line = 1;

    while (err->source[pos] && cur_line < err->position.line) {
        if (err->source[pos] == '\n') cur_line++;
		else if (err->source[pos] == '\r') {
            // CRLF handling
            if (err->source[pos+1] == '\n') pos++;
            cur_line++;
        }
        pos++;
    }
    /* pos is at the beginning of the desired line (or at the end of the source) */
    /* advance col-1 characters (but without going beyond the end of the line) */
    size_t moved = 0;
    while (err->source[pos] &&
			(err->source[pos] != '\n' &&
			(err->source[pos] != '\r' &&
			err->source[pos+1] != '\n'))&&
			moved < (err->position.col - 1)) {
        pos++;
        moved++;
    }
    /* if col is longer than the line length,
       pos points to the end of the line */
	return pos;
}

/// Finds the beginning of the line where the error occurred.
static const char* err_line_start(const char* src, size_t error_pos) {
    if (!src) return NULL;
    const char* p = src + error_pos;
    /* If error_pos points inside the line, indent to the beginning of the line */
    while (p > src && *(p - 1) != '\n' && *(p - 1) != '\r') {
        p--;
    }
    return p;
}

/// Find the end of the line where the error occurred.
static const char* err_line_end(const char* src, size_t error_pos) {
    if (!src) return NULL;
    const char* p = src + error_pos;
    while (*p && *p != '\n' && *p != '\r') {
        p++;
    }
    return p;
}

/**
 * @brief Extract a contextual source window centered on the error.
 *
 * @param err Error descriptor
 * @param win Desired window width
 * @param caret_pos Output caret offset
 * @return Allocated context string (must be freed), or NULL
 */
static char* error_get_ctx(const Error* err, size_t win, size_t* caret_pos) {
    if (!err || !err->source || !win) {
        if (caret_pos) *caret_pos = 0;
        return NULL;
    }

    /// Calculate the absolute position of the error
    size_t err_pos = error_calc_pos(err);

    /// Find line limits
    const char* line_start = err_line_start(err->source, err_pos);
    const char* line_end = err_line_end(err->source, err_pos);

    if (!line_start || !line_end || line_start >= line_end) {
        if (caret_pos) *caret_pos = 0;
        return NULL;
    }

    size_t line_len = (size_t)(line_end - line_start);
    size_t pos_in_line = (size_t)(err->source + err_pos - line_start);
    if (pos_in_line > line_len) pos_in_line = line_len;

    /// Calculate centered window on error
    size_t half = win >> 1;
    size_t start = (pos_in_line > half) ? (pos_in_line - half) : 0;

    /// Adjust if the line ends
    if (start + win > line_len) {
        start = (line_len > win) ? (line_len - win) : 0;
    }

    size_t end = start + ((win < line_len) ? win : line_len);
    if (end > line_len) end = line_len;

    size_t visible_len = end - start;

    /// Allocate and copy
    char* ctx = calloc(visible_len + 1, sizeof(char));
    if (!ctx) {
        if (caret_pos) *caret_pos = 0;
        return NULL;
    }

    memcpy(ctx, line_start + start, visible_len);

    if (caret_pos) {
        *caret_pos = pos_in_line - start;
    }

    return ctx;
}

static Error* error_new(ErrorType type, va_list args) {
    const ErrorDescriptor* d = error_desc_get(type);

    Error* err = calloc(1, sizeof(Error));
    if (!err) return NULL;

    err->type = type;
    err->sev = d ? d->sev : SEV_ERROR;
    err->has_position = false;
    err->has_context_line = false;
    err->source = NULL;

    const char* fmt = d ? d->msg : "Unknown error";
    vsnprintf(err->msg, sizeof(err->msg), fmt, args);

    return err;
}

/// Helper to validate and add error to the manager
static Error* error_push(ErrorContext* ctx, Error* err){
    if (!ctx || !err)
        return NULL;

    if (!ctx->initialized) {
        free(err);
        return NULL;
    }

    if (ctx->config.max_errors >= 0 &&
        (int)ctx->errors.count >= ctx->config.max_errors) {
        free(err);
        return NULL;
    }

    list_push_back(
        &ctx->errors.errors,
        &err->node
    );
    ctx->errors.count++;

    return err;
}

static Error* error_add_va(
    ErrorContext* ctx,
    ErrorType type,
    va_list args
){
    Error* err = error_new(type, args);
    if (!err)
        return NULL;
    return error_push(ctx, err);
}


Error* error_add(ErrorContext* ctx, ErrorType type, ...){
    va_list args;
    va_start(args, type);
    Error* err = error_add_va(ctx, type, args);
    va_end(args);
    return err;
}

static Error* error_add_va_at(
    ErrorContext* ctx,
    ErrorType type,
    size_t line,
    size_t col,
    va_list args
){
    Error* err = error_new(type, args);
    if (!err)
        return NULL;
    err->has_position = true;
    err->position.line = line;
    err->position.col  = col;
    return error_push(ctx, err);
}

Error* error_add_ex_at(
    ErrorContext* ctx, ErrorType type,
    size_t line, size_t col, ...
){
    va_list args;
    va_start(args, col);
    Error* err = error_add_va_at(
        ctx, type,
        line, col,
        args
    );
    va_end(args);
    return err;
}

static Error* error_add_va_ctx(
    ErrorContext* ctx,
    ErrorType type,
    size_t line,
    size_t col,
    size_t len,
    const char* src,
    va_list args
){
    Error* err = error_new(type, args);
    if (!err)
        return NULL;
    err->has_position = true;
    err->position.line = line;
    err->position.col  = col;
    err->position.len  = len;

    err->source = ctx->config.shared_source
                ? ctx->config.shared_source
                : src;

    err->has_context_line =
        (err->source != NULL);
    return error_push(ctx, err);
}


Error* error_add_ctx(ErrorType type, size_t line,
                            size_t col, size_t len,
                            const char* src, ...) {
    va_list args;
    va_start(args, src);
    Error* err = error_add_va_ctx(&g_default_ctx,type, line, col, len, src, args);
    va_end(args);
    return err;
}


Error* error_add_ctx(
    ErrorContext* ctx,
    ErrorType type,
    size_t line,
    size_t col,
    size_t len,
    const char* src,
    ...
){
    va_list args;
    va_start(args, src);
    Error* err = error_add_va_ctx(
        ctx, type, line,
        col, len, src,
        args
    );
    va_end(args);
    return err;
}

size_t error_count(const ErrorContext* ctx) {
    return ctx->errors.count;
}

size_t error_count_by_severity(const ErrorContext* ctx, ErrorSeverity sev) {
    size_t count = 0;
    for (size_t i = 0; i < ctx->errors.count; i++) {
        if (error_get(ctx, i)->sev == sev) {
            count++;
        }
    }
    return count;
}

size_t error_count_by_type(ErrorContext* ctx, ErrorType type) {
    size_t count = 0;
    for (size_t i = 0; i < ctx->errors.count; i++) {
        if (error_get(ctx, i)->type == type) {
            count++;
        }
    }
    return count;
}

bool error_has_error(const ErrorContext* ctx){
    return error_count_by_severity(ctx, SEV_ERROR) > 0 ||
           error_count_by_severity(ctx, SEV_FATAL) > 0;
}

bool error_has_fatal(const ErrorContext* ctx){
    return error_count_by_severity(ctx, SEV_FATAL) > 0;
}

const Error* error_get(const ErrorContext* ctx, size_t idx){
    if (idx >= ctx->errors.count)
        return NULL;
    size_t i = 0;
    list_for_each(it, &ctx->errors.errors) {
        if (i++ == idx)
            return container_of(it, Error, node);
    }
    return NULL;
}

const Error* error_get_last(ErrorContext* ctx){
    ListNode* n = list_back(&ctx->errors.errors);
    return n ? container_of(n, Error, node) : NULL;
}

void error_print_json(const Error* err) {
    if(!err) return;
    fprintf(stderr, "{\n");
    fprintf(stderr, "  \"type\": \"%s\",\n", error_type_name(err->type));
    fprintf(stderr, "  \"sev\": \"%s\",\n", error_severity_name(err->sev));
    fprintf(stderr, "  \"message\": \"%s\"", err->msg);

    if (err->has_position) {
        fprintf(stderr, ",\n  \"position\": {\n");
        fprintf(stderr, "\t\"line\":  %zu,\n", err->position.line);
        fprintf(stderr, "\t\"column\":  %zu\n", err->position.col);
        fprintf(stderr, "  }");
    }

    fprintf(stderr, "\n}\n");
}

// Escapes special characters for XML
static void error_xml_esc(const char* str) {
    if (!str) return;
    for (const char* p = str; *p; p++) {
        switch (*p) {
            case '&':  fprintf(stderr,"&amp;");  break;
            case '<':  fprintf(stderr,"&lt;");   break;
            case '>':  fprintf(stderr,"&gt;");   break;
            case '\'': fprintf(stderr,"&apos;"); break;
            case '"':  fprintf(stderr,"&quot;"); break;
            default:   fputc(*p, stderr);        break;
        }
    }
}

void error_print_xml(const Error* err) {
    if (!err) return;

    fprintf(stderr,"<error>\n");
    fprintf(stderr,"  <type>");
    error_xml_esc(error_type_name(err->type));
    fprintf(stderr,"</type>\n");

    fprintf(stderr,"  <sev>");
    error_xml_esc(error_severity_name(err->sev));
    fprintf(stderr,"</sev>\n");

    fprintf(stderr,"  <message>");
    error_xml_esc(err->msg);
    fprintf(stderr,"</message>\n");

    if (err->has_position) {
        fprintf(stderr,"  <position>\n");
        fprintf(stderr, "\t<line> %zu</line>\n", err->position.line);
        fprintf(stderr, "\t<column> %zu</column>\n", err->position.col);
        fprintf(stderr,"  </position>\n");
    }

    fprintf(stderr,"</error>\n");
}

static size_t win_len(const Error* err){
    size_t len = (ERROR_CONTEXT_WINDOW * 2 + err->position.len * 2);
    return (len&1) ? (len+1) :len;
}

static void error_print_plain(ErrorContext* ctx ,const Error* err) {
    if (!err) return;

    fprintf(stderr, "\n[%s] ",error_severity_name(err->sev));

	if (err->has_position) {
        fprintf(stderr, "<input>: %zu: %zu: ", err->position.line, err->position.col);
    }
	fprintf(stderr, "%s\n", err->msg);

	if (ctx->config.show_context && err->has_context_line) {
		size_t caret_idx = 0;
		char* ctx = error_get_ctx(err, win_len(err), &caret_idx);
		if(!ctx) return;

		fprintf(stderr, " | %s\n", ctx);
        fprintf(stderr, " | ");
        free(ctx);
        ctx = NULL;

		for (size_t i = 0; i < caret_idx; i++) fputc(' ', stderr);
        for (size_t i = caret_idx; i < (caret_idx + err->position.len); i++)
            fputc('^', stderr);
		fputc('\n', stderr);
	}
}

void error_print(ErrorContext* ctx ,const Error* err) {
    if (!err) return;
    if (ctx->config.format == FORMAT_JSON) {
        error_print_json(err);
    } else if (ctx->config.format == FORMAT_XML) {
        error_print_xml(err);
    } else {
        error_print_plain(ctx, err);
    }
}

void error_print_all(ErrorContext* ctx) {
    for (size_t i = 0; i < ctx->errors.count; i++) {
		fflush(stderr);
        error_print(ctx, error_get(ctx, i));
    }
}

void error_print_summary(ErrorContext *ctx) {
    if (!ctx->errors.count) return;

    size_t errors   = error_count_by_severity(ctx, SEV_ERROR);
    size_t warnings = error_count_by_severity(ctx, SEV_WARNING);
    size_t fatals   = error_count_by_severity(ctx, SEV_FATAL);

    fputc('\n',stderr);
    if (fatals > 0) {
        fprintf(stderr, "  %zu fatal error%s, ", fatals, fatals > 1 ? "s": "");
    }
    else{
        if (warnings > 0) {
            fprintf(stderr, ",  %zu warning%s", warnings, warnings > 1 ? "s" : "");
        }
    }
    fprintf(stderr, "  %zu error%s", (errors + warnings + fatals), errors > 1 ? "s": "");
    fputs(" generated.",stderr);
}