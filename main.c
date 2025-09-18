#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "context.h"
#include "cli.h"


static CtxNode *build_sample_context(void) {
    CtxNode *root = ctx_new("root");

    CtxNode *keyword = ctx_new("KEYWORD");
	ctx_add_child(root, keyword);
    ctx_set_prop(keyword, "FUNCTION", val_str("FUNCTION_VALUE"));
	ctx_set_prop(keyword, "FUNCTION2", val_str("FUNCTION_VALUE2"));


    CtxNode *config = ctx_new("CONFIG");
	ctx_add_child(root, config);
    CtxNode *db = ctx_new("DB");
	ctx_add_child(config, db);
    CtxNode *user = ctx_new("USER");
	ctx_add_child(db, user);
    ctx_set_prop(user, "NAME", val_str("db_user"));

    CtxNode *system = ctx_new("SYSTEM");
	ctx_add_child(root, system);
    CtxNode *syscfg = ctx_new("CONFIG");
	ctx_add_child(system, syscfg);
    CtxNode *network = ctx_new("NETWORK");
	ctx_add_child(syscfg, network);
    CtxNode *proxy = ctx_new("PROXY");
	ctx_add_child(network, proxy);
    ctx_set_prop(proxy, "HOST", val_str("10.0.0.1"));

    CtxNode *rule = ctx_new("RULE");
	ctx_add_child(root, rule);

    ctx_set_prop(root, "IDENTIFIER", val_str("X123"));

    ctx_set_prop(root, "A", val_num(10));
    ctx_set_prop(root, "B", val_num(20));
    ctx_set_prop(root, "C", val_num(5));
    ctx_set_prop(root, "D", val_num(15));
	ctx_set_prop(root, "PARAMETERS_LIST", val_str("char*,int"));
	ctx_set_prop(root, "STATEMENTS", val_str("return 0;"));

    return root;
}


int main(int argc, char **argv) {
    CLIOptions opts = parse_cli(argc, argv);
	    
	if (!opts.expr) return 1;
    
	register_builtins_default();
	CtxNode *ctx = build_sample_context();
    process_expressions(&opts, ctx);
    
	ctx_free(ctx);
	free_funcs();
    return 0;
}