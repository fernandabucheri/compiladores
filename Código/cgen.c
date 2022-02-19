/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the TINY compiler                            */
/* (generates code for the TM machine)              */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/*                     Adaptado                     */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

#define MAX_ARGS 4

void codeGen(TreeNode *syntaxTree);
static int cGen(TreeNode *t);
static int genExpression(TreeNode *t);
static int genStatement(TreeNode *t);

static int temporary = 0, label = 0, findSibling = 1, currentLabel = 0;
char temp1[20];
char temp2[20];
char temp3[20];
char temp4[20];

void codeGen(TreeNode *syntaxTree)
{
   cGen(syntaxTree);
}

static int cGen(TreeNode *t)
{
   int auxTemp = -1;
   if (t != NULL)
   {
      if (t->nodekind == StatementK)
         auxTemp = genStatement(t);

      else if (t->nodekind == ExpressionK)
         auxTemp = genExpression(t);

      if (t->sibling != NULL && findSibling == 1)
         auxTemp = cGen(t->sibling);
   }
   return auxTemp;
}

static int genExpression(TreeNode *t)
{
   int aux1, aux2;
   int operando1, operando2;

   switch (t->kind.expression)
   {
   case OpK: // operações lógicas e aritméticas
      operando1 = cGen(t->child[0]);
      operando2 = cGen(t->child[1]);
      if (t->attr.op == PLUS)
         sprintf(temp1, "PLUS");
      else if (t->attr.op == MINUS)
         sprintf(temp1, "MINUS");
      else if (t->attr.op == TIMES)
         sprintf(temp1, "TIMES");
      else if (t->attr.op == OVER)
         sprintf(temp1, "OVER");
      else if (t->attr.op == EQUAL)
         sprintf(temp1, "NOTEQUAL");
      else if (t->attr.op == NOTEQUAL)
         sprintf(temp1, "EQUAL");
      else if (t->attr.op == LESSTHAN)
         sprintf(temp1, "GREATERTHANEQUAL");
      else if (t->attr.op == GREATERTHAN)
         sprintf(temp1, "LESSTHANEQUAL");
      else if (t->attr.op == LESSTHANEQUAL)
         sprintf(temp1, "GREATERTHAN");
      else if (t->attr.op == GREATERTHANEQUAL)
         sprintf(temp1, "LESSTHAN");

      sprintf(temp2, "t%d", operando1);
      sprintf(temp3, "t%d", operando2);
      sprintf(temp4, "t%d", temporary);
      emitInstruction(temp1, temp2, temp3, temp4);

      return temporary++;
      break;

   case ConstantK: // constante
      sprintf(temp1, "CONST");
      sprintf(temp2, "t%d", temporary);
      sprintf(temp3, "%d", t->attr.val);
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);
      return temporary++;
      break;

   case IdK: // carregar um dado
      sprintf(temp1, "LOAD");
      sprintf(temp2, "t%d", temporary);
      sprintf(temp3, "%s", t->attr.name);
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);
      return temporary++;
      break;

   case VectK: // carregar um dado
      aux1 = cGen(t->child[0]);
      sprintf(temp1, "LOAD");
      sprintf(temp2, "t%d", temporary);
      sprintf(temp3, "%s", t->attr.name);
      sprintf(temp4, "t%d", aux1);
      emitInstruction(temp1, temp2, temp3, temp4);
      temporary++;
      return (temporary - 1);
      break;

   case TypeK:
      cGen(t->child[0]);
      break;

   default:
      break;
   }
}

