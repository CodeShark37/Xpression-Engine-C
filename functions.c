#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


static FuncEntry *func_registry = NULL;

/* Builtins implementations */

// Soma: se receber lista, soma todos os elementos
static Value *fn_sum(Value *this,Value **args ,size_t argc) {
	(void)this; // `this` isn´t used here
    double total = 0;
    for (int i = 0; i < argc; i++) {
        if (args[i]->kind == VALUE_LIST) {
            for (int j = 0; j < args[i]->n_items; j++) {
                if (args[i]->items[j]->kind == VALUE_NUMBER) {
                    total += args[i]->items[j]->num;
                }
            }
        } else if (args[i]->kind == VALUE_NUMBER) {
            total += args[i]->num;
        }
    }
    return val_num(total);
}

// Produto: se receber lista, multiplica todos os elementos
static Value *fn_mul(Value *this,Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    double total = 1;
    for (int i = 0; i < argc; i++) {
        if (args[i]->kind == VALUE_LIST) {
            for (int j = 0; j < args[i]->n_items; j++) {
                if (args[i]->items[j]->kind == VALUE_NUMBER) {
                    total *= args[i]->items[j]->num;
                }
            }
        } else if (args[i]->kind == VALUE_NUMBER) {
            total *= args[i]->num;
        }
    }
    return val_num(total);
}

// Concatenação de strings (listas preservadas)
static size_t concat_measure(Value *v) {
    if (!v) return 0;

    if (v->kind == VALUE_STRING) {
        return strlen(v->str);
    }
    if (v->kind == VALUE_LIST) {
        size_t total = 0;
        for (size_t i = 0; i < v->n_items; i++) {
            total += concat_measure(v->items[i]);
        }
        return total;
    }
    return 0;
}

static void concat_collect(Value *v, char *buf) {
    if (!v) return;

    if (v->kind == VALUE_STRING) {
        strcat(buf, v->str);
    } else if (v->kind == VALUE_LIST) {
        for (size_t i = 0; i < v->n_items; i++) {
            concat_collect(v->items[i], buf);
        }
    }
}

static Value *fn_concat(Value *this, Value **args, size_t argc) {
    (void)this; // `this` isn´t used here

    size_t total_len = 0;
    for (size_t i = 0; i < argc; i++) {
        total_len += concat_measure(args[i]);
    }

    char *buf = malloc(total_len + 1);
    if (!buf) return val_null();
    buf[0] = '\0';

    for (size_t i = 0; i < argc; i++) {
        concat_collect(args[i], buf);
    }
    
    Value *res = val_str(buf);
    free(buf);
    return res;
}


static Value *fn_contains(Value *this,Value **args,size_t argc){
	(void)this; // `this` isn´t used here
    if(argc < 2 || args[0]->kind != VALUE_LIST){ return val_null();}
    
	Value *list = args[0];
    Value *target = args[1];
    
    Value *it = NULL;
	
	for (int i = 0; i < list->n_items; i++) {
        it = list->items[i];
        if (val_equals(it, target)) return val_bool(1);
    }
    return val_bool(0);
}


// Função de contagem: conta quantas vezes um valor aparece na lista
static Value *fn_count(Value *this,Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2 || args[0]->kind != VALUE_LIST) {
        return val_null();
    }

    int count = 0;
    Value *list = args[0];
    Value *target = args[1];

    for (int i = 0; i < list->n_items; i++) {
        Value *item = list->items[i];
        if(val_equals(item,target)){count++;}
    }

    return val_num(count);
}

static Value *fn_uppercase(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 1 || !args[0]) return val_str("");
    char *s = value_to_string(args[0]);
    for (size_t i=0;i<strlen(s);i++) s[i] = toupper((unsigned char)s[i]);
    Value *r = val_str(s);
	free(s); s=NULL;
	return r;
}

/*static Value *fn_max(Value *this, Value **args, size_t argc) {
    (void)this; // `this` isn´t used here
	if (argc==0) return val_num(0);
    double m = -INFINITY;
    for (size_t i=0;i<argc;i++) {
        double v = (args[i]->kind==VALUE_NUMBER) ? args[i]->num : atof(value_to_string(args[i]));
        if (v > m) m = v;
    }
    return val_num(m);
}*/

// auxiliar: percorre recursivamente listas e atualiza max
static void collect_max(Value *v, double *m) {
    if (!v) return;

    if (v->kind == VALUE_NUMBER) {
        if (v->num > *m) { *m = v->num; }
    }
    else if (v->kind == VALUE_LIST) {
        for (size_t i = 0; i < v->n_items; i++) {
            collect_max(v->items[i], m);
        }
    }
    
}

static Value *fn_max(Value *this, Value **args, size_t argc) {
    (void)this; // `this` isn´t used here
	double m = 0.0;

    for (size_t i = 0; i < argc; i++) {
        collect_max(args[i], &m);
    }

    return val_num(m);
}


static Value *fn_min(Value *this, Value **args, size_t argc) {
    (void)this; // `this` isn´t used here
	if (argc==0) return val_num(0);
    double m = INFINITY;
    for (size_t i=0;i<argc;i++) {
        double v = (args[i]->kind==VALUE_NUMBER) ? args[i]->num : atof(value_to_string(args[i]));
        if (v < m) m = v;
    }
    return val_num(m);
}




