#ifndef _SYMTAB_H_
#define _SYMTAB_H_

void insert(int memloc, char *name, char *scope, char *id_type, int data_type, int param_flag, int vector_flag, int argsFunction);
int search_index(char *name, char *scope, char *id_type);
int search_scope(char *name, char *scope);
char *search_data_id_param_vect(char *name, char *scope, int flag);
int totalArgs(char *name);
void printSymTab(FILE *listing);

#endif