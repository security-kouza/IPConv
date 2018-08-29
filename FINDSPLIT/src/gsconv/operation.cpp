#include "operation.h"
#include "semantic.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <boost/unordered_map.hpp>

using namespace std ;
using namespace boost ;

extern "C" char * strdup(const char * s) ;

static void operation_error(cell * L, const string & message) {
  if(L){
    cell * lineno = L->cdr ;
    int    n      = (intptr_t)lineno->car ;
    cerr << "line " << n << " operation error: "
         << message << "." << endl ;
  }else{
    cerr << "line " << "?" << " operation error: "
         << message << "." << endl ;
  }
  exit(1);
}

#define TYPE_(X,...) LIST(TYPE_ ## X,NULL,NULL,NULL,__VA_ARGS__)

static cell * ADD        (cell * X, cell * Y, cell * ast_L) ;
static cell * MUL        (cell * X, cell * Y, cell * ast_L) ;
static cell * POW        (cell * X, cell * Y, cell * ast_L) ;
static cell * PAIRING    (cell * X, cell * Y, cell * ast_L) ;
static cell * LOGICALAND (cell * X, cell * Y, cell * ast_L) ;
static cell * GS_SETUP   (cell * X, cell * ast_L) ;
static cell * GS_PROOFWI (cell * crs, cell * predicate, cell * ast_L) ;
static cell * GS_PROOFZK (cell * crs, cell * predicate, cell * ast_L) ;
static void   PRINT      (char * header, cell * L, char * footer) ;
static void   PRINT_RAW  (cell * L) ;

cell * OPERATION_ADD       (cell * X, cell * Y, cell * ast_L){ return ADD(X,Y,ast_L) ; }
cell * OPERATION_MUL       (cell * X, cell * Y, cell * ast_L){ return MUL(X,Y,ast_L) ; }
cell * OPERATION_POW       (cell * X, cell * Y, cell * ast_L){ return POW(X,Y,ast_L) ; }
cell * OPERATION_PAIRING   (cell * X, cell * Y, cell * ast_L){ return PAIRING(X,Y,ast_L) ; }
cell * OPERATION_LOGICALAND(cell * X, cell * Y, cell * ast_L){ return LOGICALAND(X,Y,ast_L) ; }
cell * OPERATION_GS_SETUP  (cell * X, cell * ast_L){ return GS_SETUP(X,ast_L) ; }
cell * OPERATION_GS_PROOFWI(cell * crs, cell * predicate, cell * ast_L){ return GS_PROOFWI(crs,predicate,ast_L) ; }
cell * OPERATION_GS_PROOFZK(cell * crs, cell * predicate, cell * ast_L){ return GS_PROOFZK(crs,predicate,ast_L) ; }
void   OPERATION_PRINT     (char * header, cell * L, char * footer){ PRINT(header, L, footer); }
void   OPERATION_PRINT_RAW (cell * L){ PRINT_RAW(L); }

static void   PRINT_ENTRY(char const * const type_name, cell * L) ;

#ifdef __CYGWIN__
static string my_to_string( int value ){
  string s = "" ;
  string d = "" ;
  if(value<0){
    value = -value ;
    s = "-" ;
  }else if(value==0){
    d = "0" ;
  }
  while(value){
    static char const * const t[] = {
      "0", "1", "2", "3", "4",
      "5", "6", "7", "8", "9"
    } ;
    d = t[value % 10] + d ;
    value /= 10 ;
  }
  return s+d ;
}
#define to_string my_to_string 
#endif

char * new_symbol(char * var_name, char * type_name, cell * note){
  static unordered_map<string, int> table ;
  char * symbol = strdup(
    ("$" + string(var_name) + "_" + to_string(table[var_name]++)).c_str()
  ) ;
  push_identifier(symbol, type_name, note) ;
  return symbol ;
}

char * REDUCE_(cell * val, char * var_name) {
  char * type_name = type_of(val) ;
  char * symbol    = new_symbol(var_name, type_name, val) ;
  if(type_name == TYPE_g){
    register_value(symbol, val) ;
    PRINT_RAW(val); printf(" -> %s\n",symbol);
  }
  return symbol ;
}


cell * REDUCE(cell * val, char * var_name) {
  char * type_name = type_of(val) ;
  char * symbol    = REDUCE_(val, var_name) ;
  return LIST(type_name, NULL, NULL, NULL, symbol) ;
}

char * MONO      = "MONO" ;
char * PAIR      = "PAIR" ;
char * RECURSIVE = "RECURSIVE" ;

static int SAME_ENTRY(char const * const type_name, cell * X, cell * Y){
//  printf("sX="); PRINT_ENTRY(type_name, X) ; printf("\n");
//  printf("sY="); PRINT_ENTRY(type_name, Y) ; printf("\n");
  if(type_name == MONO) return !strcmp(X->car,Y->car) ;
  if(type_name == PAIR) return
    (!strcmp(((cell *)X->car)->car, ((cell *)Y->car)->car)) &&
    (!strcmp(((cell *)X->car)->cdr->car, ((cell *)Y->car)->cdr->car)) ;
  operation_error(NULL, "error in SAME_ENTRY (bug?)") ;
  return 0 ;
}

static int INCLUDE(cell * L, cell * entry){
  char const * const type_of_L = type_of(L) ; L=L->cdr ;
  cell * opt1 = (cell *) L->car ; L=L->cdr ;
  cell * opt2 = (cell *) L->car ; L=L->cdr ;
  cell * opt3 = (cell *) L->car ; L=L->cdr ;
  char * type_name ;
  if(
    (type_of_L == TYPE_i)  ||
    (type_of_L == TYPE_ic) ||
    (type_of_L == TYPE_g)  ||
    (type_of_L == TYPE_tc) ||
    (type_of_L == TYPE_gc)
  ){
    type_name = MONO ;
  }else if(
    (type_of_L == TYPE_ic2)  ||
    (type_of_L == TYPE_egc2) ||
    (type_of_L == TYPE_gic)  ||
    (type_of_L == TYPE_eggc) ||
    (type_of_L == TYPE_gcic) ||
    (type_of_L == TYPE_t)
  ){
    type_name = PAIR ;
  }
  for(;L;L=L->cdr)if(SAME_ENTRY(type_name,L,entry))return 1;
  return 0;
}

int IS_NULL(cell * X){
  if(!X){
    return 1 ;
  }else{
    cell * L=X                    ; L=L->cdr ; // type_name
    cell * opt1 = (cell *) L->car ; L=L->cdr ; // opt1
    cell * opt2 = (cell *) L->car ; L=L->cdr ; // opt2
    cell * opt3 = (cell *) L->car ; L=L->cdr ; // opt3
    return is_null(X) && IS_NULL(opt1) && IS_NULL(opt2) && IS_NULL(opt3) ;
  }
}

int EQUAL(cell * L1, cell * L2){

  if(IS_NULL(L1)) return IS_NULL(L2) ;
  if(IS_NULL(L2)) return IS_NULL(L1) ;

  cell * L1t = L1 ;
  char * L1type_name =     L1t->car ; L1t=L1t->cdr ;
  cell * L1opt1 = (cell *) L1t->car ; L1t=L1t->cdr ;
  cell * L1opt2 = (cell *) L1t->car ; L1t=L1t->cdr ;
  cell * L1opt3 = (cell *) L1t->car ; L1t=L1t->cdr ;

  cell * L2t = L2 ;
  char * L2type_name =     L2t->car ; L2t=L2t->cdr ;
  cell * L2opt1 = (cell *) L2t->car ; L2t=L2t->cdr ;
  cell * L2opt2 = (cell *) L2t->car ; L2t=L2t->cdr ;
  cell * L2opt3 = (cell *) L2t->car ; L2t=L2t->cdr ;

  if( L1type_name != L2type_name ) return 0 ;
  if(!EQUAL(L1opt1,L2opt1)) return 0 ;
  if(!EQUAL(L1opt2,L2opt2)) return 0 ;
  if(!EQUAL(L1opt2,L2opt2)) return 0 ;

  for(;L1t;L1t=L1t->cdr)if(!INCLUDE(L2, L1t))return 0 ;
  for(;L2t;L2t=L2t->cdr)if(!INCLUDE(L1, L2t))return 0 ;

  return 1 ;
}

cell * DUPLICATE(cell * L){
  if(!L)return L ;
  char const * const type_of_L = type_of(L) ; L=L->cdr ;
  cell * opt1 = DUPLICATE((cell *) L->car) ; L=L->cdr ;
  cell * opt2 = DUPLICATE((cell *) L->car) ; L=L->cdr ;
  cell * opt3 = DUPLICATE((cell *) L->car) ; L=L->cdr ;
  cell * ret  = LIST(type_of_L, opt1, opt2, opt3) ;
  cell * R    = NTHCDR(3,ret) ;
  for(;L;L=L->cdr,R=R->cdr) R->cdr = LIST(L->car) ;
  return ret ;
}

static cell * MERGE(cell * X, cell * Y){
  if(!X) return DUPLICATE(Y) ;
  if(!Y) return DUPLICATE(X) ;
  cell * ret = DUPLICATE(X) ;
  cell * L   = Y ;
  cell * R   = ret ;
  if(L){
    L=L->cdr ;                                 // type_name NTHCDR(0,Y) ;
    cell * opt1 = (cell *) L->car ; L=L->cdr ; // opt1      NTHCDR(1,Y) ;
    cell * opt2 = (cell *) L->car ; L=L->cdr ; // opt2      NTHCDR(2,Y) ;
    cell * opt3 = (cell *) L->car ; L=L->cdr ; // opt3      NTHCDR(3,Y) ;
                                               // L =       NTHCDR(4,Y) ;
    R=R->cdr ;                                                  // type_name NTHCDR(0,X)
    R->car = (char *)MERGE((cell *)R->car, opt1) ; R = R->cdr ; // opt1      NTHCDR(1,X)
    R->car = (char *)MERGE((cell *)R->car, opt2) ; R = R->cdr ; // opt2      NTHCDR(2,X)
    R->car = (char *)MERGE((cell *)R->car, opt3) ; // R = R->cdr ; // opt3      NTHCDR(3,X)
    for(;R->cdr;R=R->cdr);
    for(;L;L=L->cdr)if(!INCLUDE(X,L)){ R->cdr = LIST(L->car); R = R->cdr; }
  }
  return ret ;
}

static void PRINT_ENTRY(char const * const type_name, cell * L){
  if(type_name == MONO) printf("%s",L->car) ;
  if(type_name == PAIR) printf(
    "(%s,%s)", ((cell *)L->car)->car, ((cell *)L->car)->cdr->car ) ;
  if(type_name == RECURSIVE) PRINT("[",(cell *)L->car,"]") ;
}

