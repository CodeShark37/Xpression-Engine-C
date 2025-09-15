#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static const char *P; // parser cursor

static void skip_ws(void) { while (*P && isspace((unsigned char)*P)) P++; }
static int is_ident_char(char c) { return isalnum((unsigned char)c) || c=='_' || c=='-'; }

static Node *parse_expr(void); // forward

static char *parse_ident_str(void) {
    skip_ws();
    const char *s = P;
    if (!is_ident_char(*s)) return NULL;
    while (is_ident_char(*P)) P++;
    size_t L = P - s;
    char *r = malloc(L+1); memcpy(r,s,L); r[L]=0; return r;
}

static char *parse_string_literal(void) {
    skip_ws();
    if (*P!='\'' && *P!='"') return NULL;
    char q = *P++;
    const char *s=P;
    while (*P && *P!=q) {
        if (*P=='\\' && P[1]) P+=2;
        else P++;
    }
    size_t L = P - s;
    char *tmp = malloc(L+1);
    // simple unescape
    size_t wi=0;
    for (size_t i=0;i<L;i++) {
        char c = s[i];
        if (c=='\\' && i+1 < L) {
            char nx = s[i+1];
            if (nx=='n') tmp[wi++] = '\n';
            else if (nx=='t') tmp[wi++] = '\t';
            else tmp[wi++] = nx;
            i++;
        } else tmp[wi++] = c;
    }
    tmp[wi]=0;
    if (*P==q) P++;
    return tmp;
}

static char *parse_number_str(void) {
    skip_ws();
    const char *s=P;
    if (!(*P=='-' || isdigit((unsigned char)*P))) return NULL;
    if (*P=='-') P++;
    int any = 0;
    while (isdigit((unsigned char)*P)) { P++; any=1; }
    if (*P == '.') {
        P++;
        while (isdigit((unsigned char)*P)) { P++; any=1; }
    }
    if (!any) return NULL;
    size_t L = P - s;
    char *r = malloc(L+1); memcpy(r,s,L); r[L]=0; return r;
}

static Node *parse_list(void) {
    skip_ws();
    if (*P != '[') return NULL;
    P++; // skip [
    Node *n = node_new(NODE_LIST, NULL);
    while (1) {
        skip_ws();
        if (*P == ']') { P++; break; }
        Node *it = parse_expr();
        if (!it) {
            // skip broken token until comma or ]
            while (*P && *P!=',' && *P!=']') P++;
            if (*P==',') { P++; continue; }
            if (*P==']') { P++; break; }
            break;
        }
        node_add(n, it);
        skip_ws();
        if (*P == ',') { P++; continue; }
        if (*P == ']') { P++; break; }
    }
    return n;
}

static Node *parse_primary(void) {
    skip_ws();
    if (!*P) return NULL;
    // string
    if (*P=='\'' || *P=='"') {
        char *s = parse_string_literal();
        if (!s) return NULL;
        Node *n = node_new(NODE_STRING, s);
        free(s);
        return n;
    }
    // number
    char *num = parse_number_str();
    if (num) {
        Node *n = node_new(NODE_NUMBER, num);
        free(num);
        return n;
    }
    // list
    if (*P == '[') return parse_list();
    // identifier or function
    char *id = parse_ident_str();
    if (!id) return NULL;
    skip_ws();
    if (*P == '(') {
        // function
        P++; // skip '('
        Node *fn = node_new(NODE_FUNCTION, id);
        free(id);
        skip_ws();
        if (*P == ')') { P++; return fn; }
        while (1) {
            Node *arg = parse_expr();
            if (!arg) {
                // try to recover
                skip_ws();
                if (*P==',') { P++; continue; }
                break;
            }
            node_add(fn, arg);
            skip_ws();
            if (*P == ',') { P++; continue; }
            if (*P == ')') { P++; break; }
        }
        return fn;
    }
    Node *nid = node_new(NODE_IDENTIFIER, id);
    free(id);
    return nid;
}

// parse expression with dot chaining: primary ('.' primary)*
static Node *parse_expr(void) {
    Node *first = parse_primary();
    if (!first) return NULL;
    skip_ws();
    if (*P != '.') return first;

    Node *root = NULL;
    if (first->kind == NODE_IDENTIFIER) {
        root = node_new(NODE_OBJECT, first->value);
        node_free(first);
    } else {
        root = node_new(NODE_OBJECT, NULL);
        node_add(root, first);
    }
    Node *cur = root;
    while (*P == '.') {
        P++; skip_ws();
        Node *seg = parse_primary();
        if (!seg) break;
        if (seg->kind == NODE_IDENTIFIER) {
            Node *prop = node_new(NODE_PROPERTY, seg->value);
            node_free(seg);
            node_add(cur, prop);
            cur = prop;
        } else {
            node_add(cur, seg);
            cur = seg;
        }
        skip_ws();
    }
    return root;
}

/* wrapper */
Node *parse_expression_str(const char *s) {
    const char *saveP = P;
    P = s;
    Node *r = parse_expr();
    P = saveP;
    return r;
}

int extract_placeholders(const char *text, char ***out, size_t *out_count) {
    const char *p = text;
    size_t cap = 8, cnt = 0;
    char **arr = malloc(sizeof(char*)*cap);
    while (1) {
        const char *open = strstr(p, "${");
        if (!open) break;
        const char *q = open + 2;
        int depth = 0;
        while (*q) {
            if (*q == '{') depth++;
            else if (*q == '}') {
                if (depth == 0) break;
                depth--;
            }
            q++;
        }
        if (!*q) { for (size_t i=0;i<cnt;i++) free(arr[i]); free(arr); return -1; }
        size_t len = q - (open+2);
        char *tmp = malloc(len+1); memcpy(tmp, open+2, len); tmp[len]=0;
        // trim
        size_t a=0; while (a<len && isspace((unsigned char)tmp[a])) a++;
        size_t b=len; while (b>a && isspace((unsigned char)tmp[b-1])) b--;
        char *trim = malloc(b-a+1); memcpy(trim, tmp+a, b-a); trim[b-a]=0; free(tmp);
        if (cnt+1 > cap) { cap*=2; arr = realloc(arr, sizeof(char*)*cap); }
        arr[cnt++] = trim;
        p = q + 1;
    }
    *out = arr; *out_count = cnt; return 0;
}
