%option noyywrap

%{ 
#include "Token.h"
#include "Parse.h"
int number_line;
%} 

%x comment1
DIGIT	[1-9]
DIGIT0	[0-9]	
ALPHA	[a-zA-Z]
ALPHANUM [a-zA-Z0-9]
ID 		[a-z]
E   	[eE]
POINT   [.]
PLUS	[+]
MULTI	[*]
SMALLER [<]
BIDDER  [>]
EQUAL   [=]
ENEQUAL [!]
ASSIGNMENT [=]
COMMA [,]
COLON [:]
SEMICOLON [;]
PARENTHESES_LEFT [(]
PARENTHESES_RIGHT [)]
BRACKETS_LEFT [[]
BRACKETS_RIGHT []]
CURLY_BRACES_LEFT [{]
CURLY_BRACES_RIGHT [}]
%%

(int)		           		{ create_and_store_token(TOKEN_INTEGER,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type int , lexeme %s\n", number_line, yytext);return 1;}
[f][l][o][a][t]             { create_and_store_token(TOKEN_FLOAT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type float , lexeme %s\n", number_line, yytext);return 1;}
[v][o][i][d]                { create_and_store_token(TOKEN_VOID,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type void , lexeme %s\n", number_line, yytext);return 1;}
[i][f]						{ create_and_store_token(TOKEN_IF,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type if , lexeme %s\n", number_line, yytext);return 1;}
[r][e][t][u][r][n]			{ create_and_store_token(TOKEN_RETURN,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type return , lexeme %s\n", number_line, yytext);return 1;}				
(({DIGIT}{DIGIT0}*)|{DIGIT0}){POINT}{DIGIT0}+{E}(["+"]?|[-]?){DIGIT0}+  { create_and_store_token(TOKEN_FLOAT_NUMBER,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type float number , lexeme %s\n", number_line, yytext);return 1;}


(({DIGIT}{DIGIT0}*)|{DIGIT0})		{ create_and_store_token(TOKEN_INT_NUMBER,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type int number , lexeme %s\n", number_line, yytext);return 1;}
{PLUS}							{ create_and_store_token(TOKEN_PLUS,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type plus , lexeme %s\n", number_line, yytext);return 1;}
{MULTI}							{ create_and_store_token(TOKEN_MULTI,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type multi , lexeme %s\n", number_line, yytext);return 1;}
{SMALLER}						{ create_and_store_token(TOKEN_SMALLER,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type < , lexeme %s\n", number_line, yytext);return 1;}
{BIDDER}						{ create_and_store_token(TOKEN_BIDDER,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type > , lexeme %s\n", number_line, yytext);return 1;}
{EQUAL}{EQUAL}					{ create_and_store_token(TOKEN_EQUAL,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type == , lexeme %s\n", number_line, yytext);return 1;}
{ENEQUAL}{EQUAL}				{ create_and_store_token(TOKEN_UNEQUAL,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type != , lexeme %s\n", number_line, yytext);return 1;}
{SMALLER}{EQUAL}				{ create_and_store_token(TOKEN_SMALLER_EQUAL,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type <= , lexeme %s\n", number_line, yytext);return 1;}
{BIDDER}{EQUAL}					{ create_and_store_token(TOKEN_BIDDER_EQUAL,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type >= , lexeme %s\n", number_line, yytext);return 1;}
{ASSIGNMENT}					{ create_and_store_token(TOKEN_ASSIGNMENT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type = , lexeme %s\n", number_line, yytext);return 1;}					
{ID}([_]{ALPHANUM}|{ALPHANUM})* { create_and_store_token(TOKEN_ID,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type id , lexeme %s\n", number_line, yytext);return 1;}
{COMMA}							{ create_and_store_token(TOKEN_COMMA,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type comma , lexeme %s\n", number_line, yytext);return 1;}
{COLON} 						{ create_and_store_token(TOKEN_COLON,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type colon , lexeme %s\n", number_line, yytext);return 1;}
{SEMICOLON}						{ create_and_store_token(TOKEN_SEMICOLON,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type semicolon , lexeme %s\n", number_line, yytext);return 1;}
{PARENTHESES_LEFT}				{ create_and_store_token(TOKEN_PARENTHESES_LEFT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type ( , lexeme %s\n", number_line, yytext);return 1;}
{PARENTHESES_RIGHT}				{ create_and_store_token(TOKEN_PARENTHESES_RIGHT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type ) , lexeme %s\n", number_line, yytext);return 1;}
{BRACKETS_LEFT}					{ create_and_store_token(TOKEN_BRACKETS_LEFT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type [ , lexeme %s\n", number_line, yytext);return 1;}
{BRACKETS_RIGHT}				{ create_and_store_token(TOKEN_BRACKETS_RIGHT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type ] , lexeme %s\n", number_line, yytext);return 1;}			
{CURLY_BRACES_LEFT}				{ create_and_store_token(TOKEN_CURLY_BRACES_LEFT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type { , lexeme %s\n", number_line, yytext);return 1;}
{CURLY_BRACES_RIGHT}			{ create_and_store_token(TOKEN_CURLY_BRACES_RIGHT,  yytext, number_line);
								fprintf(yyout,"Line %d : found token of type } , lexeme %s\n", number_line, yytext);return 1;}


"/*"  BEGIN(comment1);
<comment1>[^*\n]*          /*do nothing till ‘*’  */
<comment1>[\n]                    {++number_line;}  /*delete new line in multi-line comment*/
<comment1>"*/"  BEGIN(0);      


<<EOF>>    						{create_and_store_token(TOKEN_EOF, "", number_line);  
								fprintf(yyout,"Line %d : found token of type EOF",number_line);return 0;}	
[\n] 							{++number_line;}
[ \t]							{}
.				       			fprintf(yyout, "Line %d : Unrecognized character!: %s\n",number_line, yytext );

%%

int main(int argc, char **argv)
{
	int i = 0;
	number_line = 1;
	while (1)
	{
		while (1)
		{
			printf("Insert 1 for Test1 or insert 2 for Test2 or insert 0 to exit\n");
			scanf("%d", &i);

			if (i == 0)
				break;

			if (i == 1)
			{
				if ((yyin = fopen("C:\\temp\\test1.txt", "r")) == NULL)
				{
					printf("Error! opening file");
					// Program exits if file pointer returns NULL.
					exit(1);
				}
				break;
			}
			else if (i == 2)
			{
				if ((yyin = fopen("C:\\temp\\test2.txt", "r")) == NULL)
				{
					printf("Error! opening file");
					// Program exits if file pointer returns NULL.
					exit(1);
				}
				break;
			}
			else
				printf("Wrong number\n");
		}

		if (i == 0)
			break;
		if (i == 1)
		{
			yyout = fopen("C:\\temp\\test1_205613516_308207604_lex.txt", "w");
			output = fopen("C:\\temp\\test1_205613516_308207604_syntactic.txt", "w");
			outputsemant = fopen("C:\\temp\\test1_205613516_308207604_semantic.txt", "w");
		}
		else if (i == 2)
		{
			yyout = fopen("C:\\temp\\test2_205613516_308207604_lex.txt", "w");
			output = fopen("C:\\temp\\test2_205613516_308207604_syntactic.txt", "w");
			outputsemant = fopen("C:\\temp\\test2_205613516_308207604_semantic.txt", "w");
		}

		parse_PROG();
		match(TOKEN_EOF);

		yyrestart(yyin);
		DeleteAll();
		fclose(yyin);
		fclose(yyout);
		fclose(output);
		fclose(outputsemant);
		currentIndexTemp = 0;
		flag = 0;
		number_line = 1;
	}
}