#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "evaluator.h"
#include "functions.h"
#include "context.h"
#include "printer.h"
#include "cli.h"


void process_expressions(const char *text, int as_json, int group_output, int do_eval, CtxNode *ctx) {
	
    char **exprs = NULL; size_t n_exprs = 0;
    if (extract_placeholders(text, &exprs, &n_exprs) != 0) { 	 fprintf(stderr,"Malformed placeholders\n");
		 return;
	}

    if (group_output) {
        if (as_json) printf("[\n");
		else printf("<expressions>\n");
    }
    for (size_t i=0;i<n_exprs;i++) {
        Node *ast = parse_expression_str(exprs[i]);
        if (!ast) { 
			if (group_output) { 
				if (as_json) printf("null");
				else printf("<!-- parse failed -->\n"); 
			} else {
				fprintf(stderr,"parse failed: %s\n", exprs[i]);
			} 
			free(exprs[i]);
			exprs[i]=NULL;
			continue;
		}
		Value *res = NULL;
		if(do_eval){res = eval_node(ast, ctx);}
		if (as_json){
			print_json(ast, res, group_output);
		}else {
			print_xml(ast, res, group_output);
		}
		
        val_free(res);
        node_free(ast);
        free(exprs[i]);
		exprs[i] = NULL;
    }
    if (group_output) {
        if (as_json) printf("]\n");
		else printf("</expressions>\n");
    }
    free(exprs);
	exprs = NULL;
}


static CtxNode *build_sample_context(void) {
    CtxNode *root = ctx_new("root");

    CtxNode *keyword = ctx_new("KEYWORD"); ctx_add_child(root, keyword);
    ctx_set_prop(keyword, "FUNCTION", val_str("FUNCTION_VALUE"));

    CtxNode *config = ctx_new("CONFIG"); ctx_add_child(root, config);
    CtxNode *db = ctx_new("DB"); ctx_add_child(config, db);
    CtxNode *user = ctx_new("USER"); ctx_add_child(db, user);
    ctx_set_prop(user, "NAME", val_str("db_user"));

    CtxNode *system = ctx_new("SYSTEM"); ctx_add_child(root, system);
    CtxNode *syscfg = ctx_new("CONFIG"); ctx_add_child(system, syscfg);
    CtxNode *network = ctx_new("NETWORK"); ctx_add_child(syscfg, network);
    CtxNode *proxy = ctx_new("PROXY"); ctx_add_child(network, proxy);
    ctx_set_prop(proxy, "HOST", val_str("10.0.0.1"));

    CtxNode *rule = ctx_new("RULE"); ctx_add_child(root, rule);

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
    CLIOptions opt = parse_cli(argc, argv);


    // exemplo de contexto
    /*CtxNode *root = ctx_new(NULL);
    ctx_set_prop(root, "A", val_str("apple"));
    ctx_set_prop(root, "B", val_num(42));*/

    if (opt.expr) {
		register_builtins_default();
		CtxNode *ctx = build_sample_context();
		process_expressions(opt.expr, opt.json,
						opt.grouped, opt.eval, ctx);

		ctx_free(ctx);
		free_funcs();
        
		/*Node *ast = parse_expression_str(opt.expr);
        Value *res = eval_node(ast, ctx);
        if (opt.json) print_json(ast, res, opt.grouped);
        else if (opt.xml) print_xml(ast, res, opt.grouped);
        else {
            char *s = value_to_string(res);
            printf("%s\n", s);
            free(s);
        }
        val_free(res);
        node_free(ast);*/
    }

	free((void*)opt.expr);
	opt.expr = NULL;
    return 0;
}