static Value *fn_mixed(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    size_t cap=128;
	char *buf=malloc(cap);
	size_t pos=0;
	buf[0]=0;
	
    for (size_t i=0;i<argc;i++) {
        char *s = value_to_string(args[i]);
        size_t L = strlen(s);
        if (pos + L + 2 >= cap) {
			cap = (pos+L+32)*2;
			buf = realloc(buf, cap);
		}
        memcpy(buf+pos, s, L); pos += L; buf[pos] = 0;
        if (i+1 < argc) buf[pos++]='|';
        free(s);s=NULL;
    }
    buf[pos]=0;
    Value *r = val_str(buf);
	free(buf); buf=NULL;
	return r;
}


/* UNIQUE(list) - remove duplicados */
static Value *fn_unique(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 1 || args[0]->kind != VALUE_LIST) return val_null();
	
    Value *result = val_list();
    for (size_t i = 0; i < args[0]->n_items; i++) {
        Value *item = args[0]->items[i];
        int found = 0;
        for (size_t j = 0; j < result->n_items; j++) {
            if (val_equals(result->items[j], item)) { found = 1; break; }
        }
        if (!found) val_list_append(result, val_dup(item));
    }
    return result;
}

/* MAP(list, fn) - aplica função a cada item da lista */
static Value *fn_map(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2 || args[0]->kind != VALUE_LIST || args[1]->kind != VALUE_STRING)
        return val_null();

    const char *fn_name = args[1]->str;  // função de destino
    Func cb = find_function(fn_name);
    if (!cb) return val_null();
	
    Value *result = val_list();
    for (size_t i = 0; i < args[0]->n_items; i++) {
        Value *single_arg = val_dup(args[0]->items[i]);
        Value *mapped = cb(val_null(), &single_arg, 1);
        val_list_append(result, mapped);
        val_free(single_arg);
    }
    return result;
}

/* AVG(list) - média de valores numéricos */
static Value *fn_avg(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 1 || args[0]->kind != VALUE_LIST) return val_null();

    double sum = 0;
    size_t count = 0;
    for (size_t i = 0; i < args[0]->n_items; i++) {
        if (args[0]->items[i]->kind == VALUE_NUMBER) {
            sum += args[0]->items[i]->num;
            count++;
        }
    }
    if (count == 0) return val_null();
    return val_num(sum / count);
}

/* SORT(list) - ordena strings ou números */
static Value *fn_sort(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 1 || args[0]->kind != VALUE_LIST) return val_null();

    Value *result = val_list();
    for (size_t i = 0; i < args[0]->n_items; i++) {
        val_list_append(result, val_dup(args[0]->items[i]));
    }

    qsort(result->items, result->n_items, sizeof(Value*), (int(*)(const void*,const void*))val_compare_sort);
    return result;
}

/* IF(cond, a, b) - retorna a se cond != false/null/0, senão b */
static Value *fn_if(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 3) return val_null();

    Value *cond = args[0];
    int truthy = 0;
    if (cond->kind == VALUE_BOOL) truthy = cond->num;
    else if (cond->kind == VALUE_NUMBER) truthy = (cond->num != 0);
    else if (cond->kind == VALUE_STRING) truthy = (cond->str && *cond->str);

    return truthy ? val_dup(args[1]) : val_dup(args[2]);
}

/* ==== Funções de comparação ==== */

static Value *fn_eq(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2) return val_bool(0);
    return val_bool(val_compare(args[0], args[1]) == 0);
}

static Value *fn_neq(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2) return val_bool(0);
    return val_bool(val_compare(args[0], args[1]) != 0);
}

static Value *fn_gt(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2) return val_bool(0);
    return val_bool(val_compare(args[0], args[1]) > 0);
}

static Value *fn_lt(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2) return val_bool(0);
    return val_bool(val_compare(args[0], args[1]) < 0);
}

static Value *fn_gte(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2) return val_bool(0);
    return val_bool(val_compare(args[0], args[1]) >= 0);
}

static Value *fn_lte(Value *this, Value **args, size_t argc) {
	(void)this; // `this` isn´t used here
    if (argc < 2) return val_bool(0);
    return val_bool(val_compare(args[0], args[1]) <= 0);
}


/* register builtins */
void register_builtins_default(void) {
    register_function("UPPERCASE", fn_uppercase);
	register_function("MUL", fn_mul);
    register_function("CONCAT", fn_concat);
	register_function("COUNT", fn_count);
	register_function("CONTAINS", fn_contains);
    register_function("SUM", fn_sum);
    register_function("MIN", fn_min);
    register_function("MAX", fn_max);
    register_function("MIXED", fn_mixed);
    register_function("IF", fn_if);
    register_function("MAP", fn_map);
    register_function("UNIQUE", fn_unique);
    register_function("AVG", fn_avg);
    register_function("SORT", fn_sort);
	register_function("EQ", fn_eq);
    register_function("NEQ", fn_neq);
    register_function("GT", fn_gt);
    register_function("LT", fn_lt);
    register_function("GTE", fn_gte);
    register_function("LTE", fn_lte);

}

void register_function(const char *name, Func cb) {
    FuncEntry *e = malloc(sizeof(FuncEntry));
	e->name = strdup(name);
	e->cb = cb;
	e->next = func_registry;
	func_registry = e;
}

Func find_function(const char *name) {
    for (FuncEntry *e = func_registry; e; e = e->next){ 
		if (!strcmp(e->name, name)) return e->cb;
	}
    return NULL;
}

void free_functions(void) {
    for (FuncEntry *e = func_registry; e; ) {
		FuncEntry *nx = e->next;
		free(e->name); e->name=NULL;
		free(e); e = nx;
	}
    func_registry = NULL;
}

