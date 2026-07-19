/**
 * @defgroup cli Command Line Interface (CLI)
 * @brief Command-line parsing and orchestration layer.
 *
 * This module implements a strict, deterministic command-line parser
 * based on a finite state machine (FSM). It validates argument order and
 * enforces semantic constraints.
 *
 * Design goals:
 * - Deterministic option ordering
 * - Clear separation between parsing, validation, and execution
 * - Zero ambiguity in CLI semantics
 * - Fail-fast error reporting
 *
 * The CLI supports:
 * - Expression evaluation from command line or file
 * - Output formatting (plain, JSON, XML)
 * - Grouped output
 * - Version reporting
 *
 * @note The option order is strict and enforced by a transition table.
 */


 /**
 * @file cli.h
 * @brief Public interface for the Xpression Engine CLI.
 * @ingroup cli
 *
 * Defines all public types, states, tokens, and entry points
 * used by the command-line interface.
 *
 * This header exposes only the minimal surface required to invoke
 * CLI processing. All parsing logic and helpers are internal to
 * cli.c.
 *
 * @author Joshua (Codeshark37) - labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */

#ifndef CLI_H
#define CLI_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>


/**
 * @enum CLITokenType
 * @brief Logical classification of command-line arguments.
 *
 * Tokens are derived from raw argv strings and drive
 * state transitions in the CLI finite state machine.
 */
typedef enum {
    CTOK_FORMAT,   /**< Output format flag (-json | -xml) */
    CTOK_EVAL,     /**< Evaluation flag (-eval) */
    CTOK_GROUP,    /**< Group flag (-g | -group) */
    CTOK_INPUT,    /**< File input flag (-f) or Expression literal */
    CTOK_VERSION,  /**< Version flag (--version) */
    CTOK_UNKNOWN,  /**< Invalid flag */
    CMAX_TOK       /**< Maximum number of CLI token types */
} CLITokenType;


/**
 * @enum CLIState
 * @brief Finite state machine states for CLI parsing.
 *
 * The order of this enum is **semantically significant**
 * and must match the transition table.
 *
 * @warning Modifying the order may break CLI validation.
 */
typedef enum {
    S_START,   /**< Initial state */
    S_FORMAT,  /**< After format flag */
    S_EVAL,    /**< After eval flag */
    S_GROUP,   /**< After group flag */
    S_DONE,    /**< Sucess state */
    S_ERROR,   /**< Invalid state */
    MAX_STATE  /**< Maximum number of CLI states */
} CLIState;

/**
 * @enum CLIFormat
 * @brief Output representation format.
 */
typedef enum {
    FORMAT_PLAIN, /**< Plain text output */
    FORMAT_JSON,  /**< JSON output */
    FORMAT_XML    /**< XML output */
} CLIFormat;

/**
 * @struct CLIOptions
 * @brief Parsed CLI options container.
 *
 * Holds all semantic options extracted from
 * command-line arguments.
 */
typedef struct {
    const char* filename; /**< Input filename (if -f was used) */
    const char* expr;     /**< Expression buffer or literal */
    bool group;           /**< Group output flag */
    bool eval;            /**< Evaluation flag */
    bool version;         /**< Version flag*/
    CLIFormat format;     /**< Output format */
} CLIOptions;


/**
 * @struct CLIContext
 * @brief Internal CLI parsing context.
 *
 * Tracks parsing state, argument position, and
 * accumulated options during CLI processing.
 */
typedef struct {
    size_t arg_pos;      /**< Current argv index */
    int argc;         /**< Argument count */
    const char** argv;   /**< Argument vector */
    CLIState state;      /**< Current FSM state */
    CLIOptions opts;     /**< Parsed options */
} CLIContext;

/**
 * @brief Process command-line arguments and execute expressions.
 *
 * This is the main entry point for the CLI subsystem.
 * It performs:
 * 1. Argument parsing
 * 2. State validation
 * 3. Expression parsing
 * 4. Output rendering
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @param eval_ctx Evaluation context
 *
 * @return true on success, false on error
 */
bool process_cli(int argc, char** argv, CLIOptions* opt);

#ifdef __cplusplus
}
#endif

#endif /* CLI_H */
