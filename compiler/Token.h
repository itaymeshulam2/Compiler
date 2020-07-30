#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout, *output , *outputsemant;
extern int currentIndex;
extern int currentIndexTemp;
extern int flag;
typedef enum eTOKENS
{
	TOKEN_INTEGER = 0,
	TOKEN_IF = 1,
	TOKEN_WHILE = 2,
	TOKEN_OP_EQUAL = 3,
	TOKEN_INT_NUMBER = 4,
	TOKEN_ID = 5,
	TOKEN_FLOAT_NUMBER = 6,
	TOKEN_PLUS = 7,
	TOKEN_MULTI = 8,
	TOKEN_SMALLER = 9,
	TOKEN_BIDDER = 10,
	TOKEN_EQUAL = 11,
	TOKEN_UNEQUAL = 12,
	TOKEN_EOF = 13,
	TOKEN_SMALLER_EQUAL = 14,
	TOKEN_BIDDER_EQUAL = 15,
	TOKEN_ASSIGNMENT = 16,
	TOKEN_FLOAT = 17,
	TOKEN_VOID = 18,
	TOKEN_RETURN = 19,
	TOKEN_COMMA = 20,
	TOKEN_COLON = 21,
	TOKEN_SEMICOLON = 22,
	TOKEN_PARENTHESES_LEFT = 23,
	TOKEN_PARENTHESES_RIGHT = 24,
	TOKEN_BRACKETS_LEFT = 25,
	TOKEN_BRACKETS_RIGHT = 26,
	TOKEN_CURLY_BRACES_LEFT = 27,
	TOKEN_CURLY_BRACES_RIGHT = 28,
	TOKEN_COMMENTS = 29,


	/*
		Examples of tokens, add/change according to your needs.

		Alternative way to define tokens:
		#define TOKEN_INTEGER 1
		#define TOKEN_IF 2
		...........
	*/
}eTOKENS;


typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();

#endif