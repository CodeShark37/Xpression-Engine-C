#ifndef CLI_H
#define CLI_H

typedef struct {
    int json;
    int xml;
    int grouped;
    int eval;
    const char *filename;
    const char *expr;
} CLIOptions;

CLIOptions parse_cli(int argc, char **argv);

#endif
