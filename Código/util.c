/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/*                     Adaptado                     */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char *tokenString)
{
  switch (token)
  {
  case INT:
  case VOID:
  case RETURN:
  case IF:
  case ELSE:
  case WHILE:
    fprintf(listing, "Palavra reservada: %s\n", tokenString);
    break;

  case PLUS:
    fprintf(listing, "+\n");
    break;
  case MINUS:
    fprintf(listing, "-\n");
    break;
  case TIMES:
    fprintf(listing, "*\n");
    break;
  case OVER:
    fprintf(listing, "/\n");
    break;

  case LESSTHAN:
    fprintf(listing, "<\n");
    break;
  case LESSTHANEQUAL:
    fprintf(listing, "<=\n");
    break;
  case GREATERTHAN:
    fprintf(listing, ">\n");
    break;
  case GREATERTHANEQUAL:
    fprintf(listing, ">=\n");
    break;

  case ASSIGN:
    fprintf(listing, "=\n");
    break;
  case EQUAL:
    fprintf(listing, "==\n");
    break;
  case NOTEQUAL:
    fprintf(listing, "!=\n");
    break;

  case SEMI:
    fprintf(listing, ";\n");
    break;
  case COMMA:
    fprintf(listing, ",\n");
    break;

  case LPAREN:
    fprintf(listing, "(\n");
    break;
  case RPAREN:
    fprintf(listing, ")\n");
    break;
  case LBRACKET:
    fprintf(listing, "[\n");
    break;
  case RBRACKET:
    fprintf(listing, "]\n");
    break;
  case LKEY:
    fprintf(listing, "{\n");
    break;
  case RKEY:
    fprintf(listing, "}\n");
    break;

  case ENDFILE:
    fprintf(listing, "EOF\n");
    break;

  case NUM:
    fprintf(listing, "Número: %s\n", tokenString);
    break;
  case ID:
    fprintf(listing, "ID: %s\n", tokenString);
    break;
  case ERROR:
    fprintf(listing, "\nERRO LÉXICO: %s LINHA: %d\n", tokenString, lineno); 
    Error = TRUE;
    break;
  default:
    fprintf(listing, "Token desconhecido: %d\n", token);
  }
}

/* Function newStatementNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStatementNode(StatementKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Erro! Falta de memória! Linha: %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StatementK;
    t->kind.statement = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpressionNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpressionNode(ExpressionKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Erro! Falta de memória! Linha: %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpressionK;
    t->kind.expression = kind;
    t->lineno = lineno;
    t->specifier_type = VoidK;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = malloc(n);
  if (t == NULL)
    fprintf(listing, "Erro! Falta de memória! Linha: %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++)
    fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  while (tree != NULL)
  {
    printSpaces();
    if (tree->nodekind == StatementK)
    {
      switch (tree->kind.statement)
      {
      case IfK:
        fprintf(listing, "If\n");
        break;
      case AssignK:
        fprintf(listing, "Atribuição\n");
        break;
      case WhileK:
        fprintf(listing, "While\n");
        break;
      case VariableK:
        fprintf(listing, "Variável: %s\n", tree->attr.name);
        break;
      case FunctionK:
        fprintf(listing, "Função: %s\n", tree->attr.name);
        break;
      case VectorK:
        fprintf(listing, "Vetor: %s\n", tree->attr.name);
        break;
      case ParameterK:
        fprintf(listing, "Parâmetro: %s\n", tree->attr.name);
        break;
      case CallK:
        fprintf(listing, "Chamando a função: %s\n", tree->attr.name);
        break;
      case ReturnK:
        fprintf(listing, "Return\n");
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == ExpressionK)
    {
      switch (tree->kind.expression)
      {
      case OpK:
        fprintf(listing, "Operação: ");
        printToken(tree->attr.op, "\0");
        break;
      case ConstantK:
        fprintf(listing, "Constante: %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Id: %s\n", tree->attr.name);
        break;
      case VectK:
        fprintf(listing, "Vetor: %s\n", tree->attr.name);
        break;
      case TypeK:
        fprintf(listing, "Tipo: %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else
      fprintf(listing, "Unknown ExpNode kind\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}