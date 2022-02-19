/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden   			                        */
/*                     Adaptado                     */
/****************************************************/

%{ 
#include "globals.h"
#include "util.h"
#include "scan.h" 
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

static int yylex(void); 

%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \r?\n
whitespace  [ \t\r]+

%%

"else"          {return ELSE;}
"if"            {return IF;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}

"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}

"<"             {return LESSTHAN;}
"<="            {return LESSTHANEQUAL;}
">"             {return GREATERTHAN;}
">="            {return GREATERTHANEQUAL;}

"=="            {return EQUAL;}
"!="            {return NOTEQUAL;}
"="             {return ASSIGN;}

";"             {return SEMI;}
","		          {return COMMA;}

"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACKET;}
"]"             {return RBRACKET;}
"{"             {return LKEY;}
"}"             {return RKEY;}

"/*"            { 
                  char c1 = ' ', c2 = ' ';
                  do {
                    c2 = c1;
                    c1 = input();
                    if (c1 == EOF) return ERROR;
                    if (c1 == '\n') lineno++;
                  } while ((c1 != '/') && (c2 != '*')); 
                }

{number}        {return NUM;}
{identifier}    {return ID;}

{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}

<<EOF>>	        {return ENDFILE;}
.		            {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
