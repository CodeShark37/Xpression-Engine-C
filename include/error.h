/**
 * @file error.h
 * @brief Centralized error management, reporting and formatting.
 *
 * This module provides a global error manager with support for:
 *  - Structured error types and severities
 *  - Positional and contextual diagnostics
 *  - Multiple output formats (plain, JSON, XML)
 *  - Error aggregation and summary reporting
 *
 * Errors are collected during execution and may be queried or printed
 * at the end of processing.
 *
 * @ingroup error
 * @author Joshua (Codeshark37) � labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */

#ifndef ERROR_H
#define ERROR_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include "cli.h"


/** Base window size for contextual error display */
#define ERROR_CONTEXT_WINDOW 11
#define ERROR_MAX_DEFAULT 100

/**
 * @enum ErrorType
 * @brief Enumerates all error categories produced by the system.
 *
 * @note The order in enum is very important
 * @note Keep ERR_COUNT at the end
 */
typedef enum {
    /* CLI errors */
    CERR_NO_EXPRESSION = 0,
    CERR_INVALID_OPTION,
    CERR_MISSING_FILENAME,
    CERR_FILE_READ,
    CERR_UNEXPECTED_ARGS,

    /* Parser errors */
    PERR_UNTERMINATED_STRING,
    PERR_UNTERMINATED_FUNCTION,
    PERR_UNTERMINATED_MULTI_ACCESS,
    PERR_UNTERMINATED_PLACEHOLDER,
    PERR_UNTERMINATED_ARRAY,
    PERR_UNEXPECTED_EOF,
    PERR_UNEXPECTED_CHAR,
    PERR_EXPECT_NUMBER,
    PERR_EXPECTED_COMMA_OR_OTHER,
    PERR_EMPTY_PLACEHOLDER,

    /* Validate Errors */
    VERR_EXPECTED_PROPERTY_OR_MULTI_ACCESS,
    VERR_EXPECTED_OBJECT_MULTI_ACCESS,
    VERR_EMPTY_MULTI_ACCESS,
    VERR_DUP_ARG_MULTI_ACCESS,
    VERR_FEW_ARGS_MULTI_ACCESS,
    VERR_EMPTY_ARRAY,
    VERR_NO_EXPRESSION_FOUND,
    VERR_UNKNOWN_FUNCTION,
    VERR_UNKNOWN_NODE,
    VERR_FUNCTION_ARITY_MAX,
    VERR_FUNCTION_ARITY_MIN,
    VERR_FUNCTION_ARG_TYPE,
    VERR_LITERAL_MISSING,
    VERR_MAX_DEPTH,

    /* System errors */
    SERR_SYSTEM_MEMORY,
    SERR_MEMORY_ALLOC,
    SERR_SYSTEM_IO,
    SERR_SYSTEM_INTERNAL,
    ERR_COUNT
} ErrorType;

/**
 * @struct ErrorPosition
 * @brief Source position metadata for an error.
 */
typedef struct {
    size_t line;  /**< Line number (1-based) */
    size_t col;   /**< Column number (1-based) */
    size_t len;   /**< Length of the error span */
} ErrorPosition;

/**
 * @enum ErrorSeverity
 * @brief Severity level of an error.
 */
typedef enum {
    SEV_INFO,
    SEV_WARNING,
    SEV_ERROR,
    SEV_FATAL
} ErrorSeverity;

/**
 * @struct Error
 * @brief Represents a single diagnostic error.
 */
typedef struct {
    ListNode node;
    ErrorType type;         /**< Error classification */
    ErrorSeverity sev;      /**< Severity level */
    ErrorPosition position; /**< Source position */
    char msg[512];      /**< Formatted error message */
    const char* source;     /**< Source text for contextual display */
    bool has_position;      /**< Indicates if position is valid */
    bool has_context_line;  /**< Indicates if source context is available */
} Error;

// Vetor de erros
typedef struct {
    List errors;
    size_t count;
} ErrorList;

/**
 * @struct ErrorConfig
 * @brief Configuration for the error manager.
 */
typedef struct {
    int max_errors;            /**< Maximum stored errors (-1 = unlimited) */
    size_t verbosity;          /**< Verbosity level 0=quiet, 1=normal, 2=verbose*/
    bool show_context;         /**< Show caret-based source context */
} ErrorConfig;

// Global Error Manager
typedef struct {
    ErrorList errors;
    ErrorConfig config;
    bool initialized;
} ErrorContext;

/**
 * @brief Descritor sem�ntico de um erro.
 *
 * Single source of truth:
 *  - tipo
 *  - severidade
 *  - nome est�vel
 *  - formato da mensagem
 */
typedef struct {
    ErrorSeverity sev;
    const char* name;
    const char* msg;  // printf-style format
} ErrorDescriptor;

/**
 * @brief Obt�m o descritor sem�ntico de um erro.
 *
 * @param type ErrorType
 * @return Ponteiro para descritor ou NULL se desconhecido
 */
const ErrorDescriptor* error_desc_get(ErrorType type);

/**
 * @brief Initialize the global error manager.
 *
 * @param config Optional configuration (NULL = defaults)
 * @return true on success, false on allocation failure
 */
bool error_init(ErrorContext* ctx, const ErrorConfig* config);

/**
 * @brief Release all resources used by the error system.
 */
void error_free(ErrorContext* ctx);

/**
 * @brief Clear all stored errors without destroying the manager.
 */
void error_clear(ErrorContext* ctx);

/**
 * @brief Add an error without source position.
 *
 * @param type Error type
 * @param ...  Format arguments for the error message
 * @return Pointer to the created Error, or NULL on failure
 */
Error* error_add(ErrorContext* ctx, ErrorType type, ...);

/**
 * @brief Add an error with line and column information.
 */
Error* error_add_at(ErrorContext* ctx, ErrorType type, size_t line, size_t col, ...);

/**
 * @brief Add an error with full positional and contextual information.
 *
 * @param len length of caret pointing context
 * @param source Source buffer used for caret display
 */
Error* error_add_ctx(ErrorContext* ctx, ErrorType type, size_t line, size_t col, size_t len, const char* source, ...);

/**
 * @brief Print a single error using json format.
 */
void error_print_json(const Error* err);

/**
 * @brief Print a single error using xml format.
 */
void error_print_xml(const Error* err);

/**
 * @brief Print all collected errors.
 */
void error_print_all(const ErrorContext* ctx);

/**
 * @brief Print a summary of collected diagnostics.
 */
void error_print_summary(const ErrorContext* ctx);

/**
 * @brief Query if any error or fatal error occurred.
 */
bool error_has_error(const ErrorContext* ctx);

/**
 * @brief Query if any fatal error occurred.
 */
bool error_has_fatal(const ErrorContext* ctx);

/**
 * @brief Get the number of stored errors.
 */
size_t error_count(const ErrorContext* ctx);

/**
 * @brief Retrieve an error by index.
 */
const Error* error_get(const ErrorContext* ctx, size_t index);

/**
 * @brief Retrieve the last recorded error.
 */
const Error* error_last(const ErrorContext* ctx);

/**
 * @brief Convert an ErrorType to its symbolic name.
 */
const char* error_type_name(ErrorType type);

#ifdef __cplusplus
}
#endif

#endif /* ERROR_H */