static void PRINT_RAW(cell * L){
  int first = 1 ;
  if(L){
    char const * const type_of_L = type_of(L) ; L=L->cdr ;
    cell * opt1 = (cell *) L->car ; L=L->cdr ;
    cell * opt2 = (cell *) L->car ; L=L->cdr ;
    cell * opt3 = (cell *) L->car ; L=L->cdr ;

    char * type_name = MONO ;
    if(
      (type_of_L == TYPE_ic2)  ||
      (type_of_L == TYPE_egc2) ||
      (type_of_L == TYPE_gic)  ||
      (type_of_L == TYPE_eggc) ||
      (type_of_L == TYPE_gcic) ||
      (type_of_L == TYPE_t)
    ){
      type_name = PAIR ;
    }else if(
      (type_of_L == TYPE_AND)
    ){
      type_name = RECURSIVE ;
    }
    for(;L;L=L->cdr){
      if(first) first = 0 ; else putchar(',') ;
      PRINT_ENTRY(type_name, L);
    }
  }
}

static void PRINT(char * header, cell * L, char * footer){
  printf("%s",header);
  if(L){
    char const * const type_of_L = type_of(L) ; L=L->cdr ;
    cell * opt1 = (cell *) L->car ; L=L->cdr ;
    cell * opt2 = (cell *) L->car ; L=L->cdr ;
    cell * opt3 = (cell *) L->car ; L=L->cdr ;
    printf("%s", type_of_L) ;
    if(type_of_L != TYPE_crs) PRINT(",[",opt1, "]") ;
    PRINT(",[",opt2, "]") ;
    PRINT(",[",opt3, "]") ;
    char * type_name = MONO ;
    if(
      (type_of_L == TYPE_ic2)  ||
      (type_of_L == TYPE_egc2) ||
      (type_of_L == TYPE_gic)  ||
      (type_of_L == TYPE_eggc) ||
      (type_of_L == TYPE_gcic) ||
      (type_of_L == TYPE_t)
    ){
      type_name = PAIR ;
    }else if(
      (type_of_L == TYPE_AND)
    ){
      type_name = RECURSIVE ;
    }
    for(;L;L=L->cdr){
      putchar(',');
      PRINT_ENTRY(type_name, L);
    }
  }
  printf("%s",footer);
}

/* ======================================================================
   ADD
   ====================================================================== */

static cell * ADD_ic_i(cell * X, cell * Y, cell * ast_L){
  cell * R = DUPLICATE(X) ; 
  cell * L = R ;
  L=L->cdr ;                         // type_name
  L->car = (char *)ADD((cell *)L->car, Y, ast_L) ;  // opt1
  return R ;
}

static cell * ADD_ic2_i(cell * X, cell * Y, cell * ast_L){
  cell * R = DUPLICATE(X) ; 
  cell * L = R ;
  L=L->cdr ;                         // type_name
  L=L->cdr ;                         // opt1
  L->car = (char *)ADD((cell *)L->car, Y, ast_L) ;  // opt2
  return R ;
}

static cell * ADD_ic2_ic(cell * X, cell * Y, cell * ast_L){
  cell * R = DUPLICATE(X) ; 
  cell * L = R ; L=L->cdr ;                  // type_name
  L->car = (char *)ADD((cell *)L->car, Y, ast_L) ;  // opt1

  cell * L2 = (cell *)L->car ;
  L2 = L2->cdr ;

  L=L->cdr ;                         
  L->car = (char *)ADD((cell *)L->car, (cell*)L2->car, ast_L) ;  // opt2
  L2->car = NULL ;
  return R ;
}

cell * ADD(cell * X, cell * Y, cell * ast_L){
  if(!X) return DUPLICATE(Y) ;
  if(!Y) return DUPLICATE(X) ;

  if( is_integer(X) && is_integer(Y) ){
    if(same_type(X,Y)) return MERGE(X,Y) ;

    if(is_ic(X) && is_i(Y))return ADD_ic_i(X,Y,ast_L) ;
    if(is_ic(Y) && is_i(X))return ADD_ic_i(Y,X,ast_L) ;

    if(is_ic2(X) && is_i(Y))return ADD_ic2_i(X,Y,ast_L) ;
    if(is_ic2(Y) && is_i(X))return ADD_ic2_i(Y,X,ast_L) ;

    if(is_ic2(X) && is_ic(Y))return ADD_ic2_ic(X,Y,ast_L) ;
    if(is_ic2(Y) && is_ic(X))return ADD_ic2_ic(Y,X,ast_L) ;
  }
  operation_error(ast_L,
    string("cannot operate ") + type_of(X) + " + " + type_of(Y) ) ;
  return NULL ;
}

/* ======================================================================
   MUL
   ====================================================================== */

static const int COMMUTATIVE     = 1 ;
static const int NON_COMMUTATIVE = 0 ;

static cell * generate_pair(cell * X0, cell * Y0, int commutative){
  cell * R0 = NIL();
  for(cell * X=X0;X;X=X->cdr){
    for(cell * Y=Y0;Y;Y=Y->cdr){
      char * x = X->car ;
      char * y = Y->car ;
      if(commutative){
        if(strcmp(x,y)>0){
          char * t = y ; y = x ; x = t ;
        }
      }
      cell * entry = LIST(x,y) ;
      int append = 1 ;
      for(cell * R=R0;R;R=R->cdr) 
        if(SAME_ENTRY(PAIR,R,LIST(entry))){append=0; break;}
      if(append)R0 = INTEGRATE(R0,entry) ;
    }
  }
  return R0 ;
}

static cell * MUL_i_i(cell * X, cell * Y, cell * ast_L){
  char * Z = new_symbol("mul", TYPE_i, NULL) ;
  cell * R = LIST(TYPE_i, NULL, NULL, NULL, Z) ;
  return R ;
}

static cell * MUL_ic_ic(cell * X0, cell * Y0, cell * ast_L){
  cell * X = X0 ;
  char const * const X_type = type_of(X) ; X=X->cdr ;
  cell * X_opt1 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt2 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt3 = (cell *) X->car ; X=X->cdr ;
  cell * Y = Y0 ;
  char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
  cell * Y_opt1 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;

  cell * cross = ADD(X_opt1?Y0:NULL, Y_opt1?X0:NULL, ast_L) ;
  cell * C     = cross ;
  cell * D     = MUL(X_opt1,Y_opt1,ast_L) ;
  if(C){
    char const * const C_type = type_of(C) ; C=C->cdr ;
    C->car = NULL ;
  }
  cell * R = LIST(TYPE_ic2, cross, D, NULL) ;
  R = APPEND(R, generate_pair(X, Y, COMMUTATIVE)) ;
  return R ;
}

static cell * MUL_gc_g(cell * X, cell * Y, cell * ast_L){
  cell * R = DUPLICATE(X) ; 
  cell * L = R ;
  L=L->cdr ;                         // type_name
  L->car = (char *)MUL((cell *)L->car, Y, ast_L) ;  // opt1
  return R ;
}

static cell * MUL_gic_g(cell * X, cell * Y, cell * ast_L){
  return MUL_gc_g(X, Y, ast_L) ;
}

static cell * MUL_gcic_g(cell * X, cell * Y, cell * ast_L){
  cell * R = DUPLICATE(X) ; 
  cell * L = R ;
  L=L->cdr ;                         // type_name
  L=L->cdr ;                         // opt1
  L=L->cdr ;                         // opt2
  L->car = (char *)MUL((cell *)L->car, Y, ast_L) ;  // opt3
  return R ;
}

static cell * MUL_gic_gc(cell * X, cell * Y, cell * ast_L){

  cell * X0 = DUPLICATE(X) ; X = X0 ;
  cell * X_opt1 ;
  cell * X_opt2 ;
  cell * X_opt3 ;
  cell * Y_opt1 ;
  cell * Y_opt2 ;
  cell * Y_opt3 ;

  if(X){
    char const * const X_type = type_of(X) ; X=X->cdr ;
    X_opt1 = (cell *) X->car ; X->car = NULL ; X=X->cdr ;
    X_opt2 = (cell *) X->car ; X=X->cdr ;
    X_opt3 = (cell *) X->car ; X=X->cdr ;
  }

  cell * Y0 = DUPLICATE(Y) ; Y = Y0 ;
  if(Y){
    char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
    Y_opt1 = (cell *) Y->car ; Y->car = NULL ; Y=Y->cdr ;
    Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
    Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;
  }

  cell * R_opt3 = ADD(X_opt1, Y_opt1, ast_L) ;
  cell * R = LIST(TYPE_gcic, Y0, X0, R_opt3 ) ;
  return R ;
}

static cell * MUL_gcic_gc(cell * X, cell * Y, cell * ast_L){
  cell * X_opt1 ;
  cell * X_opt2 ;
  cell * X_opt3 ;
  cell * Y_opt1 ;
  cell * Y_opt2 ;
  cell * Y_opt3 ;

  cell * Y0 = DUPLICATE(Y) ; Y = Y0 ;
  if(Y){
    char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
    Y_opt1 = (cell *) Y->car ; Y->car = NULL ; Y=Y->cdr ;
    Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
    Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;
  }

  cell * X0 = DUPLICATE(X) ; X = X0 ;
  if(X){
    char const * const X_type = type_of(X) ; X=X->cdr ;
    X_opt1 = (cell *) X->car ; X->car = (char *)MUL(X_opt1, Y0, ast_L) ; X=X->cdr ;
    X_opt2 = (cell *) X->car ; X=X->cdr ;
    X_opt3 = (cell *) X->car ; X->car = (char *)MUL(X_opt3, Y_opt1, ast_L) ; X=X->cdr ;
  }

  return X0 ;
}

static cell * MUL_gcic_gic(cell * X, cell * Y, cell * ast_L){
  cell * X_opt1 ;
  cell * X_opt2 ;
  cell * X_opt3 ;
  cell * Y_opt1 ;
  cell * Y_opt2 ;
  cell * Y_opt3 ;

  cell * Y0 = DUPLICATE(Y) ; Y = Y0 ;
  if(Y){
    char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
    Y_opt1 = (cell *) Y->car ; Y->car = NULL ; Y=Y->cdr ;
    Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
    Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;
  }

  cell * X0 = DUPLICATE(X) ; X = X0 ;
  if(X){
    char const * const X_type = type_of(X) ; X=X->cdr ;
    X_opt1 = (cell *) X->car ; X=X->cdr ;
    X_opt2 = (cell *) X->car ; X->car = (char *)MUL(X_opt2, Y0, ast_L) ; X=X->cdr ;
    X_opt3 = (cell *) X->car ; X->car = (char *)MUL(X_opt3, Y_opt1, ast_L) ; X=X->cdr ;
  }

  return X0 ;
}

