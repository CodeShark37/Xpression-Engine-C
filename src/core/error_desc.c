
#include "error.h"

const ErrorDescriptor error_table[ERR_COUNT] =
{
   /* CLI Errors */
    [CERR_NO_EXPRESSION] = {
        .sev = SEV_ERROR,
        .name = "CERR_NO_EXPRESSION",
        .msg = "Missing expression or -f [file]"
    },

    [CERR_INVALID_OPTION] = {
        .sev = SEV_ERROR,
        .name = "CERR_INVALID_OPTION",
        .msg = "Invalid option '%s' or incorrect option order"
    },

    [CERR_MISSING_FILENAME] = {
        .sev = SEV_ERROR,
        .name = "CERR_MISSING_FILENAME",
        .msg = "No filename specified after -f flag"
    },

    [CERR_FILE_READ] = {
        .sev = SEV_ERROR,
        .name = "CERR_FILE_READ",
        .msg = "Cannot open file '%s'"
    },

    [CERR_UNEXPECTED_ARGS] = {
        .sev = SEV_ERROR,
        .name = "CERR_UNEXPECTED_ARGS",
        .msg = "Unexpected command line arguments after '%s'"
    },

    /* Parser errors */
    [PERR_UNTERMINATED_STRING] = {
        .sev = SEV_ERROR,
        .name = "PERR_UNTERMINATED_STRING",
        .msg = "Unterminated string literal"
    },

    [PERR_UNTERMINATED_ARRAY] = {
        .sev = SEV_ERROR,
        .name = "PERR_UNTERMINATED_ARRAY",
        .msg = "Unterminated array - missing closing ']'"
    },

    [PERR_UNTERMINATED_PLACEHOLDER] = {
        .sev = SEV_ERROR,
        .name = "PERR_UNTERMINATED_PLACEHOLDER",
        .msg = "Unterminated placeholder - missing closing '}'"
    },

    [PERR_UNTERMINATED_MULTI_ACCESS] = {
        .sev = SEV_ERROR,
        .name = "PERR_UNTERMINATED_MULTI_ACCESS",
        .msg = "Unterminated multi-access - missing closing ']'"
    },

    [PERR_UNTERMINATED_FUNCTION] = {
        .sev = SEV_ERROR,
        .name = "PERR_UNTERMINATED_FUNCTION",
        .msg = "Unterminated function call - missing closing ')'"
    },

    [PERR_UNEXPECTED_EOF] = {
        .sev = SEV_ERROR,
        .name = "PERR_UNEXPECTED_EOF",
        .msg = "Unexpected end of file"
    },

    [PERR_UNEXPECTED_CHAR] = {
        .sev = SEV_ERROR,
        .name = "PERR_UNEXPECTED_CHAR",
        .msg = "Unexpected character '%c'"
    },

    [PERR_EXPECT_NUMBER] = {
        .sev = SEV_ERROR,
        .name = "PERR_EXPECT_NUMBER",
        .msg = "Expected number after '.'"
    },

    [PERR_EXPECTED_COMMA_OR_OTHER] = {
        .sev = SEV_ERROR,
        .name = "PERR_EXPECTED_COMMA_OR_OTHER",
        .msg = "Expected ',' or '%c' in argument list"
    },
    
    [PERR_EMPTY_PLACEHOLDER] = {
        .sev = SEV_ERROR,
        .name = "PERR_EMPTY_PLACEHOLDER",
        .msg = "Empty placeholder ${}"
    },

    /* Validate Errors */
    
    [VERR_MAX_DEPTH] = {
        .sev = SEV_ERROR,
        .name = "VERR_MAX_DEPTH",
        .msg = "Max AST node depth reached"
    },

    [VERR_NO_EXPRESSION_FOUND] = {
        .sev = SEV_ERROR,
        .name = "vERR_NO_EXPRESSION_FOUND",
        .msg = "No expression found - missing placeholders ${}"
    },

    [VERR_EXPECTED_PROPERTY_OR_MULTI_ACCESS] = {
        .sev = SEV_ERROR,
        .name = "VERR_EXPECTED_PROPERTY_OR_MULTI_ACCESS",
        .msg = "Expected property or multi-access"
    },
    
    [VERR_EXPECTED_OBJECT_MULTI_ACCESS] = {
        .sev = SEV_ERROR,
        .name = "VERR_EXPECTED_OBJECT_MULTI_ACCESS",
        .msg = "Expected Object in multi-access but argument %d is %s"
    },


    [VERR_EMPTY_MULTI_ACCESS] = {
        .sev = SEV_ERROR,
        .name = "VERR_EMPTY_MULTI_ACCESS",
        .msg = "Empty multi-access"
    },

    [VERR_EMPTY_ARRAY] = {
        .sev = SEV_ERROR,
        .name = "VERR_EMPTY_ARRAY",
        .msg = "Empty array"
    },

    [VERR_FEW_ARGS_MULTI_ACCESS] = {
        .sev = SEV_ERROR,
        .name = "VERR_FEW_ARGS_MULTI_ACCESS",
        .msg = "Multi-access require at least 2 arguments, got %d"
    },

    [VERR_DUP_ARG_MULTI_ACCESS] = {
        .sev = SEV_ERROR,
        .name = "VERR_DUP_ARG_MULTI_ACCESS",
        .msg = "Duplicated argument %d in multi access"
    },

    [VERR_UNKNOWN_FUNCTION] = {
        .sev = SEV_ERROR,
        .name = "VERR_UNKNOWN_FUNCTION",
        .msg = "Unknown function '%s'"
    },

    [VERR_FUNCTION_ARITY_MAX] = {
        .sev = SEV_ERROR,
        .name = "VERR_FUNCTION_ARITY_MAX",
        .msg = "Function '%s' expects at most %d argument(s), got %d"
    },

    [VERR_FUNCTION_ARITY_MIN] = {
        .sev = SEV_ERROR,
        .name = "VERR_FUNCTION_ARITY_MIN",
        .msg = "Function '%s' expects at least %d argument(s), got %d"
    },

    [VERR_FUNCTION_ARG_TYPE] = {
        .sev = SEV_ERROR,
        .name = "VERR_FUNCTION_ARG_TYPE",
        .msg = "Function '%s' argument %d: expected %s, got %s"
    },

    [VERR_LITERAL_MISSING] = {
        .sev = SEV_ERROR,
        .name = "VERR_LITERAL_MISSING",
        .msg = "Literal node missing value"
    },

    [VERR_UNKNOWN_NODE] = {
        .sev = SEV_ERROR,
        .name = "VERR_UNKNOWN_NODE",
        .msg = "Unsupported AST node kind: %s"
    },

    /* System errors */
    [SERR_SYSTEM_MEMORY] = {
        .sev = SEV_FATAL,
        .name = "SERR_SYSTEM_MEMORY",
        .msg = "System memory error"
    },

    [SERR_SYSTEM_IO] = {
        .sev = SEV_FATAL,
        .name = "SERR_SYSTEM_IO",
        .msg = "System I/O error"
    },

    [SERR_SYSTEM_INTERNAL] = {
        .sev = SEV_FATAL,
        .name = "SERR_SYSTEM_INTERNAL",
        .msg = "Internal system error"
    },

    [SERR_MEMORY_ALLOC] = {
        .sev = SEV_FATAL,
        .name = "SERR_MEMORY_ALLOC",
        .msg = "Memory allocation failed"
    }
};

/* garantia estrutural obrigatoria */
_Static_assert(
    ERR_COUNT >= sizeof(error_table) / sizeof(error_table[0]),
    "error_table out of sync with ErrorType enum"
);

const ErrorDescriptor* error_desc_get(ErrorType type){
    if (type >= 0 && type < ERR_COUNT){
        return &error_table[type];
    }
    return NULL;
}
