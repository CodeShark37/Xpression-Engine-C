#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>


static Parser ctx = {0};

// Error handling
static void error(const char *fmt, ...) {
    if (ctx.has_error) return;
    ctx.has_error = 1;
    
    va_list args;
    va_start(args, fmt);
    snprintf(ctx.error_msg, sizeof(ctx.error_msg), "L%d:C%d: ", ctx.line, ctx.col);
    vsnprintf(ctx.error_msg + strlen(ctx.error_msg), sizeof(ctx.error_msg) - strlen(ctx.error_msg), fmt, args);
    va_end(args);
}

static void update_pos() {
    ctx.line = ctx.col = 1;
    for (const char *p = ctx.src; p < ctx.pos; p++) {
        if (*p == '\n') { ctx.line++; ctx.col = 1; }
        else ctx.col++;
    }
}

// Parsing utilities
static void skip_ws() { 
    while (*ctx.pos && isspace(*ctx.pos)) ctx.pos++; 
    update_pos();
}

static int is_identifier(char c) {
	return isalnum(c) || c == '_'; 
}

static int expect(char c) { 
    skip_ws();
    return *ctx.pos == c ? (ctx.pos++, update_pos(), 1) : 0; 
}

static Node *expr(); // forward

// Parse string literal
static char *parse_string() {
    skip_ws();
    if (*ctx.pos != '\'' && *ctx.pos != '"') return NULL;
    
    char q = *ctx.pos++;
    const char *start = ctx.pos;
    
    while (*ctx.pos && *ctx.pos != q) {
        ctx.pos += (*ctx.pos == '\\' && ctx.pos[1]) ? 2 : 1;
    }
    
    if (!*ctx.pos) { error("unterminated string"); return NULL; }
    
    size_t len = ctx.pos - start;
    char *buf = malloc(len + 1), *w = buf;
    if (!buf) { error("memory allocation failed"); return NULL; }
    
    for (const char *r = start; r < ctx.pos; r++) {
        if (*r == '\\' && r + 1 < ctx.pos) {
            switch (*++r) {
                case 'n': *w++ = '\n'; break;
                case 't': *w++ = '\t'; break;
                case 'r': *w++ = '\r'; break;
                default: *w++ = *r; break;
            }
        } else *w++ = *r;
    }
    *w = 0;
    ctx.pos++; // skip closing quote
    update_pos();
    return buf;
}

// Parse number
static char *parse_number() {
    skip_ws();
    const char *start = ctx.pos;
    
    if (*ctx.pos == '-') ctx.pos++;
    if (!isdigit(*ctx.pos)) return NULL;
    
    while (isdigit(*ctx.pos)) ctx.pos++;
    if (*ctx.pos == '.') {
        ctx.pos++;
        while (isdigit(*ctx.pos)) ctx.pos++;
    }
    
    size_t len = ctx.pos - start;
    char *buf = malloc(len + 1);
    if (!buf) { error("memory allocation failed"); return NULL; }
    memcpy(buf, start, len);
    buf[len] = 0;
    update_pos();
    return buf;
}

// Parse identifier
static char *parse_identifier() {
    skip_ws();
    if (!isalpha(*ctx.pos) && *ctx.pos != '_') return NULL;
    
    const char *start = ctx.pos;
    while (is_identifier(*ctx.pos)) ctx.pos++;
    
    size_t len = ctx.pos - start;
    char *buf = malloc(len + 1);
    if (!buf) { error("memory allocation failed"); return NULL; }
    memcpy(buf, start, len);
    buf[len] = 0;
    update_pos();
    return buf;
}

// Parse array [item1, item2, ...]
static Node *parse_array() {
    skip_ws();
    if (!expect('[')) return NULL;
    
    Node *arr = node_new(NODE_LIST, NULL);
    if (!arr) { error("memory allocation failed"); return NULL; }
    
    while (1) {
        skip_ws();
        if (expect(']')) break;
        if (!*ctx.pos) { error("unterminated array"); goto fail; }
        
        Node *item = expr();
        if (item) {
            node_add(arr, item);
        } else if (ctx.has_error) {
            goto fail;
        } else {
            // Skip invalid token
            while (*ctx.pos && *ctx.pos != ',' && *ctx.pos != ']') ctx.pos++;
        }
        
        skip_ws();
        if (expect(',')) continue;
        if (expect(']')) break;
        
        error("expected ',' or ']' in array");
        goto fail;
    }
    return arr;
    
	fail:
		node_free(arr);
		return NULL;
}

// Parse primary expression
static Node *primary() {
    skip_ws();
    if (!*ctx.pos) { error("unexpected EOF"); return NULL; }
    
    // String literal
    char *str = parse_string();
    if (str) {
        Node *n = node_new(NODE_STRING, str);
        free(str);
        return n;
    }
    
    // Number
    char *num = parse_number();
    if (num) {
        Node *n = node_new(NODE_NUMBER, num);
        free(num);
        return n;
    }
    
    // Array
    if (*ctx.pos == '[') return parse_array();
    
    // Identifier/boolean/function
    char *id = parse_identifier();
    if (!id) { error("unexpected '%c'", *ctx.pos); return NULL; }
    
    // Boolean check
    if (!strcasecmp(id, "true") || !strcasecmp(id, "false")) {
        Node *n = node_new(NODE_BOOL, id);
        free(id);
        return n;
    }
    
    skip_ws();
    
    // Function call
    if (expect('(')) {
        Node *fn = node_new(NODE_FUNCTION, id);
        free(id);
        if (!fn) { error("memory allocation failed"); return NULL; }
        
        skip_ws();
        if (expect(')')) return fn;
        
        while (1) {
            Node *arg = expr();
            if (arg) {
                node_add(fn, arg);
            } else if (ctx.has_error) {
                node_free(fn);
                return NULL;
            }
            
            skip_ws();
            if (expect(',')) continue;
            if (expect(')')) break;
            
            error("expected ',' or ')' in function call");
            node_free(fn);
            return NULL;
        }
        return fn;
    }
    
    // Regular identifier
    Node *n = node_new(NODE_IDENTIFIER, id);
    free(id);
    return n;
}