static cell * MUL_eggc_t(cell * X, cell * Y, cell * ast_L){
  return MUL_gc_g(X, Y, ast_L) ;
}

static cell * MUL_egc2_t(cell * X, cell * Y, cell * ast_L){
  cell * R = DUPLICATE(X) ; 
  cell * L = R ;
  L=L->cdr ;                         // type_name
  L=L->cdr ;                         // opt1
  L->car = (char *)MUL((cell *)L->car, Y, ast_L) ;  // opt2
  return R ;
}

static cell * MUL_egc2_eggc(cell * X, cell * Y, cell * ast_L){
  cell * R = DUPLICATE(X) ; 
  cell * L = R ; L=L->cdr ;                  // type_name
  L->car = (char *)MUL((cell *)L->car, Y, ast_L) ;  // opt1

  cell * L2 = (cell *)L->car ;
  L2 = L2->cdr ;

  L=L->cdr ;                         
  L->car = (char *)MUL((cell *)L->car, (cell*)L2->car, ast_L) ;  // opt2
  L2->car = NULL ;
  return R ;
}

static cell * TC = LIST(TYPE_t,NULL,NULL,NULL) ;

cell * MUL(cell * X, cell * Y, cell * ast_L){

  if(!X) {
    if(Y){
      if(is_integer(Y)) return DUPLICATE(X) ;
      else return DUPLICATE(Y) ;
    }else{
      return DUPLICATE(X) ;
    }
  }

  if(!Y) {
    if(X){
      if(is_integer(X)) return DUPLICATE(Y) ;
      else return DUPLICATE(X) ;
    }else{
      return DUPLICATE(Y) ;
    }
  }

  if( is_tc(X) ) return MUL(TC,Y,ast_L);
  if( is_tc(Y) ) return MUL(X,TC,ast_L);

  if( is_integer(X) && is_integer(Y) ){
    if(is_i(X) && is_i(Y))return MUL_i_i(X,Y,ast_L) ;
    if(is_ic(X) && is_i(Y))return DUPLICATE(X) ;
    if(is_ic2(X) && is_i(Y))return DUPLICATE(X) ;
    if(is_ic(Y) && is_i(X))return DUPLICATE(Y) ;
    if(is_ic2(Y) && is_i(X))return DUPLICATE(Y) ;
    if(is_ic(Y) && is_ic(X))return MUL_ic_ic(X,Y,ast_L) ;
  }

  if( is_group(X) && is_group(Y) ){
    if(same_type(X,Y)) return MERGE(X,Y) ;

    if(is_gc(X) && is_g(Y))return MUL_gc_g(X,Y,ast_L) ;
    if(is_gc(Y) && is_g(X))return MUL_gc_g(Y,X,ast_L) ;

    if(is_gic(X) && is_g(Y))return MUL_gic_g(X,Y,ast_L) ;
    if(is_gic(Y) && is_g(X))return MUL_gic_g(Y,X,ast_L) ;

    if(is_gcic(X) && is_g(Y))return MUL_gcic_g(X,Y,ast_L) ;
    if(is_gcic(Y) && is_g(X))return MUL_gcic_g(Y,X,ast_L) ;

    if(is_gic(X) && is_gc(Y))return MUL_gic_gc(X,Y,ast_L) ;
    if(is_gic(Y) && is_gc(X))return MUL_gic_gc(Y,X,ast_L) ;

    if(is_gcic(X) && is_gc(Y))return MUL_gcic_gc(X,Y,ast_L) ;
    if(is_gcic(Y) && is_gc(X))return MUL_gcic_gc(Y,X,ast_L) ;

    if(is_gcic(X) && is_gic(Y))return MUL_gcic_gic(X,Y,ast_L) ;
    if(is_gcic(Y) && is_gic(X))return MUL_gcic_gic(Y,X,ast_L) ;
  }

  if( is_target(X) && is_target(Y) ){
    if(same_type(X,Y)) return MERGE(X,Y) ;

    if(is_eggc(X) && is_t(Y))return MUL_eggc_t(X,Y,ast_L) ;
    if(is_eggc(Y) && is_t(X))return MUL_eggc_t(Y,X,ast_L) ;

    if(is_egc2(X) && is_t(Y))return MUL_egc2_t(X,Y,ast_L) ;
    if(is_egc2(Y) && is_t(X))return MUL_egc2_t(Y,X,ast_L) ;

    if(is_egc2(X) && is_eggc(Y))return MUL_egc2_eggc(X,Y,ast_L) ;
    if(is_egc2(Y) && is_eggc(X))return MUL_egc2_eggc(Y,X,ast_L) ;
  }

  operation_error(ast_L,
    string("cannot operate ") + type_of(X) + " * " + type_of(Y) ) ;
  return NULL ;
}

/* ======================================================================
   POW
   ====================================================================== */

static cell * POW_i_i(cell * X, cell * Y, cell * ast_L){
  char * Z = new_symbol("pow",TYPE_i,NULL) ;
  cell * R = LIST(TYPE_i, NULL, NULL, NULL, Z) ;
  return R ;
}

static cell * POW_g_ic(cell * X0, cell * Y0, cell * ast_L){

  cell * X = X0 ;
  char const * const X_type = type_of(X) ; X=X->cdr ;
  cell * X_opt1 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt2 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt3 = (cell *) X->car ; X=X->cdr ;

  cell * Y = Y0 ;
  char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
  cell * Y_opt1 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;

  cell * C      = POW(X0,Y_opt1,ast_L) ;
  cell * R      = LIST(TYPE_gic, C, NULL, NULL) ;
  R = APPEND(R, generate_pair(X, Y, NON_COMMUTATIVE)) ;
  return R ;
}

static cell * POW_gc_ic(cell * X0, cell * Y0, cell * ast_L){

  cell * X = DUPLICATE(X0) ; X0 = X ;
  char const * const X_type = type_of(X) ; X=X->cdr ;
  cell * X_opt1 = (cell *) X->car ; X->car = NULL ; X=X->cdr ;
  cell * X_opt2 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt3 = (cell *) X->car ; X=X->cdr ;

  cell * Y = DUPLICATE(Y0) ; Y0 = Y ;
  char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
  cell * Y_opt1 = (cell *) Y->car ; Y->car = NULL ; Y=Y->cdr ;
  cell * Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;

  cell * R_opt1 = POW(X0    , Y_opt1, ast_L); // gc
  cell * R_opt2 = POW(X_opt1, Y0    , ast_L); // gic
  cell * R_opt3 = POW(X_opt1, Y_opt1, ast_L); // g

  cell * R = LIST(TYPE_gcic, R_opt1, R_opt2, R_opt3) ;

  R = APPEND(R, generate_pair(X, Y, NON_COMMUTATIVE)) ;
  return R ;
}

cell * POW(cell * X, cell * Y, cell * ast_L){

  if(!X) return DUPLICATE(X) ;
  if(!Y) return DUPLICATE(Y) ;

  if( is_i(Y) ){
    if( is_i(X)    ) return POW_i_i(X,Y,ast_L) ;
    if( is_g(X)    ) return DUPLICATE(X)       ;
    if( is_gc(X)   ) return DUPLICATE(X)       ;
    if( is_gic(X)  ) return DUPLICATE(X)       ;
    if( is_gcic(X) ) return DUPLICATE(X)       ;
    if( is_t(X)    ) return DUPLICATE(X)       ;
    if( is_tc(X)   ) return DUPLICATE(TC)      ;
    if( is_eggc(X) ) return DUPLICATE(X)       ;
    if( is_egc2(X) ) return DUPLICATE(X)       ;
  }

  if( is_ic(Y) ){
    if( is_g(X)    ) return POW_g_ic(X,Y,ast_L)  ;
    if( is_gc(X)   ) return POW_gc_ic(X,Y,ast_L) ;
  }

  operation_error(ast_L,
    string("cannot operate ") + type_of(X) + " ^ " + type_of(Y) ) ;
  return NULL ;
}

/* ======================================================================
   PAIRING
   ====================================================================== */

static cell * PAIRING_g_g_sub(cell * X, cell * Y, cell * ast_L){
  char * P1 ;
  char * P2 ;

  if(is_single(X)) P1 = first_arg(X)  ;
  else             P1 = REDUCE_(X,"e") ;

  if(is_single(Y)) P2 = first_arg(Y)  ;
  else             P2 = REDUCE_(Y,"e") ;

  printf("(%s,%s)\n",P1,P2) ; // == redundant ? ==
  return NULL ;
}

static cell * PAIRING_g_g(cell * X0, cell * Y0, cell * ast_L){

  PAIRING_g_g_sub(X0, Y0, ast_L) ;

  cell * X = X0 ;
  char const * const X_type = type_of(X) ; X=X->cdr ;
  cell * X_opt1 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt2 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt3 = (cell *) X->car ; X=X->cdr ;

  cell * Y = Y0 ;
  char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
  cell * Y_opt1 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;

  cell * R      = LIST(TYPE_t, NULL, NULL, NULL) ;

  R = APPEND(R, generate_pair(X, Y, COMMUTATIVE)) ;

  return R ;
}

static cell * PAIRING_g_gc(cell * X0, cell * Y0, cell * ast_L){

  cell * X = X0 ;
  char const * const X_type = type_of(X) ; X=X->cdr ;
  cell * X_opt1 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt2 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt3 = (cell *) X->car ; X=X->cdr ;

  cell * Y = Y0 ;
  char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
  cell * Y_opt1 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;

  cell * C      = PAIRING(X0,Y_opt1,ast_L) ;
  cell * R      = LIST(TYPE_eggc, C, NULL, NULL) ;

  R = APPEND(R, generate_pair(X, Y, NON_COMMUTATIVE)) ;
  return R ; // LIST(TYPE_eggc, NULL, NULL, NULL) ;
}

