/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/*                     Adaptado                     */
/****************************************************/

%{
#define YYPARSER 
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h" 

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);

%}

%token ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES OVER
%token LESSTHAN LESSTHANEQUAL GREATERTHAN GREATERTHANEQUAL
%token EQUAL NOTEQUAL ASSIGN
%token SEMI COMMA
%token LPAREN RPAREN LBRACKET RBRACKET LKEY RKEY
%token ID NUM 
%token ENDFILE ERROR

%% /* Grammar for CMENOS */

program		     :   declaration_list
				     { savedTree = $1; }
			     ;
declaration_list	:   declaration_list declaration
            	            {
             	               YYSTYPE t = $1;
             	               if(t != NULL)
				     	{ while(t->sibling != NULL)
             	                       t = t->sibling;
             	                   t->sibling = $2;
             	                   $$ = $1; }
             	               else $$ = $2;
             	           }
			     |   declaration 
				     { $$ = $1; }
			;
declaration		:   variable_declaration 
				{ $$ = $1; }
			|   function_declaration 
				{ $$ = $1; }
			;
variable_declaration 	:   specifier_type identifier SEMI
			{
                            $$ = $1;
                            $$->child[0] = $2;
                            $2->nodekind = StatementK;
                            $2->kind.statement = VariableK;
                            $2->specifier_type = $1->specifier_type;
			}
			|   specifier_type identifier LBRACKET number RBRACKET SEMI
			{
                            $$ = $1;
                            $$->child[0] = $2;
                            $2->nodekind = StatementK;
                            $2->kind.statement = VectorK;
                            $2->attr.len = $4->attr.val;
                            $2->specifier_type = $1->specifier_type;
                       }
			;
specifier_type		:   INT
			{
                            $$ = newExpressionNode(TypeK);
                            $$->specifier_type = IntK;
                            $$->attr.name = "Inteiro";
                       }

                    	|   VOID
                       {
                            $$ = newExpressionNode(TypeK);
                            $$->specifier_type = VoidK;
                            $$->attr.name = "Void";
                       }
			;
function_declaration   :   specifier_type identifier LPAREN parameters RPAREN compound_declaration 
                       {
                            $$ = $1;
                            $$->child[0] = $2;
                            $2->child[0] = $4;
                            $2->child[1] = $6;
                            $2->nodekind = StatementK;
                            $2->kind.statement = FunctionK;
                            $2->specifier_type = $1->specifier_type;
                       }
			;
parameters		:   parameter_list
                       {
                           $$ = $1;
                       }
			     |   VOID
                       {
                       }
			     ;
parameter_list		:   parameter_list COMMA parameter
                       {
                           YYSTYPE t = $1;
                           if(t != NULL)
                           {
				while(t->sibling != NULL)
                                  t = t->sibling;
				t->sibling = $3;
				$$ = $1;
                           }
                            else
                              $$ = $1;
                       }
			|   parameter
                       {
                            $$ = $1;
                       }
			;
parameter		:   specifier_type identifier
                       {
                           $$ = $1;
                           $$->child[0] = $2;
                           $2->nodekind = StatementK;
                           $2->kind.statement = ParameterK;
                           $2->specifier_type = $1->specifier_type;

                       }
			|   specifier_type identifier LBRACKET RBRACKET
			{
                            $$ = $1;
                            $$->child[0] = $2;
                            $2->nodekind = StatementK;
                            $2->kind.expression = VectK;
                            $2->kind.statement = ParameterK;
                            $2->specifier_type = $1->specifier_type;
                       }
               	;
compound_declaration  	:   LKEY local_declarations statement_list RKEY
                       {
                            YYSTYPE t = $2;
                            if(t != NULL)
                            {
                               while(t->sibling != NULL)
                                  t = t->sibling;
                                t->sibling = $3;
                                $$ = $2;
                            }
                            else
                               $$ = $3;
                       }
			|   LKEY local_declarations RKEY
                       {
                            $$ = $2;
                       }
	               |   LKEY statement_list RKEY
                       {
                            $$ = $2;
                       }
	               |   LKEY RKEY
                       {
                       }
			;
local_declarations  	:   local_declarations variable_declaration
                       {
                            YYSTYPE t = $1;
                            if(t != NULL)
                            {
                              while(t->sibling != NULL)
                                   t = t->sibling;
                              t->sibling = $2;
                              $$ = $1;
                            }
                            else
                               $$ = $2;
                       }
			|    variable_declaration
                       {
                            $$ = $1;
                       }
			;
statement_list 	:   statement_list statement
                       {
                           YYSTYPE t = $1;
                           if(t != NULL)
                           {
                              while(t->sibling != NULL)
                                   t = t->sibling;
                              t->sibling = $2;
                              $$ = $1;
                           }
                           else
                             $$ = $2;
			}
			|   statement
                       {
                           $$ = $1;
                       }
			;
