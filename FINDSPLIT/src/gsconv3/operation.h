#ifndef OPERATION_H
#define OPERATION_H

#include <vector>
#include <iostream>
#include <boost/unordered_map.hpp>
#include "lisp.h"
#include "polynomial.h"

#ifdef  USE_LONG_LONG
#define BOOST_VERSION 105200
#else
#include <boost/version.hpp>
#endif

#if BOOST_VERSION >= 105300
#include <boost/multiprecision/cpp_int.hpp>
typedef boost::multiprecision::cpp_int                   integer_   ;
#else
#ifndef USE_LONG_LONG
#define USE_LONG_LONG
#endif
typedef long long int                                    integer_   ;
#endif

typedef Polynomial       <std::string, integer_, integer_> INTEGER   ;
typedef Monomial         <std::string, INTEGER         > GROUP     ;
typedef SymmetricPairing <GROUP                        > TARGETVAR ;
typedef Monomial         <TARGETVAR, INTEGER           > TARGET    ;

struct excell : cell {
  inline excell * element(int n){ return (excell *)cell::element(n) ; }
  inline excell * pointer(int n){ return (excell *)cell::pointer(n) ; }
} ;

extern std::vector<std::string> reduction_history ;
extern std::string              gsconv_input      ;
extern boost::unordered_map<std::string, int> gsconv_declared ;

std::ostream & operator<<(std::ostream & out, excell * L) ;

inline static char * type_of    (excell * L);

#define declare_TYPE_(NAME) extern char * TYPE_ ## NAME ;    \
inline static int is_ ## NAME (excell * X){                  \
  return (X) && (type_of(X) == TYPE_ ## NAME) ;             \
}                                                            \
inline static const int is_ ## NAME (                        \
  excell * X,                                                \
  excell * Y                                                 \
){                                                           \
  return is_ ## NAME (X) && is_ ## NAME (Y) ;                \
}

declare_TYPE_(NULL) ;
declare_TYPE_(INTEGER) ;
declare_TYPE_(GROUP) ;
declare_TYPE_(TARGET) ;
declare_TYPE_(LOGICAL_AND) ;
declare_TYPE_(LIST_VARIABLE) ;
declare_TYPE_(IDENTIFIER) ;
declare_TYPE_(LIST_LITERAL) ;
declare_TYPE_(OPTION) ;
declare_TYPE_(IMPOSSIBLE) ;
declare_TYPE_(POINTER) ;
declare_TYPE_(REFERENCE) ;

#define define_as_(TYPE)                                      \
inline static TYPE & as_ ## TYPE (excell * X){                \
  return *(TYPE *)(X->element(2)) ;                           \
}

define_as_(INTEGER) ;
define_as_(GROUP)   ;
define_as_(TARGET)  ;

typedef char * LIST_VARIABLE ;
inline static char * as_LIST_VARIABLE (excell * X){
  return (char *)(X->element(2)) ;
}

inline static char * type_of    (excell * L){ return L?L->car:TYPE_NULL ; }
inline static int    is_not_null(excell * L){ return (intptr_t)NTHCDR(2,L) ; }
inline static int    is_null    (excell * L){ return ! (intptr_t)NTHCDR(2,L) ; }
inline static int    is_single  (excell * L){ return (intptr_t)NTHCDR(2,L) && ! (intptr_t)NTHCDR(3,L) ; }
inline static char * first_arg  (excell * L){ return NTH(2,L) ; }


#define EXLIST(NAME, L, ...) ((excell*)LIST(TYPE_ ## NAME, L->element(1), ##__VA_ARGS__))

extern excell * operation_COM    (excell * X, cell * L);
extern excell * operation_PI     (excell * X, cell * L);
extern excell * operation_UPLUS  (excell * X, cell * L);
extern excell * operation_UMINUS (excell * X, cell * L);
extern excell * operation_ADD    (excell * X, excell * Y, cell * L);
extern excell * operation_SUB    (excell * X, excell * Y, cell * L);
extern excell * operation_MUL    (excell * X, excell * Y, cell * L);
extern excell * operation_DIV    (excell * X, excell * Y, cell * L);
extern excell * operation_POW    (excell * X, excell * Y, cell * L);
extern excell * operation_PAIRING(excell * X, excell * Y, cell * L);
extern excell * operation_ASSIGN (excell * X, excell * Y, cell * L);
extern excell * operation_EQ     (excell * X, excell * Y, cell * L);
extern excell * operation_ARRAY_REF(excell * X, excell * Y, cell * L);
extern excell * operation_LOGICAL_AND(excell * X, excell * Y, cell * L);
extern excell * operation_DOT    (excell * X, excell * Y, cell * L);
extern excell * operation_GS_COMMIT(excell * crs, excell * X, cell * L);
extern excell * operation_GS_PROOFWI(excell * crs, excell * predicate, cell * L) ;
extern excell * operation_GS_PROOFZK(excell * crs, excell * predicate, cell * L) ;
extern excell * operation_GS_VERIFYWI(excell * crs, excell * predicate, excell * proof, cell * L) ;
extern excell * operation_GS_VERIFYZK(excell * crs, excell * predicate, excell * proof, cell * L) ;

extern excell * operation_OPTION_CRS(cell * L);
extern excell * operation_GS_COMMIT_G(excell * crs, cell * L);

extern excell * operation_GS_DUMMY_COMMIT (excell * crs, excell * G, cell * L) ;

extern excell * operation_GS_VERIFY2(excell * proof, cell * L) ;
extern excell * operation_GS_SETUPB2(excell * X, cell * L);
extern excell * operation_GS_SETUPH2(excell * X, cell * L);
extern excell * operation_STATISTICS(excell * X, cell * L);
extern excell * operation_SETWEIGHT  (excell * X, excell * Y, cell * L);
extern excell * operation_SETPRIORITY(excell * X, excell * Y, cell * L);
extern excell * operation_EXCLUDE    (excell * X, excell * Y, cell * L);

extern int operation_is_CRS (excell * X);
extern int operation_is_COMMIT (excell * X);
extern std::string as_string(excell * X);
extern char * as_char_star(excell * X);
extern excell * GENERATE_CRS_if_listvar (excell * crs, cell * L);
extern void record_reduction(const std::string & s) ;


/*
extern void reduction (char * var, excell * X, char * operator_, excell * Y);
extern void reduction (excell * X, excell * Y);
extern void reduction (char * X, excell * Y);
*/

extern boost::unordered_map<std::string, int> group_of ;
extern boost::unordered_map<std::string, int> weight ;
extern boost::unordered_map<std::string, int> weight_candidate ;
extern std::map<int, boost::unordered_map<std::string, int> > priority ;
extern std::set< std::pair<char *, char *> > exclusion ;

extern int reconstruction_mode ;

#endif