static cell * PAIRING_gc_gc(cell * X0, cell * Y0, cell * ast_L){

  cell * X = DUPLICATE(X0) ; X0 = X ;
  char const * const X_type = type_of(X) ; X=X->cdr ;
  cell * X_opt1 = (cell *) X->car ; X->car = NULL ; X=X->cdr ;
  cell * X_opt2 = (cell *) X->car ; X=X->cdr ;
  cell * X_opt3 = (cell *) X->car ; X=X->cdr ;

  cell * Y = DUPLICATE(Y0) ; Y0 = Y ;
  char const * const Y_type = type_of(Y) ; Y=Y->cdr ;
  cell * Y_opt1 = (cell *) Y->car ; Y->car = NULL ; Y=Y->cdr ;
  cell * Y_opt2 = (cell *) Y->car ; Y=Y->cdr ;
  cell * Y_opt3 = (cell *) Y->car ; Y=Y->cdr ;

  cell * A0     = PAIRING(X0    ,Y_opt1,ast_L) ;
  cell * A_opt1 = NULL ;

  if(A0){
    cell * A      = A0 ;
    A=A->cdr ; // type_name
    A_opt1 = (cell *) A->car ; A->car = NULL ;
  }

  cell * B0     = PAIRING(X_opt1,Y0    ,ast_L) ;
  cell * B_opt1 = NULL ;
  if(B0){
    cell * B      = B0 ;
    B=B->cdr ; // type_name
    B_opt1 = (cell *) B->car ; B->car = NULL ;
  }

  cell * C0     = MUL(A0,B0,A_opt1) ;
  cell * D0     = MUL(MUL(PAIRING(X_opt1,Y_opt1,ast_L),A_opt1,ast_L),B_opt1,A_opt1) ;
  cell * R      = LIST(TYPE_egc2, C0, D0, NULL) ;
  R = APPEND(R, generate_pair(X, Y, COMMUTATIVE)) ;
  return R ;
}

cell * PAIRING(cell * X, cell * Y, cell * ast_L){
  if(!X) return DUPLICATE(X) ;
  if(!Y) return DUPLICATE(Y) ;

  if( is_g(X) && is_g(Y) ) return PAIRING_g_g(X, Y, ast_L) ;

  if( is_g(X) && is_gc(Y) ) return PAIRING_g_gc(X, Y, ast_L) ;
  if( is_g(Y) && is_gc(X) ) return PAIRING_g_gc(Y, X, ast_L) ;

  if( is_gc(X) && is_gc(Y) ) return PAIRING_gc_gc(X, Y, ast_L) ;

  operation_error(ast_L,
    string("cannot operate e(") + type_of(X) + "," + type_of(Y) + ")" ) ;
  return NULL ;
}

static cell * AND_A_A(cell * X, cell * Y, cell * ast_L){
  X = DUPLICATE(X) ;
  Y = DUPLICATE(Y) ;
  Y = Y->cdr ; // type_name
  Y = Y->cdr ; // opt1
  Y = Y->cdr ; // opt2
  Y = Y->cdr ; // opt3
  X = APPEND(X, Y) ;
  return X ;
}

static cell * AND_A_NA(cell * X, cell * Y, cell * ast_L){
  X = DUPLICATE(X) ;
  Y = DUPLICATE(Y) ;
  X = INTEGRATE(X, Y) ;
  return X ;
}

static cell * AND_NA_NA(cell * X, cell * Y, cell * ast_L){
  X = DUPLICATE(X) ;
  Y = DUPLICATE(Y) ;
//  printf("AND_NA_NA\n");
//  PRINT("X = [",X,"]\n");
//  PRINT("Y = [",Y,"]\n");
  return LIST(TYPE_AND,NULL,NULL,NULL,X,Y) ;
}

cell * LOGICALAND(cell * X, cell * Y, cell * ast_L){
  if(!X) return DUPLICATE(X) ;
  if(!Y) return DUPLICATE(Y) ;

  if(  is_AND(X) &&  is_AND(Y) ) return AND_A_A  (X, Y, ast_L) ;
  if(  is_AND(X) && !is_AND(Y) ) return AND_A_NA (X, Y, ast_L) ;
  if( !is_AND(X) &&  is_AND(Y) ) return AND_A_NA (Y, X, ast_L) ;
                                 return AND_NA_NA(Y, X, ast_L) ;
}

typedef struct {
  char * symbol        ;
  cell * note          ;
} commit_stack_entry   ;

static int find_commit(char * symbol, vector<commit_stack_entry> & commit_stack){
  for(int i = commit_stack.size()-1; i>=0; i--){
    if(!strcmp(symbol,commit_stack[i].symbol))return i ;
  }
  return -1;
}

static int find_commit(char * symbol, cell * crs){
  cell * L = crs ;
  L=L->cdr ; // type_name
  vector<commit_stack_entry> * commit_stack_p = 
    (vector<commit_stack_entry> *)L->car ;
  vector<commit_stack_entry> & commit_stack = * commit_stack_p ;
  return find_commit(symbol, commit_stack) ;
}

static void push_commit(char * symbol, cell * note, vector<commit_stack_entry> & commit_stack){
  commit_stack_entry e = {symbol, note} ;
  commit_stack.push_back(e);
}

static void push_commit(char * symbol, cell * note, cell * crs){
  cell * L = crs ;
  L=L->cdr ; // type_name
  vector<commit_stack_entry> * commit_stack_p = 
    (vector<commit_stack_entry> *)L->car ;
  vector<commit_stack_entry> & commit_stack = * commit_stack_p ;
  push_commit(symbol, note, commit_stack) ;
}

cell * GS_SETUP(cell * X, cell * ast_L){
  if( is_g(X) ) {
    char * g ;
    if(is_single(X)) g = first_arg(X)  ;
    else             g = REDUCE_(X,"g") ;

    cell * g_g = TYPE_(g, g) ;

    char * u  = new_symbol("u" ,TYPE_g, g_g) ;
    char * v  = new_symbol("v" ,TYPE_g, g_g) ;
    char * w1 = new_symbol("w1",TYPE_g, g_g) ;
    char * w2 = new_symbol("w2",TYPE_g, g_g) ;
    char * w3 = new_symbol("w3",TYPE_g, g_g) ;

    printf("%s -> %s\n",g,u)  ;
    printf("%s -> %s\n",g,v)  ;
    printf("%s -> %s\n",g,w1) ;
    printf("%s -> %s\n",g,w2) ;
    printf("%s -> %s\n",g,w3) ;

    char * dG  = new_symbol("dG" ,TYPE_g, NULL) ;
    cell * dG_g = TYPE_(g, dG) ;

    register_value(g, dG_g);
    printf("%s -> %s\n",dG,g)   ;

    char * dG1 = new_symbol("dG1",TYPE_g, dG_g) ;
    char * dG2 = new_symbol("dG2",TYPE_g, dG_g) ;
    char * dV1 = new_symbol("dV1",TYPE_g, dG_g) ;
    char * dV2 = new_symbol("dV2",TYPE_g, dG_g) ;
    char * dV3 = new_symbol("dV3",TYPE_g, dG_g) ;

    printf("%s -> %s\n",dG,dG1) ;
    printf("%s -> %s\n",dG,dG2) ;
    printf("%s -> %s\n",dG,dV1) ;
    printf("%s -> %s\n",dG,dV2) ;
    printf("%s -> %s\n",dG,dV3) ;

    register_value(u , TYPE_(g, dG1));
    register_value(v , TYPE_(g, dG2));
    register_value(w1, TYPE_(g, dV1));
    register_value(w2, TYPE_(g, dV2));
    register_value(w3, TYPE_(g, dV3));

    printf("%s -> %s\n",dG1,u)  ;
    printf("%s -> %s\n",dG2,v)  ;
    printf("%s -> %s\n",dV1,w1) ;
    printf("%s -> %s\n",dV2,w2) ;
    printf("%s -> %s\n",dV3,w3) ;

    vector<commit_stack_entry> * commit_stack_p = new(vector<commit_stack_entry>) ;
    vector<commit_stack_entry> & commit_stack = * commit_stack_p ;
    return LIST(TYPE_crs, commit_stack_p, NULL, NULL, g,u,v,w1,w2,w3) ;
  }
  operation_error(ast_L, string("cannot setup(") + type_of(X) + ")" ) ;
  return NULL ;
}

static cell * GS_PROOFWI_AND(cell * crs, cell * predicate, cell * ast_L){
  cell * L = predicate ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  for(;L;L=L->cdr) GS_PROOFWI(crs, (cell *)L->car, ast_L) ;
  return NULL ;
}

cell * integer_commit(cell * crs, char * symbol){
  cell * L = crs ;
  L=L->cdr ; // type_name
  vector<commit_stack_entry> * commit_stack_p = 
    (vector<commit_stack_entry> *)L->car ;
  vector<commit_stack_entry> & commit_stack = * commit_stack_p ;
  int i = find_commit(symbol, commit_stack) ;
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;
  char * d1 ;
  char * d2 ;
  char * d3 ;
  char * z  ;

  if(i<0){
    printf("%% commit integer %s\n", symbol) ;

    d1 = new_symbol("d1",TYPE_g, TYPE_(g, u,w1)) ;
    d2 = new_symbol("d2",TYPE_g, TYPE_(g, v,w2)) ;
    d3 = new_symbol("d3",TYPE_g, TYPE_(g, g,w3)) ;
    z  = new_symbol("z" ,TYPE_g, TYPE_(g, d1,d2,d3)) ;

    printf("%s,%s -> %s\n",u,w1,d1)  ;
    printf("%s,%s -> %s\n",v,w2,d2)  ;
    printf("%s,%s -> %s\n",g,w3,d3) ;
    printf("%s,%s,%s -> %s\n",d1,d2,d3,z) ;

    printf("%% --------------\n");
    push_commit(symbol, LIST(d1,d2,d3,z), commit_stack) ;
  }else{
    L = commit_stack[i].note ;
    d1 = L->car ; L=L->cdr ;
    d2 = L->car ; L=L->cdr ;
    d3 = L->car ; L=L->cdr ;
    z  = L->car ; L=L->cdr ;
  }
  return LIST(TYPE_commit,NULL,NULL,NULL,d1,d2,d3,z);
}

cell * group_commit(cell * crs, char * symbol){
  cell * L = crs ;
  L=L->cdr ; // type_name
  vector<commit_stack_entry> * commit_stack_p = 
    (vector<commit_stack_entry> *)L->car ;
  vector<commit_stack_entry> & commit_stack = * commit_stack_p ;
  int i = find_commit(symbol, commit_stack) ;
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;
  char * c1 ;
  char * c2 ;
  char * c3 ;
  if(i<0){
    printf("%% commit group %s\n", symbol) ;

    c1 = new_symbol("c1",TYPE_g,TYPE_(g,u,w1)) ;
    c2 = new_symbol("c2",TYPE_g,TYPE_(g,v,w2)) ;
    c3 = new_symbol("c3",TYPE_g,TYPE_(g,symbol,g,w3)) ;

    printf("%s,%s -> %s\n",u,w1,c1)  ;
    printf("%s,%s -> %s\n",v,w2,c2)  ;
    printf("%s,%s,%s -> %s\n",symbol,g,w3,c3) ;
    printf("%s,%s,%s -> %s\n",c1,c2,c3,symbol) ;

    printf("%% --------------\n");
    push_commit(symbol, LIST(c1,c2,c3,symbol), commit_stack) ;
  }else{
    L = commit_stack[i].note ;
    c1 = L->car ; L=L->cdr ;
    c2 = L->car ; L=L->cdr ;
    c3 = L->car ; L=L->cdr ;
  }
  return LIST(TYPE_commit,NULL,NULL,NULL,c1,c2,c3,symbol);
}

