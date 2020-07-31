#include "Parse.h"
#include "Token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern number_line;
extern int size = 0;
extern int tempScope = 0;

Token *t;
Table table[100];
Table tempTable;
Table tempTableVar;
Params tempParams;

int parse_var = 0;
int lastFunc = 0;
int numPrams = 0;
int numberVar = 0;
int numberVarOfFunc = 0;
int legalNumberLeft = -1;
int legalNumberRight = -1;
int returnNumber = -1;
int Number = 0; 
int dec = 0;
int flagOfAllocationParam = 0;
int idArr = 0;
int numberOfcell = 0;
int tempintNumber = -1;
int flagPlus = 0;
int flagMulti = 0;
int lastArr = -1;
int lastParamArr = -1;
int callfunc = -1;
int *callarr;
int callSize = 0;
int parse_call = 0;
int typeLastVar = - 1;



void parse_FUNC_FULL_DEFS()
{
	fprintf(output, "Line %d : FUNC_FULL_DEFS -> parse_FUNC_WITH_BODY\n", number_line);
	parse_FUNC_WITH_BODY();
	fprintf(output, "Line %d : FUNC_FULL_DEFS -> FUNC_FULL_DEFS_TAG\n", number_line);
	parse_FUNC_FULL_DEFS_TAG();
}
void parse_FUNC_FULL_DEFS_TAG()
{
	t = next_token();

	switch (t->kind) {
	case TOKEN_INTEGER:
		fprintf(output, "Line %d : FUNC_FULL_DEFS_TAG -> FUNC_FULL_DEFS\n", number_line);
		back_token();
		parse_FUNC_FULL_DEFS();
		break;
	case TOKEN_FLOAT:
		fprintf(output, "Line %d : FUNC_FULL_DEFS_TAG -> FUNC_FULL_DEFS\n", number_line);
		back_token();
		parse_FUNC_FULL_DEFS();
		break;
	case TOKEN_VOID:
		fprintf(output, "Line %d : FUNC_FULL_DEFS_TAG -> FUNC_FULL_DEFS\n", number_line);
		back_token();
		parse_FUNC_FULL_DEFS();
		break;
	default:
		fprintf(output, "Line %d : FUNC_FULL_DEFS_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_FUNC_WITH_BODY()
{
	fprintf(output, "Line %d : FUNC_WITH_BODY -> FUNC_PROTOTYPE\n", number_line);
	parse_FUNC_PROTOTYPE();
	fprintf(output, "Line %d : FUNC_WITH_BODY -> COMP_STMT\n", number_line);
	matchBetweendecToDefs();
	parse_COMP_STMT();
}
void parse_COMP_STMT()
{
	eTOKENS Follow[] = { TOKEN_INTEGER,TOKEN_FLOAT,TOKEN_VOID,TOKEN_SEMICOLON,TOKEN_CURLY_BRACES_RIGHT,TOKEN_EOF};

	if (match(TOKEN_CURLY_BRACES_LEFT))
	{
		tempScope++;
		fprintf(output, "Line %d : COMP_STMT -> {\n", number_line);
		fprintf(output, "Line %d : COMP_STMT -> VAR_DEC_LIST\n", number_line);
		parse_VAR_DEC_LIST();
		fprintf(output, "Line %d : COMP_STMT -> STMT_LIST\n", number_line);
		parse_STMT_LIST();
		if (match(TOKEN_CURLY_BRACES_RIGHT))
		{
			fprintf(output, "Line %d : COMP_STMT -> }\n", number_line);
			if (tempScope == 1 && table[lastFunc].returnParam == 1 && ((strcmp(table[lastFunc].type, "int") == 0) || (strcmp(table[lastFunc].type, "float") == 0)))
				fprintf(outputsemant, "Line %d : The function \"%s\" has no return parameter\n", number_line, table[lastFunc].name);
			
			DeleteItem(tempScope);
			tempScope--;
		}
		else
			fprintf(output, "Line %d : missing }\n", number_line);
	}
	else
	{
		fprintf(output, "Line %d : missing {\n", number_line);
		back_token();
		Error_Recoverry(Follow, 7);
	}
}
void parse_STMT()
{
	eTOKENS Follow[] = { TOKEN_SEMICOLON,TOKEN_CURLY_BRACES_RIGHT };
	int varOrFunc = 0;

	t = next_token();

	switch (t->kind) {

	case TOKEN_ID: // var or call
		varOrFunc = isExistsfunc(t->lexeme);
		if (match(TOKEN_PARENTHESES_LEFT)) // call
		{
			back_token();
			back_token();
			fprintf(output, "Line %d : STMT -> CALL\n", number_line);
			parse_CALL();
		}
		else // var
		{
			back_token();
			back_token();
			fprintf(output, "Line %d : STMT -> VAR\n", number_line);
			parse_VAR(1);
			if (!match(TOKEN_ASSIGNMENT))
				fprintf(output, "Line %d : missing assignment sign\n", number_line);
			else
				fprintf(output, "Line %d : STMT -> =\n", number_line);
			if (varOrFunc == 1)
				fprintf(outputsemant, "Line %d : \"%s\" assignment into function\n", number_line, t->lexeme);

			Number = 1; // from var (!return)
			fprintf(output, "Line %d : STMT -> EXPR\n", number_line);
			parse_EXPR();
		}
		break;
	case TOKEN_CURLY_BRACES_LEFT: //comp_stmt

		back_token();
		fprintf(output, "Line %d : STMT -> COMP_STMT\n", number_line);
		parse_COMP_STMT();
		break;
	case TOKEN_IF: //if
		back_token();
		fprintf(output, "Line %d : STMT -> IF_STMT\n", number_line);
		parse_IF_STMT();
		break;
	case TOKEN_RETURN: // RETURN_STMT
		back_token();
		fprintf(output, "Line %d : STMT -> RETURN_STMT\n", number_line);
		parse_RETURN_STMT();
		break;
	default:
		fprintf(output, "Expected token of type '{ | id | if | return' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 2);
	}
}
void parse_CALL()
{
	eTOKENS Follow[] = { TOKEN_SEMICOLON,TOKEN_PLUS,TOKEN_MULTI,TOKEN_CURLY_BRACES_RIGHT,TOKEN_COMMA,TOKEN_PARENTHESES_RIGHT,TOKEN_BRACKETS_RIGHT,TOKEN_EQUAL,TOKEN_SMALLER,TOKEN_BIDDER,TOKEN_UNEQUAL,TOKEN_SMALLER_EQUAL,TOKEN_BIDDER_EQUAL};
	int temp = -1;
	parse_call = 1;
	
	t = next_token();

	switch (t->kind) {

	case TOKEN_ID:
		findcallfunc(t->lexeme);
		if (legalNumberLeft != -1)
		{
			legalNumberRight = isExistsVar(t->lexeme);

			if (legalNumberLeft == 1 && legalNumberRight == 0)
				fprintf(outputsemant, "Line %d : \"%s\" is float and the type of the expression is int\n", number_line, t->lexeme);
			
			legalNumberLeft = legalNumberRight = -1;
		}
		temp = isExistsfunc(t->lexeme);
		if (temp == 0)
			fprintf(outputsemant, "Line %d : \"%s\" is not func\n", number_line, t->lexeme);
		else if (temp == -1)
		{
			fprintf(outputsemant, "Line %d : \"%s\" the type of the expression is undefined\n", number_line, t->lexeme);
		}
		else
			arrcall();

		fprintf(output, "Line %d : CALL -> id\n", number_line);
		
		if(match(TOKEN_PARENTHESES_LEFT))
			fprintf(output, "Line %d : CALL -> (\n", number_line);
		else
			fprintf(output, "Line %d :  missing (\n", number_line);
		
		fprintf(output, "Line %d : CALL -> ARGS\n", number_line);
		parse_ARGS();
		if(match(TOKEN_PARENTHESES_RIGHT))
			fprintf(output, "Line %d : CALL -> )\n", number_line);
		else
			fprintf(output, "Line %d :  missing )\n", number_line);
		break;
	default:
		fprintf(output, "Expected token of type 'id' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 13);
	}
	checkarrcall();
	parse_call = 0;
	callSize = 0;
	free(callarr);
}
void parse_ARGS()
{
	t = next_token();

	switch (t->kind) {

	case TOKEN_ID: // var or call
		if (match(TOKEN_PARENTHESES_LEFT))
		{
			back_token();
			back_token();
			fprintf(output, "Line %d : ARGS -> ARG_LIST\n", number_line);
			parse_ARGS_LIST();
		}
		else
		{
			back_token();
			back_token();
			fprintf(output, "Line %d : ARGS -> ARG_LIST\n", number_line);
			parse_ARGS_LIST();
		}
		break;
	case TOKEN_INT_NUMBER:
		back_token();
		fprintf(output, "Line %d : ARGS -> ARG_LIST\n", number_line);
		parse_ARGS_LIST();
		break;
	case TOKEN_FLOAT_NUMBER:
		back_token();
		fprintf(output, "Line %d : ARGS -> ARG_LIST\n", number_line);
		parse_ARGS_LIST();
		break;
	case TOKEN_PARENTHESES_LEFT:
		back_token();
		fprintf(output, "Line %d : ARGS -> ARG_LIST\n", number_line);
		parse_ARGS_LIST();
		break;
	default:
		fprintf(output, "Line %d : ARGS -> epsilon\n", number_line);
		back_token();
	}
}
void parse_ARGS_LIST()
{
	fprintf(output, "Line %d : ARG_LIST -> EXPR\n", number_line);
	parse_EXPR();
	fprintf(output, "Line %d : ARG_LIST -> ARG_LIST_TAG\n", number_line);
	parse_ARGS_LIST_TAG();
}
void parse_ARGS_LIST_TAG()
{
	if (match(TOKEN_COMMA))
	{
		fprintf(output, "Line %d : ARG_LIST_TAG -> ,\n", number_line);
		fprintf(output, "Line %d : ARG_LIST_TAG -> EXP\n", number_line);
		parse_EXPR();
		fprintf(output, "Line %d : ARG_LIST_TAG -> ARGS_LIST_TAG\n", number_line);
		parse_ARGS_LIST_TAG();
	}
	else
	{
		fprintf(output, "Line %d : ARG_LIST_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_VAR(int temp)
{
	eTOKENS Follow[] = { TOKEN_ASSIGNMENT,TOKEN_SEMICOLON,TOKEN_PLUS,TOKEN_MULTI,TOKEN_CURLY_BRACES_RIGHT,TOKEN_COMMA,TOKEN_PARENTHESES_RIGHT,TOKEN_BRACKETS_RIGHT,TOKEN_EQUAL,TOKEN_SMALLER,TOKEN_BIDDER,TOKEN_UNEQUAL,TOKEN_SMALLER_EQUAL,TOKEN_BIDDER_EQUAL };
	parse_var = 1;
	t = next_token();

	switch (t->kind) {

	case TOKEN_ID:
		if (temp)
			legalNumberLeft = isExistsVar(t->lexeme);
		else
		{
			legalNumberRight = isExistsVar(t->lexeme);
			
			if (returnNumber == -1 && legalNumberRight == 0 /*float*/ ) // return number
				returnNumber = 0; // float

			if (legalNumberLeft == 1 && legalNumberRight == 0 )
			{
				fprintf(outputsemant, "Line %d : \"%s\" is float and the type of the expression is int\n", number_line, t->lexeme);
			}
		}
		if (parse_call == 1)
			matchBetweendecTocallid();
		fprintf(output, "Line %d : VAR -> id\n", number_line);
		fprintf(output, "Line %d : VAR -> VAR_TAG\n", number_line);
		parse_VAR_TAG(t->lexeme);
		break;
	default:
		fprintf(output, "Expected token of type 'id' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 14);
	}
	parse_var = 0;
}
void parse_VAR_TAG(char *id)
{
	 idArr = isIdarr();

	if (match(TOKEN_BRACKETS_LEFT))
	{
		if (idArr == 0)
		{
			fprintf(outputsemant, "Line %d : \"%s\" is not array\n", number_line,id);
		}
		fprintf(output, "Line %d : VAR_TAG -> [\n", number_line);
		fprintf(output, "Line %d : VAR_TAG -> EXPR_LIST\n", number_line);
		parse_EXPR_LIST();
		if (match(TOKEN_BRACKETS_RIGHT))
		{
			fprintf(output, "Line %d : VAR_TAG -> ]\n", number_line);
			
			if (lastArr != lastFunc && lastArr != -1)
			{
				if ((numberOfcell + 1) < table[lastArr].sizelist)
						fprintf(outputsemant, "Exceeding the size of the array in line %d\n", number_line);

			}
			else if (lastArr == lastFunc)
			{
				if ((numberOfcell + 1) < table[lastFunc].param[lastParamArr].sizelist)
						fprintf(outputsemant, "Exceeding the size of the array in line %d\n", number_line);
	
			}
			numberOfcell = 0;
			idArr = 0;
		}
	}
	else
	{
		if (idArr == 1)
		{
			fprintf(outputsemant, "Line %d : \"%s\" assignment to array\n", number_line, id);
			idArr = 0;
		}
		fprintf(output, "Line %d : VAR_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_EXPR_LIST()
{
	fprintf(output, "Line %d : EXPR_LIST -> EXPR\n", number_line);
	parse_EXPR();
	fprintf(output, "Line %d : EXPR_LIST -> EXPR_LIST_TAG\n", number_line);
	parse_EXPR_LIST_TAG();
}
void parse_EXPR_LIST_TAG()
{
	checkArrayParam();
	if (match(TOKEN_COMMA))
	{
		numberOfcell++;
		fprintf(output, "Line %d : EXPR_LIST_TAG -> ,\n", number_line);
		fprintf(output, "Line %d : EXPR_LIST_TAG -> EXPR\n", number_line);
		parse_EXPR();
		fprintf(output, "Line %d : EXPR_LIST_TAG -> EXPR_LIST_TAG\n", number_line);
		parse_EXPR_LIST_TAG();
	}
	else
	{
		fprintf(output, "Line %d : EXPR_LIST_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_RETURN_STMT()
{
	eTOKENS Follow[] = { TOKEN_SEMICOLON,TOKEN_CURLY_BRACES_RIGHT };
	returnNumber = -1;
	table[lastFunc].returnParam = 0;

	t = next_token();

	if (t->kind == TOKEN_RETURN)
	{
		isFuncVoid();
		
		fprintf(output, "Line %d : RETURN_STMT -> return\n", number_line);
		fprintf(output, "Line %d : RETURN_STMT -> RETURN_STMT_TAG\n", number_line);
		parse_RETURN_STMT_TAG();
		if (funcParam() == 1 && returnNumber == 0)
			fprintf(outputsemant, "Line %d : The function \"%s\" returns an int and the type of the expression is float\n", number_line, table[lastFunc].name);
		returnNumber = -1;
	}
	else
	{
		fprintf(output, "Expected token of type 'return' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 2);
	}
}
void parse_RETURN_STMT_TAG()
{
	t = next_token();

	switch (t->kind) {

	case TOKEN_ID:
		fprintf(output, "Line %d : parse_RETURN_STMT_TAG -> EXPR\n", number_line);
		back_token();
		parse_EXPR();
		break;
	case TOKEN_INT_NUMBER:
		fprintf(output, "Line %d : parse_RETURN_STMT_TAG -> EXPR\n", number_line);
		back_token();
		parse_EXPR();
		break;
	case TOKEN_FLOAT_NUMBER:
		fprintf(output, "Line %d : parse_RETURN_STMT_TAG -> EXPR\n", number_line);
		back_token();
		parse_EXPR();
		break;
	case TOKEN_PARENTHESES_LEFT:
		fprintf(output, "Line %d : parse_RETURN_STMT_TAG -> EXPR\n", number_line);
		back_token();
		parse_EXPR();
		break;
	default:
		fprintf(output, "Line %d : parse_RETURN_STMT_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_CONDITION()
{
	eTOKENS Follow[] = { TOKEN_PARENTHESES_RIGHT };
	
	fprintf(output, "Line %d : CONDITION -> EXPR\n", number_line);
	parse_EXPR();

	t = next_token();

	if (t->kind == TOKEN_EQUAL || t->kind == TOKEN_SMALLER || t->kind == TOKEN_BIDDER || t->kind == TOKEN_UNEQUAL || t->kind == TOKEN_SMALLER_EQUAL || t->kind == TOKEN_BIDDER_EQUAL)
	{
		fprintf(output, "Line %d : CONDITION -> %s\n", number_line,t->lexeme);
		parse_EXPR();
	}
	else
	{
		fprintf(output, "Expected token of type 'rel_op' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 1);
	}
}
void parse_EXPR()
{
	fprintf(output, "Line %d : EXPR -> TERM\n", number_line);
	parse_TERM();
	fprintf(output, "Line %d : EXPR -> TERM_TAG\n", number_line);
	parse_EXPR_TAG();
}
void parse_EXPR_TAG()
{
	if (match(TOKEN_PLUS))
	{
		flagPlus = 1;
		fprintf(output, "Line %d : TERM_TAG -> +\n", number_line);
		fprintf(output, "Line %d : TERM_TAG -> TERM\n", number_line);
		parse_TERM();
		fprintf(output, "Line %d : TERM_TAG -> TERM_TAG\n", number_line);
		parse_EXPR_TAG();
	}
	else
	{
		fprintf(output, "Line %d : TERM_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_TERM()
{
	fprintf(output, "Line %d : TERM -> FACTOR\n", number_line);
	parse_FACTOR();
	fprintf(output, "Line %d : TERM -> TERM_TAG\n", number_line);
	parse_TERM_TAG();
}
void parse_TERM_TAG()
{
	if (match(TOKEN_MULTI))
	{
		flagMulti = 1;
		fprintf(output, "Line %d : TERM_TAG -> *\n", number_line);
		fprintf(output, "Line %d : TERM_TAG -> FACTOR\n", number_line);
		parse_FACTOR();
		fprintf(output, "Line %d : TERM_TAG -> TERM_TAG\n", number_line);
		parse_TERM_TAG();
	}
	else
	{
		fprintf(output, "Line %d : TERM_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_FACTOR()
{
	eTOKENS Follow[] = { TOKEN_MULTI,TOKEN_SEMICOLON,TOKEN_PLUS,TOKEN_MULTI,TOKEN_CURLY_BRACES_RIGHT,TOKEN_COMMA,TOKEN_PARENTHESES_RIGHT,TOKEN_BRACKETS_RIGHT,TOKEN_EQUAL,TOKEN_SMALLER,TOKEN_BIDDER,TOKEN_UNEQUAL,TOKEN_SMALLER_EQUAL,TOKEN_BIDDER_EQUAL };

	t = next_token();

	switch (t->kind) {

	case TOKEN_ID: // var or call
		if (match(TOKEN_PARENTHESES_LEFT)) // call
		{
			fprintf(output, "Line %d : FACTOR -> CALL\n", number_line);
			back_token();
			back_token();
			parse_CALL();
		}
		else // var
		{
			fprintf(output, "Line %d : FACTOR -> VAR\n", number_line);
			back_token();
			back_token();
			parse_VAR(0);
		}
		break;
	case TOKEN_INT_NUMBER:
		if (Number)
		{
			legalNumberRight = isExistsVar(t->lexeme);
			
			if (legalNumberLeft == 1 && legalNumberRight == 0)
				fprintf(outputsemant, "Line %d : \"%s\" is float and the type of the expression is int\n", number_line, t->lexeme);
		}
		else
			if (returnNumber == -1)
				returnNumber = 1; // int
		
		if (flagMulti)
		{
			tempintNumber = tempintNumber * intNumber(t->lexeme);
		}
		else if (flagPlus)
		{
			tempintNumber = tempintNumber + intNumber(t->lexeme);
		}
		else
		{
			tempintNumber = intNumber(t->lexeme);
		}
		if (parse_call == 1 && parse_var == 0)
		{
			matchBetweendecTocallNumber("int");
		}
		fprintf(output, "Line %d : FACTOR -> int_number\n", number_line);
		break;
	case TOKEN_FLOAT_NUMBER:
		if (Number)
		{
			legalNumberRight = isExistsVar(t->lexeme);

			if (legalNumberLeft == 1 && legalNumberRight == 0)
				fprintf(outputsemant, "Line %d : \"%s\" is float and the type of the expression is int\n", number_line, t->lexeme);
		}
		else
			if (returnNumber == -1)
				returnNumber = 0; // float

		if (idArr == 1)
			fprintf(outputsemant, "Line %d :float number cannot be an array size\n", number_line);
		if (parse_call == 1 && parse_var == 0)
		{
			matchBetweendecTocallNumber("float");
		}
		fprintf(output, "Line %d : FACTOR -> float_number\n", number_line);
		break;
	case TOKEN_PARENTHESES_LEFT:
		fprintf(output, "Line %d : FACTOR -> (\n", number_line);
		fprintf(output, "Line %d : FACTOR -> EXPR\n", number_line);
		parse_EXPR();
		if (match(TOKEN_PARENTHESES_RIGHT))
			fprintf(output, "Line %d : FACTOR -> )\n", number_line);
		break;
	default:
		fprintf(output, "Expected token of type 'int_num | float_num | ( | id' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 14);
	}
	flagMulti = flagPlus = 0;
}
void parse_STMT_LIST()
{
	fprintf(output, "Line %d : STMT_LIST -> STMT\n", number_line);
	parse_STMT();
	fprintf(output, "Line %d : STMT_LIST -> STMT_LIST_TAG\n", number_line);
	parse_STMT_LIST_TAG();
}
void parse_STMT_LIST_TAG()
{
	t = next_token();

	if (t->kind==TOKEN_SEMICOLON)
	{
		fprintf(output, "Line %d : STMT_LIST_TAG -> ;\n", number_line);
		fprintf(output, "Line %d : STMT_LIST_TAG -> STMT\n", number_line);
		legalNumberLeft = legalNumberRight = -1;
		Number = 0;
		parse_STMT();
		fprintf(output, "Line %d : STMT_LIST_TAG -> STMT_LIST_TAG\n", number_line);
		parse_STMT_LIST_TAG();
	}
	else
	{
		legalNumberLeft = legalNumberRight = -1;
		Number = 0;
		fprintf(output, "Line %d : STMT_LIST_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_IF_STMT()
{
	eTOKENS Follow[] = { TOKEN_SEMICOLON,TOKEN_CURLY_BRACES_RIGHT };

	t = next_token();

	switch (t->kind) {

	case TOKEN_IF:
		fprintf(output, "Line %d : IF_STMT -> if\n", number_line);
		if(!match(TOKEN_PARENTHESES_LEFT))
		{
			back_token();
			fprintf(output, "Line %d :  missing opening sign(\n", number_line);
		}
		else
			fprintf(output, "Line %d : IF_STMT -> (\n", number_line);
		fprintf(output, "Line %d : IF_STMT -> CONDITION\n", number_line);
		parse_CONDITION();
		if (!match(TOKEN_PARENTHESES_RIGHT))
		{
			back_token();
			fprintf(output, "Line %d :  missing closing sign )\n", number_line);
		}
		else
			fprintf(output, "Line %d : IF_STMT -> )\n", number_line);
		fprintf(output, "Line %d : IF_STMT -> STMT\n", number_line);
		parse_STMT();
		break;
	default:
		fprintf(output, "Expected token of type 'if' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 2);
	}
}
void parse_VAR_DEC_LIST()
{
	t = next_token();

	switch (t->kind)
	{
	case TOKEN_INTEGER:
		back_token();
		fprintf(output, "Line %d : VAR_DEC_LIST -> VAR_DEC\n", number_line);
		parse_VAR_DEC();
		fprintf(output, "Line %d : VAR_DEC_LIST -> VAR_DEC_LIST_TAG\n", number_line);
		parse_VAR_DEC_LIST_TAG();
		break;
	case TOKEN_FLOAT:
		back_token();
		fprintf(output, "Line %d : VAR_DEC_LIST -> VAR_DEC\n", number_line);
		parse_VAR_DEC();
		fprintf(output, "Line %d : VAR_DEC_LIST -> VAR_DEC_LIST_TAG\n", number_line);
		parse_VAR_DEC_LIST_TAG();
		break;
	default:
		fprintf(output, "Line %d : VAR_DEC_LIST -> epsilon\n", number_line);
		back_token();
	}
}
void parse_VAR_DEC_LIST_TAG()
{
	fprintf(output, "Line %d : parse_VAR_DEC_LIST_TAG -> VAR_DEC_LIST\n", number_line);
	parse_VAR_DEC_LIST();
}
void parse_FUNC_PREDEFS()
{

	t = next_token();

	switch (t->kind) {

	case TOKEN_INTEGER:
		back_token();
		fprintf(output, "Line %d : FUNC_PREDEFS -> FUNC_PROTOTYPE\n", number_line);
		parse_FUNC_PROTOTYPE();
		if (match(TOKEN_SEMICOLON))
			fprintf(output, "Line %d : FUNC_PREDEFS -> ;\n", number_line);
		fprintf(output, "Line %d : FUNC_PREDEFS -> FUNC_PREDEFS_TAG\n", number_line);
		parse_FUNC_PREDEFS_TAG();
		break;

	case TOKEN_VOID:
		back_token();
		fprintf(output, "Line %d : FUNC_PREDEFS -> FUNC_PROTOTYPE\n", number_line);
		parse_FUNC_PROTOTYPE();
		if (match(TOKEN_SEMICOLON))
			fprintf(output, "Line %d : FUNC_PREDEFS -> ;\n", number_line);
		fprintf(output, "Line %d : FUNC_PREDEFS -> FUNC_PREDEFS_TAG\n", number_line);
		parse_FUNC_PREDEFS_TAG();
		break;

	case TOKEN_FLOAT:
		back_token();
		fprintf(output, "Line %d : FUNC_PREDEFS -> FUNC_PROTOTYPE\n", number_line);
		parse_FUNC_PROTOTYPE();
		if (match(TOKEN_SEMICOLON))
			fprintf(output, "Line %d : FUNC_PREDEFS -> ;\n", number_line);
		fprintf(output, "Line %d : FUNC_PREDEFS -> FUNC_PREDEFS_TAG\n", number_line);
		parse_FUNC_PREDEFS_TAG();
		break;
	}
}
void parse_FUNC_PREDEFS_TAG()
{
	t = next_token();

	switch (t->kind) {

	case TOKEN_INTEGER:
		parse_FUNC_PROTOTYPE();
		if (!match(TOKEN_SEMICOLON))
			fprintf(output, "Line %d : Missing ;\n", number_line);
		parse_FUNC_PREDEFS_TAG();
		break;
	case TOKEN_FLOAT:
		parse_FUNC_PROTOTYPE();
		if (!match(TOKEN_SEMICOLON))
			fprintf(output, "Line %d : Missing ;\n", number_line);
		parse_FUNC_PREDEFS_TAG();
		break;
	case TOKEN_VOID:
		parse_FUNC_PROTOTYPE();
		if (!match(TOKEN_SEMICOLON))
			fprintf(output, "Line %d : Missing ;\n", number_line);
		parse_FUNC_PREDEFS_TAG();
		break;
	}
	back_token();
}
void parse_FUNC_PROTOTYPE()
{
	eTOKENS Follow[] = { TOKEN_SEMICOLON,TOKEN_CURLY_BRACES_LEFT };
	int length = 0;
	
	dec = dec_or_body();
	fprintf(output, "Line %d : FUNC_PROTOTYPE -> RETURNED_TYPE\n", number_line);
	
	parse_RETURNED_TYPE();

	t = next_token();
	length = strlen(t->lexeme) + 1;

	switch (t->kind) {

	case TOKEN_ID:
		tempTable.name = (char*)malloc(sizeof(char)*length);
		strcpy(tempTable.name, t->lexeme);
		tempTable.scope = tempScope;
		fprintf(output, "Line %d : FUNC_PROTOTYPE -> id\n", number_line);
		lastFunc = size;
		if (match(TOKEN_PARENTHESES_LEFT))
		{
			if (isExists(tempTable, 0, dec))
			{
				tempTable.func = 1;
				tempTable.funcOrVar = 1;
				insert_to_table(tempTable);
			}
			fprintf(output, "Line %d : FUNC_PROTOTYPE -> (\n", number_line);
		}
		else
			fprintf(output, "Line %d : Missing (\n", number_line);
		fprintf(output, "Line %d : FUNC_PROTOTYPE -> PARAMS\n", number_line);
		parse_PARAMS();
		if(match(TOKEN_PARENTHESES_RIGHT))
			fprintf(output, "Line %d : FUNC_PROTOTYPE -> )\n", number_line);
		else
			fprintf(output, "Line %d : Missing )\n", number_line);
		break;
	default:
		fprintf(output, "Expected token of type 'id' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 2);
	}
}
void parse_PARAMS()
{
	t = next_token();

	switch (t->kind) {

	case TOKEN_INTEGER:
		back_token();
		fprintf(output, "Line %d : PARAMS -> PARAM_LIST\n", number_line);
		parse_PARAM_LIST();
		break;
	case TOKEN_FLOAT:
		back_token();
		fprintf(output, "Line %d : PARAMS -> PARAM_LIST\n", number_line);
		parse_PARAM_LIST();
		break;
	default:
		back_token();
		fprintf(output, "Line %d : PARAMS -> epsilon\n", number_line);
	}
}
void parse_PARAM_LIST()
{
	fprintf(output, "Line %d : PARAM_LIST -> PARAMS\n", number_line);
	parse_PARAM();
	fprintf(output, "Line %d : PARAM_LIST -> PARAM_LIST_TAG\n", number_line);
	parse_PARAM_LIST_TAG();
}
void parse_PARAM()
{
	eTOKENS Follow[] = { TOKEN_COMMA , TOKEN_PARENTHESES_RIGHT };
	int length = 0;

	fprintf(output, "Line %d : PARAM -> TYPE\n", number_line);
	parse_TYPE(1);
	
	t = next_token();
	length = strlen(t->lexeme) + 1;

	if (t->kind==TOKEN_ID)
	{
		tempParams.name = (char*)malloc(sizeof(char) * length);
		strcpy(tempParams.name, t->lexeme);
		numPrams++;
		insert_params();
		fprintf(output, "Line %d : PARAM -> id\n", number_line);
		fprintf(output, "Line %d : PARAM -> PARAM_TAG\n", number_line);
		parse_PARAM_TAG();
	}
	else
	{
		fprintf(output, "Expected token of type 'id' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 2);
	}
}
void parse_PARAM_LIST_TAG()
{
	eTOKENS Follow[] = { TOKEN_PARENTHESES_RIGHT };

	t = next_token();

	switch (t->kind) {
	case TOKEN_COMMA:
		fprintf(output, "Line %d : PARAM_LIST_TAG -> ,\n", number_line);
		fprintf(output, "Line %d : PARAM_LIST_TAG -> PARAM\n", number_line);
		parse_PARAM();
		fprintf(output, "Line %d : PARAM_LIST_TAG -> PARAM_LIST_TAG\n", number_line);
		parse_PARAM_LIST_TAG();
		break; 
	case TOKEN_INTEGER:
		fprintf(output, "Line %d : Missing COMMA\n", number_line);
		back_token();
		fprintf(output, "Line %d : PARAM_LIST_TAG -> PARAM\n", number_line);
		parse_PARAM();
		fprintf(output, "Line %d : PARAM_LIST_TAG -> PARAM_LIST_TAG\n", number_line);
		parse_PARAM_LIST_TAG();
		break;
	case TOKEN_PARENTHESES_RIGHT:
		numPrams = 0;
		back_token();
		break;
	default:
		fprintf(output, "Expected token of type ',' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 1);
	}
}
void parse_PARAM_TAG()
{
	if (match(TOKEN_BRACKETS_LEFT))
	{
		fprintf(output, "Line %d : PARAM_TAG -> [\n", number_line);
		fprintf(output, "Line %d : PARAM_TAG -> DIM_SIZES\n", number_line);
		parse_DIM_SIZES(1);
		if (match(TOKEN_BRACKETS_RIGHT))
		{
			fprintf(output, "Line %d : PARAM_TAG -> ]\n", number_line);
			flagOfAllocationParam = 0;
			numberVarOfFunc = 0;
		}
	}
	else
		back_token();
}
void parse_PROG()
{ 
	fprintf(output, "Line %d : PROG -> GLOBAL_VARS\n", number_line);
	parse_GLOBAL_VARS();

	do {
		fprintf(output, "Line %d : PROG -> FUNC_PROTOTYPE\n", number_line);
		parse_FUNC_PROTOTYPE();
		t = next_token();
		/* legally, the next token after FUNC_PROTOTYPE
		   can be either semicolon or {
		   any other token is erroneous/unexpected */
	} while (t->kind == TOKEN_SEMICOLON);  /* if FUNC_PROTOTYPE is followed by a semicolon,
											then at least one more FUNC_PROTOTYPE is expected */
	
	if (t->kind == TOKEN_CURLY_BRACES_LEFT)  /* if FUNC_PROTOTYPE is followed by {
												 then actually the first FUNC_WITH_BODY is detected
												 and hence now the body of the function is expected */
	{
		matchBetweendecToDefs();
		t = back_token();        /* because every function parse_X starts itself with a call to next_token  */
		fprintf(output, "Line %d : PROG -> COMP_STMT\n", number_line);
		parse_COMP_STMT();       /* body of the first FUNC_WITH_BODY */
		t = next_token();
		if (t->kind != TOKEN_EOF)    /* something more after the first FUNC_WITH_BODY -
									this can be only FUNC_FULL_DEFS */
		{
			t = back_token();     /* because every function parse_X starts itself with a call to next_token  */
			fprintf(output, "Line %d : PROG -> FUNC_FULL_DEFS\n", number_line);
			parse_FUNC_FULL_DEFS();
		}
		else
			t = back_token();     /* because in the main of parser, match(EOF) appears after the call to parse_PROG(),
									 and function match() itself starts with next_token  */
	}
	else
	{
		fprintf(output, "Line %d : FUNC_PROTOTYPE can be followed only by semicolon or by {\n", number_line);   /* FUNC_PROTOTYPE can be followed only by semicolon or by {   */
		while (t->kind != TOKEN_EOF) 
			t = next_token();   /* skip till the end of input */
		t = back_token();
	}
}  /* end of parse_PROG  */
void parse_GLOBAL_VARS()
{
	if (!func())
	{
		fprintf(output, "Line %d : GLOBAL_VARS -> _VAR_DEC\n", number_line);
		parse_VAR_DEC();
	}
	if (!func())
	{
		fprintf(output, "Line %d : GLOBAL_VARS -> GLOBAL_VARS_TAG\n", number_line);
		parse_GLOBAL_VARS_TAG();
	}
}
void parse_GLOBAL_VARS_TAG()
{
	t = next_token();
	
	switch (t->kind)
	{
	case TOKEN_INTEGER:
		back_token();
		fprintf(output, "Line %d : GLOBAL_VARS_TAG -> _VAR_DEC\n", number_line);
		parse_VAR_DEC();
		if (!func())
		{
			fprintf(output, "Line %d : GLOBAL_VARS_TAG -> GLOBAL_VARS_TAG\n", number_line);
			parse_GLOBAL_VARS_TAG();
		}
		break;
	case TOKEN_FLOAT:
		back_token();
		fprintf(output, "Line %d : GLOBAL_VARS_TAG -> _VAR_DEC\n", number_line);
		parse_VAR_DEC();
		if (!func())
		{
			fprintf(output, "Line %d : GLOBAL_VARS_TAG -> GLOBAL_VARS_TAG\n", number_line);
			parse_GLOBAL_VARS_TAG();
		}
		break;
	default:
		fprintf(output, "Line %d : GLOBAL_VARS_TAG -> epsilon\n", number_line);
		back_token();
	}
	
}
void parse_VAR_DEC()
{
	eTOKENS Follow[] = {TOKEN_INTEGER,TOKEN_FLOAT,TOKEN_VOID,TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_CURLY_BRACES_LEFT};
	int length = 0;

	fprintf(output, "Line %d : VAR_DEC -> TYPE\n", number_line);
	parse_TYPE(0);

	t = next_token();
	length = strlen(t->lexeme) + 1;

	switch (t->kind)
	{
		case TOKEN_ID:
			tempTable.name = (char*)malloc(sizeof(char)*length);
			strcpy(tempTable.name, t->lexeme);
			tempTable.scope = tempScope;

			fprintf(output, "Line %d : VAR_DEC -> id\n", number_line);
			fprintf(output, "Line %d : VAR_DEC -> VAR_DEC_TAG\n", number_line);
			parse_VAR_DEC_TAG();
			break;	
		default:
			fprintf(output, "Expected token of type 'id' at line : %d,\nActual token of lexeme : '%s'\n", number_line,t->lexeme);
			back_token();
			Error_Recoverry(Follow, 7);
	}
}
void parse_VAR_DEC_TAG()
{
	eTOKENS Follow[] = { TOKEN_INTEGER,TOKEN_FLOAT,TOKEN_VOID,TOKEN_ID,TOKEN_IF,TOKEN_RETURN,TOKEN_CURLY_BRACES_LEFT };

	t = next_token();
	if (isExists(tempTable, 1, 0))
	{
		tempTable.func = 0;
		tempTable.funcOrVar = 0;
		insert_to_table(tempTable);
	}

	switch (t->kind) {
	case TOKEN_SEMICOLON:
		fprintf(output, "Line %d : VAR_DEC_TAG -> ;\n", number_line);
		break;
	case TOKEN_BRACKETS_LEFT:
		fprintf(output, "Line %d : VAR_DEC_TAG -> [\n", number_line);
		fprintf(output, "Line %d : VAR_DEC_TAG -> DIM_SIZES\n", number_line);
		parse_DIM_SIZES(0);

		numberVar = 0;

		if (!match(TOKEN_BRACKETS_RIGHT))
		{
			back_token();
			fprintf(output, "Line %d : missing closing sign ]\n", number_line);
		}
		else
			fprintf(output, "Line %d : VAR_DEC_TAG -> ]\n", number_line);
		
		if (!match(TOKEN_SEMICOLON))
			fprintf(output, "Line %d : Missing ; in array definition\n", number_line);
		else
		{
			fprintf(output, "Line %d : VAR_DEC_TAG -> ;\n", number_line);
			//size++;
		}
		break;
	default:
		fprintf(output, "Expected token of type '; | [' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 7);
	}
}
void parse_TYPE(int func)
{
	eTOKENS Follow[] = { TOKEN_ID };

	t = next_token();
	switch (t->kind)
	{
	case TOKEN_INTEGER:
		if (func)
		{
			tempParams.type = (char*)malloc(sizeof(char) * 4);
			strcpy(tempParams.type, "int");
			fprintf(output, "Line %d : TYPE -> int\n", number_line);
		}
		else
		{
			tempTable.type = (char*)malloc(sizeof(char) * 4);
			strcpy(tempTable.type, "int");
			fprintf(output, "Line %d : TYPE -> int\n", number_line);
		}
		break;
	case TOKEN_FLOAT:
		if (func)
		{
			tempParams.type = (char*)malloc(sizeof(char) * 6);
			strcpy(tempParams.type, "float");
			fprintf(output, "Line %d : TYPE -> float\n", number_line);
		}
		else
		{
			tempTable.type = (char*)malloc(sizeof(char) * 6);
			strcpy(tempTable.type, "float");
			fprintf(output, "Line %d : TYPE -> float\n", number_line);
		}
		break;
	default:
		fprintf(output, "Expected token of type 'int | float' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		back_token();
		Error_Recoverry(Follow, 1);
	}
}
void parse_DIM_SIZES(int func)
{
	eTOKENS Follow[] = { TOKEN_BRACKETS_RIGHT };

	t = next_token();

	switch (t->kind)
	{
	case TOKEN_INT_NUMBER:
		if (func)
		{
			numberVarOfFunc++;
			insert_list_to_table_params_func();
			fprintf(output, "Line %d : DIM_SIZES -> int_num\n", number_line);
			fprintf(output, "Line %d : DIM_SIZES -> DIM_SIZES_TAG\n", number_line);
			parse_DIM_SIZES_TAG(func);
		}
		else
		{
			numberVar++;
			insert_list_to_table();
			fprintf(output, "Line %d : DIM_SIZES -> int_num\n", number_line);
			fprintf(output, "Line %d : DIM_SIZES -> DIM_SIZES_TAG\n", number_line);
			parse_DIM_SIZES_TAG(func);
		}
		break;

	default:
		back_token();
		if (Error_Recoverry(Follow, 1))
			fprintf(output, "Line %d : missing value inside []\n", number_line);
	}	
}

void parse_DIM_SIZES_TAG(int func1)
{
	t = next_token();

	switch (t->kind)
	{
	case TOKEN_COMMA:
		fprintf(output, "Line %d : DIM_SIZES_TAG -> ,\n", number_line);
		fprintf(output, "Line %d : DIM_SIZES_TAG -> DIM_SIZE\n", number_line);
		parse_DIM_SIZES(func1);
		break;
	case TOKEN_INT_NUMBER:
		fprintf(output, "Line %d : missing sign ',' between 2 elements\n", number_line);
		break;
	default:
		fprintf(output, "Line %d : DIM_SIZES_TAG -> epsilon\n", number_line);
		back_token();
	}
}
void parse_RETURNED_TYPE()
{
	eTOKENS Follow[] = { TOKEN_ID };

	t = next_token();
	switch (t->kind)
	{
	case TOKEN_VOID:
		tempTable.type = (char*)malloc(sizeof(char)*5);
		strcpy(tempTable.type, "void");
		fprintf(output, "Line %d : RETURNED_TYPE -> void\n", number_line);
		break;
	case TOKEN_INTEGER:
		fprintf(output, "Line %d : RETURNED_TYPE -> TYPE\n", number_line);
		back_token();
		parse_TYPE(0);
		break;
	case TOKEN_FLOAT:
		fprintf(output, "Line %d : RETURNED_TYPE -> TYPE\n", number_line);
		back_token();
		parse_TYPE(0);
		break;
	default:
		fprintf(output, "Expected token of type 'int | float | void' at line : %d,\nActual token of lexeme : '%s'\n", number_line, t->lexeme);
		Error_Recoverry(Follow, 1);
	}
}
int match(eTOKENS kind)
{
	t = next_token();

	if (t->kind == kind)
		return 1;
	return 0;
}

int Error_Recoverry(eTOKENS kind[], int size)
{
	t = next_token();

	while (1)
	{

		for (int i = 0; i < size; i++)
		{
			if (t->kind == kind[i])
			{
				back_token();
				return 1;
			}
			if (t->kind == TOKEN_EOF)
			{
				back_token();
				return 0;
			}
		}
		t = next_token();
	}
	return 0;
}
int dec_or_body()
{
	int count = 0;
	t = next_token();

	while (t->kind != TOKEN_EOF)
	{
		count++;

		if (t->kind == TOKEN_CURLY_BRACES_LEFT)
		{
			while (count != 0)
			{
				count--;
				t = back_token();
			}
			return 0; // body
		}
		if (t->kind == TOKEN_SEMICOLON)
		{
			while (count != 0)
			{
				count--;
				t = back_token();
			}
			return 1; // dec
		}
		t = next_token();
	}
	return 0;
}
int func()
{
	int count = 0;
	t = next_token();

	while (t->kind != TOKEN_EOF)
	{
		count++;

		if (t->kind == TOKEN_PARENTHESES_LEFT)
		{
			while (count != 0)
			{
				count--;
				t = back_token();
			}
			return 1;
		}
		if (t->kind == TOKEN_SEMICOLON)
		{
			while (count != 0)
			{
				count--;
				t = back_token();
			}
			return 0;
		}
		t = next_token();
	}
	return 0;
}
void insert_to_table(Table current)//insert id to table
{	
	table[size] = current;
	table[size].returnParam = 1;
	table[size].used = 0;
	size++;
}
void insert_list_to_table() // inesrt list of varible [int number]
{
	int x;

	table[size - 1].list = (int*)realloc(table[size - 1].list, numberVar*sizeof(int));
	x = intNumber(t->lexeme);
	table[size - 1].list[numberVar - 1] = x;
	table[size - 1].sizelist = numberVar;
}
void insert_list_to_table_params_func() // inesrt list of varible to func type id (type id[int number])
{
	int x;
	
	if (flagOfAllocationParam == 0)
	{
		table[size - 1].param[numPrams - 1].list = (int*)calloc(numberVarOfFunc, sizeof(int));
		flagOfAllocationParam = 1;
	}
	else
		table[size - 1].param[numPrams - 1].list = (int*)realloc(table[size - 1].param[numPrams - 1].list, numberVarOfFunc * sizeof(int));
	x = intNumber(t->lexeme);
	table[size - 1].param[numPrams - 1].list[numberVarOfFunc - 1] = x;
	table[size - 1].param[numPrams - 1].sizelist = numberVarOfFunc;
}
int isExists(Table current , int var , int dec) // if id exists
{
	if (size > 0)
	{
		if (var)
		{
			for (int i = size - 1; i >= 0; i--)
			{
				if (strcmp(table[i].name, current.name) == 0 && table[i].scope == current.scope)
				{
					fprintf(outputsemant, "Line %d : ""%s"" Already declared\n", number_line, current.name);
					return 1;
				}
			}
			return 1;
		}
		if (dec)
		{
			for (int i = size - 1; i >= 0; i--)
			{
				if (strcmp(table[i].name, current.name) == 0 && table[i].scope == current.scope)
				{
					fprintf(outputsemant, "Line %d : %s Already declared\n", number_line, current.name);
					return 1;
				}
			}
			return 1;
		}
	}
	return 1;
}
int intNumber(char *str)//change string to numbert
{
	int result = 0;
	int flag = 0;

	while ((*str >= '0') && (*str <= '9'))
	{
		flag = 1;
		result = (result * 10) + ((*str) - '0');
		str++;
	}
	return (result);
}
void insert_params()
{
	if (dec == 0)//body
	{
		for (int i = 1; i < numPrams; i++)
		{
			if (strcmp(table[size - 1].param[numPrams - i - 1].name, tempParams.name) == 0)
				fprintf(outputsemant, "Line %d : \"%s\" Already declared\n", number_line, tempParams.name);
		}
	}
	table[size - 1].param = (Params*)realloc(table[size - 1].param, (numPrams /*+1*/) * sizeof(Params));
	table[size - 1].param[numPrams - 1].type = tempParams.type;
	table[size - 1].param[numPrams - 1].name = tempParams.name;
	table[size - 1].param[numPrams - 1].sizelist = 0;
	table[size - 1].param[numPrams - 1].used = 0;
	table[size - 1].ParamSize = numPrams; 
}
void matchBetweendecToDefs()
{
	for (int i = size - 1; i >= 0; i--)
	{
		for (int j = size - 1; i >= 0; i--)
		{
			if (strcmp(table[i].name, table[j].name) == 0)
			{
				if (table[i].func == 0)
					fprintf(outputsemant, "Line %d : ""%s"" Already declared\n", number_line, table[i].name);

				if (strcmp(table[i].type, table[j].type) != 0)
					fprintf(outputsemant, "Line %d : Different type of the returned value between pre-definition (%s) to full definition (%s)\n", number_line, table[i].type, table[j].type);
				
				if (table[j].ParamSize != table[i].ParamSize)
					fprintf(outputsemant, "Line %d : The number of parameters is different between pre-definition (%d) to full definition (%d)\n", number_line, (table[i].ParamSize /*+ 1*/), (table[j].ParamSize /*+ 1*/));
				
				for (int k = 0; k < table[j].ParamSize; k++)
				{
					if (table[j].param[k].sizelist == table[i].param[k].sizelist && table[j].param[k].sizelist > 0)
					{
						for (int l = 0; l < table[j].param[k].sizelist; l++)
						{
							if (table[j].param[k].list[l]!= table[i].param[k].list[l])
								fprintf(outputsemant, "Line %d :  Different size of arr in position %d between pre-definition (%d) to full definition (%d), %s \n", number_line, l, table[j].param[k].list[l], table[i].param[k].list[l], table[j].param[k].name);
						}
					}
					if (table[j].param[k].sizelist != table[i].param[k].sizelist)
						fprintf(outputsemant, "Line %d :  Different size of arr in position  %d between pre-definition (%d) to full definition (%d), %s \n", number_line, k, table[j].param[k].sizelist, table[i].param[k].sizelist, table[j].param[k].name);
					if (strcmp(table[j].param[k].type, table[i].param[k].type) != 0)
						fprintf(outputsemant, "Line %d : Different type in prameter number %d , %s , %s\n", number_line, k, table[i].param[k].type, table[j].param[k].type);
					if (strcmp(table[j].param[k].name, table[i].param[k].name) != 0)
						fprintf(outputsemant, "Line %d : Different Name in parameter in position %d , %s , %s\n", number_line, k, table[i].param[k].name, table[j].param[k].name);

				}
				table[i].func = 0;
			}
		}
	}
}
void DeleteItem(int tempScope)
{
	noUseVariable(tempScope);
	for (int i = size - 1; i >= 0; i--)
	{
		if (table[i].scope == tempScope)
		{
			table[i].name = NULL;
			table[i].type = NULL;
			table[i].scope = 0;
			table[i].sizelist = 0;
			table[i].ParamSize = 0;
			free(table[i].param);
			free(table[i].list);
			size--;
		}
		else
			break;
	}
}
void noUseVariable(int tempScope)
{
	for (int i = size - 1; i >= 0; i--)
	{
		if (table[i].scope == tempScope && table[i].used == 0 && table[i].funcOrVar == 0)
		{
			fprintf(outputsemant, "\"%s\" no used in this declared variable in the \"%s\" \n", table[i].name, table[lastFunc].name);
		}
	}
	if (tempScope == 1)
	{
		for (int i = table[lastFunc].ParamSize; i > 0; i--)
		{
			if (table[lastFunc].param[i - 1].used == 0)
			{
				fprintf(outputsemant, "\"%s\" no used in this declared variable in the \"%s\" \n", table[lastFunc].param[i - 1].name, table[lastFunc].name);
			}
		}

	}
}
void DeleteAll()
{
	declarationOfFunctionWithoutImplementation();
	noUseGlobalVariable();
	for (int i = size - 1; i >= 0; i--)
	{
			table[i].name = NULL;
			table[i].type = NULL;
			table[i].scope = 0;
			table[i].sizelist = 0;
			table[i].ParamSize = 0;
			free(table[i].param);
			free(table[i].list);
	}
	size = 0;
	tempScope = 0;
	parse_var = 0;
	lastFunc = 0;
	numPrams = 0;
	numberVar = 0;
	numberVarOfFunc = 0;
	legalNumberLeft = -1;
	legalNumberRight = -1;
	returnNumber = -1;
	Number = 0;
	dec = 0;
	flagOfAllocationParam = 0;
	idArr = 0;
	numberOfcell = 0;
	tempintNumber = -1;
	flagPlus = 0;
	flagMulti = 0;
	lastArr = -1;
	lastParamArr = -1;
	callfunc = -1;
	callSize = 0;
	parse_call = 0;
	typeLastVar = -1;
}
void noUseGlobalVariable()
{
	for (int i = size - 1; i >= 0; i--)
	{
		if (table[i].scope == 0 && table[i].used == 0 && table[i].funcOrVar == 0)
		{
			fprintf(outputsemant, "\"%s\" no use in this declared global variable\n", table[i].name);
		}
	}
}
int isExistsVar(char *id ) // if id exists in parse_var
{
	if (t->kind == TOKEN_INT_NUMBER)
		return 1; //int

	if (t->kind == TOKEN_FLOAT_NUMBER)
		return 0; //float

	if (size > 0)
	{
		for (int i = size - 1; i >= 0; i--)
		{
			if (strcmp(table[i].name, id) == 0)
			{
				table[i].used = 1;
				if (strcmp(table[i].type, "int") == 0)
				{
					typeLastVar = 1;
					return 1; //int
				}
				else
				{
					typeLastVar = 0;
					return 0; //float
				}
			}

			if (i == lastFunc)
			{
				for (int j = 0; j </*=*/ table[lastFunc].ParamSize; j++)
				{
						if (strcmp(table[lastFunc].param[j].name, id) == 0)
						{
							table[lastFunc].param[j].used = 1;
							if (strcmp(table[lastFunc].param[j].type, "int") == 0)
							{
								typeLastVar = 1;
								return 1; //int
							}
							else
							{
								typeLastVar = 0;
								return 0; //float
							}
						}
				}
			}
		}
		fprintf(outputsemant, "Line %d : \"%s\" the type of the expression is undefined\n", number_line, id);
	}
	return -1;
}
void findcallfunc(char *id) // if id exists in parse_var
{
	if (size > 0)
	{
		for (int i = size - 1; i >= 0; i--)
		{
			if (strcmp(table[i].name, id) == 0)
			{
				callfunc = i;
				break;
			}
		}
	}
}
int isExistsfunc(char *id) // if exists func
{
	if (size > 0)
	{
		for (int i = size - 1; i >= 0; i--)
		{
			if (strcmp(table[i].name, id) == 0)
			{
				if (table[i].funcOrVar == 0)
					return 0; // not func
				if (table[i].funcOrVar == 1)
					return 1; // func
			}
		}
	}
	return -1;
}
void isFuncVoid()
{
	if (strcmp(table[lastFunc].type, "void") == 0)
		fprintf(outputsemant, "Line %d : \"%s\" 'void' function returning a value\n", number_line, table[lastFunc].name);
}
int funcParam()
{
	if (strcmp(table[lastFunc].type, "int") == 0)
		return 1;//int
	
	if (strcmp(table[lastFunc].type, "float") == 0)
		return 0;//float
	return -1;
}
int isIdarr()
{
	if (size > 0)
	{
		for (int i = size - 1; i >= 0; i--)
		{
			if (strcmp(table[i].name, t->lexeme) == 0)
			{
				if (table[i].sizelist > 0)
				{
					lastArr = i;
					return 1; // id is arr
				}
				else
					return 0; // not arr
			}

			if (i == lastFunc)
			{
				for (int j = 0; j </*=*/ table[lastFunc].ParamSize; j++)
				{

					if (strcmp(table[lastFunc].param[j].name, t->lexeme) == 0)
					{
						if (table[lastFunc].param[j].sizelist > 0)
						{
							lastArr = lastFunc;
							lastParamArr = j;
							return 1; // id is arr
						}
						else
							return 0; // not arr
					}
				}
			}
		}
	}
	return -1;
}
void checkArrayParam()
{
	if (lastArr != lastFunc && lastArr != -1)
	{
		if (numberOfcell <= table[lastArr].sizelist)
		{
			if (table[lastArr].list[numberOfcell] < tempintNumber)
			{
				fprintf(outputsemant, "Exceeding the size of the array in line %d\n", number_line);
			}
		}
		else
		{
			fprintf(outputsemant, "Improper array size in line %d\n", number_line);
		}
	}
	else if (lastArr == lastFunc)
	{
		if (numberOfcell < table[lastFunc].param[lastParamArr].sizelist)
		{
			if (table[lastFunc].param[lastParamArr].list[numberOfcell] < tempintNumber)
			{
				fprintf(outputsemant, "Exceeding the size of the array in line %d\n", number_line);
			}
		}
		else
		{
			fprintf(outputsemant, "Improper array size in line %d\n", number_line);
		}
	}

}
void declarationOfFunctionWithoutImplementation()
{
	for (int i = size; i >= 0; i--)
	{
		if (table[i].func == 1)
		{
			fprintf(outputsemant, "The function \"%s\" declaration of function without implementation\n", table[i].name);
		}
	}
}
void arrcall()
{
	callarr = (int*)calloc(table[callfunc].ParamSize, sizeof(int));
}
void checkarrcall()
{
	if (callSize <= table[callfunc].ParamSize)
	{
		for (int i = 0; i < table[callfunc].ParamSize; i++)
		{
			if (callarr[i] == 0)
			{
				fprintf(outputsemant, "\"%s\" number of parameters does not match function declaration\n", table[callfunc].name);
				break;
			}
		}
	}
}
void matchBetweendecTocallNumber(char *id)
{
	if (callSize < table[callfunc].ParamSize)
	{
			if (strcmp(table[callfunc].param[callSize].type, id) == 0)
				callarr[callSize] = 1;
			else 
			{
				callarr[callSize] = 0;
			}
	}
	else
	{
		fprintf(outputsemant, "\"%s\" number of parameters does not match function declaration\n", table[callfunc].name);
	}
	callSize++;
}
void matchBetweendecTocallid()
{
	if (callSize < table[callfunc].ParamSize)
	{
		if (strcmp(table[callfunc].param[callSize].type, "int") == 0 && typeLastVar ==1)
			callarr[callSize] = 1;
		else if (strcmp(table[callfunc].param[callSize].type, "float") == 0 && typeLastVar == 0)
		{
			callarr[callSize] = 1;
		}
		else
		{
			callarr[callSize] = 0;
		}
	}
	else
	{
		fprintf(outputsemant, "\"%s\" number of parameters does not match function declaration\n", table[callfunc].name);
	}
	callSize++;
}