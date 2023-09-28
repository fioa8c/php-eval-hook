/* $Id$ */

#include "php.h"
#include "ext/standard/info.h"


#define EVAL_CALLBACK_FUNCTION  "__eval"


static zend_op_array* (*old_compile_string)(zend_string *source_string, const char *filename);


static zend_op_array* hookevals_compile_string(zend_string *source_string, const char *filename)
{
	zend_op_array *op_array = NULL;
	int op_compiled = 0;

	if(strstr(filename, "eval()'d code")) {
		if(zend_hash_str_exists(CG(function_table), EVAL_CALLBACK_FUNCTION, strlen(EVAL_CALLBACK_FUNCTION))) {
			zval function;
			zval retval;
			zval parameter[2];

			ZVAL_STR(&parameter[0], source_string);
			ZVAL_STRING(&function, EVAL_CALLBACK_FUNCTION);
			ZVAL_STRING(&parameter[1], filename);

			if(call_user_function(CG(function_table), NULL, &function, &retval, 2, parameter) == SUCCESS) {
				switch(Z_TYPE(retval)) {
					case IS_STRING:
						op_array = old_compile_string(Z_STR(retval), filename);
					case IS_FALSE:
						op_compiled = 1;
						break;
				}
			}

			zval_dtor(&function);
			zval_dtor(&retval);
			zval_dtor(&parameter[1]);
		}
	}

	if(op_compiled) {
		return op_array;
	} else {
		return old_compile_string(source_string, filename);
	}
}


PHP_MINIT_FUNCTION(hookevals)
{
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hookevals)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(hookevals)
{
	old_compile_string = zend_compile_string;
	zend_compile_string = hookevals_compile_string;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(hookevals)
{
	zend_compile_string = old_compile_string;
	return SUCCESS;
}

PHP_MINFO_FUNCTION(hookevals)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "eval() hooking", "enabled");
	php_info_print_table_row(2, "callback function", EVAL_CALLBACK_FUNCTION);
	php_info_print_table_end();
}


zend_function_entry hookevals_functions[] = {
	ZEND_FE_END
};

zend_module_entry hookevals_module_entry = {
	STANDARD_MODULE_HEADER,
	"hookevals",
	hookevals_functions,
	PHP_MINIT(hookevals),
	PHP_MSHUTDOWN(hookevals),
	PHP_RINIT(hookevals),
	PHP_RSHUTDOWN(hookevals),
	PHP_MINFO(hookevals),
	"0.0.1-dev",
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(hookevals)

