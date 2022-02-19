/****************************************************/
/* File: code.h                                     */
/* Code emitting utilities for the TINY compiler    */
/* and interface to the TM machine                  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/*                     Adaptado                     */
/****************************************************/

#ifndef _CODE_H_
#define _CODE_H_

void emitComment(char *c);
void emitLabel(int label);
void emitInstruction(char *op, char *first, char *second, char *third);

#endif