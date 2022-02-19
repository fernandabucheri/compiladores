/****************************************************/
/* File: code.c                                     */
/* TM Code emitting utilities                       */
/* implementation for the TINY compiler             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/*                     Adaptado                     */
/****************************************************/

#include "globals.h"
#include "code.h"

void emitComment(char *c)
{
  if (TraceCode)
    fprintf(listing, " %s\n", c);
}

void emitLabel(int label)
{
  fprintf(listing, "(LABEL,L%d,-,-)\n", label);
}

void emitInstruction(char *op, char *first, char *second, char *third)
{
  fprintf(listing, "(%s,%s,%s,%s)\n", op, first, second, third);
}