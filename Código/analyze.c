/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/*                     Adaptado                     */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <string.h>

static int location = 0;

char *currentScope;

static void printError(TreeNode *t, char *msg)
{
  if (t->attr.name == NULL)
    fprintf(listing, "ERRO SEMÂNTICO: %s LINHA: %d\n", msg, t->lineno);
  else
    fprintf(listing, "ERRO SEMÂNTICO: %s (%s) LINHA: %d\n", t->attr.name, msg, t->lineno);
  Error = TRUE;
}

static void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *))
{
  if (t != NULL)
  {
    preProc(t);
    {
      int i;
      for (i = 0; i < MAXCHILDREN; i++)
      {
        traverse(t->child[i], preProc, postProc);
      }
    }
    postProc(t);
    {
      traverse(t->sibling, preProc, postProc);
    }
  }
}

static void nullProc(TreeNode *t)
{
  if (t == NULL)
    return;
  else
    return;
}

static void verify(TreeNode *t)
{
  TreeNode *aux;
  int vet = 0;
  int contArgs = 0;
  int checkArgs = 0;

  if (t->nodekind == StatementK)
  {
    switch (t->kind.statement)
    {
    case VariableK:
    case VectorK:
      if (t->specifier_type == VoidK)
        printError(t, "Variavel declarada como void");
      else if ((search_index(t->attr.name, "global", "função") == 0))
        printError(t, "Uma função já possui esse nome");
      else if ((search_scope(t->attr.name, t->attr.scope) == -1))
        insert(location++, t->attr.name, t->attr.scope, "variável", t->specifier_type, 0, 0, 0);
      else
        printError(t, "A variável já foi declarada");
      break;

    case ParameterK:
      if (t->specifier_type == VoidK)
        printError(t, "Variavel declarada como void");
      else if ((search_index(t->attr.name, "global", "função") == 0))
        printError(t, "Uma função já possui esse nome");
      else if (search_index(t->attr.name, t->attr.scope, "variável") == -1)
        insert(location++, t->attr.name, t->attr.scope, "variável", t->specifier_type, 1, -1, 0);
      else if (search_scope(t->attr.name, t->attr.scope) == -1 && (strcmp(search_data_id_param_vect(t->attr.name, t->attr.scope, 4), "sim") || strcmp(search_data_id_param_vect(t->attr.name, "global", 4), "sim")))
      {
        t->kind.expression = VectK;
        vet = 1;
        insert(location++, t->attr.name, t->attr.scope, "variável", t->specifier_type, 1, vet, 0);
        vet = 0;
      }
      else
        printError(t, "Variável já foi declarada");
      break;

    case FunctionK:
      for (aux = t->child[1]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.name;

      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
      {
        if (aux->nodekind == ExpressionK && aux->kind.expression == TypeK)
        {
          aux->attr.scope = t->attr.name;
          contArgs++;
        }
      }
      if (search_index(t->attr.name, t->attr.scope, "função") == -1)
        insert(0, t->attr.name, t->attr.scope, "função", t->specifier_type, -1, -1, contArgs);
      else
        printError(t, "Nome já foi declarado");
      break;

    case CallK:
      if (strcmp(search_data_id_param_vect(t->attr.name, "global", 2), "null") == 0 && strcmp(t->attr.name, "input") != 0 && strcmp(t->attr.name, "output"))
        printError(t, "A função não foi declarada");

      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        contArgs++;

      if (totalArgs(t->attr.name) != -1 && totalArgs(t->attr.name) != contArgs)
        printError(t, "Número de argumentos incorreto para a função");

      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;
      break;

    case WhileK:
      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      for (aux = t->child[1]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      break;

    case IfK:
      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      for (aux = t->child[1]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      for (aux = t->child[2]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      break;

    case AssignK:
      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      for (aux = t->child[1]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      if (t->child[1]->kind.statement == CallK && strcmp(search_data_id_param_vect(t->child[1]->attr.name, "global", 1), "int") != 0 && strcmp(search_data_id_param_vect(t->child[1]->attr.name, "global", 1), "null") != 0)
        printError(t->child[1], "Esperando retorno de uma função void");
      break;

    case ReturnK:
      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;
      break;

    default:
      break;
    }
  }
  else if (t->nodekind == ExpressionK)
  {
    switch (t->kind.expression)
    {
    case IdK:
      if (search_index(t->attr.name, t->attr.scope, "variável") == -1 && search_index(t->attr.name, "global", "variável") == -1)
        printError(t, "A variável não foi declarada");
      else
      {
        if (t->kind.expression == 3)
          vet = 1;
        else
          vet = 0;
        if (search_scope(t->attr.name, "global") == 1)
          insert(0, t->attr.name, "global", "variável", t->specifier_type, -1, vet, 0);
        else
          insert(0, t->attr.name, t->attr.scope, "variável", t->specifier_type, -1, -1, 0);
        vet = 0;
      }
      break;

    case TypeK:
      t->child[0]->attr.scope = t->attr.scope;
      break;

    case VectK:
      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      if (search_index(t->attr.name, t->attr.scope, "variável") == -1 && search_index(t->attr.name, "global", "variável") == -1)
        printError(t, "A variável não foi declarada");
      else if (search_scope(t->attr.name, "global") == 1)
        insert(0, t->attr.name, "global", "variável", t->specifier_type, -1, -1, 0);
      else
        insert(0, t->attr.name, t->attr.scope, "variável", t->specifier_type, -1, -1, 0);
      break;

    case OpK:
      for (aux = t->child[0]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;

      for (aux = t->child[1]; aux != NULL; aux = aux->sibling)
        aux->attr.scope = t->attr.scope;
      break;

    default:
      break;
    }
  }
}

void buildSymtab(TreeNode *syntaxTree)
{
  TreeNode *aux;
  syntaxTree->attr.scope = "global";

  for (aux = syntaxTree->sibling; aux != NULL; aux = aux->sibling)
  {
    if (aux->nodekind == ExpressionK)
      if (aux->kind.expression == TypeK)
        aux->attr.scope = "global";
  }

  traverse(syntaxTree, verify, nullProc);

  if (search_index("main", "global", "função") == -1)
  {
    fprintf(listing, "\nERRO SEMÂNTICO: Função main não foi declarada\n");
    Error = TRUE;
  }

  if (TraceAnalyze)
  {
    fprintf(listing, "\nSymbol Table:\n\n");
    printSymTab(listing);
  }
}

static void checkNode(TreeNode *t)
{

  if (t->attr.scope != NULL)
    currentScope = t->attr.scope;

  if (t->nodekind == ExpressionK)
  {
    switch (t->kind.expression)
    {
    case OpK:
      if (t->child[0]->specifier_type != IntK)
        printError(t->child[0], "A variável não é do tipo int");
      if (t->child[1]->specifier_type != IntK)
        printError(t->child[1], "A variável não é do tipo int");
      if ((t->attr.op == ASSIGN) || (t->attr.op == LESSTHANEQUAL) || (t->attr.op == GREATERTHAN) || (t->attr.op == GREATERTHANEQUAL) || (t->attr.op == LESSTHAN) || (t->attr.op == NOTEQUAL))
        t->specifier_type = IntK;
      else
        t->specifier_type = IntK;
      break;

    case ConstantK:
      t->specifier_type = IntK;
      break;

    case VectK:
      if (strcmp(search_data_id_param_vect(t->attr.name, currentScope, 1), "int") == 0)
        t->specifier_type = IntK;
      else if (strcmp(search_data_id_param_vect(t->attr.name, "global", 1), "int") == 0)
        t->specifier_type = IntK;
      else if (strcmp(search_data_id_param_vect(t->attr.name, currentScope, 1), "null") == 0 || strcmp(search_data_id_param_vect(t->attr.name, "global", 1), "null") == 0)
        t->specifier_type = VoidK;
      else
        printError(t, "Variável não foi declarada");
      break;

    case IdK:
      if (strcmp(search_data_id_param_vect(t->attr.name, currentScope, 1), "int") == 0)
        t->specifier_type = IntK;
      else if (strcmp(search_data_id_param_vect(t->attr.name, "global", 1), "int") == 0)
        t->specifier_type = IntK;
      else if (strcmp(search_data_id_param_vect(t->attr.name, currentScope, 1), "null") == 0 || strcmp(search_data_id_param_vect(t->attr.name, "global", 1), "null") == 0)
        t->specifier_type = VoidK;
      else
        printError(t, "Variável não foi declarada");
      break;

    default:
      break;
    }
  }
  else if (t->nodekind == StatementK)
  {
    switch (t->kind.statement)
    {
    case AssignK:
      if (t->child[0]->specifier_type != IntK)
        printError(t->child[0], "Atribuição para uma variável do tipo não inteiro");
      break;

    case IfK:
      if (t->child[0]->specifier_type != IntK)
        printError(t, "Operação inválida");
      break;

    default:
      break;
    }
  }
}

void typeCheck(TreeNode *syntaxTree)
{
  traverse(syntaxTree, nullProc, checkNode);
}