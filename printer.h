#ifndef PRINTER_H
#define PRINTER_H

#include "value.h"
#include "ast.h"

void print_json(Node *n, Value *v, int grouped, int is_last);
void print_xml(Node *n, Value *v, int grouped);

#endif
