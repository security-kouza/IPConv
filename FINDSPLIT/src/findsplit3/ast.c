#include "ast.h"

#define VA_WRAPPER(name, generator)                 \
static cell * _ ## name (va_list ap){               \
  cell * s = va_arg(ap, cell *) ;                   \
  if(s!=ENDOFVA) return generator(s,_ ## name(ap)); \
  return NULL ;                                     \
}                                                   \
                                                    \
cell * name (int dummy, ... ){                      \
  cell * c ;                                        \
  va_list ap ;                                      \
  va_start(ap, dummy) ;                             \
  c = _ ## name(ap) ;                               \
  va_end(ap) ;                                      \
  return c ;                                        \
}

static cell * cons (void * car, void * cdr) {
  cell * c = (cell *) malloc(sizeof(cell)) ;
  if(c) {
    c->car = car ;
    c->cdr = cdr ;
  }
  return c ;
}

VA_WRAPPER(_ast, cons) ;

static cell * append_cons(cell * L1, cell * L2){
  if(!L1) return L2 ;
  L1->cdr = append_cons(L1->cdr, L2) ;
  return L1 ;
}

VA_WRAPPER(_ast_append, append_cons) ;