static int genStatement(TreeNode *t)
{
   TreeNode *p1, *p2, *p3, *brother;
   int aux1, aux2, aux3;
   int pos, cont, parameter;
   int while_init, while_end;
   char *name;

   switch (t->kind.statement)
   {
   case IfK:
      p1 = t->child[0];
      p2 = t->child[1];
      p3 = t->child[2];

      aux1 = cGen(p1);
      sprintf(temp1, "IF");
      sprintf(temp2, "t%d", aux1);
      sprintf(temp3, "L%d", label);
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);

      cGen(p2);
      sprintf(temp1, "GOTO");
      sprintf(temp2, "L%d", label + 1);
      sprintf(temp3, "-");
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);
      emitLabel(label);
      label++;

      cGen(p3);
      emitLabel(label);
      label++;
      break;

   case AssignK: // atribuição
      p1 = t->child[0];
      p2 = t->child[1];

      if (p1->child[0] != NULL)
         pos = cGen(p1->child[0]);
      else
         pos = -1;

      aux3 = cGen(p2);
      sprintf(temp1, "ASSIGN");
      sprintf(temp2, "t%d", aux3);
      sprintf(temp3, "%s", p1->attr.name);

      if (pos == -1)
      {
         sprintf(temp4, "-");
         emitInstruction(temp1, temp2, temp3, temp4);
      }
      else
      {
         sprintf(temp4, "t%d", pos);
         emitInstruction(temp1, temp2, temp3, temp4);
      }
      temporary = 0;
      break;

   case VariableK: // aloca uma nova variável
      sprintf(temp1, "ALLOC");
      sprintf(temp2, "%s", t->attr.name);
      sprintf(temp3, "%s", t->attr.scope);
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);
      break;

   case VectorK: // aloca um novo vetor
      sprintf(temp1, "ALLOC_VECTOR");
      sprintf(temp2, "%s", t->attr.name);
      sprintf(temp3, "%s", t->attr.scope);
      sprintf(temp4, "%d", t->attr.len);
      emitInstruction(temp1, temp2, temp3, temp4);
      return -1;
      break;

   case WhileK:
      p1 = t->child[0];
      p2 = t->child[1];

      while_init = label;
      emitLabel(while_init);
      label++;
      while_end = label;
      label++;

      aux2 = cGen(p1);

      sprintf(temp1, "IF");
      sprintf(temp2, "t%d", aux2);
      sprintf(temp3, "L%d", while_end);
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);

      aux1 = cGen(p2);

      sprintf(temp1, "GOTO");
      sprintf(temp2, "L%d", while_init);
      sprintf(temp3, "-");
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);

      emitLabel(while_end);
      break;

   case ParameterK: // carrega os parametros (argumentos) de uma função
      sprintf(temp1, "ARGUMENT");
      if (t->specifier_type == IntK)
         sprintf(temp2, "INT");
      else if (t->specifier_type == VoidK)
         sprintf(temp2, "VOID");
      sprintf(temp3, "%s", t->attr.name);
      sprintf(temp4, "%s", t->attr.scope);
      emitInstruction(temp1, temp2, temp3, temp4);
      break;

   case FunctionK: // função
      p1 = t->child[0];
      p2 = t->child[1];

      sprintf(temp1, "FUNCTION");
      if (t->specifier_type == IntK)
         sprintf(temp2, "INT");
      else if (t->specifier_type == VoidK)
         sprintf(temp2, "VOID");
      sprintf(temp3, "%s", t->attr.name);
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);

      cGen(p1);
      emitLabel(label);
      label++;
      currentLabel = label;
      label++;

      cGen(p2);
      emitLabel(currentLabel);

      sprintf(temp1, "END");
      sprintf(temp2, "%s", t->attr.name);
      sprintf(temp3, "-");
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);
      break;

   case CallK: // chamando uma função
      p1 = t->child[0];

      findSibling = 0;
      name = t->attr.name;
      cont = 0;

      if (p1 != NULL)
      {
         parameter = cGen(p1);

         sprintf(temp1, "PARAMETER");
         sprintf(temp2, "t%d", parameter);
         sprintf(temp3, "-");
         sprintf(temp4, "-");
         emitInstruction(temp1, temp2, temp3, temp4);
         cont++;

         for (brother = p1->sibling; brother != NULL; brother = brother->sibling)
         {
            parameter = cGen(brother);
            sprintf(temp1, "PARAMETER");
            sprintf(temp2, "t%d", parameter);
            sprintf(temp3, "-");
            sprintf(temp4, "-");
            emitInstruction(temp1, temp2, temp3, temp4);
            cont++;
         }
      }
      sprintf(temp1, "CALL");
      sprintf(temp2, "t%d", temporary);
      sprintf(temp3, "%s", name);
      sprintf(temp4, "%d", cont);
      emitInstruction(temp1, temp2, temp3, temp4);

      sprintf(temp1, "PLUS");
      sprintf(temp2, "$zero");
      sprintf(temp3, "$v0");
      sprintf(temp4, "t%d", temporary);
      emitInstruction(temp1, temp2, temp3, temp4);

      findSibling = 1;
      return temporary++;
      break;

   case ReturnK:
      p1 = t->child[0];
      aux1 = cGen(p1);

      sprintf(temp1, "PLUS");
      sprintf(temp2, "$zero");
      sprintf(temp3, "t%d", aux1);
      sprintf(temp4, "$v0");
      emitInstruction(temp1, temp2, temp3, temp4);

      sprintf(temp1, "RETURN");
      sprintf(temp2, "$v0");
      sprintf(temp3, "-");
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);

      sprintf(temp1, "GOTO");
      sprintf(temp2, "L%d", currentLabel);
      sprintf(temp3, "-");
      sprintf(temp4, "-");
      emitInstruction(temp1, temp2, temp3, temp4);
      break;

   default:
      break;
   }
}