static cell * GS_PROOFWI_ic_sub(cell * crs, char * symbol, cell * ast_L){
  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com = integer_commit(crs, symbol) ;

  L = com ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * d1 = L->car ; L=L->cdr ;
  char * d2 = L->car ; L=L->cdr ;
  char * d3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",d1,w1);
  printf("(%s,%s)\n",d1,w2);
  printf("(%s,%s)\n",d1,w3);
  printf("(%s,%s)\n",d1,g );

  printf("(%s,%s)\n",d2,w1);
  printf("(%s,%s)\n",d2,w2);
  printf("(%s,%s)\n",d2,w3);
  printf("(%s,%s)\n",d2,g );

  printf("(%s,%s)\n",d3,w1);
  printf("(%s,%s)\n",d3,w2);
  printf("(%s,%s)\n",d3,w3);
  printf("(%s,%s)\n",d3,g );

  return NULL ;
}

static cell * GS_PROOFWI_ic(cell * crs, cell * predicate, cell * ast_L){
  PRINT("% pattern 3.3.1 [",predicate,"]\n");

  cell * L = predicate ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  for(;L;L=L->cdr) GS_PROOFWI_ic_sub(crs, L->car, ast_L) ;

  L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",u,w1);
  printf("(%s,%s)\n",u,w2);
  printf("(%s,%s)\n",u,w3);
  printf("(%s,%s)\n",u,g );

  printf("(%s,%s)\n",v,w1);
  printf("(%s,%s)\n",v,w2);
  printf("(%s,%s)\n",v,w3);
  printf("(%s,%s)\n",v,g );

  printf("(%s,%s)\n",g,w1);
  printf("(%s,%s)\n",g,w2);
  printf("(%s,%s)\n",g,w3);
  printf("(%s,%s)\n",g,g );

  return NULL ;
}

static cell * GS_PROOFWI_ic2_sub1(cell * crs, char * symbol, cell * ast_L){
  return GS_PROOFWI_ic_sub(crs, symbol, ast_L) ;
}

static cell * GS_PROOFWI_ic2_sub2(cell * crs, cell * pair, cell * ast_L){
  char * symbol1 = pair->car ;
  char * symbol2 = pair->cdr->car ;

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com1 = integer_commit(crs, symbol1) ;
  L = com1 ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * d11 = L->car ; L=L->cdr ;
  char * d12 = L->car ; L=L->cdr ;
  char * d13 = L->car ; L=L->cdr ;

  cell * com2 = integer_commit(crs, symbol2) ;
  L = com2 ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * d21 = L->car ; L=L->cdr ;
  char * d22 = L->car ; L=L->cdr ;
  char * d23 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",d11,d21);
  printf("(%s,%s)\n",d12,d22);
  printf("(%s,%s)\n",d13,d23);

  return NULL ;
}

static cell * GS_PROOFWI_ic2(cell * crs, cell * predicate, cell * ast_L){
  PRINT("% pattern 3.3.2 [",predicate,"]\n");

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  char * phi11 = new_symbol("phi11",TYPE_g,TYPE_(g, u,w1)) ;
  char * phi12 = new_symbol("phi12",TYPE_g,TYPE_(g, v,w2)) ;
  char * phi13 = new_symbol("phi13",TYPE_g,TYPE_(g, g,w3)) ;

  char * phi21 = new_symbol("phi21",TYPE_g,TYPE_(g, u,w1)) ;
  char * phi22 = new_symbol("phi22",TYPE_g,TYPE_(g, v,w2)) ;
  char * phi23 = new_symbol("phi23",TYPE_g,TYPE_(g, g,w3)) ;

  printf("%s,%s -> %s\n",u,w1,phi11) ;
  printf("%s,%s -> %s\n",v,w2,phi12) ;
  printf("%s,%s -> %s\n",g,w3,phi13) ;

  printf("%s,%s -> %s\n",u,w1,phi21) ;
  printf("%s,%s -> %s\n",v,w2,phi22) ;
  printf("%s,%s -> %s\n",g,w3,phi23) ;

  L = predicate ;
  L=L->cdr ; // type_name
  cell * LINEAR = (cell *)L->car ; L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  for(;L;L=L->cdr) GS_PROOFWI_ic2_sub2(crs, (cell *)L->car, ast_L) ;

  L = LINEAR ;
  if(LINEAR){
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
    for(;L;L=L->cdr) GS_PROOFWI_ic2_sub1(crs, L->car, ast_L) ;
  }

  printf("(%s,%s)\n",u,phi11) ;
  printf("(%s,%s)\n",u,phi12) ;
  printf("(%s,%s)\n",u,phi13) ;

  printf("(%s,%s)\n",g,phi11) ;
  printf("(%s,%s)\n",g,phi12) ;
  printf("(%s,%s)\n",g,phi13) ;

  printf("(%s,%s)\n",v,phi21) ;
  printf("(%s,%s)\n",v,phi22) ;
  printf("(%s,%s)\n",v,phi23) ;

  printf("(%s,%s)\n",g,phi21) ;
  printf("(%s,%s)\n",g,phi22) ;
  printf("(%s,%s)\n",g,phi23) ;

  return NULL ;
}

static cell * GS_PROOFWI_gc_sub(cell * crs, char * symbol, cell * ast_L){
  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com = group_commit(crs, symbol) ;
  L = com ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * c1 = L->car ; L=L->cdr ;
  char * c2 = L->car ; L=L->cdr ;
  char * c3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",c1,w1);
  printf("(%s,%s)\n",c1,w2);
  printf("(%s,%s)\n",c1,w3);
  printf("(%s,%s)\n",c1,g );

  printf("(%s,%s)\n",c2,w1);
  printf("(%s,%s)\n",c2,w2);
  printf("(%s,%s)\n",c2,w3);
  printf("(%s,%s)\n",c2,g );

  printf("(%s,%s)\n",c3,w1);
  printf("(%s,%s)\n",c3,w2);
  printf("(%s,%s)\n",c3,w3);
  printf("(%s,%s)\n",c3,g );

  return NULL ;
}

static cell * GS_PROOFWI_gc(cell * crs, cell * predicate, cell * ast_L){
  PRINT("% pattern 3.2.1 [",predicate,"]\n");

  cell * L = predicate ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  for(;L;L=L->cdr) GS_PROOFWI_gc_sub(crs, L->car, ast_L) ;

  L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",u,w1);
  printf("(%s,%s)\n",u,w2);
  printf("(%s,%s)\n",u,w3);
  printf("(%s,%s)\n",u,g );

  printf("(%s,%s)\n",v,w1);
  printf("(%s,%s)\n",v,w2);
  printf("(%s,%s)\n",v,w3);
  printf("(%s,%s)\n",v,g );

  printf("(%s,%s)\n",g,w1);
  printf("(%s,%s)\n",g,w2);
  printf("(%s,%s)\n",g,w3);
  printf("(%s,%s)\n",g,g );

// ========================

  printf("(%s,%s)\n",w1,w1 );
  printf("(%s,%s)\n",w1,w2 );
  printf("(%s,%s)\n",w1,w3 );
  printf("(%s,%s)\n",w2,w2 );
  printf("(%s,%s)\n",w2,w3 );
  printf("(%s,%s)\n",w3,w3 );

  return NULL ;
}

static cell * GS_PROOFWI_gic_sub(cell * crs, cell * pair, cell * ast_L){

  char * B = pair->car ;
  char * y = pair->cdr->car ;

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com = integer_commit(crs, y) ;
  L = com ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * d1 = L->car ; L=L->cdr ;
  char * d2 = L->car ; L=L->cdr ;
  char * d3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",d1,B);
  printf("(%s,%s)\n",d2,B);
  printf("(%s,%s)\n",d3,B);

  return NULL ;
}

static cell * GS_PROOFWI_gic(cell * crs, cell * predicate, cell * ast_L){
  PRINT("% pattern 3.2.2 [",predicate,"]\n");

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;


  L = predicate ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  cell * start = L ;
  int    first ;

  /* Proof */

  cell * val1 = LIST(TYPE_g,NULL,NULL,NULL) ;
  for(first = 1, L = start;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    if(first)first=0; else printf(",");
    printf("%s", pair->car);
    val1 = INTEGRATE(val1, pair->car) ;
  }

  char * phi1 = new_symbol("phi1",TYPE_g,val1) ;
  printf(" -> %s\n", phi1) ;

  cell * val2 = LIST(TYPE_g,NULL,NULL,NULL) ;
  for(first = 1, L = start;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    if(first)first=0; else printf(",");
    printf("%s", pair->car);
    val2 = INTEGRATE(val2, pair->car) ;
  }

  char * phi2 = new_symbol("phi2",TYPE_g,val2) ;
  printf(" -> %s\n", phi2) ;

  /* Verify */

  for(L = start;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    GS_PROOFWI_gic_sub(crs, pair, ast_L) ;
  }

  printf("(%s,%s)\n",u,phi1);
  printf("(%s,%s)\n",g,phi1);
  printf("(%s,%s)\n",v,phi2);
  printf("(%s,%s)\n",g,phi2);

  return NULL ;
}

static cell * GS_PROOFWI_gcic_sub1(cell * crs, cell * pair, cell * ast_L){

  char * B = pair->car ;
  char * y = pair->cdr->car ;

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com = integer_commit(crs, y);
  L = com ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * d1 = L->car ; L=L->cdr ;
  char * d2 = L->car ; L=L->cdr ;
  char * d3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",w1,d1);
  printf("(%s,%s)\n",w1,d2);
  printf("(%s,%s)\n",w1,d3);

  printf("(%s,%s)\n",w2,d1);
  printf("(%s,%s)\n",w2,d2);
  printf("(%s,%s)\n",w2,d3);

  printf("(%s,%s)\n",w3,d1);
  printf("(%s,%s)\n",w3,d2);
  printf("(%s,%s)\n",w3,d3);

  printf("(%s,%s)\n",g,d1);

  printf("(%s,%s)\n",d1,B);
  printf("(%s,%s)\n",d2,B);
  printf("(%s,%s)\n",d3,B);

  return NULL ;
}

