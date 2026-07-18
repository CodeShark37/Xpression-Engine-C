/**
 * @file parser.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-06-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Advance one character and update position incrementally */
static void advance_pos(ParserContext* ctx) {
    if (!*ctx->pos) return;

    if (*ctx->pos == '\n') {
        ctx->line++;
        ctx->col = 1;
    } else {
        ctx->col++;
    }
    ctx->pos++;
}

/* Parsing utilities */
static void skip_ws(ParserContext* ctx) {
    while (*ctx->pos && isspace((unsigned char)*ctx->pos)) {
        advance_pos(ctx);
    }
}

static int is_identifier(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static bool expect(ParserContext* ctx, char c) {
    skip_ws(ctx);
    if (*ctx->pos == c) {
        advance_pos(ctx);
        return true;
    }
    return false;
}

static Node* expr(ParserContext* ctx); /* forward declaration */
static Node* primary(ParserContext* ctx); /* forward declaration */

/* Find next "${" starting at ctx->pos. If found, advance ctx->pos so that it
 * points to the character AFTER the '{' (i.e. the start of expression content)
 * and return the pointer to the beginning of the placeholder ("${").
 * If not found, return NULL and do not modify ctx.
 */
static const char* find_next_placeholder(ParserContext* ctx) {
    if (!ctx || !ctx->pos || !*ctx->pos) return NULL;
    const char* found = strstr(ctx->pos, "${");
    if (!found) return NULL;

    const char* target = found + 2; /* after ${ */
    while (ctx->pos < target) {
        advance_pos(ctx);
    }
    return found;
}

/* Helper para extrair e alocar lexemas da source com segurança */
static char* xstrndup(ParserContext* ctx, char* start, size_t len) {
    char* buf = calloc(len + 1, sizeof(char));
    if (!buf) {
        error_add(ctx->errors, SERR_MEMORY_ALLOC);
        return NULL;
    }
    memcpy(buf, start, len);
    return buf;
}

/* parse_string: returns NODE_STRING (position set inside) or NULL */
static Node* parse_string (ParserContext* ctx) {
    skip_ws(ctx);
    if (*ctx->pos != '\'' && *ctx->pos != '"') return NULL;

    /* Save opening quote position for node metadata */
    const char* start_ptr = ctx->pos;
    size_t start_line = ctx->line;
    size_t start_col  = ctx->col;

    char q = *ctx->pos;
    advance_pos(ctx); /* skip opening quote */

    /* Count length first*/
    const char* p = ctx->pos;
    size_t len = 0;
    while (*p && *p != q) {
        if (*p == '\\' && *(p + 1)) {
            p += 2;
            len++;
        } else {
            p++;
            len++;
        }
    }

    if (!*p) {
        error_add_ctx(ctx->errors, PERR_UNTERMINATED_STRING, start_line, start_col, len, NULL);
        return NULL;
    }

    char* buf = calloc(len + 1, sizeof(char));
    if (!buf) {
        error_add(ctx->errors,SERR_MEMORY_ALLOC);
        return NULL;
    }

    char* w = buf;
    while (*ctx->pos && *ctx->pos != q) {
        if (*ctx->pos == '\\' && *(ctx->pos + 1)) {
            advance_pos(ctx); /* skip backslash */
            switch (*ctx->pos) {
                case 'n': *w++ = '\n'; break;
                case 't': *w++ = '\t'; break;
                case 'r': *w++ = '\r'; break;
                default: *w++ = *ctx->pos; break;
            }
            advance_pos(ctx);
        } else {
            *w++ = *ctx->pos;
            advance_pos(ctx);
        }
    }

    if (*ctx->pos == q) advance_pos(ctx); /* skip closing quote */

    Node* n = node_new_pos(NODE_STRING, buf, start_ptr, start_line, start_col);
    node_set_len(n, (ctx->pos - start_ptr));
    free(buf); buf = NULL;
    if (!n) return NULL;

    return n;
}

/* parse_number: returns NODE_NUMBER with position set inside, or NULL */
static Node* parse_number(ParserContext* ctx) {
    skip_ws(ctx);

    const char* start = ctx->pos;
    size_t start_line = ctx->line;
    size_t start_col = ctx->col;

    if (*ctx->pos == '-') advance_pos(ctx);

    if (!isdigit((unsigned char)*ctx->pos)) {
        ctx->pos = start;
        ctx->line = start_line;
        ctx->col = start_col;
        return NULL;
    }

    /* Consome a parte inteira */
    while (isdigit((unsigned char)*ctx->pos)) advance_pos(ctx);

    /* Consome a parte fracionária, se existir */
    if (*ctx->pos == '.') {
        advance_pos(ctx);
        if (!isdigit((unsigned char)*ctx->pos)) {
            error_add_ctx(ctx->errors, PERR_EXPECT_NUMBER, ctx->line, ctx->col, 1, NULL, *ctx->pos);
            return NULL;
        }
        while (isdigit((unsigned char)*ctx->pos)) advance_pos(ctx);
    }
    size_t len = ctx->pos - start;
    
    char* buf = xstrndup(ctx, (char*)start, len);
    if (!buf) return NULL;

    Node* n = node_new_pos(NODE_NUMBER, buf, start, start_line, start_col);
    free(buf);
    
    return n;
}

/* parse_identifier: returns allocated char* (caller must free) or NULL */
static char* parse_identifier (ParserContext* ctx) {
    skip_ws(ctx);
    if (!isalpha((unsigned char)*ctx->pos) && *ctx->pos != '_') return NULL;

    const char* start = ctx->pos;
    size_t start_line = ctx->line;
    size_t start_col = ctx->col;

    while (is_identifier(*ctx->pos)) advance_pos(ctx);

    size_t len = ctx->pos - start;
    char* buf = xstrndup(ctx, (char*)start, len);
    
    if (!buf) {
        /* rollback on allocation failure */
        ctx->pos = start;
        ctx->line = start_line;
        ctx->col = start_col;
        return NULL; /* O erro já foi adicionado pelo xstrndup */
    }
    
    return buf;
}

/* parse_keywords helper: original behavior, frees id inside on success.
 * Caller should set position on returned node (primary).
 */
static Node* parse_keyword(char* id) {
    if (!id) return NULL;
    Node* n = NULL;
    if (!strcmp(id, "null")) {
        n = node_new(NODE_NULL, NULL);
        n->len = 4;
        return n;
    }
    if (!strcmp(id, "true") || !strcmp(id, "false")) {
        n = node_new(NODE_BOOL, id);
        return n;
    }
    return NULL;
}

/* parse_args centralizes parsing of comma-separated arguments/items.
 * - parent: node to which parsed args will be appended (via node_add)
 * - closing: closing character to expect (']' for arrays, ')' for function calls)
 * - eof_err: ErrorType to report if EOF encountered before closing
 * - arg_parser: parser function for a single argument. If NULL, primary() is used.
 */
/* parse_args centraliza o parsing de argumentos separados por vírgula. */
static bool parse_args(ParserContext* ctx, Node* parent,
                       char closing, ErrorType eof_err,
                       ParserFn arg_parser) {
    skip_ws(ctx);
    /* Argumentos vazios: ex. [] ou () */
    if (expect(ctx, closing)) return true;

    do {
        /* EOF prematuro antes da leitura do conteúdo */
        if (!*ctx->pos) {
            error_add_ctx(ctx->errors, eof_err, ctx->line, ctx->col, 1, NULL);
            return false;
        }

        /* Resolve o parser (customizado ou primário) e avalia o nó */
        Node* arg = arg_parser ? arg_parser(ctx) : primary(ctx);
        if (!arg || !node_add(parent, arg)) {
            return false;
        }

        skip_ws(ctx);
        
        /* Encontrando o fecho, a lista é terminada com sucesso */
        if (expect(ctx, closing)) return true;

        /* O loop continua estritamente se houver uma vírgula */
    } while (expect(ctx, ','));

    /* Se não encontrou o fecho nem a vírgula de separação, falha de sintaxe */
    error_add_ctx(ctx->errors, PERR_EXPECTED_COMMA_OR_OTHER, ctx->line, ctx->col, 1, NULL, closing);
    return false;
}

/* parse_array: sets node position internally (position at '[') */
static Node* parse_array (ParserContext* ctx) {
    skip_ws(ctx);

    const char* start = ctx->pos;
    size_t start_line = ctx->line;
    size_t start_col = ctx->col;

    /* consume '[' */
    if (!expect(ctx, '[')) return NULL;

    Node* arr = node_new_pos(NODE_LIST, NULL, start, start_line, start_col);
    if (!arr) {
        error_add(ctx->errors, SERR_MEMORY_ALLOC);
        return NULL;
    }

    if (!parse_args(ctx, arr, ']', PERR_UNTERMINATED_ARRAY, expr)) {
        node_free(&arr);
        return NULL;
    }
    node_set_len(arr,  (size_t)(ctx->pos - start));
    return arr;
}

static Node* parse_function(ParserContext* ctx, char* id) {
    skip_ws(ctx);

    if (!expect(ctx, '(')) return NULL;

    Node* fn = node_new(NODE_FUNCTION, id);
    if (!fn) {
        error_add(ctx->errors, SERR_MEMORY_ALLOC);
        return NULL;
    }

    if (!parse_args(ctx, fn, ')', PERR_UNTERMINATED_FUNCTION, expr)) {
        node_free(&fn);
        return NULL;
    }
    return fn;
}


/* primary: O ponto de entrada principal para literais e identificadores. */
static Node* primary(ParserContext* ctx) {
    skip_ws(ctx);
    if (!*ctx->pos) {
        error_add_ctx(ctx->errors, PERR_UNEXPECTED_EOF, ctx->line, ctx->col, 1, NULL);
        return NULL;
    }

    /* 1. Avaliação de Literais (Early returns) */
    Node* n = parse_string(ctx);
    if (n || ctx->has_error) return n;

    n = parse_number(ctx);
    if (n || ctx->has_error) return n;

    if (*ctx->pos == '[') {
        return parse_array(ctx);
    }

    /* 2. Tratamento Unificado de Identificadores */
    const char* id_start = ctx->pos;
    size_t id_line = ctx->line;
    size_t id_col  = ctx->col;

    char* id = parse_identifier(ctx);
    if (!id) {
        error_add_ctx(ctx->errors, PERR_UNEXPECTED_CHAR, ctx->line, ctx->col, 1, NULL, *ctx->pos);
        return NULL;
    }

    /* Tentativa de capturar palavras reservadas ou funções */
    n = parse_keyword(id);
    
    skip_ws(ctx);
    if (!n && *ctx->pos == '(') {
        n = parse_function(ctx, id);
    }

    /* Se for resolvido, aplicar meta-dados e limpar buffer temporário */
    if (n) {
        node_set_pos(n, id_start, id_line, id_col);
        free(id);
        return n;
    }

    /* Intercetar erros ocorridos durante parse_function para não vazar a string 'id' */
    if (ctx->has_error) {
        free(id);
        return NULL;
    }

    /* 3. Fallbacks: Identificador de Objeto Regular. */
    n = node_new_pos(NODE_OBJECT, id, id_start, id_line, id_col);
    free(id); /* Free the temporary buffer since the node copied it */
    return n;
}

/* Parse property access and multi-access */
static Node* parse_access(ParserContext* ctx) {
    Node* obj = primary(ctx);
    if (!obj) return NULL;
 
    skip_ws(ctx);
    if (*ctx->pos != '.') return obj;
 
    /* Build a NODE_ACCESS node to hold the chain.
     * children[0] is the lhs; subsequent children are the access steps. */
    Node* access = node_new_pos(NODE_ACCESS, NULL, obj->start, obj->line, obj->col);
    if (!access) goto cleanup;
 
    if (!node_add(access, obj)) goto cleanup;
    obj = NULL; /* access owns it now */
 
    while (expect(ctx, '.')) {
        skip_ws(ctx);
        const char* mstart = ctx->pos;
        size_t mline = ctx->line;
        size_t mcol  = ctx->col;
 
        /* Multi-access: .[prop1, prop2, ...] */
        if (expect(ctx, '[')) {
            Node* multi_list = node_new_pos(NODE_MULTI_ACCESS, NULL, mstart, mline, mcol);
            if (!multi_list) goto cleanup;
 
            if (!parse_args(ctx, multi_list, ']', PERR_UNTERMINATED_MULTI_ACCESS, expr)) {
                goto cleanup;
            }
            node_set_len(multi_list,  (size_t)(ctx->pos - mstart));
            if (!node_add(access, multi_list)) goto cleanup;
            
            node_set_len(access,  (size_t)(ctx->pos - access->start));
            return access; /* multi-access encerra a cadeia */
        }
 
        /* Regular property access */
        Node* right = primary(ctx);
        if (!right) {
            if (!ctx->has_error) {
                error_add_ctx(ctx->errors, VERR_EXPECTED_PROPERTY_OR_MULTI_ACCESS, ctx->line, ctx->col, 1, NULL);
            }
            goto cleanup;
        }
 
        if (!node_add(access, right)) goto cleanup;
    }
    node_set_len(access, (size_t)(ctx->pos - access->start));
    return access;
 
/* Single point of exit for failures */
cleanup:
    node_free(&obj);
    node_free(&access);
    return NULL;
}

/* Main expression parser */
static Node* expr(ParserContext* ctx) {
    return parse_access(ctx);
}

/* Public API for single expression parsing */
Node* parse_xpression(XpressionContext* xpr) {
    if (!xpr->source || !*xpr->source) return NULL;

    ParserContext ctx = (ParserContext){
        .src  = xpr->source,
        .pos  = xpr->source,
        .errors = &xpr->errors,
        .line = 1,
        .col  = 1
    };

    Node* root = node_new_pos(NODE_LIST, "root", ctx.src, 1, 1);
    if (!root) return NULL;

    Node* node = NULL; 

    while (*ctx.pos) {
        const char* start = find_next_placeholder(&ctx);
        if (!start) break; /* Saída natural de sucesso (ou aviso) */

        size_t content_line = ctx.line;
        size_t content_col  = ctx.col;

        skip_ws(&ctx);
        if (*ctx.pos == '}') {
            error_add_ctx(ctx.errors, PERR_EMPTY_PLACEHOLDER, content_line, content_col-1,(size_t)(ctx.pos - ctx.src), NULL);
            goto cleanup;
        }

        node = expr(&ctx);
        if (!node) goto cleanup;

        skip_ws(&ctx);
        if (*ctx.pos && *ctx.pos != '}') {
            error_add_ctx(ctx.errors, PERR_UNEXPECTED_CHAR, ctx.line, ctx.col, 1, NULL, *ctx.pos);
            goto cleanup;
        }

        if (!expect(&ctx, '}')) {
            error_add_ctx(ctx.errors, PERR_UNTERMINATED_PLACEHOLDER, ctx.line, ctx.col, 1, NULL );
            goto cleanup;
        }

        if (!node_add(root, node)) goto cleanup;
        
        /* Reset para evitar double-free no cleanup caso uma iteração futura falhe */
        node = NULL; 
    }
    return root;

/* Single point of failure */
cleanup:
    node_free(&node); /* node_free já deve lidar de forma segura com ponteiros nulos */
    node_free(&root);
    return NULL;
}

Node* parse_xpression_ex(XpressionContext* xpr) {
    Node* root = parse_xpression(xpr);
    //Semantic validation
    if(!root) return NULL;

    if(!validate_xpression(root,xpr->eval_ctx)){
        node_free(&root);
        return NULL;
    }
    return root;
}