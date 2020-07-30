#ifndef PARSE_H
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Params
{
	char *type;
	char *name;
	int *list;
	int sizelist;
	int used;
}Params;

typedef struct Table
{
	char *type;
	char *name;
	int *list;
	int func;
	int funcOrVar;
	int scope;
	int sizelist;
	int ParamSize;
	int returnParam;
	int used;
	Params *param;
}Table;



extern int size;
extern int tempScope;

void noUseGlobalVariable();
void noUseVariable(int tempScope);
void declarationOfFunctionWithoutImplementation();
int isExistsfunc(char *id);
void checkArrayParam();
void insert_list_to_table_params_func();
int isIdarr();
int funcParam();
void isFuncVoid();
int isExistsVar(char *id);
void DeleteAll();
void DeleteItem(int tempScope);
void matchBetweendecToDefs();
int dec_or_body();
void insert_params();
int intNumber(char *str);
void insert_list_to_table();
int isExists(Table current, int var, int dec);
int match(eTOKENS);
int Error_Recoverry(eTOKENS);
int func();
void parse_PROG();
void parse_GLOBAL_VARS();
void parse_VAR_DEC();
void parse_STMT();
void parse_VAR_DEC_TAG();
void parse_DIM_SIZES(int func);
void parse_DIM_SIZES_TAG(int func1);
void parse_FUNC_PREDEFS();
void parse_FUNC_PROTOTYPE();
void parse_PARAMS();
void parse_PARAM_LIST();
void parse_PARAM();
void parse_PARAM_LIST_TAG();
void parse_PARAM_TAG();
void parse_FUNC_PREDEFS_TAG();
void parse_TYPE(int func);
void parse_GLOBAL_VARS_TAG();
void parse_VAR_DEC_LIST();
void parse_VAR_DEC_LIST_TAG();
void parse_COMP_STMT();
void parse_STMT_LIST();
void parse_IF_STMT();
void parse_CONDITION();
void parse_EXPR();
void parse_EXPR_TAG();
void parse_TERM();
void parse_TERM_TAG();
void parse_FACTOR();
void parse_STMT_LIST_TAG();
void parse_RETURN_STMT();
void parse_RETURN_STMT_TAG();
void parse_VAR(int temp);
void parse_VAR_TAG(char *id);
void parse_EXPR_LIST();
void parse_EXPR_LIST_TAG();
void parse_CALL();
void parse_ARGS();
void parse_ARGS_LIST();
void parse_ARGS_LIST_TAG();
void parse_FUNC_FULL_DEFS();
void parse_FUNC_FULL_DEFS_TAG();
void parse_FUNC_WITH_BODY();
void parse_RETURNED_TYPE();
void insert_to_table();

#endif