static cell * GS_PROOFWI_gcic_sub2(cell * crs, cell * pair, cell * ast_L){

  char * X = pair->car ;
  char * y = pair->cdr->car ;

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com1 = group_commit(crs, X) ;
  L = com1 ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * c1 = L->car ; L=L->cdr ;
  char * c2 = L->car ; L=L->cdr ;
  char * c3 = L->car ; L=L->cdr ;

  cell * com2 = integer_commit(crs, y);
  L = com2 ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * d1 = L->car ; L=L->cdr ;
  char * d2 = L->car ; L=L->cdr ;
  char * d3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",d1,c1);
  printf("(%s,%s)\n",d1,c2);
  printf("(%s,%s)\n",d1,c3);

  printf("(%s,%s)\n",d2,c1);
  printf("(%s,%s)\n",d2,c2);
  printf("(%s,%s)\n",d2,c3);

  printf("(%s,%s)\n",d3,c1);
  printf("(%s,%s)\n",d3,c2);
  printf("(%s,%s)\n",d3,c3);

  return NULL ;
}

static cell * GS_PROOFWI_gcic(cell * crs, cell * predicate, cell * ast_L){
  PRINT("% pattern 3.2.3 [",predicate,"]\n");

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;


  /* Proof */

  char * phi11 = new_symbol("phi11",TYPE_g,TYPE_(g, u,w1)) ;
  char * phi21 = new_symbol("phi21",TYPE_g,TYPE_(g, u,w1)) ;
  char * phi31 = new_symbol("phi31",TYPE_g,TYPE_(g, u,w1)) ;

  printf("%s,%s -> %s\n",u,w1,phi11)  ;
  printf("%s,%s -> %s\n",u,w1,phi21)  ;
  printf("%s,%s -> %s\n",u,w1,phi31)  ;

  char * phi12 = new_symbol("phi12",TYPE_g,TYPE_(g, v,w2)) ;
  char * phi22 = new_symbol("phi22",TYPE_g,TYPE_(g, v,w2)) ;
  char * phi32 = new_symbol("phi32",TYPE_g,TYPE_(g, v,w2)) ;

  printf("%s,%s -> %s\n",v,w2,phi12)  ;
  printf("%s,%s -> %s\n",v,w2,phi22)  ;
  printf("%s,%s -> %s\n",v,w2,phi32)  ;

  L = predicate ;
  L=L->cdr ; // type_name
  cell * LINEAR_gc  = (cell *)L->car ; L=L->cdr ; // opt1
  cell * LINEAR_gic = (cell *)L->car ; L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  cell * start_gcic = L ;

  cell * start_gic = NULL ;
  L = LINEAR_gic ;
  if(L){
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
    start_gic = L;
  }

  cell * start_gc = NULL ;
  L = LINEAR_gc ;
  if(L){
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
    start_gc = L ;
  }

  printf("%s,%s",g,w3) ;
  cell * val1 = TYPE_(g, g,w3) ;
  if(start_gic){
    for(L = start_gic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      printf(",%s", pair->car);
      val1 = INTEGRATE(val1, pair->car) ;
    }
  }
  if(start_gcic){
    for(L = start_gcic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      printf(",%s", pair->car) ;
      val1 = INTEGRATE(val1, pair->car) ;
    }
  }
  char * phi13 = new_symbol("phi13",TYPE_g,val1) ;
  printf(" -> %s\n", phi13) ;

  printf("%s,%s",g,w3) ;
  cell * val2 = TYPE_(g, g,w3) ;
  if(start_gic){
    for(L = start_gic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      printf(",%s", pair->car);
      val2 = INTEGRATE(val2, pair->car) ;
    }
  }
  if(start_gcic){
    for(L = start_gcic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      printf(",%s", pair->car) ;
      val2 = INTEGRATE(val2, pair->car) ;
    }
  }
  char * phi23 = new_symbol("phi23",TYPE_g,val2) ;
  printf(" -> %s\n", phi23) ;

  printf("%s,%s",g,w3) ;
  cell * val3 = TYPE_(g, g,w3) ;
  if(start_gic){
    for(L = start_gic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      printf(",%s", pair->car);
      val3 = INTEGRATE(val3, pair->car) ;
    }
  }
  if(start_gcic){
    for(L = start_gcic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      printf(",%s", pair->car) ;
      val3 = INTEGRATE(val3, pair->car) ;
    }
  }
  char * phi33 = new_symbol("phi33",TYPE_g,val3) ;
  printf(" -> %s\n", phi33) ;

  /* Verify */

  printf("(%s,%s)\n",u,phi11);
  printf("(%s,%s)\n",u,phi12);
  printf("(%s,%s)\n",u,phi13);

  printf("(%s,%s)\n",g,phi11);
  printf("(%s,%s)\n",g,phi12);
  printf("(%s,%s)\n",g,phi13);

  printf("(%s,%s)\n",v,phi21);
  printf("(%s,%s)\n",v,phi22);
  printf("(%s,%s)\n",v,phi23);

  printf("(%s,%s)\n",g,phi21);
  printf("(%s,%s)\n",g,phi22);
  printf("(%s,%s)\n",g,phi23);

  printf("(%s,%s)\n",w1,phi31);
  printf("(%s,%s)\n",w2,phi31);
  printf("(%s,%s)\n",w3,phi31);

  printf("(%s,%s)\n",w1,phi32);
  printf("(%s,%s)\n",w2,phi32);
  printf("(%s,%s)\n",w3,phi32);

  printf("(%s,%s)\n",w1,phi33);
  printf("(%s,%s)\n",w2,phi33);
  printf("(%s,%s)\n",w3,phi33);

  if(start_gic){
    for(L = start_gic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      GS_PROOFWI_gcic_sub1(crs, pair, ast_L) ;
    }
  }

  if(start_gcic){
    for(L = start_gic;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      GS_PROOFWI_gcic_sub2(crs, pair, ast_L) ;
    }
  }

  return NULL ;
}

static cell * GS_PROOFWI_eggc_sub(cell * crs, cell * pair, cell * ast_L){

  char * A = pair->car ;
  char * X = pair->cdr->car ;

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com = group_commit(crs,X) ;
  L = com ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * c1 = L->car ; L=L->cdr ;
  char * c2 = L->car ; L=L->cdr ;
  char * c3 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",A,c1);
  printf("(%s,%s)\n",A,c2);
  printf("(%s,%s)\n",A,c3);

  return NULL ;

}

static cell * GS_PROOFWI_eggc(cell * crs, cell * predicate, cell * ast_L){
  PRINT("% pattern 3.1.1 [",predicate,"]\n");

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  L = predicate ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  cell * start = L ;
  int    first ;

  /* Proof */

  cell * val = LIST(TYPE_g,NULL,NULL,NULL) ;
  for(first = 1, L = start;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    if(first)first=0; else printf(",");
    printf("%s", pair->car);
    val = INTEGRATE(val, pair->car) ;
  }
  char * phi1 = new_symbol("phi1",TYPE_g,val) ;
  printf(" -> %s\n", phi1) ;

  val = LIST(TYPE_g,NULL,NULL,NULL) ;
  for(first = 1, L = start;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    if(first)first=0; else printf(",");
    printf("%s", pair->car);
    val = INTEGRATE(val, pair->car) ;
  }
  char * phi2 = new_symbol("phi2",TYPE_g,val) ;
  printf(" -> %s\n", phi2) ;

  val = LIST(TYPE_g,NULL,NULL,NULL) ;
  for(first = 1, L = start;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    if(first)first=0; else printf(",");
    printf("%s", pair->car);
    val = INTEGRATE(val, pair->car) ;
  }
  char * phi3 = new_symbol("phi3",TYPE_g,val) ;
  printf(" -> %s\n", phi3) ;

  /* Verify */

  if(start){
    for(L = start;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      GS_PROOFWI_eggc_sub(crs, pair, ast_L) ;
    }
  }

  printf("(%s,%s)\n",phi1,u);
  printf("(%s,%s)\n",phi1,g);
  printf("(%s,%s)\n",phi2,v);
  printf("(%s,%s)\n",phi2,g);

  printf("(%s,%s)\n",phi3,w1);
  printf("(%s,%s)\n",phi3,w2);
  printf("(%s,%s)\n",phi3,w3);

  return NULL ;
}

static cell * GS_PROOFWI_egc2_sub1(cell * crs, cell * pair, cell * ast_L){
  return GS_PROOFWI_eggc_sub(crs, pair, ast_L) ;
}

static cell * GS_PROOFWI_egc2_sub2(cell * crs, cell * pair, cell * ast_L){

  char * X = pair->car ;
  char * Y = pair->cdr->car ;

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * com1 = group_commit(crs, X) ;
  L = com1 ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * c11 = L->car ; L=L->cdr ;
  char * c12 = L->car ; L=L->cdr ;
  char * c13 = L->car ; L=L->cdr ;

  cell * com2 = group_commit(crs, Y) ;
  L = com2 ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * c21 = L->car ; L=L->cdr ;
  char * c22 = L->car ; L=L->cdr ;
  char * c23 = L->car ; L=L->cdr ;

  printf("(%s,%s)\n",c11,c21);
  printf("(%s,%s)\n",c11,c22);
  printf("(%s,%s)\n",c11,c23);

  return NULL ;
}

