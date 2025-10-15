#include "printer.h"
#include <stdio.h>
#include <stdlib.h>

static void indent_print(int n) { 
	for (int i=0;i<n;i++) putchar(' ');
}

static void print_json_string_esc(const char *s) {
    putchar('"');
    for (const unsigned char *p = (const unsigned char*)s; *p; ++p) {
        unsigned char c = *p;
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

/* recursive JSON-like AST printer */
static void print_json_node_rec(Node *n, int indent) {
    
	indent_print(indent);
	printf("{\n");
    indent_print(indent+2);
	printf("\"type\": ");

    switch (n->kind) {
        case NODE_OBJECT: printf("\"object\""); break;
        case NODE_PROPERTY: printf("\"property\""); break;
        case NODE_FUNCTION: printf("\"function\""); break;
        case NODE_LIST: printf("\"list\""); break;
        case NODE_STRING: printf("\"string\""); break;
        case NODE_NUMBER: printf("\"number\""); break;
		case NODE_BOOL: printf("\"boolean\""); break;
        case NODE_IDENTIFIER: printf("\"identifier\""); break;
    }
    if (n->value) {
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

void print_json(Node *n, Value *v, int grouped,int is_last) {
   
	int indent = 0;
    if (grouped) {
		indent = 2; 
	}
	print_json_node_rec(n, indent);

    printf((grouped && !is_last)?",\n":"\n");
    if (v) {
        char *s = value_to_string(v);
		indent_print(indent);
        printf("\"EVALUATED\": ");
        print_json_string_esc(s);
	    printf((grouped && !is_last)?",\n":"\n");

        free(s);
		s = NULL;
    }
}

/* XML printer */
static void print_xml_node_rec(Node *n, int indent) {
    indent_print(indent);
    const char *tag = "node";
    switch (n->kind) {
        case NODE_OBJECT: tag = "object"; break;
        case NODE_PROPERTY: tag = "property"; break;
        case NODE_FUNCTION: tag = "function"; break;
        case NODE_LIST: tag = "list"; break;
        case NODE_STRING: tag = "string"; break;
        case NODE_NUMBER: tag = "number"; break;
        case NODE_BOOL: tag = "boolean"; break;
        case NODE_IDENTIFIER: tag = "identifier"; break;
    }
    printf("<%s", tag);
    if (n->value) {
        printf(" value=\"");
        for (const unsigned char *p=(const unsigned char*)n->value; *p; ++p) {
            if (*p == '&') printf("&amp;");
            else if (*p == '"') printf("&quot;");
            else putchar(*p);
        }
        printf("\"");
    }
    if (n->n_children == 0) { printf(" />\n"); return; }
    printf(">\n");
    for (size_t i=0;i<n->n_children;i++) print_xml_node_rec(n->children[i], indent+2);
    indent_print(indent); printf("</%s>\n", tag);
}

void print_xml(Node *n, Value *v, int grouped) {
    //(void)grouped;
	int indent = 0;
	if (grouped) {
		indent = 2;
	}
    print_xml_node_rec(n, indent);
	indent_print(indent);
    if (v) {
        char *s = value_to_string(v);
        printf("<evaluated>");
        for (char *p = s; *p; ++p) {
            if (*p == '&') printf("&amp;"); else putchar(*p);
        }
        printf("</evaluated>\n");
        free(s);
		s = NULL;
    }
}