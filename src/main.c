/**
	XPression Engine C (c)2025

	@author: Joshua (Codeshark37)
	@contact: labprogramming7@gmail.com

	main.c
*/

#include "context.h"
#include "error.h"
#include "functions.h"
#include "value.h"

static CtxNode* build_sample_context(void) {
    CtxNode* root = ctx_new("root");

    CtxNode* keyword = ctx_new("KEYWORD");
	ctx_add_child(root, keyword);
    ctx_set_prop(keyword, "FUNCTION", val_str("FUNCTION_VALUE"));
	ctx_set_prop(keyword, "FUNCTION2", val_str("FUNCTION_VALUE2"));

    CtxNode* config = ctx_new("CONFIG");
    CtxNode* db = ctx_new("DB");
    CtxNode* user = ctx_new("USER");
    ctx_set_prop(user, "NAME", val_str("db_user"));
	ctx_add_child(config, db);
	ctx_add_child(db, user);
	ctx_add_child(root, config);

    CtxNode* system = ctx_new("SYSTEM");
    CtxNode* syscfg = ctx_new("CONFIG");
    CtxNode* network = ctx_new("NETWORK");
    CtxNode* proxy = ctx_new("PROXY");
    ctx_set_prop(proxy, "HOST", val_str("10.0.0.1"));
	ctx_add_child(system, syscfg);
	ctx_add_child(syscfg, network);
	ctx_add_child(network, proxy);
	ctx_add_child(root, system);

    ctx_set_prop(root, "IDENTIFIER", val_str("X123"));

    ctx_set_prop(root, "A", val_num(10));
    ctx_set_prop(root, "B", val_num(20));
    ctx_set_prop(root, "C", val_num(5));
    ctx_set_prop(root, "D", val_num(15));
	ctx_set_prop(root, "PARAMETERS_LIST", val_str("char*,int"));
	ctx_set_prop(root, "STATEMENTS", val_str("return 0;"));

    return root;
}


int main(int argc, char** argv) {

	register_builtins_default_ex();
	CtxNode* ctx = build_sample_context();

	process_cli(argc, argv, ctx);

	int has_error = 0;
    if(error_has_error()) {
        error_print_all();
        error_print_summary();
        has_error = 1;
    }

	ctx_free(&ctx);
	functions_cleanup();
	errors_destroy();
    return has_error;
}