static cell * GS_PROOFWI_egc2(cell * crs, cell * predicate, cell * ast_L){
  PRINT("% pattern 3.1.2 [",predicate,"]\n");

  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  L = predicate ;
  L=L->cdr ; // type_name
  cell * LINEAR_eggc = (cell *)L->car ; L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  cell * start_egc2 = L ;

  cell * start_eggc = NULL ;
  L = LINEAR_eggc ;
  if(L){
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
    start_eggc = L ;
  }

  /* Proof */

  cell * val   = NULL ;

  int    first ;

  if(start_eggc){
    val = LIST(TYPE_g,NULL,NULL,NULL) ;
    for(first = 1, L = start_eggc;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      if(first)first=0; else printf(",");
      printf("%s", pair->car);
      val = INTEGRATE(val, pair->car) ;
    }
    printf(" -> ") ;
  }
  char * phi13 = new_symbol("phi13",TYPE_g,val) ;
  printf("%s\n", phi13) ;

  if(start_eggc){
    for(first = 1, L = start_eggc;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      if(first)first=0; else printf(",");
      printf("%s", pair->car);
    }
    printf(" -> ") ;
  }
  char * phi23 = new_symbol("phi23",TYPE_g,val) ;
  printf("%s\n", phi23) ;

  if(start_eggc){
    for(first = 1, L = start_eggc;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      if(first)first=0; else printf(",");
      printf("%s", pair->car);
    }
    printf(" -> ") ;
  }
  char * phi33 = new_symbol("phi33",TYPE_g,val) ;
  printf("%s\n", phi33) ;

  val = NULL ;
  if(start_egc2){
    val = LIST(TYPE_g,NULL,NULL,NULL) ;
    for(first = 1, L = start_egc2;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      if(first)first=0; else printf(",");
      printf("%s,%s", pair->car, pair->cdr->car);
      val = INTEGRATE(val, pair->car) ;
    }
    printf(" -> ") ;
  }
  register_value(phi13, val) ;
  printf("%s\n", phi13) ;

  if(start_egc2){
    for(first = 1, L = start_egc2;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      if(first)first=0; else printf(",");
      printf("%s,%s", pair->car, pair->cdr->car);
    }
    printf(" -> ") ;
  }
  register_value(phi23, val) ;
  printf("%s\n", phi23) ;

  if(start_egc2){
    for(first = 1, L = start_egc2;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      if(first)first=0; else printf(",");
      printf("%s,%s", pair->car, pair->cdr->car);
    }
    printf(" -> ") ;
  }
  register_value(phi33, val) ;
  printf("%s\n", phi33) ;

  val = TYPE_(g, u,w1) ;
  char * phi11 = new_symbol("phi11",TYPE_g,val) ;
  char * phi21 = new_symbol("phi21",TYPE_g,val) ;
  char * phi31 = new_symbol("phi31",TYPE_g,val) ;
  printf("%s,%s -> %s\n",u,w1,phi11) ;
  printf("%s,%s -> %s\n",u,w1,phi21) ;
  printf("%s,%s -> %s\n",u,w1,phi31) ;

  val = TYPE_(g, v,w2) ;
  char * phi12 = new_symbol("phi12",TYPE_g,val) ;
  char * phi22 = new_symbol("phi22",TYPE_g,val) ;
  char * phi32 = new_symbol("phi32",TYPE_g,val) ;
  printf("%s,%s -> %s\n",v,w2,phi12) ;
  printf("%s,%s -> %s\n",v,w2,phi22) ;
  printf("%s,%s -> %s\n",v,w2,phi32) ;

  val = TYPE_(g, g,w3) ;
  register_value(phi13, val) ;
  register_value(phi23, val) ;
  register_value(phi33, val) ;
  printf("%s,%s -> %s\n",g,w3,phi13) ;
  printf("%s,%s -> %s\n",g,w3,phi23) ;
  printf("%s,%s -> %s\n",g,w3,phi33) ;

  /* Verify */

  if(start_eggc){
    for(L = start_eggc;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      GS_PROOFWI_egc2_sub1(crs, pair, ast_L) ;
    }
  }

  if(start_egc2){
    for(L = start_egc2;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      GS_PROOFWI_egc2_sub2(crs, pair, ast_L) ;
    }
  }

  printf("(%s,%s)\n",u,phi11);
  printf("(%s,%s)\n",u,phi12);
  printf("(%s,%s)\n",u,phi13);

  printf("(%s,%s)\n",g,phi11);
  printf("(%s,%s)\n",g,phi12);
  printf("(%s,%s)\n",g,phi13);

  printf("(%s,%s)\n",v,phi21);
  printf("(%s,%s)\n",v,phi22);
  printf("(%s,%s)\n",v,phi23);

  printf("(%s,%s)\n",g,phi21);
  printf("(%s,%s)\n",g,phi22);
  printf("(%s,%s)\n",g,phi23);

  printf("(%s,%s)\n",w1,phi11);
  printf("(%s,%s)\n",w1,phi12);
  printf("(%s,%s)\n",w1,phi13);

  printf("(%s,%s)\n",w2,phi21);
  printf("(%s,%s)\n",w2,phi22);
  printf("(%s,%s)\n",w2,phi23);

  printf("(%s,%s)\n",w3,phi31);
  printf("(%s,%s)\n",w3,phi32);
  printf("(%s,%s)\n",w3,phi33);

  return NULL ;
}

int NOT_YET_PROVEN(cell * crs, cell * predicate){
  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1 (commit stack)
  cell * proof_history = (cell *) L->car ; L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;
  int ret = 1 ;
  L = proof_history ;
  for(;L;L=L->cdr){
    if(EQUAL(predicate, (cell *)L->car)){
      ret = 0 ;
      break ;
    }
  }
  return ret ;
}

void push_proof_history(cell * crs, cell * predicate){
  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1 (commit stack)
  cell * proof_history = (cell *) L->car ;
  if(NOT_YET_PROVEN(crs, predicate)){
    L->car = (char *) INTEGRATE(proof_history, predicate) ;
  }
}

cell * GS_PROOFWI(cell * crs, cell * predicate, cell * ast_L){
  if( is_AND (predicate) ){
    return GS_PROOFWI_AND (crs, predicate, ast_L) ;
  } else {
    if(NOT_YET_PROVEN(crs, predicate)){
      if( is_ic  (predicate) ) return GS_PROOFWI_ic  (crs, predicate, ast_L) ;
      if( is_ic2 (predicate) ) return GS_PROOFWI_ic2 (crs, predicate, ast_L) ;
      if( is_gc  (predicate) ) return GS_PROOFWI_gc  (crs, predicate, ast_L) ;
      if( is_gic (predicate) ) return GS_PROOFWI_gic (crs, predicate, ast_L) ;
      if( is_gcic(predicate) ) return GS_PROOFWI_gcic(crs, predicate, ast_L) ;
      if( is_eggc(predicate) ) return GS_PROOFWI_eggc(crs, predicate, ast_L) ;
      if( is_egc2(predicate) ) return GS_PROOFWI_egc2(crs, predicate, ast_L) ;
      operation_error(ast_L, string("invalid predicate: ") + type_of(predicate) ) ;
    }
  }
  return NULL ;
}

// ==========================================================================================
// GS_PROOFZK
// ==========================================================================================

cell * GS_PROOFZK(cell * crs, cell * predicate, cell * ast_L) ;

static cell * GS_PROOFZK_AND(cell * crs, cell * predicate, cell * ast_L){
  cell * L = predicate ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  for(;L;L=L->cdr) GS_PROOFZK(crs, (cell *)L->car, ast_L) ;
  return NULL ;
}

static cell * GS_PROOFZK_QE_ic2(cell * crs, cell * predicate, cell * ast_L){
  cell * R = DUPLICATE(predicate) ;
  cell * L = R ;
  L=L->cdr ; // type_name
  cell * LINEAR_ic = (cell *)L->car ; L=L->cdr ; // opt1
  cell * CONST_i   = (cell *)L->car ; L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  char * t ;

  if(!CONST_i){
    t = new_symbol("t",TYPE_i,NULL) ;
  }else if(!is_single(CONST_i)){
    t = new_symbol("t",TYPE_i,NULL) ;
  }else{
    t = first_arg(CONST_i) ;
  }
  GS_PROOFWI(crs, LIST(TYPE_ic, LIST(TYPE_i,NULL,NULL,NULL,t), NULL, NULL, t), ast_L) ;

  L = R ;
  L=L->cdr ; // type_name
  L->car = (char *) ADD(LINEAR_ic, LIST(TYPE_ic,NULL,NULL,NULL,t),ast_L) ; L=L->cdr ; // opt1
  L->car = NULL ; L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  return GS_PROOFWI(crs, R , ast_L) ;
}

static cell * GS_PROOFZK_QE(cell * crs, cell * predicate, cell * ast_L){
  if(type_of(predicate) == TYPE_ic2) {
    PRINT("% pattern 4.3.2 [",predicate,"]\n");
    return GS_PROOFZK_QE_ic2(crs, predicate, ast_L);
  }else if(type_of(predicate) == TYPE_ic) {
    PRINT("% pattern 4.3.1 [",predicate,"]\n");
    return GS_PROOFWI(crs, predicate, ast_L) ;
  }else{
    operation_error(ast_L, string("invalid ZK preprocess(QE): ") + type_of(predicate) ) ;
  }
  return NULL ;
}

static char * constant_z = new_symbol("z$",TYPE_i,NULL) ;

static cell * GS_PROOFZK_MSM_CONST_g(cell * crs, cell * predicate, cell * ast_L, cell * CONST_g){

  char * z = constant_z ;

  cell * B_g    = REDUCE(CONST_g,"B") ;
  cell * z_ic   = LIST(TYPE_ic, NULL, NULL, NULL, z) ;
  cell * Bz_gic = POW(B_g, z_ic, ast_L) ;
  cell * R      = MUL(predicate, Bz_gic, ast_L) ;

  GS_PROOFWI(crs, z_ic, ast_L) ;
  push_proof_history(crs, z_ic) ;
  return GS_PROOFWI(crs, R , ast_L) ;
}

static cell * GS_PROOFZK_MSM(cell * crs, cell * predicate, cell * ast_L){
  cell * L      = predicate ;
  cell * CONST_g = NULL ;

  if(type_of(predicate) == TYPE_gcic) {
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    CONST_g = (cell *)L->car ; L=L->cdr ; // opt3
  }else if(
    (type_of(predicate) == TYPE_gic) || (type_of(predicate) == TYPE_gc)
  ) {
    L=L->cdr ; // type_name
    CONST_g = (cell *)L->car ; L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
  }else{
    operation_error(ast_L, string("invalid ZK preprocess(MSM): ") + type_of(predicate) ) ;
  }

  cell * start_t = NULL ;
  L = CONST_g ;
  if(L){
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
    start_t = L ;
  }

  if(! start_t){
    PRINT("% pattern 4.2.1 [",predicate,"]\n");
    return GS_PROOFWI(crs, predicate, ast_L) ;
  }else{
    PRINT("% pattern 4.2.2 [",predicate,"]\n");
    return GS_PROOFZK_MSM_CONST_g(crs, predicate, ast_L, CONST_g) ;
  }
  return NULL ;
}

static unordered_map<string, int> dl_known_table ;

static void check_dl_known (cell * g, cell * y, cell * ast_L) {
  if( !(is_g(g) && is_g(y)) ) {
    operation_error(ast_L, string("set_dl_known: ")
      + type_of(g) + "," + type_of(y) ) ;
  }
  if( ! is_single(g) )
    operation_error(ast_L, string("set_dl_known(g,y): g is not symbol"));
  if( ! is_single(y) )
    operation_error(ast_L, string("set_dl_known(g,y): y is not symbol"));
}

