/**
	XPression Engine C (c)2025

	@author: Joshua (Codeshark37)
	@contact: labprogramming7@gmail.com

	printer.c
*/

#include "printer.h"

#include <stdio.h>
#include <stdlib.h>

static void indent_print(size_t n) {
	while(n--) putchar(' ');
}

static void print_json_string_esc(const char* s) {
    putchar('"');
    for (const char* p = (const char*)s; *p; ++p) {
        char c = *p;
        switch (c) {
            case '\\': printf("\\\\"); break;
            case '"': printf("\\\""); break;
            case '\n': printf("\\n"); break;
            case '\r': printf("\\r"); break;
            case '\t': printf("\\t"); break;
            default:
                if (c < 0x20) printf("\\u%04x", c);
                else putchar(c);
        }
    }
    putchar('"');
}

static void print_group_boundary(CLIOptions opts, bool is_start) {

    if (!opts.group) return;

    if (opts.format == FORMAT_XML) {
        printf(is_start ? "<expressions>\n" : "</expressions>\n");
    }else{
		//format last json ast
		if(!is_start && opts.format == FORMAT_JSON)
			putchar('\n');
        printf(is_start ? "[\n" : "]\n");
	}
}

/* recursive JSON-like AST printer */
static void print_json_node_rec(Node* n, size_t indent) {

	indent_print(indent);
	printf("{\n");
    indent_print(indent+2);
	printf("\"type\": ");

    switch (n->kind) {
        case NODE_OBJECT: printf("\"object\""); break;
        case NODE_FUNCTION: printf("\"function\""); break;
        case NODE_LIST: printf("\"list\""); break;
        case NODE_STRING: printf("\"string\""); break;
        case NODE_NUMBER: printf("\"number\""); break;
		case NODE_BOOL: printf("\"boolean\""); break;
        case NODE_ACCESS: printf("\"access\""); break;
        case NODE_MULTI_ACCESS: printf("\"multi_access\""); break;
        default : break;

    }
    if (n->value && *n->value) {
        printf(",\n");
		indent_print(indent+2);
		printf("\"value\": ");
        print_json_string_esc(n->value);
    }
    if (n->n_children) {
        printf(",\n");
		indent_print(indent+2);
		printf("\"children\": [\n");
        for (size_t i=0;i<n->n_children;i++) {
            print_json_node_rec(n->children[i], indent+4);
            if (i+1 < n->n_children) printf(",\n");
			else putchar('\n');
        }
        indent_print(indent+2);
		putchar(']');
    }
    putchar('\n');
	indent_print(indent);
	putchar('}');
}

// Escapa caracteres especiais para XML
static void print_xml_esc(const char* str) {
    if (!str) return;
    for (const char* p = str; *p; p++) {
        switch (*p) {
            case '&':  printf("&amp;");  break;
            case '<':  printf("&lt;");   break;
            case '>':  printf("&gt;");   break;
            case '\'': printf("&apos;"); break;
            case '"':  printf("&quot;"); break;
            default:   putchar(*p);      break;
        }
    }
}

/* XML printer */
static void print_xml_node_rec(Node* n, size_t indent) {
    indent_print(indent);
    const char* tag = "node";
    switch (n->kind) {
        case NODE_OBJECT: tag = "object"; break;
        case NODE_FUNCTION: tag = "function"; break;
        case NODE_LIST: tag = "list"; break;
        case NODE_STRING: tag = "string"; break;
        case NODE_NUMBER: tag = "number"; break;
        case NODE_BOOL: tag = "boolean"; break;
        case NODE_NULL: tag = "null"; break;
        case NODE_ACCESS: tag = "access"; break;
        case NODE_MULTI_ACCESS: tag = "multi_access"; break;
        default: break;
    }
    printf("<%s", tag);
    if (n->value && *n->value) {
        printf(" value=\"");
        print_xml_esc((const char*)n->value);
        printf("\"");
    }
    if (n->n_children == 0) { printf(" />\n"); return; }
    printf(">\n");
    for (size_t i=0;i<n->n_children;i++) print_xml_node_rec(n->children[i], indent+2);
    indent_print(indent);
	printf("</%s>\n", tag);
}

static void print_plain(Value result, CLIOptions opts, int is_first) {
    char* output = value_to_string(result);
    if (output) {
        if (opts.group && !is_first) printf("  ,%s\n", output);
        else printf("  %s\n", output);
        free(output);
    }
}

static void print_json(Node* n, Value v, CLIOptions opts, bool is_first) {
    size_t indent = opts.group ? 2 : 0;
    if (opts.group && !is_first) printf(",\n");
    print_json_node_rec(n, indent);

    if (opts.eval) {
        char* s = value_to_string(v);
        printf(",\n");
        indent_print(indent);
        printf("\"EVALUATED\": ");
        print_json_string_esc(s);
        free(s);
    }
}

static void print_xml(Node* n, Value v, CLIOptions opts) {
    size_t indent = opts.group ? 2 : 0;
    print_xml_node_rec(n, indent);

    if (opts.eval) {
        char* s = value_to_string(v);
        indent_print(indent);
        printf("<evaluated>");
        print_xml_esc(s);
        printf("</evaluated>\n");
        free(s);
    }
}

void print_xpression(XpressionContext* xpr) {
    Node* root = xpr->ast;
    CtxNode* eval_ctx = xpr->eval_ctx;
    CLIOptions opts = xpr->cli;
    
    if (!root || root->kind != NODE_LIST) return;

    bool do_eval = opts.eval;

    print_group_boundary(opts, true);

    for (size_t i = 0; i < root->n_children; i++) {
        Node* ast = root->children[i];
        bool is_first = (i == 0);

        Value result = do_eval ? eval_xpression(xpr) : val_null();

        if (opts.format == FORMAT_JSON) {
            print_json(ast, result, opts, is_first);
        } else if (opts.format == FORMAT_XML) {
            print_xml(ast, result, opts);
        } else {
            if (do_eval)
                print_plain(result, opts, is_first);
        }
        val_free(&result);
    }
    print_group_boundary(opts, false);
}