// Parse property access and multi-access
static Node *parse_access() {
    Node *left = primary();
    if (!left) return NULL;
    
    skip_ws();
    if (*ctx.pos != '.') return left;
    
    // Create object chain
    Node *obj = (left->kind == NODE_IDENTIFIER) ? 
        node_new(NODE_OBJECT, left->value) : 
        node_new(NODE_OBJECT, NULL);
    
    if (!obj) { 
		error("memory allocation failed");
		node_free(left);
		return NULL;
	}
    
    if (left->kind == NODE_IDENTIFIER) {
        node_free(left);
    } else {
        node_add(obj, left);
    }
	
    Node *cur = obj;
    
    while (expect('.')) {
        skip_ws();
        
        // Multi-access: .[prop1, prop2, ...]
        if (expect('[')) {
            Node *multi_list = node_new(NODE_LIST, NULL);
            if (!multi_list) { error("memory allocation failed"); node_free(obj); return NULL; }
            
            while (1) {
                skip_ws();
                if (expect(']')) break;
                if (!*ctx.pos) { error("unterminated multi-access"); node_free(multi_list); node_free(obj); return NULL; }
                
                //char *prop_name = parse_identifier();
				Node *item = expr();
                if (!item) {
                    error("expected expression name in multi-access");
                    node_free(multi_list); 
                    node_free(obj);
                    return NULL;
                }   
                node_add(multi_list, item);
                
                skip_ws();
                if (expect(',')) continue;
                if (expect(']')) break;
                
                error("expected ',' or ']' in multi-access");
                node_free(multi_list);
                node_free(obj);
                return NULL;
            }
            node_add(cur, multi_list);
            return obj;
        }
        
        // Regular property access
        Node *right = primary();
        if (!right) {
            if (!ctx.has_error) error("expected property after '.'");
            node_free(obj);
            return NULL;
        }
        
        if (right->kind == NODE_IDENTIFIER) {
            Node *prop = node_new(NODE_PROPERTY, right->value);
            node_free(right);
            if (!prop) { error("memory allocation failed"); node_free(obj); return NULL; }
            node_add(cur, prop);
            cur = prop;
        } else {
            node_add(cur, right);
            cur = right;
        }   
        skip_ws();
    }
    return obj;
}

// Main expression parser
static Node *expr() {
    return parse_access();
}

static void free_nodes(char **nodes) {
    if (!nodes) return;
    for (char **p = nodes; *p; p++) {
        free(*p);
    }
    free(nodes);
}

// Extract and parse placeholders from text
PlaceholderResult parse_placeholders(const char *text) {
    
	PlaceholderResult result = { 
		.nodes = NULL, 
		.error_code = 0 
	};

    if (!text) {
        result.error_code = -1;
        return result;
    }

    size_t cap = 8;
    size_t count = 0;
    char **nodes = (char **)malloc(cap * sizeof(char *));
    
	if (!nodes) {
        result.error_code = -1;
        return result;
    }
    nodes[0] = NULL; // Keep NULL for dynamic iteration

    const char *p = text;
    while (1) {
        const char *start = strstr(p, "${");
        if (!start) break;
        
        const char *content_start = start + 2;
        const char *end = content_start;
        int depth = 0;
        
        while (*end) {
            if (*end == '{') depth++;
            else if (*end == '}') {
                if (depth == 0) break;
                depth--;
            }
            end++;
        }
        
        if (!*end) {
            result.error_code = -1;
            free_nodes(nodes);
            return result;
        }

        size_t content_len = end - content_start;
        char *content = (char*)malloc(content_len + 1);
        if (!content) {
            result.error_code = -1;
            free_nodes(nodes);
            return result;
        }
        memcpy(content, content_start, content_len);
        content[content_len] = '\0';
        
        // parsing simulation of content
        int parse_error = 0; // ctx.has_error
        
        if (!parse_error) {
            if (count + 1 >= cap) {
                cap *= 2;
                char **tmp = (char **)realloc(nodes, cap * sizeof(char *));
                if (!tmp) {
                    result.error_code = -1;
                    free(content);
                    free_nodes(nodes);
                    return result;
                }
                nodes = tmp;
            }
            nodes[count++] = content;
            nodes[count] = NULL; //Keep NULL for dynamic iteration
        } else {
            fprintf(stderr, "Parse error in placeholder '${%.*s}'\n", (int)content_len, content);
            free(content);
        }
        
        p = end + 1;
    }
    
    result.nodes = nodes;
    return result;
}

// Public API for single expression parsing
Node *parse_expression_str(const char *source) {
    if (!source) return NULL;
    
    ctx = (Parser){.src = source, .pos = source, .line = 1, .col = 1, .has_error = 0};
    
    Node *result = expr();
	
	 if (result && !ctx.has_error) {
        // Check for unexpected content after valid expression
        skip_ws();
        if (*ctx.pos) {
            error("unexpected content after expression: '%c'", *ctx.pos);
            node_free(result);
            result = NULL;
        }
    }
	
    if (ctx.has_error) {
        fprintf(stderr, "%s\n", ctx.error_msg);
        if (result) { node_free(result); result = NULL; }
    }
    
    return result;
}

const char *parser_get_last_error() {
    return ctx.has_error ? ctx.error_msg : NULL;
}