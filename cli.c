#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-json|-xml] [-group|-g] [-eval] [-f file] \"<text>\"\n", prog);
    fprintf(stderr, "  -json (default) or -xml must come first if present\n");
    fprintf(stderr, "  -group must appear after format flag (if used)\n");
}

static char *read_file_all(const char *fn) {
    FILE *f = fopen(fn,"rb"); if (!f) return NULL;
    fseek(f,0,SEEK_END); long sz = ftell(f); rewind(f);
    char *buf = malloc(sz+1); if (!buf) { fclose(f); return NULL; }
    size_t got = fread(buf,1,sz,f); buf[got]=0; fclose(f); return buf;
}

CLIOptions parse_cli(int argc, char **argv) {

	
    CLIOptions opt = { .json = 1, .xml = 0, .grouped = 0, .eval = 0, .filename = NULL, .expr = NULL };
    int i = 1;
	
	if (argc < 2) { print_usage(argv[0]); return opt; }

	
    // format flags (if present) must come first
    if (i < argc && strcmp(argv[i], "-xml") == 0) { opt.json = 0; opt.xml = 1; i++; }
    else if (i < argc && strcmp(argv[i], "-json") == 0) { opt.json = 1; opt.xml = 0; i++; }

    if (i < argc && (strcmp(argv[i], "-group") == 0 || strcmp(argv[i], "-g") == 0)) { opt.grouped = 1; i++; }

    if (i < argc && strcmp(argv[i], "-eval") == 0) { opt.eval = 1; i++; }

    if (i >= argc){ return opt;}
	
	
    if (strcmp(argv[i], "-f") == 0) {
        if (i + 1 < argc) opt.filename = argv[i+1];
        
		if (i+1 >= argc) { fprintf(stderr,"Missing file after -f\n"); return opt; }
        opt.expr = read_file_all(opt.filename);
        if (!opt.expr) { fprintf(stderr,"Failed to read %s\n", argv[i+1]); return opt;}
		i += 2;
    } else {
        // remaining args compose the expression/text
        size_t total = 0;
        for (int j=i;j<argc;j++){ 
			total += strlen(argv[j]) + 1;
		}
        char *buf = malloc(total + 1);
        buf[0] = '\0';
        for (int j=i;j<argc;j++) {
            strcat(buf, argv[j]);
            if (j + 1 < argc) strcat(buf, " ");
        }
        opt.expr = buf;
    }

    return opt;
}
