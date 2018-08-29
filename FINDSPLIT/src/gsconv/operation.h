#ifndef OPERATION_H
#define OPERATION_H

#include "lisp.h"

inline static char * type_of  (cell const * const L){ return L->car ; }
inline static int    is_null  (cell * L){ return ! NTHCDR(4,L) ; }
inline static int    is_single(cell * L){ return NTHCDR(4,L) && ! NTHCDR(5,L) ; }
inline static char * first_arg(cell * L){ return NTH(4,L) ; }

#define type_define(name)         char * TYPE_ ## name = "TYPE_" # name ;
#define type_declare(name) extern char * TYPE_ ## name ;     \
inline static const int is_ ## name (cell const * const X){  \
  return type_of(X) == TYPE_ ## name ;                       \
}

// === basic operand type ================================================

type_declare(i)    ;
type_declare(ic)   ;
type_declare(ic2)  ;
type_declare(g)    ;
type_declare(gc)   ;
type_declare(gic)  ;
type_declare(gcic) ;
type_declare(tc)   ;
type_declare(t)    ;
type_declare(eggc) ;
type_declare(egc2) ;

// === extended operand type =============================================

type_declare(crs)        ;
type_declare(IDENTIFIER) ;
type_declare(function)   ;
type_declare(AND)        ;
type_declare(commit)     ;
type_declare(ELLIPSIS)   ;

// === type check functions ==============================================

inline static const int is_integer(cell const * const X){
  return is_i(X) || is_ic(X) || is_ic2(X) ;
}

inline static const int is_group(cell const * const X){
  return is_g(X) || is_gc(X) || is_gic(X) || is_gcic(X) ;
}

inline static const int is_target(cell const * const X){
  return is_tc(X) || is_t(X) || is_eggc(X) || is_egc2(X) ;
}

inline static const int same_type(cell const * const X, cell const * const Y){
  return type_of(X) == type_of(Y) ;
}

inline static const int same_category(cell const * const X, cell const * const Y){
  return same_type(X,Y)                   ||
         (is_integer(X) && is_integer(Y)) ||
         (is_group  (X) && is_group  (Y)) ||
         (is_target (X) && is_target (Y)) ;
}

// === operation =========================================================

extern cell * DUPLICATE (cell * L) ;
extern cell * OPERATION_ADD    (cell * X, cell * Y, cell * ast_L) ;
extern cell * OPERATION_MUL    (cell * X, cell * Y, cell * ast_L) ;
extern cell * OPERATION_POW    (cell * X, cell * Y, cell * ast_L) ;
extern cell * OPERATION_PAIRING(cell * X, cell * Y, cell * ast_L) ;
extern cell * OPERATION_LOGICALAND(cell * X, cell * Y, cell * ast_L) ;
extern cell * OPERATION_GS_SETUP(cell * X, cell * ast_L);
extern cell * OPERATION_GS_PROOFWI(cell * crs, cell * predicate, cell * ast_L);
extern cell * OPERATION_GS_PROOFZK(cell * crs, cell * predicate, cell * ast_L);

extern void   OPERATION_PRINT(char * header, cell * L, char * footer) ;
extern void   OPERATION_PRINT_RAW(cell * L) ;

extern cell * REDUCE(cell * val, char * var_name) ;
extern char * new_symbol(char * var_name, char * type_name, cell * note) ;
extern cell * integer_commit(cell * crs, char * symbol) ;
extern cell * group_commit(cell * crs, char * symbol) ;

extern cell * SET_DL_KNOWN  (cell * g, cell * y, cell * ast_L);
extern cell * SET_DL_UNKNOWN(cell * g, cell * y, cell * ast_L);

#endif /* OPERATION_H */