cell * SET_DL_KNOWN (cell * g, cell * y, cell * ast_L) {
  check_dl_known(g, y, ast_L) ;
  string pair_name = string(first_arg(g)) + "," + first_arg(y) ;
  dl_known_table[pair_name] = 1 ;
  return NULL ;
}

cell * SET_DL_UNKNOWN (cell * g, cell * y, cell * ast_L) {
  check_dl_known(g, y, ast_L);
  string pair_name = string(first_arg(g)) + "," + first_arg(y) ;
  dl_known_table[pair_name] = 0 ;
  return NULL ;
}

static int dl_known(char * g, char * y){
  string pair_name = string(g) + "," + y ;
  return dl_known_table[pair_name] ;
}

static int sym_knows_dl(char * g, char * var, cell * ast_L){
  return dl_known(g,var) ;
#if 0
  int i = find_identifier(var) ;
  if(i>=0){
    cell * L = identifier_stack[i].history ;
    for(;L;L=L->cdr){
      cell * val = (cell *)L->car ;
      if(val){
        if(is_single(val)){
	  if(!strcmp(g,first_arg(val))) return 1 ;
        }
      }
    }
  }else{
    operation_error(ast_L, string("identifier ") + var + " is not found (bug?)");
  }
  return 0 ;
#endif
}

static cell * GS_PROOFZK_PPE_sym_knows_all_dl(cell * crs, cell * predicate, cell * ast_L, cell * start_t){
  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;

  cell * R = DUPLICATE(predicate) ;
  L = R ;
  if(type_of(R) == TYPE_egc2) {
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L->car = NULL; L=L->cdr ; // opt2
    L=L->cdr ; // opt3
  }else if(type_of(R) == TYPE_eggc) {
    L=L->cdr ; // type_name
    L->car = NULL; L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
  }else{
    operation_error(ast_L, string("invalid ZK preprocess(PPE): ") + type_of(predicate) ) ;
  }

  cell * S = LIST(TYPE_eggc, NULL, NULL, NULL) ;

  for(L = start_t;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    char * A  = pair->car      ;
    char * Gb = pair->cdr->car ;
    if(sym_knows_dl(g,pair->car,ast_L)){
      A  = pair->cdr->car ;
      Gb = pair->car ;
    }
    GS_PROOFWI(crs, LIST(TYPE_gc, LIST(TYPE_g,NULL,NULL,NULL,Gb), NULL, NULL, Gb), ast_L) ;
    S = INTEGRATE(S, LIST(A,Gb) ) ;
  }
  R = MUL(R, S, ast_L) ;
  return GS_PROOFWI(crs, R, ast_L) ;
}

static int has_a_commit(char * commit, cell * predicate);

static int has_a_commit_eggc(char * commit, cell * predicate){
  cell * L = predicate ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  for(;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    char * A = pair->car      ;
    char * B = pair->cdr->car ;
    if(!strcmp(commit,B)) return 1 ;
  }
  return 0 ;
}

static int has_a_commit_egc2(char * commit, cell * predicate){
  cell * L = predicate ;
  L=L->cdr ; // type_name
  cell * LINEAR_eggc = (cell *)L->car ; L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3

  for(;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    char * A = pair->car      ;
    char * B = pair->cdr->car ;
    if(!strcmp(commit,A)) return 1 ;
    if(!strcmp(commit,B)) return 1 ;
  }
  return has_a_commit(commit, LINEAR_eggc);
}

static int has_a_commit(char * commit, cell * predicate){
  if(!predicate) return 0 ;
  if(type_of(predicate) == TYPE_egc2) return has_a_commit_egc2(commit, predicate) ;
  else if(type_of(predicate) == TYPE_eggc) return has_a_commit_eggc(commit, predicate) ;
  else return 0 ;
}

static cell * GS_PROOFZK_PPE_sym_doesnt_know_all_dl(cell * crs, cell * predicate, cell * ast_L, cell * start_t){
  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * R = DUPLICATE(predicate) ;
         L = R ;
  if(type_of(R) == TYPE_egc2) {
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L->car = NULL; L=L->cdr ; // opt2
    L=L->cdr ; // opt3
  }else if(type_of(R) == TYPE_eggc) {
    L=L->cdr ; // type_name
    L->car = NULL; L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
  }else{
    operation_error(ast_L, string("invalid ZK preprocess(PPE): ") + type_of(predicate) ) ;
  }

  static char * z = constant_z ;
  cell * z_ic   = LIST(TYPE_ic, NULL, NULL, NULL, z) ;
  GS_PROOFWI(crs, z_ic, ast_L) ;
  push_proof_history(crs, z_ic) ;

  cell * S = LIST(TYPE_eggc, NULL, NULL, NULL) ;

  for(L = start_t;L;L=L->cdr){
    cell * pair = (cell *) L->car ;
    char * A = pair->car      ;
    char * B = pair->cdr->car ;

    if( find_commit(A, crs) >= 0 || has_a_commit(A, predicate) ){
      char * T = A ; A = B ; B = T ;
    }
#if 0
    cell * B_g    = LIST(TYPE_g , NULL, NULL, NULL, B) ;
    cell * B_gc   = LIST(TYPE_gc, NULL, NULL, NULL, B) ;
    cell * Bz_gic = POW(B_g, z_ic, ast_L) ;
    cell * MSM    = MUL(B_gc, Bz_gic, ast_L) ;
    GS_PROOFWI(crs, MSM, ast_L) ;
#else
    cell * MSM = LIST(TYPE_eggc, NULL, NULL, NULL, LIST(g,B), LIST(B,z)) ;
    GS_PROOFWI(crs, MSM, ast_L) ;
    push_proof_history(crs, MSM) ;
#endif
    S = INTEGRATE(S, LIST(A,B)) ;
  }
  R = MUL(R, S, ast_L) ;
  return GS_PROOFWI(crs, R, ast_L) ;
}

static cell * GS_PROOFZK_PPE(cell * crs, cell * predicate, cell * ast_L){
  cell * L = crs ;
  L=L->cdr ; // type_name
  L=L->cdr ; // opt1
  L=L->cdr ; // opt2
  L=L->cdr ; // opt3
  char * g  = L->car ; L=L->cdr ;
  char * u  = L->car ; L=L->cdr ;
  char * v  = L->car ; L=L->cdr ;
  char * w1 = L->car ; L=L->cdr ;
  char * w2 = L->car ; L=L->cdr ;
  char * w3 = L->car ; L=L->cdr ;

  cell * CONST_t = NULL ;

  L = predicate ;
  if(type_of(predicate) == TYPE_egc2) {
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    CONST_t = (cell *)L->car ; L=L->cdr ; // opt2
    L=L->cdr ; // opt3
  }else if(type_of(predicate) == TYPE_eggc) {
    L=L->cdr ; // type_name
    CONST_t = (cell *)L->car ; L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
  }else{
    operation_error(ast_L, string("invalid ZK preprocess(PPE): ") + type_of(predicate) ) ;
  }

  cell * start_t = NULL ;
  L = CONST_t ;
  if(L){
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
    start_t = L ;
  }

  if(! start_t){
    PRINT("% pattern 4.1.1 [",predicate,"]\n");
    return GS_PROOFWI(crs, predicate, ast_L) ;
  }else{
    int sym_knows_all_dl = 1 ;
    for(L = start_t;L;L=L->cdr){
      cell * pair = (cell *) L->car ;
      if( ! (sym_knows_dl(g,pair->car,ast_L) || sym_knows_dl(g,pair->cdr->car,ast_L)) ){
        sym_knows_all_dl = 0 ;
        break ;
      }
    }
    if(sym_knows_all_dl){
      PRINT("% pattern 4.1.2 [",predicate,"]\n");
      return GS_PROOFZK_PPE_sym_knows_all_dl(crs, predicate, ast_L, start_t) ;
    }else{
      PRINT("% pattern 4.1.3 [",predicate,"]\n");
      return GS_PROOFZK_PPE_sym_doesnt_know_all_dl(crs, predicate, ast_L, start_t) ;
    }
  }
  return NULL ;
}

cell * GS_PROOFZK(cell * crs, cell * predicate, cell * ast_L){
  if( is_AND (predicate) ) return GS_PROOFZK_AND(crs, predicate, ast_L) ;
  if( is_ic  (predicate) ) return GS_PROOFZK_QE (crs, predicate, ast_L) ;
  if( is_ic2 (predicate) ) return GS_PROOFZK_QE (crs, predicate, ast_L) ;
  if( is_gc  (predicate) ) return GS_PROOFZK_MSM(crs, predicate, ast_L) ;
  if( is_gic (predicate) ) return GS_PROOFZK_MSM(crs, predicate, ast_L) ;
  if( is_gcic(predicate) ) return GS_PROOFZK_MSM(crs, predicate, ast_L) ;
  if( is_eggc(predicate) ) return GS_PROOFZK_PPE(crs, predicate, ast_L);
  if( is_egc2(predicate) ) return GS_PROOFZK_PPE(crs, predicate, ast_L);
  operation_error(ast_L, string("invalid predicate: ") + type_of(predicate) ) ;
  return NULL ;
}

#ifdef OPERATION_DEBUG

#include "type_define.txt"

int main(int argc, char **argv) {
  cell * X = LIST(TYPE_ic2, NULL, NULL, NULL, LIST("X", "Y"), LIST("Z","W") ) ;
  cell * Y = LIST(TYPE_ic, NULL, NULL, NULL, "W", "Y", "Z") ;
  cell * W = LIST(TYPE_i, NULL, NULL, NULL, "A", "B", "C") ;
  cell * V = LIST(TYPE_g, NULL, NULL, NULL, "g", "h", "f") ;
  cell * U = LIST(TYPE_gc, NULL, NULL, NULL, "g", "h", "f") ;
  PRINT("X=",X,"\n") ;
  PRINT("Y=",Y,"\n") ;
  cell * Z = ADD(W,Y,NULL) ;
  PRINT("Z=",Z,"\n") ;
  Z = ADD(Y,W,NULL) ;
  PRINT("Z=",Z,"\n") ;
  Z = MUL(Z,Z,NULL) ;
  PRINT("Z=",Z,"\n") ;
  Z = PAIRING(V,V,NULL) ;
  PRINT("Z=",Z,"\n") ;
  Z = PAIRING(U,V,NULL) ;
  PRINT("Z=",Z,"\n") ;

  Z = PAIRING(U,U,NULL) ;
  PRINT("Z=",Z,"\n") ;

//  cell * W = ADD(Z,Z) ;
//  PRINT("W=",W,"\n") ;
  return 0 ;
}

#endif
