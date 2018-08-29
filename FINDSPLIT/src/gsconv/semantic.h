#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "lisp.h"
#include <vector>

typedef struct {
  char * symbol          ;
  char * type_name       ;
  cell * note            ;
  cell * history         ;
} identifier_stack_entry ;

extern std::vector<identifier_stack_entry> identifier_stack ;

extern int find_identifier(char * symbol) ;
extern int find_identifier(cell * var);
extern void push_identifier(
  char * symbol, char * type_name, cell * note = NULL);
extern void push_identifier(
  cell * var   , char * type_name, cell * note = NULL);

int top_of_identifier_stack();
void restore_identifier_stack(int n);

extern void register_value(int i, cell * val) ;
extern void register_value(char * symbol, cell * val) ;
extern void register_value(cell * var, cell * val) ;

extern cell * process_all(cell * L) ;

#endif /* SEMANTIC_H */
