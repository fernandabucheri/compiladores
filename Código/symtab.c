/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/*                     Adaptado                     */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#include "globals.h"
#include "analyze.h"
#include <string.h>

#define SIZE 211
#define SHIFT 4

static int hash(char *name, char *scope)
{
  int temp = 0;
  int i = 0;
  while (name[i] != '\0')
  {
    temp = ((temp << SHIFT) + name[i]) % SIZE;
    i++;
  }
  i = 0;
  while (scope[i] != '\0')
  {
    temp = ((temp << SHIFT) + scope[i]) % SIZE;
    i++;
  }
  return temp;
}

/* the list of line numbers of the source
 * code in which a variable is referenced
 */
typedef struct LineListRec
{
  int lineno;
  struct LineListRec *next;
} * LineList;

/* The record in the bucket lists for
 * each variable, including name,
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
{
  int memloc; /* memory location for variable */
  char *name;
  char *scope;
  char *data_type;
  char *id_type;
  char *param_flag;
  char *vector_flag;
  int argsFunction;
  struct BucketListRec *next;
} * BucketList;

/* the hash table */
static BucketList hashTable[SIZE];

void insert(int memloc, char *name, char *scope, char *id_type, int data_type, int param_flag, int vector_flag, int argsFunction)
{
  int h;
  h = hash(name, scope);
  BucketList aux = hashTable[h];
  while ((aux != NULL) && ((strcmp(id_type, aux->id_type) != 0) || (strcmp(name, aux->name) != 0) || (strcmp(scope, aux->scope) != 0)))
    aux = aux->next;

  if (aux == NULL)
  {
    aux = (BucketList)malloc(sizeof(struct BucketListRec));
    aux->name = name;
    aux->scope = scope;

    if (data_type == 0)
      aux->data_type = "void";
    else if (data_type == 1)
      aux->data_type = "int";

    aux->id_type = id_type;

    if (param_flag == 0)
      aux->param_flag = "não";
    else if (param_flag == 1)
      aux->param_flag = "sim";
    else if (param_flag == -1)
      aux->param_flag = "-";

    if (vector_flag == 0)
      aux->vector_flag = "não";
    else if (vector_flag == 1)
      aux->vector_flag = "sim";
    else if (vector_flag == -1)
      aux->vector_flag = "-";

    aux->argsFunction = argsFunction;
    aux->memloc = memloc;
    aux->next = hashTable[h];
    hashTable[h] = aux;
  }
}

int search_index(char *name, char *scope, char *id_type)
{
  int h = hash(name, scope);
  BucketList aux = hashTable[h];

  while ((aux != NULL) && ((strcmp(id_type, aux->id_type) != 0) || (strcmp(name, aux->name) != 0) || (strcmp(scope, aux->scope) != 0)))
    aux = aux->next;

  if (aux == NULL)
    return -1;
  return aux->memloc;
}

/*
  Função search_data_id_param_vect ()
  Flag 1: retorna o tipo do dado
  Flag 2: retorna o tipo do id
  Flag 3: retorna se possui parâmetros
  Flag 4: retorna se é um vetor
*/

char *search_data_id_param_vect(char *name, char *scope, int flag)
{
  int h = hash(name, scope);
  BucketList aux = hashTable[h];
  while ((aux != NULL) && (strcmp(name, aux->name) != 0) && (strcmp(scope, aux->scope) != 0))
    aux = aux->next;

  if (aux == NULL)
    return "null";

  if (flag == 1)
    return aux->data_type;
  else if (flag == 2)
    return aux->id_type;
  else if (flag == 3)
    return aux->param_flag;
  else if (flag == 4)
    return aux->vector_flag;
}

int search_scope(char *name, char *scope)
{
  int h = hash(name, scope);
  BucketList aux = hashTable[h];
  while ((aux != NULL) && (strcmp(name, aux->name) != 0) && (strcmp(scope, aux->scope) != 0))
    aux = aux->next;
  if (aux == NULL)
    return -1;
  return 1;
}

int totalArgs(char *name)
{
  int h = hash(name, "global");
  BucketList aux = hashTable[h];
  if (aux == NULL)
    return -1;
  return aux->argsFunction;
}

void printSymTab(FILE *listing)
{
  int i;
  fprintf(listing, "Nome                Tipo      Escopo       \n");
  fprintf(listing, "------------------  -------   -----------  \n");
  for (i = 0; i < SIZE; i++)
  {
    if (hashTable[i] != NULL)
    {
      BucketList aux = hashTable[i];
      while (aux != NULL)
      {
        fprintf(listing, "%-20s", aux->name);
        fprintf(listing, "%-10s", aux->data_type);
        fprintf(listing, "%s", aux->scope);
        fprintf(listing, "\n");
        aux = aux->next;
      }
    }
  }
}