statement 	     	:   expression_decl
                       {
                           $$ = $1;
                       }
               	|   compound_declaration
                       {
                           $$ = $1;
                       }
               	|   if_decl
                       {
                           $$ = $1;
                       }
               	|   loop_decl
                       {
                           $$ = $1;
                       }
			|   return_decl
                       {
                           $$ = $1;
                       }
               	;
expression_decl	:   expression SEMI
                       {
                           $$ = $1;
                       }
               	|   SEMI
                       {
                       }
               	;
if_decl	 	:   IF LPAREN expression RPAREN statement
                       {
                             $$ = newStatementNode(IfK);
                             $$->child[0] = $3;
                             $$->child[1] = $5;
                       }
               	|   IF LPAREN expression RPAREN statement ELSE statement
                       {
               
                             $$ = newStatementNode(IfK);
                             $$->child[0] = $3;
                             $$->child[1] = $5;
                             $$->child[2] = $7;
                       }
               	;
loop_decl	  	:   WHILE LPAREN expression RPAREN statement
                       {
                             $$ = newStatementNode(WhileK);
                             $$->child[0] = $3;
                             $$->child[1] = $5;
                       }
               	;
return_decl        	:   RETURN SEMI
                       {
                            $$ = newStatementNode(ReturnK);
                       }
	               |   RETURN expression SEMI
                       {
                            $$ = newStatementNode(ReturnK);
                            $$->child[0] = $2;
                       }
			;
expression      	:   var ASSIGN expression
                       {
                            $$ = newStatementNode(AssignK);
                            $$->child[0] = $1;
                            $$->child[1] = $3;
                       }
			|   simple_expression
                       {
                            $$ = $1;
                       }
			;
var			:   identifier
                       {
                            $$ = $1;
                       }
			|   identifier LBRACKET expression RBRACKET
                       {
                            $$ = $1;
                            $$->child[0] = $3;
                            $$->kind.expression = VectK;
                       }
			;
simple_expression	: sum_expression relational sum_expression
                       {
                            $$ = $2;
                            $$->child[0] = $1;
                            $$->child[1] = $3;
                       }
			|  sum_expression
                       {
                            $$ = $1;
                       }
			;
relational		:  EQUAL
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = EQUAL;                            
                       }
			|  NOTEQUAL
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = NOTEQUAL;                            
                       }
			|  LESSTHAN
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = LESSTHAN;                            
                       }
			|  LESSTHANEQUAL
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = LESSTHANEQUAL;                            
                       }
			|  GREATERTHAN
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = GREATERTHAN;                            
                       }
			|  GREATERTHANEQUAL
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = GREATERTHANEQUAL;                            
                       }
			;
sum_expression		:  sum_expression sum term
                       {
                            $$ = $2;
                            $$->child[0] = $1;
                            $$->child[1] = $3;
                       }
			|  term
                       {
                            $$ = $1;
                       }
			;
sum			:  PLUS
			{
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = PLUS;                            
			}
			| MINUS
			{
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = MINUS;                            
			}
			;
term			:   term mult factor
                       {
                            $$ = $2;
                            $$->child[0] = $1;
                            $$->child[1] = $3;
                       }
			|   factor
                       {
                            $$ = $1;
                       }
			;
mult 			:   TIMES
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = TIMES;                            
                       }
			|   OVER
                       {
                            $$ = newExpressionNode(OpK);
                            $$->attr.op = OVER;                            
                       }
			;
factor			:   LPAREN expression RPAREN
                       {
                            $$ = $2;
                       }
			|   var
                       {
                            $$ = $1;
                       }
			|   activation
                       {
                            $$ = $1;
                       }
			|   number
                       {
                            $$ = $1;
                       }
			;
activation		:   identifier LPAREN argument_list RPAREN
                       {
                            $$ = $1;
                            $$->child[0] = $3;
                            $$->nodekind = StatementK;
                            $$->kind.statement = CallK;
                       }
			|   identifier LPAREN RPAREN
			{
                            $$ = $1;
                            $$->nodekind = StatementK;
                            $$->kind.statement = CallK;
                       }                       
			;
argument_list		:   argument_list COMMA expression
                       {
                            YYSTYPE t = $1;
                             if(t != NULL)
                             {
                                while(t->sibling != NULL)
                                   t = t->sibling;
                                 t->sibling = $3;
                                 $$ = $1;
                             }
                             else
                                 $$ = $3;
                       }
			|  expression
                       {
                             $$ = $1;
                       }
			;
identifier             :   ID
                       {
                             $$ = newExpressionNode(IdK);
                             $$->attr.name = copyString(tokenString);
                       }
			;
number			:   NUM
                       {
                             $$ = newExpressionNode(ConstantK);
                             $$->attr.val = atoi(tokenString);
                       }
			;

%%

int yyerror(char* message){
     if(yychar != ENDFILE && !Error){
          fprintf(listing,"\nERRO SINTÁTICO: ");
          printToken(yychar,tokenString);
          fprintf(listing, "LINHA: %d\n",lineno);
          Error = TRUE;
     }
     return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}