#include "operation.h"
#include "semantic.h"
#include <iostream>

#include <set>
#include <vector>
#include <tuple>
#include <stdint.h>
#include <boost/unordered_map.hpp>
#include <string.h>
#include "reserved_word.h"
#include "ast2str.h"
#include "declaration.h"
// #include <regex>
#include <string>

static excell * CRS        (excell * X, cell * L) ;
static excell * COM        (excell * X, cell * L) ;
static excell * PI         (excell * X, cell * L) ;
static excell * UPLUS      (excell * X, cell * L) ;
static excell * UMINUS     (excell * X, cell * L) ;
static excell * ADD        (excell * X, excell * Y, cell * L) ;
static excell * SUB        (excell * X, excell * Y, cell * L) ;
static excell * MUL        (excell * X, excell * Y, cell * L) ;
static excell * DIV        (excell * X, excell * Y, cell * L) ;
static excell * POW        (excell * X, excell * Y, cell * L) ;
static excell * PAIRING    (excell * X, excell * Y, cell * L) ;
static excell * EQ         (excell * X, excell * Y, cell * L) ;
static excell * ASSIGN     (excell * X, excell * Y, cell * L, int w = 0) ;
static excell * ARRAY_REF  (excell * X, excell * Y, cell * L) ;
static excell * DOT        (excell * X, excell * Y, cell * L) ;
static excell * LOGICAL_AND(excell * X, excell * Y, cell * L) ;
static excell * GS_PROOFWI (excell * crs, excell * predicate, cell * L) ;
static excell * GS_PROOFZK (excell * crs, excell * predicate, cell * L) ;
static excell * GS_VERIFYWI(excell * crs, excell * predicate, excell * proof, cell * L) ;
static excell * GS_VERIFYZK(excell * crs, excell * predicate, excell * proof, cell * L) ;
static excell * COMMIT     (excell * crs, excell * G, cell * L);
static excell * COMMIT_G   (excell * crs, cell * L);
static excell * DUMMY_COMMIT(excell * crs, excell * G, cell * L);
static excell * OPTION_CRS (cell * L) ;
static excell * GS_SETUPB2 (excell * G, cell * L);
static excell * GS_SETUPH2 (excell * G, cell * L);
static excell * GS_VERIFY2 (excell * proof, cell * L) ;
static excell * STATISTICS (excell * X, cell * L);
static excell * SETWEIGHT  (excell * X, excell * Y, cell * L);
static excell * SETPRIORITY(excell * X, excell * Y, cell * L);
static excell * EXCLUDE    (excell * X, excell * Y, cell * L);

excell * operation_COM        (excell * X, cell * L){
//  std::cout << "/* COM begin " << std::endl ;
//  std::cout << X         << std::endl ;
//  std::cout << "*/"      << std::endl ;
  excell * R = COM(X,L) ; 
//  std::cout << "/* COM end " << std::endl ;
//  std::cout << R         << std::endl ;
//  std::cout << "*/"      << std::endl ;
  return R ;
}
excell * operation_PI         (excell * X, cell * L){ return PI(X,L) ; }
excell * operation_UPLUS      (excell * X, cell * L){ return UPLUS(X,L) ; }
excell * operation_UMINUS     (excell * X, cell * L){ return UMINUS(X,L) ; }
excell * operation_ADD        (excell * X, excell * Y, cell * L){ return ADD(X,Y,L) ; }
excell * operation_SUB        (excell * X, excell * Y, cell * L){ return SUB(X,Y,L) ; }
excell * operation_MUL        (excell * X, excell * Y, cell * L){ return MUL(X,Y,L) ; }
excell * operation_DIV        (excell * X, excell * Y, cell * L){ return DIV(X,Y,L) ; }
excell * operation_POW        (excell * X, excell * Y, cell * L){ return POW(X,Y,L) ; }
excell * operation_PAIRING    (excell * X, excell * Y, cell * L){ return PAIRING(X,Y,L) ; }
excell * operation_EQ         (excell * X, excell * Y, cell * L){ return EQ(X,Y,L) ; }
excell * operation_ASSIGN     (excell * X, excell * Y, cell * L){ return ASSIGN(X,Y,L,1) ; }
excell * operation_ARRAY_REF  (excell * X, excell * Y, cell * L){ return ARRAY_REF(X,Y,L) ; }
excell * operation_DOT        (excell * X, excell * Y, cell * L){ return DOT(X,Y,L) ; }
excell * operation_LOGICAL_AND(excell * X, excell * Y, cell * L){ return LOGICAL_AND(X,Y,L) ; }
excell * operation_GS_PROOFWI (excell * crs, excell * predicate, cell * L){ return GS_PROOFWI(crs,predicate,L) ; }
excell * operation_GS_PROOFZK (excell * crs, excell * predicate, cell * L){ return GS_PROOFZK(crs,predicate,L) ; }
excell * operation_GS_VERIFYWI(excell * crs, excell * predicate, excell * proof, cell * L){
  return GS_VERIFYWI(crs,predicate,proof,L) ;
}
excell * operation_GS_VERIFYZK(excell * crs, excell * predicate, excell * proof, cell * L){
  return GS_VERIFYZK(crs,predicate,proof,L) ;
}
excell * operation_GS_COMMIT       (excell * crs, excell * G, cell * L){ return COMMIT(crs,G,L) ; }
excell * operation_GS_COMMIT_G     (excell * crs, cell * L){ return COMMIT_G(crs,L) ; }
excell * operation_GS_DUMMY_COMMIT (excell * crs, excell * G, cell * L){ return DUMMY_COMMIT(crs,G,L) ; }
excell * operation_OPTION_CRS      (cell * L){ return OPTION_CRS (L) ; } 

excell * operation_GS_SETUPB2 (excell * X, cell * L){ return GS_SETUPB2(X,L) ; }
excell * operation_GS_SETUPH2 (excell * X, cell * L){ return GS_SETUPH2(X,L) ; }
excell * operation_GS_VERIFY2 (excell * proof, cell * L){ return GS_VERIFY2(proof,L) ; }
excell * operation_STATISTICS (excell * X, cell * L) { return STATISTICS(X,L) ; }
excell * operation_SETWEIGHT (excell * X, excell * Y, cell * L){ return SETWEIGHT(X,Y,L); }
excell * operation_SETPRIORITY(excell * X, excell * Y, cell * L){return SETPRIORITY(X,Y,L);}
excell * operation_EXCLUDE    (excell * X, excell * Y, cell * L){return EXCLUDE(X,Y,L);}

static int is_OPT_CRS   (excell * X);
static int is_CRS   (excell * X);
static int is_COMMIT(excell * X);
int operation_is_CRS (excell * X){ return is_CRS(X); }
int operation_is_COMMIT (excell * X){ return is_COMMIT(X); }

static excell * GS_DUMMY_SETUP(cell * L) ;

using namespace std   ;
using namespace boost ;
#if BOOST_VERSION >= 105300
using namespace boost::multiprecision ;
#endif

vector<string> reduction_history = {""} ;
string         gsconv_input      =  ""  ;
unordered_map<string, int> gsconv_declared ;

void record_reduction(const string & s){
//  COUT(s) ;
  reduction_history.back() += s + "\n" ;
  gsconv_input += s + "\n" ;
}

template<typename key, typename val>
struct ordered_unordered_map : unordered_map<key, val> {
  vector<key> vec ;
  val & operator[](const key & k){
    auto i = this->find(k) ;
    if(i==this->end()) vec.push_back(k) ;
    val & v = (* (unordered_map<key, val> *) this)[k] ;
//    COUT(this->size()) ;
//    COUT(vec.size()) ;
    return v ;
  }
} ;

typedef ordered_unordered_map<string, excell *> COMMIT_STACK  ;
typedef ordered_unordered_map<string, excell *> PROOF_HISTORY ;

#define define_TYPE_(X) char * TYPE_ ## X = "TYPE_" # X

define_TYPE_(NULL)         ;
define_TYPE_(INTEGER)      ;
define_TYPE_(GROUP)        ;
define_TYPE_(TARGET)       ;
define_TYPE_(LOGICAL_AND)  ;
define_TYPE_(LIST_VARIABLE);
define_TYPE_(IDENTIFIER)   ;
define_TYPE_(LIST_LITERAL) ;

define_TYPE_(IMPOSSIBLE)   ;
define_TYPE_(OPTION)       ;
define_TYPE_(POINTER)      ;
define_TYPE_(REFERENCE)    ;


static excell * as_COMMITTED_VARIABLE(excell * X);
static int is_COMMITTED(excell * X);
static int is_RAW_PROOF(excell * X);

ostream & operator<<(ostream & out, excell * L){
  if(L){
         if(is_INTEGER       (L)) out << as_INTEGER(L) ;
    else if(is_GROUP         (L)) out << as_GROUP  (L) ;
    else if(is_TARGET        (L)) out << as_TARGET (L) ;
    else if(is_LIST_VARIABLE (L)) out << as_LIST_VARIABLE(L) ;
    else if(is_IDENTIFIER    (L)) out << as_LIST_VARIABLE(L) ;   // <=====
    else if(is_IMPOSSIBLE    (L)) out << as_LIST_VARIABLE(L) ;   // <=====
    else if(is_OPTION        (L)) out << as_LIST_VARIABLE(L)+4 ; // <=====
    else if(is_LIST_LITERAL  (L) || is_LOGICAL_AND(L)){
       char * separator ;
      if(is_LIST_LITERAL(L)) separator = ", " ;
      else if(is_LOGICAL_AND(L)) separator = " && " ;
      int is_raw_proof = is_RAW_PROOF(L) ;
      int first = 1;
      out << "(" ;
      for(
        excell * arg = (excell*)(L->pointer(2));
        is_raw_proof ? arg->cdr : arg ;
	arg=(excell*)arg->cdr
      ){
	if(first) first = 0; else out << separator ;
        out << (excell*)(arg->car) ;
      }
      out << ")" ;
    }
    else out << L->car + 5 ;
  }else{
    out << "NULL" ;
  }
  return out ;
}

static cell * DUPLICATE(cell * L);

string as_string(excell * X){
  stringstream s ;
  s << X ;
  return s.str() ;
}

template<typename TYPE>
static string as_string(const TYPE & X){
  stringstream s ;
  s << X ;
  return s.str() ;
}

char * as_char_star(excell * X){
  string s = as_string(X) ;
  return unique_symbol((char *)s.c_str()) ;
}

template<typename TYPE>
static char * as_char_star(TYPE & X){
  string s = as_string(X) ;
  return unique_symbol((char *)s.c_str()) ;
}

static char * reduction_resolve_type(excell * X){
  if ( X->car == TYPE_INTEGER      ) return "integer " ;
  if ( X->car == TYPE_GROUP        ) return "group "   ;
  if ( X->car == TYPE_TARGET       ) return "target "  ;
  if ( X->car == TYPE_LIST_VARIABLE) return "list "    ;
  if ( X->car == TYPE_OPTION       ) return "option "  ;
  if ( X->car == TYPE_LOGICAL_AND  ) return "integer "  ;
  return "" ;
}

excell * lastvalue(excell * X) ;

static inline void REDUCE_OPTION(excell * X, excell * Y){
#if 0
  if(is_OPT_CRS(X) && is_LIST_VARIABLE(Y)) REDUCE_OPTION(X,lastvalue(Y)) ;
  if(is_LIST_VARIABLE(X) && is_OPT_CRS(Y)) REDUCE_OPTION(lastvalue(X),Y) ;
  if(is_OPT_CRS(X) && is_OPT_CRS(Y)){
    printf("X=%p\n",X) ;
    printf("Y=%p\n",Y) ;
    cout << "----" << endl ;
    if(X!=Y){
// EXLIST(OPTION, L, OPT_CRS, commit_stack, proof_history, proof_history_zk)
      COMMIT_STACK & cs1 = *(COMMIT_STACK *) X->element(3) ;
      cout << "-- cs1 --" << endl ;
      for(auto i : cs1.vec){ COUT(i) ; }
      COMMIT_STACK & cs2 = *(COMMIT_STACK *) Y->element(3) ;
      cout << "-- cs2 --" << endl ;
      for(auto i : cs2.vec){ COUT(i) ; }
    }
  }
#endif
}

static inline void REDUCE_OPTION(char * X, excell * Y){
#if 0
  REDUCE_OPTION(EXLIST(LIST_VARIABLE, dummy_LOCATION, X), Y);
#endif
}


unordered_map<string, int> weight_candidate ;
unordered_map<string, int> weight ;
unordered_map<string, int> group_of ;
int reconstruction_mode = 0 ;

void process_REDUCE_CORE(const string & s, int w = 0){
  int index1  = s.find(" ",0) ;
  int index2  = s.find(" ",index1+1) ;
  string type = s.substr(0, index1) ;
  string var  = s.substr(index1+1, index2 - index1 - 1) ;
  string rest = s.substr(index2+1) ;

  if(type == "group"){
    if(weight.find(var) == weight.end()){
      if(weight_candidate.find(var) == weight_candidate.end()){
        weight[var] = w ;
      }else{
        weight[var] = weight_candidate[var] ;
      }
    }
  }

  string t ;
  if(gsconv_declared[var]){
    t =              var + " " + rest ;
  }else{
    if(reconstruction_mode && (type == "group")){
      switch(group_of[var]){
        case  1: type = "group0" ; break;
        case  2: type = "group1" ; break;
        case  3: type = "duplex" ; break;
        default: type = "unused" ; break;
      }
    }
    t = type + " " + var + " " + rest ;
    gsconv_declared[var] = 1 ;
  }
  record_reduction(t) ;
}

int is_reduce_const(const string & s){
  if (reconstruction_mode) return 0 ;
  int index1  = s.find(" ",0) ;
  int index2  = s.find(" ",index1+1) ;
  string type = s.substr(0, index1) ;
  string var  = s.substr(index1+1, index2 - index1 - 1) ;
  string rest = s.substr(index2+1) ;
  if(((type=="group")||(type=="target"))&&(rest.find("= 1 ;")!=string::npos))return 1;
  else                                                                       return 0;
}

void REDUCE_CORE (const string & s, int w = 0){
  if(s.find('~')!=string::npos) return ;
  if(s.substr(0,5)=="list "   ) return ;
  if(s.substr(0,7)=="option " ) return ;
  if(is_reduce_const(s)       ) return ;
  process_REDUCE_CORE(s, w) ;
}

void REDUCE_CORE (const stringstream & ss, int w = 0){ REDUCE_CORE(ss.str(), w); }

// ========================================================================================

void REDUCE (char * var, excell * X, char * op, excell * Y){
  char * type = reduction_resolve_type(X) ;
  string s = string("(") + as_string(X) + ") " + op + " (" + as_string(Y) + ")" ;
  stringstream ss ;

  history[var].push_back(
    EXLIST(IMPOSSIBLE, dummy_LOCATION, unique_symbol((char *)s.c_str()), X, op, Y)
  ) ;

  ss << type << var << " = " << s << " ;" ;
  REDUCE_CORE(ss) ;
}

static excell * copy_attr(excell * X, excell * Y){
  if(is_LIST_VARIABLE(Y)){
    if(is_INTEGER(X))
      return EXLIST(INTEGER,dummy_LOCATION,new INTEGER(string((char*)Y->element(2))));
    else if(is_GROUP(X))
      return EXLIST(GROUP,dummy_LOCATION,new GROUP(string((char*)Y->element(2))));
    else if(is_TARGET(X))
      return EXLIST(TARGET,dummy_LOCATION,new TARGET(TARGETVAR(string((char*)Y->element(2))))) ;
    else
      return NULL ;
  }else return NULL ;
}

void REDUCE (excell * X, excell * Y, int w = 0){

  char * type ;
  if ( Y->car == TYPE_LIST_VARIABLE){
    type = reduction_resolve_type(X) ;
    if (
      (string(type) != "option ") && 
      (string(type) != "list ")){
      string y = as_string(Y) ;
      if(! gsconv_declared[y]){
        gsconv_declared[y] = 1 ;
        if(string(type) == "group ") {
          if(weight.find(y) == weight.end()){
            if(weight_candidate.find(y) == weight_candidate.end()){
              weight[y] = w ;
            }else{
              weight[y] = weight_candidate[y] ;
            }
          }
        }
        if(reconstruction_mode && (string(type) == "group ")){
          switch(group_of[y]){
            case  1: type = "group0 " ; break;
            case  2: type = "group1 " ; break;
            case  3: type = "duplex " ; break;
            default: type = "unused " ; break;
          }
        }
        string t = type + y + ";" ;
        record_reduction(t) ;

        {
          char * true_name = unique_symbol((char *)as_string(Y).c_str()) ;
          history[true_name].push_back(copy_attr(X,Y)) ;
        }

      }

    }
  }
  else type = reduction_resolve_type(Y) ;
  if(string(type) != "option "){
    stringstream ss ;
    ss << type << as_string(X) << " = " << as_string(Y) << " ;" ;
    REDUCE_CORE(ss, w) ;
  }else{
    REDUCE_OPTION(X, Y) ;
  }

  {
    char * true_name = unique_symbol((char *)as_string(X).c_str()) ;
    history[true_name].push_back(Y) ;
  }

}

void REDUCE (INTEGER & X, INTEGER & Y){
  history[ unique_symbol((char *)as_string(X).c_str()) ].push_back(
    EXLIST(INTEGER, dummy_LOCATION, new INTEGER(Y))
  ) ;

  char * type = "integer " ;
  stringstream ss ;
  ss << type << X << " = " << Y << " ;" ;
  REDUCE_CORE(ss) ;
}

void REDUCE (GROUP & X, GROUP & Y){
  history[ unique_symbol((char *)as_string(X).c_str()) ].push_back(
    EXLIST(GROUP, dummy_LOCATION, new GROUP(Y))
  ) ;

  char * type = "group " ;
  stringstream ss ;
  ss << type << X << " = " << Y << " ;" ;
  REDUCE_CORE(ss) ;
}

void REDUCE (TARGET & X, TARGET & Y){
  history[ unique_symbol((char *)as_string(X).c_str()) ].push_back(
    EXLIST(TARGET, dummy_LOCATION, new TARGET(Y))
  ) ;

  char * type = "target " ;
  stringstream ss ;
  ss << type << X << " = " << Y << " ;" ;
  REDUCE_CORE(ss) ;
}

void REDUCE (char * X, excell * Y){
  history[X].push_back(Y) ;

  char * type = reduction_resolve_type(Y) ;
  if(string(type) != "option "){
    stringstream ss ;
    ss << type << X << " = " << as_string(Y) << " ;" ;
    REDUCE_CORE(ss) ;
  }else{
    REDUCE_OPTION(X, Y);
  }
}

void REDUCE (char * X, GROUP & Y){
  history[X].push_back(EXLIST(GROUP, dummy_LOCATION, new GROUP(Y))) ;

  char * type = "group " ;
  stringstream ss ;
  ss << type << X << " = " << Y << " ;" ;
  REDUCE_CORE(ss) ;
}

void REDUCE (char * X, INTEGER & Y){
  history[X].push_back(EXLIST(INTEGER, dummy_LOCATION, new INTEGER(Y))) ;

  char * type = "integer " ;
  stringstream ss ;
  ss << type << X << " = " << Y << " ;" ;
  REDUCE_CORE(ss) ;
}

#if 0
char * DEREFERENCE (string s){
  static const regex re1( "^\\$"     ) ;
  static const regex re2( "_[0-9]+$" ) ;
  s = regex_replace( s, re1, string("") ) ;
  s = regex_replace( s, re2, string("") ) ;
  return unique_symbol((char *)s.c_str()) ;
}
#else
char * DEREFERENCE (string s){
  int length = s.size() ;
  int head   = 0 ;
  int tail   = length - 1 ;

  if(length > 0){
    for(int i=tail ; i>=head ; i--){
      if((s[i] < '0') || (s[i] > '9')){
        if(s[i]=='$') length = i ; // <== ¤³¤³
        break ;
      }
    }
//    if(s[0] == '$') { head++ ; length -- ; }
    s = s.substr(head,length) ;
  }
  return unique_symbol((char *)s.c_str()) ;
}
#endif

char * DEREFERENCE (excell * X){ return DEREFERENCE (as_string(X)) ; }

excell * lastvalue(excell * X){
  char * true_name = as_LIST_VARIABLE(X) ;
  vector<excell *> & h = history[true_name] ;
  for(int i=h.size()-1; i>=0; i--){
    if(h[i])return h[i] ;
  }
  return NULL ;
}

/*
static int is_LIST_VARIABLE_LIST(excell * X){
  for(excell * t = X->pointer(2); t ; t = (excell*) t->cdr ){
    if(! is_LIST_VARIABLE( (excell *) t->car )){
      if(! is_LIST_LITERAL( (excell *) t->car )) return 0 ;
      if(! is_LIST_VARIABLE_LIST( (excell *) t->car )) return 0 ;
    }
  }
  return 1 ;
}
*/

static int is_LIST_VARIABLE_LIST(excell * X){
  excell * t = X->pointer(2);
//  if(!t) return 0 ;
  for(; t ; t = (excell*) t->cdr){
    if(! is_LIST_VARIABLE( (excell *) t->car )){
      if(! is_LIST_LITERAL( (excell *) t->car )) return 0 ;
      if(! is_LIST_VARIABLE_LIST( (excell *) t->car )) return 0 ;
    }
  }
  return 1 ;
}

static excell * lastlist(excell * X){
  X = (excell *)DUPLICATE(X) ;
  for(excell * t = X->pointer(2); t ; t = (excell*) t->cdr ){
    if(is_LIST_VARIABLE( (excell *) t->car )){
      t->car = (char *) lastvalue( (excell *) t->car ) ;
    }else{
      if(is_LIST_VARIABLE_LIST( (excell *) t->car ))
        t->car = (char *) lastlist( (excell *) t->car ) ;
    }
  }
  return X ;
}

static excell * INTEGER_ZERO = EXLIST(INTEGER , dummy_LOCATION, new INTEGER()) ;
static excell * INTEGER_ONE  = EXLIST(INTEGER , dummy_LOCATION, new INTEGER(1)) ;
static excell * GROUP_ONE    = EXLIST(GROUP   , dummy_LOCATION, new GROUP()) ;
static excell * TARGET_ONE   = EXLIST(TARGET  , dummy_LOCATION, new TARGET()) ;

static excell * as_ONE(excell * X){
  if(is_INTEGER(X)) return INTEGER_ONE ;
  if(is_GROUP  (X)) return GROUP_ONE   ;
  if(is_TARGET (X)) return TARGET_ONE  ;
  return NULL ;
}

static inline int is_INTEGER_ZERO(INTEGER & X){ return X.size() == 0 ; }
static inline int is_INTEGER_ZERO(excell  * X){ return is_INTEGER_ZERO(as_INTEGER(X)) ; }
static inline int is_MONOMIAL(INTEGER & X){ return X.size() == 1 ; }
static inline int is_MONOMIAL(excell  * X){ return is_MONOMIAL(as_INTEGER(X)) ; }
static inline int is_integer (INTEGER & X){
  return is_INTEGER_ZERO(X) || (is_MONOMIAL(X) && (X.rbegin()->first == Monomial<string, integer_>())) ;
}
static inline int is_integer (excell  * X){ return is_integer(as_INTEGER(X)) ; }

static inline Monomial<string, integer_> last_base(INTEGER & X){ return X.begin()->first; }
static inline Monomial<string, integer_> last_base(excell * X){
  return last_base(as_INTEGER(X)) ;
}

static inline integer_ last_coefficient (INTEGER & X){ 
  if(is_INTEGER_ZERO(X))return 0 ;
  else return X.begin()->second ; 
}

static inline integer_ last_coefficient (excell  * X){
  return last_coefficient(as_INTEGER(X));
}

static inline int is_INTEGER_ONE (excell * X){
  return is_integer(X) && (last_coefficient(X) == integer_(1)) ; 
}

static inline integer_ as_integer (INTEGER & X){ return last_coefficient(X) ; }
static inline integer_ as_integer (excell  * X){ return last_coefficient(X) ; }
static inline int     as_int     (INTEGER & X){ return (int) as_integer(X) ; }
static inline int     as_int     (excell  * X){ return as_int(as_INTEGER(X)) ; }

#define UNARY_LIST_PROCESS(X,L,FUNC)                                                  \
  if(X==NULL)                              return NULL ;                              \
  if(is_LIST_VARIABLE(X))                  return FUNC(lastvalue(X), L) ;             \
  if(is_LIST_VARIABLE_LIST(X))             return FUNC(lastlist (X), L) ;             \
  if(is_LIST_LITERAL(X))  if(is_COMMIT(X)) return FUNC(as_COMMITTED_VARIABLE(X), L) ;

#define BINARY_LIST_PROCESS_0(X,Y,L,FUNC)                                               \
  if(X==NULL)                             return NULL ;                                 \
  if(Y==NULL)                             return NULL ;                                 \
  if(is_LIST_VARIABLE(X))                 return FUNC(lastvalue(X), Y, L) ;             \
  if(is_LIST_VARIABLE(Y))                 return FUNC(X, lastvalue(Y), L) ;             \
  if(is_LIST_VARIABLE_LIST(X))          return FUNC(lastlist (X), Y, L) ;             \
  if(is_LIST_VARIABLE_LIST(Y))          return FUNC(X, lastlist (Y), L) ;

#define BINARY_LIST_PROCESS(X,Y,L,FUNC)                                                 \
  BINARY_LIST_PROCESS_0(X,Y,L,FUNC) ;                                                   \
  if(is_LIST_LITERAL(X)) if(is_COMMIT(X)) return FUNC(as_COMMITTED_VARIABLE(X), Y, L) ; \
  if(is_LIST_LITERAL(Y)) if(is_COMMIT(Y)) return FUNC(X, as_COMMITTED_VARIABLE(Y), L) ;


#define TERNARY_LIST_PROCESS(X,Y,Z,L,FUNC)                                                 \
  if(X==NULL)                             return NULL ;                                    \
  if(Y==NULL)                             return NULL ;                                    \
  if(Z==NULL)                             return NULL ;                                    \
  if(is_LIST_VARIABLE(X))                 return FUNC(lastvalue(X), Y, Z, L) ;             \
  if(is_LIST_VARIABLE(Y))                 return FUNC(X, lastvalue(Y), Z, L) ;             \
  if(is_LIST_VARIABLE(Z))                 return FUNC(X, Y, lastvalue(Z), L) ;             \
  if(is_LIST_VARIABLE_LIST(X))            return FUNC(lastlist (X), Y, Z, L) ;             \
  if(is_LIST_VARIABLE_LIST(Y))            return FUNC(X, lastlist (Y), Z, L) ;             \
  if(is_LIST_VARIABLE_LIST(Z))            return FUNC(X, Y, lastlist (Z), L) ;             \
  if(is_LIST_LITERAL(Y)) if(is_COMMIT(Y)) return FUNC(X, as_COMMITTED_VARIABLE(Y), Z, L) ; \

// --------------------------------------------------------------
// SETWEIGHT
// --------------------------------------------------------------

static int is_left_value (excell * X) ;

static excell * SETWEIGHT  (excell * X, excell * Y, cell * L) {
  BINARY_LIST_PROCESS_0 (X,Y,L,SETWEIGHT) ;
  if(! is_INTEGER(X)) return NULL ;
  if(is_left_value(X)) return SETWEIGHT(lastvalue(X), Y, L) ;
  if(! is_integer(X)) return NULL ;
  if(is_LIST_LITERAL(Y)){
    for(auto arg=Y->pointer(2); arg; arg=(excell*)arg->cdr){
      SETWEIGHT(X,(excell*)arg->car,L) ;
    }
    return NULL ;
  }
  if(! is_left_value(Y)) return NULL ;
  if(! is_GROUP(Y)) return NULL ;
  if(is_COMMITTED(Y)) return NULL ;
  weight[as_string(Y)] = as_int(X) ;
  return NULL ;
}

// --------------------------------------------------------------
// SETPRIORITY
// --------------------------------------------------------------

map<int, unordered_map<string, int> > priority ;

static excell * SETPRIORITY(excell * X, excell * Y, cell * L) {
//  COUT(X) ;
//  COUT(Y) ;
  BINARY_LIST_PROCESS_0 (X,Y,L,SETPRIORITY) ;
  if(! is_INTEGER(X)) return NULL ;
  if(is_left_value(X)) return SETPRIORITY(lastvalue(X), Y, L) ;
  if(! is_integer(X)) return NULL ;
  if(is_LIST_LITERAL(Y)){
    for(auto arg=Y->pointer(2); arg; arg=(excell*)arg->cdr){
      SETPRIORITY(X,(excell*)arg->car,L) ;
    }
    return NULL ;
  }
  if(! is_left_value(Y)) return NULL ;
  if(! is_GROUP(Y)) return NULL ;
  if(is_COMMITTED(Y)) return NULL ;
  int n = as_int(X) ;
  priority[n][as_string(Y)] = n ;
  return NULL ;
}

// --------------------------------------------------------------
// EXCLUDE
// --------------------------------------------------------------

set< pair<char *, char *> > exclusion ;

static excell * EXCLUDE (excell * X, excell * Y, cell * L){
//  COUT(X) ;
//  COUT(Y) ;
  BINARY_LIST_PROCESS_0 (X,Y,L,EXCLUDE) ;
  if(is_LIST_LITERAL(Y)){
    for(auto arg=Y->pointer(2); arg; arg=(excell*)arg->cdr){
      EXCLUDE (X,(excell*)arg->car,L) ;
    }
    return NULL ;
  }
  if(! is_GROUP(Y)) return NULL ;
  if(is_COMMITTED(Y)) return NULL ;
  if(is_LIST_LITERAL(X)){
    for(auto arg=X->pointer(2); arg; arg=(excell*)arg->cdr){
      EXCLUDE ((excell*)arg->car,Y,L) ;
    }
    return NULL ;
  }
  if(! is_GROUP(X)) return NULL ;
  if(is_COMMITTED(X)) return NULL ;

  char * true_name_X = unique_symbol((char *)as_string(X).c_str()) ;
  char * true_name_Y = unique_symbol((char *)as_string(Y).c_str()) ;
  exclusion.insert(make_pair(true_name_X, true_name_Y)) ;
  return NULL ;
}

// --------------------------------------------------------------
// STATISTICS
// --------------------------------------------------------------

typedef LinearCombination<string, long long> linearcombination ;

linearcombination STATISTICS_RAW (excell * X){
  linearcombination R ;
  if(X){
         if(is_INTEGER       (X)) R += linearcombination("integer") ;
    else if(is_GROUP         (X)) R += linearcombination("group") ;
    else if(is_TARGET        (X)) R += linearcombination("target") ;
    else if(is_LIST_VARIABLE (X)) R += linearcombination("list_variable") ;
    else if(is_IDENTIFIER    (X)) R += linearcombination("identifier") ;
    else if(is_OPTION        (X)) R += linearcombination("option") ;
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      for(
        excell * arg = (excell*)(X->pointer(2));
        arg;
        arg=(excell*)arg->cdr
      ) R += STATISTICS_RAW((excell*)(arg->car)) ;
    }
  }else{
//    R += linearcombination("null") ;
  }
  return R ;
}

linearcombination STATISTICS_RAW_PROOF (excell * X){
  linearcombination R ;
  for(
    excell * arg = X->pointer(2) ;
    arg->cdr;
    arg=(excell*)arg->cdr
  ){
    excell * Y = (excell*)(arg->car) ;
         if(is_INTEGER (Y)) R += linearcombination("integer") ;
    else if(is_GROUP   (Y)) R += linearcombination("group") ;
    else if(is_TARGET  (Y)) R += linearcombination("target") ;
  }
  return R ;
}

linearcombination STATISTICS_PROOF (excell * X){
  linearcombination R ;
  if(X){
    if(is_LIST_LITERAL(X) && is_RAW_PROOF(X)){
      return STATISTICS_RAW_PROOF(X) ;
    }
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      for(
        excell * arg = (excell*)(X->pointer(2));
        arg;
	arg=(excell*)arg->cdr
      ){
        R += STATISTICS_PROOF ((excell*)(arg->car)) ;
      }
    }
  }
  return R ;
}

linearcombination VARIABLES_RAW_PROOF (excell * X){
  linearcombination R ;
  for(
    excell * arg = (excell*)(X->pointer(2));
    arg->cdr;
    arg=(excell*)arg->cdr
  ){
    excell * Y = (excell*)(arg->car) ;
         if(is_INTEGER (Y)) R += linearcombination(as_string(Y)) ;
    else if(is_GROUP   (Y)) R += linearcombination(as_string(Y)) ;
    else if(is_TARGET  (Y)) R += linearcombination(as_string(Y)) ;
  }
  return R ;
}

linearcombination VARIABLES_PROOF (excell * X){
  linearcombination R ;
  if(X){
    if(is_LIST_LITERAL(X) && is_RAW_PROOF(X)){
      return VARIABLES_RAW_PROOF(X) ;
    }
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      for(
        excell * arg = (excell*)(X->pointer(2));
        arg;
	arg=(excell*)arg->cdr
      ){
        R += VARIABLES_PROOF ((excell*)(arg->car)) ;
      }
    }
  }
  return R ;
}

linearcombination resolve_raw_group(excell * X){
  linearcombination R ;
  if(is_GROUP   (X)){
    string var = as_string(X) ;
    string type ;
    switch(group_of[var]){
      case  1: type = "group0" ; break;
      case  2: type = "group1" ; break;
      case  3: type = "duplex" ; break;
      default: type = reconstruction_mode ? "unused" : "group" ; break;
    }
    R += linearcombination(type) ;
  }
  return R ;
}

linearcombination STATISTICS_CONVR(excell * X){
  linearcombination R ;
  if(X){
         if(is_INTEGER       (X)) R += linearcombination("integer") ;
    else if(is_GROUP         (X)) R += resolve_raw_group(X) ;
    else if(is_TARGET        (X)) R += linearcombination("target") ;
    else if(is_LIST_VARIABLE (X)) R += linearcombination("list_variable") ;
    else if(is_IDENTIFIER    (X)) R += linearcombination("identifier") ;
    else if(is_OPTION        (X)) R += linearcombination("option") ;
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      for(
        excell * arg = (excell*)(X->pointer(2));
        arg;
        arg=(excell*)arg->cdr
      ) R += STATISTICS_CONVR((excell*)(arg->car)) ;
    }
  }else{
//    R += linearcombination("null") ;
  }
  return R ;
}

linearcombination STATISTICS_RAW_CONV (excell * X){
  linearcombination R ;
  for(
    excell * arg = (excell*)(X->pointer(2));
    arg->cdr;
    arg=(excell*)arg->cdr
  ){
    excell * Y = (excell*)(arg->car) ;
         if(is_INTEGER (Y)) R += linearcombination("integer") ;
    else if(is_GROUP   (Y)){
      string var = as_string(Y) ;
      string type ;
      switch(group_of[var]){
        case  1: type = "group0" ; break;
        case  2: type = "group1" ; break;
        case  3: type = "duplex" ; break;
        default: type = reconstruction_mode ? "unused" : "group" ; break;
      }
      R += linearcombination(type) ;
    }
    else if(is_TARGET  (Y)) R += linearcombination("target") ;
  }
  return R ;
}

linearcombination STATISTICS_CONV (excell * X){
  linearcombination R ;
  if(X){
    if(is_LIST_LITERAL(X) && is_RAW_PROOF(X)){
      return STATISTICS_RAW_CONV(X) ;
    }
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      for(
        excell * arg = (excell*)(X->pointer(2));
        arg;
	arg=(excell*)arg->cdr
      ){
        R += STATISTICS_CONV ((excell*)(arg->car)) ;
      }
    }
  }
  return R ;
}

linearcombination PREDICATE_VARIABLES_STAT(const linearcombination & Vec) ;
linearcombination PREDICATE_VARIABLES_INTEGER(const INTEGER & predicate) ;
linearcombination PREDICATE_VARIABLES_GROUP (const GROUP & predicate, int type = 2) ;
linearcombination PREDICATE_VARIABLES_TARGET (const TARGET  & predicate) ;

linearcombination VARIABLES_RAW_PREDICATE(excell * predicate){
  linearcombination Vec ;
       if(is_INTEGER(predicate)) Vec = PREDICATE_VARIABLES_INTEGER(as_INTEGER(predicate)) ;
  else if(is_GROUP  (predicate)) Vec = PREDICATE_VARIABLES_GROUP  (as_GROUP  (predicate)) ;
  else if(is_TARGET (predicate)) Vec = PREDICATE_VARIABLES_TARGET (as_TARGET (predicate)) ;
  return Vec ;
}

static int is_OPT_PROOF(excell * X) ;

linearcombination VARIABLES_PREDICATE (excell * X){
  linearcombination R ;
  if(X){
    if(is_OPTION(X) && is_OPT_PROOF(X)){
      excell * predicate = X->element(4) ;
      return VARIABLES_RAW_PREDICATE(predicate) ;
    }
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      for(
        excell * arg = (X->pointer(2));
        arg;
	arg=(excell*)arg->cdr
      ) R += VARIABLES_PREDICATE((excell*)(arg->car)) ;
    }
  }
  return R ;
}

linearcombination STATISTICS_PREDICATE (excell * X){
  return PREDICATE_VARIABLES_STAT(VARIABLES_PREDICATE(X)) ;
}

static excell * STATISTICS (excell * X, cell * L){
  UNARY_LIST_PROCESS(X,L,STATISTICS) ;
  record_reduction(string() +
    "/*\n" +
    "  raw  =" + as_string(STATISTICS_RAW(X))   + "\n" +
    "  convr=" + as_string(STATISTICS_CONVR(X)) + "\n" +
    "  proof=" + as_string(STATISTICS_PROOF(X)) + "\n" +
    "  convp=" + as_string(STATISTICS_CONV(X))  + "\n" +
    "  predicate=" + as_string(STATISTICS_PREDICATE(X)) + "\n" +
    "*/\n" +
    "\n"   +
    "/* X = \n" +
      as_string(X) +
    "*/\n" +
    "\n"   +
    "/* CRS(X) = \n" +
      as_string(CRS(X,L)) +
    "*/\n" +
    "\n"   +
    "/* COM(X) = \n" +
      as_string(COM(X,L)) +
    "*/\n" +
    "\n"   +
    "/* PI(X) = \n" +
      as_string(PI(X,L)) +
    "*/\n" +
    ""
  ) ;
  return NULL ;
}

// --------------------------------------------------------------
// CRS
// --------------------------------------------------------------

// -------------------------------------
// PROOF_x_y_z
// -------------------------------------
//        0            1 2   3    4    5
// EXLIST(LIST_LITERAL,L,Opt,PHI1,PHI2,PHI3,...,Vars) ;
// -------------------------------------
//        0       1  2                3    4
// EXLIST(OPTION, L, OPT_PROOF_x_y_z, crs, predicate) ;
// -------------------------------------
//

static excell * CRS (excell * X, cell * L){
  if(is_INTEGER(X)) return NULL ;
  if(is_GROUP(X))   return NULL ;
  if(is_TARGET(X))  return NULL ;

//  std::cout << "/* (CRS) " << std::endl ;
//  std::cout << type_of(X)  << std::endl ;
//  std::cout << X           << std::endl ;
//  std::cout << "*/"        << std::endl ;

  UNARY_LIST_PROCESS(X,L,CRS) ;

  if(X){
    if(is_OPTION(X) && is_OPT_PROOF(X)){
      excell * crs = X->element(3) ;
      return crs ;
    }
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      excell * R = NULL ;
      for(
        excell * arg = (X->pointer(2));
        arg;
	arg=(excell*)arg->cdr
      ) {
        excell * S = CRS((excell*)(arg->car), L) ;
        if(S){
          if(!R) R = EXLIST(LIST_LITERAL, L) ;
          R = (excell *)INTEGRATE(R, S) ;
	}
      }
      return R ;
    }
  }
  return NULL ;
}

// --------------------------------------------------------------
// COM
// --------------------------------------------------------------

static excell * COM (excell * X, cell * L){
  if(is_INTEGER(X)) return NULL ;
  if(is_GROUP(X))   return NULL ;
  if(is_TARGET(X))  return NULL ;

//  std::cout << "/* (COM) "  << std::endl ;
//  std::cout << type_of(X)  << std::endl ;
//  std::cout << X           << std::endl ;
//  std::cout << "*/"        << std::endl ;

  UNARY_LIST_PROCESS(X,L,COM) ;

  if(X){
    if(is_RAW_PROOF(X)){
// EXLIST(LIST_LITERAL,L,Opt,PHI1,PHI2,PHI3,...,Vars) ;
      excell * arg ;
      for(
        arg = (X->pointer(3));
        arg->cdr;
	arg=(excell*)arg->cdr
      );
//      return (excell*)(arg->car) ;
      excell * R   = (excell*)(arg->car) ;
      excell * com = (excell*)(R->element(2)) ;
      return com ;
    }
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      excell * R = NULL ;
      for(
        excell * arg = (X->pointer(2));
        arg;
	arg=(excell*)arg->cdr
      ) {
        excell * S = COM((excell*)(arg->car), L) ;
        if(S){
          if(!R) R = EXLIST(LIST_LITERAL, L) ;
          R = (excell *)INTEGRATE(R, S) ;
	}
      }
      return R ;
    }
  }
  return NULL ;
}

// --------------------------------------------------------------
// PI
// --------------------------------------------------------------

// static int is_RAW_PROOF(excell * X){

static excell * PI (excell * X, cell * L){
  if(is_INTEGER(X)) return NULL ;
  if(is_GROUP(X))   return NULL ;
  if(is_TARGET(X))  return NULL ;

//  std::cout << "/* (PI) "  << std::endl ;
//  std::cout << type_of(X)  << std::endl ;
//  std::cout << X           << std::endl ;
//  std::cout << "*/"        << std::endl ;

  UNARY_LIST_PROCESS(X,L,PI) ;

  if(X){
    if(is_RAW_PROOF(X)){
// EXLIST(LIST_LITERAL,L,Opt,PHI1,PHI2,PHI3,...,Vars) ;
      excell * R = NULL ;
      for(
        excell * arg = (X->pointer(3));
        arg->cdr;
	arg=(excell*)arg->cdr
      ) {
        excell * S = (excell*)(arg->car) ;
        if(S){
          if(!R) R = EXLIST(LIST_LITERAL, L) ;
          R = (excell *)INTEGRATE(R, S) ;
	}
      }
      return R ;
    }
    else if(is_LIST_LITERAL  (X) || is_LOGICAL_AND(X)){
      excell * R = NULL ;
      for(
        excell * arg = (X->pointer(2));
        arg;
	arg=(excell*)arg->cdr
      ) {
        excell * S = PI((excell*)(arg->car), L) ;
        if(S){
          if(!R) R = EXLIST(LIST_LITERAL, L) ;
          R = (excell *)INTEGRATE(R, S) ;
	}
      }
      return R ;
    }
  }
  return NULL ;
}

// --------------------------------------------------------------
// UPLUS
// --------------------------------------------------------------

static excell * UPLUS (excell * X, cell * L){
  UNARY_LIST_PROCESS(X,L,UPLUS) ;
  if(is_INTEGER(X)) return X ;
  return NULL ;
}

// --------------------------------------------------------------
// UMINUS
// --------------------------------------------------------------

static excell * UMINUS (excell * X, cell * L){
  UNARY_LIST_PROCESS(X,L,UMINUS) ;
  if(is_INTEGER(X)) return SUB(INTEGER_ZERO, X, L) ;
  return NULL ;
}

// --------------------------------------------------------------
// ARRAY_REF
// --------------------------------------------------------------

static excell * ARRAY_REF  (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS_0(X,Y,L,ARRAY_REF) ;
  if(is_LIST_LITERAL(X) && is_INTEGER(Y) && is_integer(Y)){
    int y = as_int(Y) ;
    if(y>=0) return X->element(y+2) ;
  }
  return NULL ;
}

// --------------------------------------------------------------
// ADD
// --------------------------------------------------------------

static excell * ADD (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS(X,Y,L,ADD) ;
  if(is_INTEGER(X,Y)) return EXLIST(INTEGER, L, new INTEGER(as_INTEGER(X) + as_INTEGER(Y)));
  return NULL ;
}

// --------------------------------------------------------------
// SUB
// --------------------------------------------------------------

static excell * SUB (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS(X,Y,L,SUB) ;
  if(is_INTEGER(X,Y)) return EXLIST(INTEGER, L, new INTEGER(as_INTEGER(X) - as_INTEGER(Y)));
  return NULL ;
}

// --------------------------------------------------------------
// MUL
// --------------------------------------------------------------

static excell * MUL (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS(X,Y,L,MUL) ;
  if(is_INTEGER(X,Y)) return EXLIST(INTEGER, L, new INTEGER(as_INTEGER(X) * as_INTEGER(Y)));
  if(is_GROUP  (X,Y)) return EXLIST(GROUP  , L, new GROUP  (as_GROUP  (X) * as_GROUP  (Y)));
  if(is_TARGET (X,Y)) return EXLIST(TARGET , L, new TARGET (as_TARGET (X) * as_TARGET (Y)));
  if(is_INTEGER(X) && (is_INTEGER_ONE(X))) return Y ;
  if(is_INTEGER(Y) && (is_INTEGER_ONE(Y))) return X ;
  return NULL ;
}

// --------------------------------------------------------------
// DIV
// --------------------------------------------------------------

static INTEGER DIV_F (INTEGER & X, INTEGER & Y, cell * L){ // division by factor
  INTEGER Z = INTEGER() ;
  auto j = Y.begin();
  auto y = j->first ;
  for(auto i = X.begin(); i != X.end(); i++){
    auto x = i->first  ;
    auto c = i->second ;
    Z += INTEGER(c) * INTEGER(x/y) ;
  }
  return Z ;
}

static excell * DIV (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS(X,Y,L,DIV) ;

  if(is_INTEGER(X,Y)) {
    if(is_INTEGER_ZERO(Y)) return NULL ;
    if(is_INTEGER_ONE (Y)) return X    ;

    if(is_COMMITTED(Y)) return NULL ;

    if( (! is_MONOMIAL(Y)) || (is_integer(Y)) ){
      char * div_s = new_symbol("div") ;
      REDUCE (div_s, INTEGER_ONE, "/", Y) ; // OK
      excell * div = EXLIST(INTEGER , L, new INTEGER(string(div_s))) ;
      return MUL(X, div, L) ;
    }
    if( last_coefficient(Y) != 1 ) {
      char * div_s = new_symbol("div") ;
      excell * coef = EXLIST(INTEGER , L, new INTEGER(last_coefficient(Y))) ;
      excell * base = EXLIST(INTEGER , L, new INTEGER(
        last_base(Y) / Monomial<string, integer_>(div_s)
      )) ;
      REDUCE (div_s, INTEGER_ONE, "/", coef) ; // OK
      Y = base ;
    }
    return EXLIST(INTEGER , L, new INTEGER(DIV_F(as_INTEGER(X), as_INTEGER(Y), L))) ;
  }
  if(is_GROUP (X,Y)) return EXLIST(GROUP  , L, new GROUP  (as_GROUP (X) / as_GROUP (Y)));
  if(is_TARGET(X,Y)) return EXLIST(TARGET , L, new TARGET (as_TARGET(X) / as_TARGET(Y)));

  if(is_INTEGER(X) && (is_INTEGER_ONE(X))) return DIV(as_ONE(Y), Y, L) ;
  if(is_INTEGER(Y) && (is_INTEGER_ONE(Y))) return X ;

  return NULL ;
}

// --------------------------------------------------------------
// POW
// --------------------------------------------------------------

static excell * POW (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS(X,Y,L,POW) ;
  if(is_INTEGER(X,Y)){
    if(is_INTEGER_ZERO(Y)) return EXLIST(INTEGER, L, new INTEGER(integer_(1))) ;
    if(is_integer     (Y)) {
      INTEGER & x = as_INTEGER(X) ;
      integer_   y = last_coefficient(Y) ;
      if(y < 0){
        if(is_MONOMIAL(x) && (last_coefficient(x) == 1))
	  return EXLIST(INTEGER, L, new INTEGER((x.begin()->first)^y)) ;
        else return NULL ;
      }else{
        return EXLIST(INTEGER, L, new INTEGER(as_INTEGER(X) ^ y));
      }
    }
    if(is_INTEGER_ZERO(X)) return EXLIST(INTEGER, L, new INTEGER()) ;
    if(is_INTEGER_ONE (X)) return EXLIST(INTEGER, L, new INTEGER(integer_(1))) ;
    else{
      char * tmp = new_symbol("tmp") ;
      REDUCE(tmp, X, "^", Y) ; // OK
      return EXLIST(INTEGER , L, new INTEGER(string(tmp))) ;
    }
  }
  if(is_GROUP(X) && is_INTEGER(Y))
    return EXLIST(GROUP, L, new GROUP(as_GROUP (X) ^ as_INTEGER(Y))) ;
  if(is_TARGET(X) && is_INTEGER(Y))
    return EXLIST(TARGET, L, new TARGET(as_TARGET(X) ^ as_INTEGER(Y))) ;
  return NULL ;
}

// --------------------------------------------------------------
// PAIRING
// --------------------------------------------------------------

static TARGET PAIRING_RAW(GROUP X, GROUP Y){
  if(X==GROUP()) return TARGET() ;
  if(Y==GROUP()) return TARGET() ;
  return TARGET(TARGETVAR(X,Y)) ;
}

static TARGET PAIRING_F(GROUP X, GROUP Y){
  auto i = X.begin() ;
  auto j = Y.begin() ;
  return TARGET(TARGETVAR(GROUP(i->first), GROUP(j->first)))^(i->second * j->second) ;
}

static TARGET PAIRING(GROUP X, GROUP Y){
  TARGET t = TARGET() ;
  int n = X.size() ;
  int m = Y.size() ;
  for(int i=0;i<n; i++){
    for(int j=0;j<m; j++){
      t *= PAIRING_F(X.Factor(i), Y.Factor(j)) ;
    }
  }
  return t ;
}

static excell * PAIRING (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS(X,Y,L,PAIRING) ;
  if(is_GROUP(X,Y)) return EXLIST(TARGET, L, new TARGET(PAIRING(as_GROUP(X), as_GROUP(Y)))) ;
  if(is_INTEGER(X) && (is_INTEGER_ONE(X))) return EXLIST(TARGET, L, new TARGET()) ;
  if(is_INTEGER(Y) && (is_INTEGER_ONE(Y))) return EXLIST(TARGET, L, new TARGET()) ;
  return NULL ;
}

// --------------------------------------------------------------
// EQ
// --------------------------------------------------------------

static excell * EQ (INTEGER X, INTEGER Y, cell * L){
  excell * R = EXLIST(INTEGER, L, new INTEGER(X-Y)) ;
  REDUCE(new_symbol("eq"), R) ; // OK
  return R ;
}

static excell * EQ (GROUP X, GROUP Y, cell * L){
  excell * R = EXLIST(GROUP, L, new GROUP(X-Y)) ;
  REDUCE(new_symbol("eq"), R) ; // OK
  return R ;
}

static excell * EQ (TARGET X, TARGET Y, cell * L){
  excell * R = EXLIST(TARGET, L, new TARGET(X-Y)) ;
  REDUCE(new_symbol("eq"), R) ; // OK
  return R ;
}

static excell * EQ (excell * X, excell * Y, cell * L){
  BINARY_LIST_PROCESS(X,Y,L,EQ) ;
  if(is_INTEGER(X) && (is_INTEGER_ONE(X)) && (! is_INTEGER(Y))) return EQ(as_ONE(Y), Y, L) ;
  if(is_INTEGER(Y) && (is_INTEGER_ONE(Y)) && (! is_INTEGER(X))) return EQ(X, as_ONE(X), L) ;

  excell * R = NULL ;
  if(is_INTEGER(X,Y)) R = SUB(X,Y,L) ;
  if(is_GROUP  (X,Y)) R = DIV(X,Y,L) ;
  if(is_TARGET (X,Y)) R = DIV(X,Y,L) ;
  if(R)REDUCE(new_symbol("eq"), R) ; // OK
  return R ;
}

// ================================================================================
// ASSIGN
// ================================================================================

static inline INTEGER last_index (GROUP & X){
  auto    It = X.begin() ;
  return  It->second ;
}

static inline INTEGER last_GROUP_index (excell * X){
  return last_index (as_GROUP(X));
}

static inline INTEGER last_index (TARGET & X){
  auto    It = X.begin() ;
  return  It->second ;
}

static inline INTEGER last_TARGET_index (excell * X){
  return last_index (as_TARGET(X));
}

static inline int is_variable (TARGET & X){
  auto     It = X.begin() ;
  auto     var= It->first ;
  return (var.second == GROUP()) && (var.first != GROUP()) ;
}

static inline int is_TARGET_variable (excell * X){
  return is_variable (as_TARGET(X)) ;
}

static int is_COMMITTED(INTEGER & X) ;
static int is_COMMITTED(GROUP   & X) ;

static int is_left_value (INTEGER & X){
  return
    (X.size() == 1) &&
    (!is_integer(X)) && 
    (last_coefficient(X) == integer_(1)) &&
    ! is_COMMITTED(X) ;
}

static int is_INTEGER_left_value (excell * X){
  return (is_INTEGER(X)) && is_left_value (as_INTEGER(X)) ;
}

static int is_left_value (GROUP & X){
  return
    (X.size() == 1) &&
    (last_index(X) == INTEGER(1)) &&
    ! is_COMMITTED(X) ;
}

static int is_GROUP_left_value (excell * X){
  return (is_GROUP(X)) && is_left_value (as_GROUP(X)) ;
}

static int is_left_value (TARGET & X){
  return
    (X.size() == 1) &&
    (last_index(X) == INTEGER(1)) &&
    is_variable(X) ;
}

static int is_TARGET_left_value (excell * X){
  return (is_TARGET(X)) && is_left_value (as_TARGET(X)) ;
}

static int is_left_value (excell * X){
  if(is_LIST_LITERAL(X)){
    excell * arg = (excell*)(X->pointer(2));
    if(!arg) return 0 ;
    int ret = 1 ;
    for(;arg;arg=(excell*)arg->cdr){
      if( ! (ret &= is_left_value ((excell*)arg->car)) ) break ;
    }
    return ret ;
  }else{
    return
      is_LIST_VARIABLE(X) ||
      is_OPTION(X) ||
      is_INTEGER_left_value(X) ||
      is_GROUP_left_value(X) ||
      is_TARGET_left_value(X) ;
  }
}

static excell * DOT (excell * X, excell * Y, cell * L){
//  if(is_LIST_VARIABLE(X))      return DOT(lastvalue(X), Y, L) ;
  if(is_LIST_LITERAL(Y)) return Y ;
  if(is_LIST_VARIABLE(X)){
    char * true_name = as_LIST_VARIABLE(X) ;
    vector<excell *> & h = history[true_name] ;
    int n = h.size() ;
    for(int i=n-1; i>=0; i--){
      excell * R = DOT(h[i], Y, L) ;
      if(R) return R ;
    }
    return NULL ;
  }
  if(is_LIST_VARIABLE_LIST(X)){
    excell * R = DOT(lastlist (X), Y, L) ;
    if(R) return R ;
  }
  if(is_CRS(X) && is_COMMIT(Y)){
    return Y ;
  }
  if(is_LIST_LITERAL(X) && is_left_value(Y)){
    char * y_name = DEREFERENCE(Y) ;
    for(auto arg = X->pointer(2); arg; arg=(excell*)arg->cdr){
      if(DEREFERENCE(arg->element(0)) == y_name)return arg->element(0) ;
    }
  }
  return NULL ;
}

static excell * internal_ARRAY_REF(excell * X, int i, cell * L){
  return EXLIST(LIST_VARIABLE, L, unique_symbol(
    (char *)(string((char *)X->element(2)) + "$" + to_string(i) + "$").c_str()
  )) ;
}

static int COMMIT_type(excell * X) ;

static excell * ASSIGN (excell * X, excell * Y, cell * L, int w){
  if(is_left_value(X)) {
    if(! Y) return X ;
    if(is_LIST_VARIABLE(X,Y)){
      excell * YL = lastvalue(Y) ;
      if( as_LIST_VARIABLE(X) != as_LIST_VARIABLE(Y) ){
        char * true_name = as_LIST_VARIABLE(X) ;
        history[true_name].push_back(Y) ;
      }
      if(YL) return ASSIGN(X,YL,L) ;
    }
    if(is_LIST_VARIABLE(X) && is_LIST_LITERAL(Y)) {
      excell * arg = Y->pointer(2);
      excell * R = EXLIST(LIST_LITERAL, L) ;
      for(int i=0 ;arg; arg=(excell*)arg->cdr, i++){
        R = (excell *)INTEGRATE(R, ASSIGN(internal_ARRAY_REF(X, i, L), arg->element(0), L)) ;
      }
      char * true_name = as_LIST_VARIABLE(X) ;
      history[true_name].push_back(Y) ;
      history[true_name].push_back(R) ;
      return R ;
    }
/*
    if( is_LIST_VARIABLE(X) && ! is_LIST_VARIABLE(Y) ) {
      char * true_name = as_LIST_VARIABLE(X) ;
      history[true_name].push_back(Y) ;
    }
*/
    if(is_LIST_LITERAL(X) && is_LIST_VARIABLE(Y)) {
      // <<===
      excell * eval_Y = lastvalue(Y) ;
      if(eval_Y){
        return ASSIGN(X,eval_Y,L) ;
      }else{
        excell * arg = X->pointer(2);
        excell * R = EXLIST(LIST_LITERAL, L) ;
        for(int i=0 ;arg; arg=(excell*)arg->cdr, i++){
          R = (excell *)INTEGRATE(R, ASSIGN(arg->element(0), internal_ARRAY_REF(Y,i,L),L));
        }
        return R ;
      }
      // <<===
    }
    if(is_LIST_LITERAL(X, Y)){
      excell * arg0 = X->pointer(2);
      excell * arg1 = Y->pointer(2);
      excell * R = EXLIST(LIST_LITERAL, L) ;
      for( ;arg0 && arg1; arg0=(excell*)arg0->cdr, arg1=(excell*)arg1->cdr){
        R = (excell *)INTEGRATE(R, ASSIGN (arg0->element(0), arg1->element(0), L)) ;
      }
      return R ;
    }
/*
    if( is_INTEGER(X) || is_GROUP(X) || is_TARGET(X) ) {
      char * true_name = unique_symbol((char *)as_string(X).c_str()) ;
      history[true_name].push_back(Y) ;
    }
*/
    REDUCE(X, Y, w) ; // OK
    return X ;
  }
  else return NULL ;
}

static excell * ASSIGN (GROUP & X, GROUP Y, cell * L){
  if(is_left_value(X)) {
    stringstream s ;
    s << X ;
/*
    char * true_name = unique_symbol((char *)(s.str().c_str())) ;
    history[true_name].push_back(EXLIST(GROUP,L,new GROUP(Y))) ;
*/
    REDUCE(X, Y) ; // OK
    return EXLIST(GROUP, L, new GROUP(X)) ;
  }
  else return NULL ;
}

static excell * ASSIGN (INTEGER & X, INTEGER Y, cell * L){
  if(is_left_value(X)) {
    stringstream s ;
    s << X ;
/*
    char * true_name = unique_symbol((char *)(s.str().c_str())) ;
    history[true_name].push_back(EXLIST(INTEGER,L,new INTEGER(Y))) ;
*/
    REDUCE(X, Y) ; // OK
    return EXLIST(INTEGER, L, new INTEGER(X)) ;
  }
  else return NULL ;
}

static void DECLARE(INTEGER & X){

  char * true_name = unique_symbol((char *)(as_string(X).c_str())) ;
  history[true_name].push_back(EXLIST(INTEGER, dummy_LOCATION,new INTEGER(X))) ;

  if(is_left_value(X)) {
    const string s = string("integer ") + as_string(X) + " ;" ;
    REDUCE_CORE(s) ;
  }
}

static void DECLARE(GROUP & X){

  char * true_name = unique_symbol((char *)(as_string(X).c_str())) ;
  history[true_name].push_back(EXLIST(GROUP,dummy_LOCATION,new GROUP(X))) ;

  if(is_left_value(X)) {
    const string s = string("group ") + as_string(X) + " ;" ;
    REDUCE_CORE(s) ;
  }
}

static void DECLARE(TARGET & X){

  char * true_name = unique_symbol((char *)(as_string(X).c_str())) ;
  history[true_name].push_back(EXLIST(TARGET,dummy_LOCATION,new TARGET(X))) ;

  if(is_left_value(X)) {
    const string s = string("target ") + as_string(X) + " ;" ;
    REDUCE_CORE(s) ;
  }
}

static void DECLARE(excell * X){
       if(is_INTEGER(X))DECLARE(as_INTEGER(X));
  else if(is_GROUP(X))DECLARE(as_GROUP(X));
  else if(is_TARGET(X))DECLARE(as_TARGET(X));
}

// ================================================================================
// LOGICAL_AND
// ================================================================================

static cell * DUPLICATE(cell * L){
  cell * R = NULL ;
  if(L){
    R = LIST(L->car) ;
    R->cdr = DUPLICATE(L->cdr) ;
  }
  return R ;
}

inline excell * DUPLICATE(excell * L){ return (excell *)DUPLICATE((cell *)L); }

cell * INSERTX(cell * L, cell * M){
  if(L){
    cell * lineno = L->cdr      ;
    cell * arg0   = lineno->cdr ;
    lineno->cdr   = M           ;
    M->cdr        = arg0        ;
  }
  return L ;
}

#define INSERT(L,...) INSERTX(L,LIST(__VA_ARGS__))

static excell * LOGICAL_AND_NN (excell * X, excell * Y, cell * L){
  X = DUPLICATE(X) ;
  Y = DUPLICATE(Y) ;
  return EXLIST(LOGICAL_AND, L, X, Y) ;
}

static excell * LOGICAL_AND_AN (excell * X, excell * Y, cell * L){
  X = DUPLICATE(X) ;
  Y = DUPLICATE(Y) ;
  X->cdr->car = (char*)L ;
  return (excell *)INTEGRATE(X, Y) ;
}

static excell * LOGICAL_AND_NA (excell * X, excell * Y, cell * L){
  X = DUPLICATE(X) ;
  Y = DUPLICATE(Y) ;
  Y->cdr->car = (char*)L ;
  return (excell *)INSERT(Y, X) ;
}

static excell * LOGICAL_AND_AA (excell * X, excell * Y, cell * L){
  X = DUPLICATE(X) ;
  Y = (excell*)DUPLICATE(Y->cdr->cdr) ;
  X->cdr->car = (char*)L ;
  return (excell *)APPEND(X, Y) ;
}

static excell * LOGICAL_AND    (excell * X, excell * Y, cell * L){
  if(!X) return DUPLICATE(Y) ;
  if(!Y) return DUPLICATE(X) ;

  if(  is_LOGICAL_AND(X) &&  is_LOGICAL_AND(Y) ) return LOGICAL_AND_AA (X, Y, L) ;
  if(  is_LOGICAL_AND(X) && !is_LOGICAL_AND(Y) ) return LOGICAL_AND_AN (X, Y, L) ;
  if( !is_LOGICAL_AND(X) &&  is_LOGICAL_AND(Y) ) return LOGICAL_AND_NA (X, Y, L) ;
                                                 return LOGICAL_AND_NN (X, Y, L) ;
}

// ================================================================================
// GS_SETUPB2
// ================================================================================

static int is_OPT_OPT_X(char * OPT, excell * X){
  excell * Opt = X ;
  if(!is_OPTION(Opt)) return 0 ;
  char * Sig = (char *) Opt->element(2) ;
  return (Sig == OPT) ;
}

static int is_OPT_X(char * OPT, excell * X){
  if(!is_LIST_LITERAL(X)) return 0 ;
  excell * Opt = (excell *) X->element(2) ;
  return is_OPT_OPT_X(OPT, Opt) ;
}

#define define_OPT0_(X, Y) \
char * OPT_ ## X = "OPT_" # Y ;

#define define_OPT1_(Y) \
static int is_ ## Y (excell * X){ return is_OPT_X(OPT_ ## Y, X) ; } \
static int is_OPT_ ## Y (excell * X){ return is_OPT_OPT_X(OPT_ ## Y, X) ; }

#define define_OPT_(Y) \
define_OPT0_(Y, Y)     \
define_OPT1_(Y)

#define define_OPT_XY(X, Y) \
define_OPT0_(X, Y)          \
define_OPT1_(X)

define_OPT_(GROUP_COMMIT) ;
define_OPT_(INTEGER_COMMIT) ;
define_OPT_(COMMIT) ;
define_OPT_(CRS) ;

static excell * COMMIT_G(excell * crs, cell * L){
  UNARY_LIST_PROCESS(crs,L,COMMIT_G) ;
  if( is_CRS(crs) ) {
    excell       *   option       = crs->element(2) ;
    excell       *   G            = crs->element(3) ;
    const string     s            = as_string(G) ;
    COMMIT_STACK   & commit_stack = *(COMMIT_STACK *) option->element(3) ;
    excell       * & R            = commit_stack[s] ;

    if(R) return R ;

    char * true_name        = new_symbol("") ;
    option->pointer(6)->car = true_name ;

    excell * C1     = EXLIST(GROUP,L,new GROUP()) ;
    excell * C2     = EXLIST(GROUP,L,new GROUP()) ;
    excell * C3     = EXLIST(GROUP,L,new GROUP(as_GROUP(G))) ;
    excell * S1     = EXLIST(INTEGER,L,new INTEGER()) ;
    excell * S2     = EXLIST(INTEGER,L,new INTEGER()) ;
    excell * S3     = EXLIST(INTEGER,L,new INTEGER()) ;
    excell * Opt    = EXLIST(OPTION, L, OPT_COMMIT, OPT_GROUP_COMMIT, crs, G, S1, S2, S3) ;
             R      = EXLIST(LIST_LITERAL,L,Opt,C1,C2,C3) ;
    return   R ;
  }
  return NULL ;
}

static excell * OPTION_CRS(cell * L){
  return EXLIST(OPTION,               //  for wi               for zk
      L, OPT_CRS, new COMMIT_STACK(), new PROOF_HISTORY(), new PROOF_HISTORY(), NULL);
}

static excell * GS_SETUPB2 (excell * G, cell * L){
  if( is_LIST_VARIABLE(G) ) return GS_SETUPB2 (lastvalue(G), L) ;
  if( is_GROUP(G) ) {
    if( ! is_left_value (G) ){
      GROUP g = GROUP (string(new_symbol("g"))) ;
      G = ASSIGN(g ,as_GROUP(G) ,L) ;
    }
    GROUP   g     = as_GROUP(G) ;
    GROUP   u     = GROUP  (string(new_symbol("u"))) ;
    GROUP   v     = GROUP  (string(new_symbol("v"))) ;
    GROUP   w1    = GROUP  (string(new_symbol("w1"))) ;
    GROUP   w2    = GROUP  (string(new_symbol("w2"))) ;
    GROUP   w3    = GROUP  (string(new_symbol("w3"))) ;
    INTEGER alpha = INTEGER(string(new_symbol("alpha"))) ;
    INTEGER beta  = INTEGER(string(new_symbol("beta"))) ;
    INTEGER r     = INTEGER(string(new_symbol("r"))) ;
    INTEGER s     = INTEGER(string(new_symbol("s"))) ;

    DECLARE(alpha);
    DECLARE(beta);
    DECLARE(r);
    DECLARE(s);

    excell * U    = ASSIGN(u , g^alpha, L ) ;
    excell * V    = ASSIGN(v , g^beta , L ) ;
    excell * W1   = ASSIGN(w1, u^r    , L ) ;
    excell * W2   = ASSIGN(w2, v^s    , L ) ;
    excell * W3   = ASSIGN(w3, g^(r+s), L ) ;

    excell * Opt  = OPTION_CRS(L);
    excell * Crs  = EXLIST(LIST_LITERAL,L,Opt,G,U,V,W1,W2,W3) ;
                    COMMIT_G(Crs,G) ;

    excell * Alpha= EXLIST(INTEGER, L, new INTEGER(alpha)) ;
    excell * Beta = EXLIST(INTEGER, L, new INTEGER(beta )) ;
    excell * Exk  = EXLIST(LIST_LITERAL,L,Alpha, Beta) ;

    return EXLIST(LIST_LITERAL,L,Crs,Exk) ;
  }
  return NULL ;
}

static excell * GS_SETUPH2 (excell * G, cell * L){
  if( is_LIST_VARIABLE(G) ) return GS_SETUPH2 (lastvalue(G), L) ;
  if( is_GROUP(G) ) {
    if( ! is_left_value (G) ){
      GROUP g = GROUP (string(new_symbol("g"))) ;
      G = ASSIGN(g ,as_GROUP(G) ,L) ;
    }
    GROUP   g     = as_GROUP(G) ;
    GROUP   u     = GROUP  (string(new_symbol("u"))) ;
    GROUP   v     = GROUP  (string(new_symbol("v"))) ;
    GROUP   w1    = GROUP  (string(new_symbol("w1"))) ;
    GROUP   w2    = GROUP  (string(new_symbol("w2"))) ;
    GROUP   w3    = GROUP  (string(new_symbol("w3"))) ;
    INTEGER alpha = INTEGER(string(new_symbol("alpha"))) ;
    INTEGER beta  = INTEGER(string(new_symbol("beta"))) ;
    INTEGER r     = INTEGER(string(new_symbol("r"))) ;
    INTEGER s     = INTEGER(string(new_symbol("s"))) ;

    DECLARE(alpha);
    DECLARE(beta);
    DECLARE(r);
    DECLARE(s);

    excell * U    = ASSIGN(u , g^alpha, L ) ;
    excell * V    = ASSIGN(v , g^beta , L ) ;
    excell * W1   = ASSIGN(w1, u^r    , L ) ;
    excell * W2   = ASSIGN(w2, v^s    , L ) ;
    excell * W3   = ASSIGN(w3, g^(r+s-INTEGER(1)), L ) ;

    excell * Opt  = OPTION_CRS(L);
    excell * Crs  = EXLIST(LIST_LITERAL,L,Opt,G,U,V,W1,W2,W3) ;
                    COMMIT_G(Crs,G) ;

    excell * R    = EXLIST(INTEGER, L, new INTEGER(r)) ;
    excell * S    = EXLIST(INTEGER, L, new INTEGER(s)) ;
    excell * Eqk  = EXLIST(LIST_LITERAL,L,R,S) ;

    return EXLIST(LIST_LITERAL,L,Crs,Eqk) ;
  }
  return NULL ;
}

/*
static int is_CRS(excell * X){
  if(!is_LIST_LITERAL(X)) return 0 ;
  excell * Opt = (excell *) X->element(2) ;
  if(!is_OPTION(Opt)) return 0 ;
  char * Sig = (char *) Opt->element(2) ;
  return (Sig == OPT_CRS) ;
}
*/

// ================================================================================
// COMMIT
// ================================================================================



excell * COMMIT_GROUP(excell * crs, excell * X, cell * L){

/*
  (g,u,v,w1,w2,w3) = crs ;
  integer_ s1,s2,s3 ;
  group c1 = (u^s1) * (w1^s3) ;
  group c2 = (v^s2) * (w2^s3) ;
  group c3 = x * (g^(s1+s2)) * (w3^s3) ;
  return (c1,c2,c3) ;
*/

  GROUP & g  = as_GROUP( crs->element(3) ) ;
  GROUP & u  = as_GROUP( crs->element(4) ) ;
  GROUP & v  = as_GROUP( crs->element(5) ) ;
  GROUP & w1 = as_GROUP( crs->element(6) ) ;
  GROUP & w2 = as_GROUP( crs->element(7) ) ;
  GROUP & w3 = as_GROUP( crs->element(8) ) ;
  GROUP & x  = as_GROUP(X) ;

  INTEGER s1 = INTEGER(string(new_symbol("s1"))) ;
  INTEGER s2 = INTEGER(string(new_symbol("s2"))) ;
  INTEGER s3 = INTEGER(string(new_symbol("s3"))) ;

  GROUP   c1 = GROUP(string(new_symbol("c1"))) ;
  GROUP   c2 = GROUP(string(new_symbol("c2"))) ;
  GROUP   c3 = GROUP(string(new_symbol("c3"))) ;

  DECLARE(s1) ;
  DECLARE(s2) ;
  DECLARE(s3) ;

  excell * C1 = ASSIGN(c1,(u^s1) * (w1^s3),L) ;
  excell * C2 = ASSIGN(c2,(v^s2) * (w2^s3),L) ;
  excell * C3 = ASSIGN(c3,x * (g^(s1+s2)) * (w3^s3), L) ;

  excell * S1 = EXLIST(INTEGER,L,new INTEGER(s1)) ;
  excell * S2 = EXLIST(INTEGER,L,new INTEGER(s2)) ;
  excell * S3 = EXLIST(INTEGER,L,new INTEGER(s3)) ;

  excell * Opt= EXLIST(OPTION, L, OPT_COMMIT, OPT_GROUP_COMMIT, crs, X, S1, S2, S3) ;

  return EXLIST(LIST_LITERAL,L,Opt,C1,C2,C3) ;
}

excell * COMMIT_INTEGER(excell * crs, excell * X, cell * L){
/*
  (g,u,v,w1,w2,w3) = crs ;
  integer_ r1,r2 ;
  group d1 = (u^r1) * (w1^x) ;
  group d2 = (v^r2) * (w2^x) ;
  group d3 = (g^(r1+r2)) * ((w3*g)^x) ;
  return (c1,c2,c3) ;
*/

  GROUP   & g  = as_GROUP( crs->element(3) ) ;
  GROUP   & u  = as_GROUP( crs->element(4) ) ;
  GROUP   & v  = as_GROUP( crs->element(5) ) ;
  GROUP   & w1 = as_GROUP( crs->element(6) ) ;
  GROUP   & w2 = as_GROUP( crs->element(7) ) ;
  GROUP   & w3 = as_GROUP( crs->element(8) ) ;
  INTEGER & x  = as_INTEGER(X) ;

  INTEGER   r1 = INTEGER(string(new_symbol("r1"))) ;
  INTEGER   r2 = INTEGER(string(new_symbol("r2"))) ;

  GROUP   d1 = GROUP(string(new_symbol("d1"))) ;
  GROUP   d2 = GROUP(string(new_symbol("d2"))) ;
  GROUP   d3 = GROUP(string(new_symbol("d3"))) ;

  DECLARE(r1) ;
  DECLARE(r2) ;

  excell * D1 = ASSIGN(d1, (u^r1) * (w1^x), L);
  excell * D2 = ASSIGN(d2, (v^r2) * (w2^x), L);
  excell * D3 = ASSIGN(d3, (g^(r1+r2)) * ((w3*g)^x), L);

  excell * R1 = EXLIST(INTEGER,L,new INTEGER(r1)) ;
  excell * R2 = EXLIST(INTEGER,L,new INTEGER(r2)) ;

  excell * Opt= EXLIST(OPTION, L, OPT_COMMIT, OPT_INTEGER_COMMIT, crs, X, R1, R2) ;

  return EXLIST(LIST_LITERAL,L,Opt,D1,D2,D3) ;
}

excell * COMMIT(excell * crs, excell * X, cell * L){
  BINARY_LIST_PROCESS_0(crs,X,L,COMMIT) ;
  if(is_COMMITTED(X)) return 0 ;
  if(is_LIST_LITERAL(X)) if(is_COMMIT(X)) return 0 ;

  if( is_CRS(crs) && ( is_GROUP(X) || is_INTEGER(X) ) ) {

    excell       *   option       = crs->element(2) ;
    string           s            = as_string(X) ;
    COMMIT_STACK   & commit_stack = *(COMMIT_STACK *) option->element(3) ;
    excell       * & R            = commit_stack[s] ;

    if(R) return R ;

    if(! is_left_value (X) ) {
      if(is_GROUP(X)){
        GROUP com = GROUP (string(new_symbol("com"))) ;
        X = ASSIGN(com ,as_GROUP(X) ,L) ;
      }else if(is_INTEGER(X)){
        INTEGER com = INTEGER(string(new_symbol("com"))) ;
        X = ASSIGN(com ,as_INTEGER(X) ,L) ;
      }
    }

    if( is_GROUP(X) ) R = COMMIT_GROUP  (crs, X, L) ;
    else              R = COMMIT_INTEGER(crs, X, L) ;

    commit_stack[s] = R ;

    return R ;
  }
  return NULL ;
}

excell * DUMMY_COMMIT_GROUP(excell * crs, excell * X, cell * L){

  GROUP & g  = as_GROUP( crs->element(3) ) ;
  GROUP & u  = as_GROUP( crs->element(4) ) ;
  GROUP & v  = as_GROUP( crs->element(5) ) ;
  GROUP & w1 = as_GROUP( crs->element(6) ) ;
  GROUP & w2 = as_GROUP( crs->element(7) ) ;
  GROUP & w3 = as_GROUP( crs->element(8) ) ;
  GROUP & x  = as_GROUP(X) ;

  INTEGER s1 = INTEGER(string(new_symbol("s1"))) ;
  INTEGER s2 = INTEGER(string(new_symbol("s2"))) ;
  INTEGER s3 = INTEGER(string(new_symbol("s3"))) ;

  DECLARE(s1) ;
  DECLARE(s2) ;
  DECLARE(s3) ;

  GROUP   c1 = GROUP(string(new_symbol("c1"))) ;
  GROUP   c2 = GROUP(string(new_symbol("c2"))) ;
  GROUP   c3 = GROUP(string(new_symbol("c3"))) ;

  DECLARE(c1) ;
  DECLARE(c2) ;
  DECLARE(c3) ;

  excell * C1 = EXLIST(GROUP, L, new GROUP(c1)) ;
  excell * C2 = EXLIST(GROUP, L, new GROUP(c2)) ;
  excell * C3 = EXLIST(GROUP, L, new GROUP(c3)) ;

  excell * S1 = EXLIST(INTEGER,L,new INTEGER(s1)) ;
  excell * S2 = EXLIST(INTEGER,L,new INTEGER(s2)) ;
  excell * S3 = EXLIST(INTEGER,L,new INTEGER(s3)) ;

  excell * Opt= EXLIST(OPTION, L, OPT_COMMIT, OPT_GROUP_COMMIT, crs, X, S1, S2, S3) ;

  return EXLIST(LIST_LITERAL,L,Opt,C1,C2,C3) ;
}

excell * DUMMY_COMMIT_INTEGER(excell * crs, excell * X, cell * L){

  GROUP   & g  = as_GROUP( crs->element(3) ) ;
  GROUP   & u  = as_GROUP( crs->element(4) ) ;
  GROUP   & v  = as_GROUP( crs->element(5) ) ;
  GROUP   & w1 = as_GROUP( crs->element(6) ) ;
  GROUP   & w2 = as_GROUP( crs->element(7) ) ;
  GROUP   & w3 = as_GROUP( crs->element(8) ) ;
  INTEGER & x  = as_INTEGER(X) ;

  INTEGER   r1 = INTEGER(string(new_symbol("r1"))) ;
  INTEGER   r2 = INTEGER(string(new_symbol("r2"))) ;

  DECLARE(r1) ;
  DECLARE(r2) ;

  GROUP   d1 = GROUP(string(new_symbol("d1"))) ;
  GROUP   d2 = GROUP(string(new_symbol("d2"))) ;
  GROUP   d3 = GROUP(string(new_symbol("d3"))) ;

  DECLARE(d1) ;
  DECLARE(d2) ;
  DECLARE(d3) ;

  excell * D1 = EXLIST(GROUP, L, new GROUP(d1));
  excell * D2 = EXLIST(GROUP, L, new GROUP(d2));
  excell * D3 = EXLIST(GROUP, L, new GROUP(d3));

  excell * R1 = EXLIST(INTEGER,L,new INTEGER(r1)) ;
  excell * R2 = EXLIST(INTEGER,L,new INTEGER(r2)) ;

  excell * Opt= EXLIST(OPTION, L, OPT_COMMIT, OPT_INTEGER_COMMIT, crs, X, R1, R2) ;

  return EXLIST(LIST_LITERAL,L,Opt,D1,D2,D3) ;
}

excell * DUMMY_COMMIT(excell * crs, excell * X, cell * L){
  if(is_LIST_VARIABLE     (crs)) return DUMMY_COMMIT(lastvalue(crs),X,L) ;
  if(is_LIST_VARIABLE_LIST(crs)) return DUMMY_COMMIT(lastlist(crs),X,L) ;
  if(is_LIST_VARIABLE     (X  )) return DUMMY_COMMIT(crs,lastvalue(X),L) ;

  if( is_CRS(crs) && ( is_GROUP(X) || is_INTEGER(X) ) ) {

    excell       *   option       = crs->element(2) ;
    string           s            = as_string(X) ;
    COMMIT_STACK   & commit_stack = *(COMMIT_STACK *) option->element(3) ;
    excell       * & R            = commit_stack[s] ;

    if(R) return R ;

    if(! is_left_value (X) ) {
      if(is_GROUP(X)){
        GROUP com = GROUP (string(new_symbol("com"))) ;
        X = ASSIGN(com ,as_GROUP(X) ,L) ;
      }else if(is_INTEGER(X)){
        INTEGER com = INTEGER(string(new_symbol("com"))) ;
        X = ASSIGN(com ,as_INTEGER(X) ,L) ;
      }
    }

    if( is_GROUP(X) ) R = DUMMY_COMMIT_GROUP  (crs, X, L) ;
    else              R = DUMMY_COMMIT_INTEGER(crs, X, L) ;

    commit_stack[s] = R ;

    return R ;
  }
  return NULL ;
}

excell * find_COMMIT(excell * crs, const GROUP & X){
  if( is_CRS(crs) ){
    excell       *   option       = crs->element(2) ;
    COMMIT_STACK   & commit_stack = *(COMMIT_STACK *) option->element(3) ;
    string           s            = as_string((GROUP&)X) ;
    return commit_stack[s] ;
  }
  return NULL ;
}

excell * find_COMMIT(excell * crs, const INTEGER & X){
  if( is_CRS(crs) ){
    excell       *   option       = crs->element(2) ;
    COMMIT_STACK   & commit_stack = *(COMMIT_STACK *) option->element(3) ;
    string           s            = as_string((INTEGER&)X) ;
    return commit_stack[s] ;
  }
  return NULL ;
}

GROUP & COMMIT_GROUP_X(excell * crs, GROUP & X, cell * L){
  excell * X_    = EXLIST(GROUP, L, new GROUP(X)) ;
  excell * com_X = crs_stack.back().dummycom ? DUMMY_COMMIT(crs, X_, L) : COMMIT(crs, X_, L) ;
  return as_GROUP(as_COMMITTED_VARIABLE(com_X)) ;
}

GROUP & COMMIT_GROUP_G(excell * crs, cell * L){
  excell * G     = crs->element(3) ;
  excell * com_G = crs_stack.back().dummycom ? DUMMY_COMMIT(crs, G, L) : COMMIT(crs, G, L) ;
  return as_GROUP(as_COMMITTED_VARIABLE(com_G)) ;
}

INTEGER & COMMIT_INTEGER_1(excell * crs, cell * L){
  INTEGER  one     = INTEGER(1) ;
  excell * com_one = find_COMMIT(crs, one) ;
  if(!com_one){
    excell * ONE   = EXLIST(INTEGER, L, new INTEGER(1)) ;
    com_one = crs_stack.back().dummycom ? DUMMY_COMMIT(crs, ONE, L) : COMMIT(crs, ONE, L) ;
  }
  return as_INTEGER(as_COMMITTED_VARIABLE(com_one)) ;
}

// ========================================================
// as_COMMITTED_VARIABLE
// ========================================================

static excell * as_COMMITTED_VARIABLE(excell * X){

  excell * Opt      = X->element(2) ;
  excell * Y        = DUPLICATE(Opt->element(5)) ;
  excell * crs      = Opt->element(4) ;
  excell * crs_opt  = crs->element(2) ;
  char   * crs_name = (char*) crs_opt->element(6) ;
  string   s   = string("~[") + as_string(Y) + "]." + crs_name ;
  char *   true_name = unique_symbol((char *)s.c_str()) ;
       if(is_INTEGER(Y))Y->cdr->cdr->car = (char *) new INTEGER(s) ;
  else if(is_GROUP  (Y))Y->cdr->cdr->car = (char *) new GROUP(s) ;
  if(history[true_name].size() == 0) history[true_name].push_back(X) ;
  return Y ;
}

// ========================================================
// is_COMMITTED
// ========================================================

static int is_COMMITTED(const string & v){
//  return v.size() && (v[0] == ' ') ;
  return v.size() && (v[0] == '~') ;
}

static int is_COMMITTED(const Monomial<string, integer_> & m){
  if(m == Monomial<string, integer_>()) return 0 ;
  return is_COMMITTED(m.rbegin()->first) ;
}

static int is_COMMITTED(INTEGER & X){
  if(is_integer(X)) return 0 ;
  auto i = X.rbegin() ;
  return is_COMMITTED(i->first) ;
}

static int is_COMMITTED(GROUP & X){
  if(X == GROUP()) return 0 ;
  return is_COMMITTED(X.rbegin()->first) ;
}

static int is_COMMITTED(TARGET & X){
  if(X == TARGET()) return 0 ;
  if(is_variable (X)) return 0 ;
  return is_COMMITTED((GROUP &) X.rbegin()->first.second) ;
}

static int is_COMMITTED(excell * X){
       if(is_INTEGER(X)) return is_COMMITTED(as_INTEGER(X)) ;
  else if(is_GROUP  (X)) return is_COMMITTED(as_GROUP  (X)) ;
  else if(is_TARGET (X)) return is_COMMITTED(as_TARGET (X)) ;
  return 0 ;
}

// ========================================================
// COMMIT_degree
// ========================================================

static integer_ COMMIT_degree(Monomial<string, integer_> m){
  integer_ j = 0 ;
  for(auto i = m.rbegin(); i != m.rend() ; i++){
    if(! is_COMMITTED(i->first) ) break ;
    if(i->second < 0) return -1 ;
    j += i->second ;
  }
  return j ;
}

static integer_ COMMIT_degree(INTEGER & X){
  integer_ j = 0 ;
  for(auto i = X.rbegin(); i != X.rend() ; i++){
    integer_ k = COMMIT_degree(i->first) ;
    if(k == integer_(0)) break     ;
    if(k <  integer_(0)) return -1 ;
    if(j<k)j = k ;
  }
  return j ;
}

static int COMMIT_degree(GROUP & g){
  int j = 0 ;
  for(auto i = g.rbegin(); i != g.rend() ; i++){
    auto di = COMMIT_degree(i->second) ;
    if ((di< 0) || (di > 1)) return -1 ;
    auto dg = is_COMMITTED(i->first) ;
    if ((di==1) &&   dg ) j |= 4 ;
    if ((di==1) && ! dg ) j |= 2 ;
    if ((di==0) &&   dg ) j |= 1 ;
  }
  j = (j<4) ? j : 4 ;
  return j ;
}

static int COMMIT_degree(TARGET & g){
  int j = 0 ;
  for(auto i = g.rbegin() ; i != g.rend() ; i++ ) {
    auto sp    = i->first ;
    auto index = i->second ;
    if(COMMIT_degree(index) != 0) return -1 ;
    int g0 = COMMIT_degree(sp.first) ;
    int g1 = COMMIT_degree(sp.second) ;
    int k ;
         if( (g0 == 1) && (g1 == 1 ) ) k = 2 ;
    else if( (g0 == 1) && (g1 == 0 ) ) k = 1 ;
    else if( (g0 == 0) && (g1 == 1 ) ) k = 1 ;
    else if( (g0 == 0) && (g1 == 0 ) ) k = 0 ;
    else return -1 ;
    if(j<k)j = k ;
  }
  return j ;
}

// ========================================================
// COMMIT_type
// ========================================================

static int COMMIT_type(excell * X){
//  COUT(X) ;
//  COUT(is_COMMITTED(X)) ;
        if(is_TARGET(X)){
    int n = COMMIT_degree(as_TARGET(X)) ;
         if(n == 0) return 0 ;
    else if(n == 1) return 1 ; // 4.1.1
    else if(n == 2) return 2 ; // 4.1.2
    else            return 0 ;
  }else if(is_GROUP(X)){
    int n = COMMIT_degree(as_GROUP(X)) ;
         if(n == 0) return 0 ;
    else if(n == 1) return 3 ; // 4.2.1
    else if(n == 2) return 4 ; // 4.2.2
    else if(n == 3) return 5 ; // 4.2.3
    else if(n == 4) return 6 ; // 4.2.4
    else            return 0 ;
  }else if(is_INTEGER(X)){
    integer_ n = COMMIT_degree(as_INTEGER(X)) ;
         if(n == 0) return 0 ;
    else if(n == 1) return 7 ; // 4.3.1
    else if(n == 2) return 8 ; // 4.3.2
    else            return 0 ;
  }else             return 0 ;
}

/*
typedef Polynomial       <string,    integer_, integer_> INTEGER   ;
typedef Monomial         <string,    INTEGER         > GROUP     ;
typedef SymmetricPairing <GROUP                      > TARGETVAR ;
typedef Monomial         <TARGETVAR, INTEGER         > TARGET    ;
*/

// ================================================================================
// GS_PROOFWI
// ================================================================================

static int EQUAL(excell * X, excell * Y){
       if(is_INTEGER(X,Y)) return (as_INTEGER(X) == as_INTEGER(Y)) ;
  else if(is_GROUP  (X,Y)) return (as_GROUP  (X) == as_GROUP  (Y)) ;
  else if(is_TARGET (X,Y)) return (as_TARGET (X) == as_TARGET (Y)) ;
  else return 0 ;
}

/* CRS

  crs->element(0)    : TYPE_LIST_LITERAL
  crs->element(1)    : location

  crs->element(2)    : option
  crs->element(3)    : g
  crs->element(4)    : u
  crs->element(5)    : v
  crs->element(6)    : w1
  crs->element(7)    : w2
  crs->element(8)    : w3

  option->element(0) : TYPE_OPTION
  option->element(1) : location
  option->element(2) : OPT_CRS
  option->element(3) : commit_stack
  option->element(4) : proof_history
  option->element(5) : proof_history_zk
*/

static PROOF_HISTORY & Proof_History (excell * crs){
  excell * option = (excell *)crs->element(2) ;
  PROOF_HISTORY & proof_history = *(PROOF_HISTORY*)(option->element(4)) ;
  return proof_history ;
}

static PROOF_HISTORY & Proof_History_ZK (excell * crs){
  excell * option = (excell *)crs->element(2) ;
  PROOF_HISTORY & proof_history = *(PROOF_HISTORY*)(option->element(5)) ;
  return proof_history ;
}

static excell * ALREADY_PROVEN(excell * crs, excell * predicate){
  PROOF_HISTORY & proof_history = Proof_History(crs) ;
  string s = as_string(predicate) ;
  return proof_history[s] ;
}

static excell * ALREADY_PROVEN_ZK(excell * crs, excell * predicate){
  PROOF_HISTORY & proof_history = Proof_History_ZK(crs) ;
  string s = as_string(predicate) ;
  return proof_history[s] ;
}

static void PUSH_PROOF(excell * crs, excell * predicate, excell * proof){
  PROOF_HISTORY & proof_history    = Proof_History(crs) ;
  string          s                = as_string(predicate)  ;
                  proof_history[s] = proof              ;
}

static void PUSH_PROOF_ZK(excell * crs, excell * predicate, excell * proof){
  PROOF_HISTORY & proof_history    = Proof_History_ZK(crs) ;
  string          s                = as_string(predicate)  ;
                  proof_history[s] = proof              ;
}

define_OPT_XY(PROOF_4_1_1,PROOF_Linear_PPE) ;
define_OPT_XY(PROOF_4_1_2,PROOF_Non_Linear_PPE) ;
define_OPT_XY(PROOF_4_2_1,PROOF_Group_MSE) ;
define_OPT_XY(PROOF_4_2_2,PROOF_Scalar_MSE) ;
define_OPT_XY(PROOF_4_2_3,PROOF_Mixed_MSE) ;
define_OPT_XY(PROOF_4_2_4,PROOF_General_MSE) ;
define_OPT_XY(PROOF_4_3_1,PROOF_Linear_QE) ;
define_OPT_XY(PROOF_4_3_2,PROOF_Non_Linear_QE) ;

#define OPT_PROOF_(X, CRS, PREDICATE) EXLIST(OPTION, L, OPT_PROOF_ ## X, CRS, PREDICATE) ;

static int is_OPT_PROOF(excell * X){
  excell * Opt = X ;
  if(Opt->car != TYPE_OPTION)     return 0 ;
  char* Otype  =  (char *) Opt->element(2) ;
  if(Otype    == OPT_PROOF_4_1_1) return 1 ;
  if(Otype    == OPT_PROOF_4_1_2) return 1 ;
  if(Otype    == OPT_PROOF_4_2_1) return 1 ;
  if(Otype    == OPT_PROOF_4_2_2) return 1 ;
  if(Otype    == OPT_PROOF_4_2_3) return 1 ;
  if(Otype    == OPT_PROOF_4_2_4) return 1 ;
  if(Otype    == OPT_PROOF_4_3_1) return 1 ;
  if(Otype    == OPT_PROOF_4_3_2) return 1 ;
                                  return 0 ;
}

static int is_RAW_PROOF(excell * X){
  if(X->car != TYPE_LIST_LITERAL) return 0 ;
  if(!X->cdr->cdr)                return 0 ;
  excell * Opt = X->element(2) ;
  return is_OPT_PROOF(Opt) ;
}

// typedef LinearCombination<string, INTEGER> linearcombination ;

static unordered_map<string, int> type_map ;

linearcombination PREDICATE_VARIABLES_Monomial (const Monomial<string, integer_> & predicate){
  linearcombination Vec ;
  for(auto i = predicate.rbegin(); i != predicate.rend() ; i++){
    const string & x = i->first ;
    type_map[x] = 1 ;
    Vec += linearcombination(x) ;
  }
  return Vec ;
}

linearcombination PREDICATE_VARIABLES_INTEGER (const INTEGER & predicate){
  linearcombination Vec ;
  for(auto i = predicate.rbegin(); i != predicate.rend() ; i++){
    const Monomial<string, integer_> & m = i->first ;
    Vec += PREDICATE_VARIABLES_Monomial(m) ;
  }
  return Vec ;
}

linearcombination PREDICATE_VARIABLES_GROUP (const GROUP & predicate, int type){
  linearcombination Vec ;
  for(auto i = predicate.rbegin(); i != predicate.rend() ; i++){
    const string  & base  = i->first ;
    const INTEGER & index = i->second ;
    type_map[base] = type ;
    Vec += linearcombination(base) ;
    Vec += PREDICATE_VARIABLES_INTEGER(index) ;
  }
  return Vec ;
}

linearcombination PREDICATE_VARIABLES_TARGET (const TARGET & predicate){
  linearcombination Vec ;
  for(auto i = predicate.rbegin(); i != predicate.rend() ; i++) {
    const TARGETVAR & base  = i->first ;
    const INTEGER   & index = i->second ;
          if(base.first == GROUP() && base.second == GROUP()){
    }else if(base.first != GROUP() && base.second == GROUP()){
      Vec += PREDICATE_VARIABLES_GROUP(base.first, 3) ;
    }else if(base.first == GROUP() && base.second != GROUP()){
      // impossible
    }else if(base.first != GROUP() && base.second != GROUP()){
      Vec += PREDICATE_VARIABLES_GROUP(base.first) ;
      Vec += PREDICATE_VARIABLES_GROUP(base.second) ;
    }
    Vec += PREDICATE_VARIABLES_INTEGER(index) ;
  }
  return Vec ;
}

excell * PREDICATE_VARIABLES_COM(excell * predicate, cell * L){
  linearcombination Vec ;
       if(is_INTEGER(predicate)) Vec = PREDICATE_VARIABLES_INTEGER(as_INTEGER(predicate)) ;
  else if(is_GROUP  (predicate)) Vec = PREDICATE_VARIABLES_GROUP  (as_GROUP  (predicate)) ;
  else if(is_TARGET (predicate)) Vec = PREDICATE_VARIABLES_TARGET (as_TARGET (predicate)) ;
  excell * R = NULL ;
  for(auto i = Vec.rbegin() ; i != Vec.rend() ; i++){
    auto x = i->first ;
    int n = type_map[x] ;
    if(is_COMMITTED(x)){
      if(!R) R = EXLIST(LIST_LITERAL, L) ;
      char * true_name = unique_symbol((char *)x.c_str()) ;
      excell * & Com   = history[true_name].back() ;
      excell   * Opt   = Com->element(2) ;
      excell   * C1    = Com->element(3) ;
      excell   * C2    = Com->element(4) ;
      excell   * C3    = Com->element(5) ;
      R = (excell*)INTEGRATE(R,C1) ;
      R = (excell*)INTEGRATE(R,C2) ;
      R = (excell*)INTEGRATE(R,C3) ;
      switch(n){
        case  1: 
	{
          excell * R1  = Opt->element(6) ;
          excell * R2  = Opt->element(7) ;
          R = (excell*)INTEGRATE(R,R1) ;
          R = (excell*)INTEGRATE(R,R2) ;
	}
	break ;
        case  2:
	{
          excell * S1  = Opt->element(6) ;
          excell * S2  = Opt->element(7) ;
          excell * S3  = Opt->element(8) ;
          R = (excell*)INTEGRATE(R,S1) ;
          R = (excell*)INTEGRATE(R,S2) ;
          R = (excell*)INTEGRATE(R,S3) ;
	}
	break ;
      }
    }else{
    }
  }
  return R ;
}

excell * PREDICATE_VARIABLES_CONST(excell * predicate, cell * L){
  linearcombination Vec ;
       if(is_INTEGER(predicate)) Vec = PREDICATE_VARIABLES_INTEGER(as_INTEGER(predicate)) ;
  else if(is_GROUP  (predicate)) Vec = PREDICATE_VARIABLES_GROUP  (as_GROUP  (predicate)) ;
  else if(is_TARGET (predicate)) Vec = PREDICATE_VARIABLES_TARGET (as_TARGET (predicate)) ;
  excell * R = NULL ;
  for(auto i = Vec.rbegin() ; i != Vec.rend() ; i++){
    auto x = i->first ;
    int n = type_map[x] ;
    if(is_COMMITTED(x)){
    }else{
      if(!R) R = EXLIST(LIST_LITERAL, L) ;
      switch(n){
        case  1: R = (excell*)INTEGRATE(R,EXLIST(INTEGER,L,new INTEGER(x))) ; break ;
        case  2: R = (excell*)INTEGRATE(R,EXLIST(GROUP  ,L,new GROUP  (x))) ; break ;
        case  3: R = (excell*)INTEGRATE(R,EXLIST(TARGET ,L,new TARGET (x))) ; break ;
      }
    }
  }
  return R ;
}

excell * PREDICATE_VARIABLES(excell * predicate, cell * L){
  return EXLIST(LIST_LITERAL, L,
    PREDICATE_VARIABLES_COM(predicate, L),
    PREDICATE_VARIABLES_CONST(predicate, L)
  ) ;
}

/*
excell * PREDICATE_VARIABLES(excell * predicate, cell * L){
  linearcombination Vec ;
       if(is_INTEGER(predicate)) Vec = PREDICATE_VARIABLES_INTEGER(as_INTEGER(predicate)) ;
  else if(is_GROUP  (predicate)) Vec = PREDICATE_VARIABLES_GROUP  (as_GROUP  (predicate)) ;
  else if(is_TARGET (predicate)) Vec = PREDICATE_VARIABLES_TARGET (as_TARGET (predicate)) ;
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(auto i = Vec.rbegin() ; i != Vec.rend() ; i++){
    auto x = i->first ;
    int n = type_map[x] ;
    if(is_COMMITTED(x)){
      char * true_name = unique_symbol((char *)x.c_str()) ;
      excell * & Com   = history[true_name].back() ;
      excell   * Opt   = Com->element(2) ;
      excell   * C1    = Com->element(3) ;
      excell   * C2    = Com->element(4) ;
      excell   * C3    = Com->element(5) ;
      R = (excell*)INTEGRATE(R,C1) ;
      R = (excell*)INTEGRATE(R,C2) ;
      R = (excell*)INTEGRATE(R,C3) ;
      switch(n){
        case  1: 
	{
          excell * R1  = Opt->element(6) ;
          excell * R2  = Opt->element(7) ;
          R = (excell*)INTEGRATE(R,R1) ;
          R = (excell*)INTEGRATE(R,R2) ;
	}
	break ;
        case  2:
	{
          excell * S1  = Opt->element(6) ;
          excell * S2  = Opt->element(7) ;
          excell * S3  = Opt->element(8) ;
          R = (excell*)INTEGRATE(R,S1) ;
          R = (excell*)INTEGRATE(R,S2) ;
          R = (excell*)INTEGRATE(R,S3) ;
	}
	break ;
      }
    }else{
      switch(n){
        case  1: R = (excell*)INTEGRATE(R,EXLIST(INTEGER,L,new INTEGER(x))) ; break ;
        case  2: R = (excell*)INTEGRATE(R,EXLIST(GROUP  ,L,new GROUP  (x))) ; break ;
        case  3: R = (excell*)INTEGRATE(R,EXLIST(TARGET ,L,new TARGET (x))) ; break ;
      }
    }
  }
  return R ;
}
*/






linearcombination PREDICATE_VARIABLES_STAT(const linearcombination & Vec){
  linearcombination R ;
  for(auto i = Vec.rbegin() ; i != Vec.rend() ; i++){
    auto x = i->first ;
    int n = type_map[x] ;
    if(is_COMMITTED(x)){
      switch(n){
        case  1: R += linearcombination("integer_commit") ; break ;
        case  2: R += linearcombination("group_commit") ; break ;
      }
    }else{
      switch(n){
        case  1: R += linearcombination("integer") ; break ;
        case  2: R += linearcombination("group") ; break ;
        case  3: R += linearcombination("target") ; break ;
      }
    }
  }
  return R ;
}

// =======================================================================================
// GS_PROOFWI_PPE
// =======================================================================================

typedef std::tuple<int, GROUP, GROUP, INTEGER, TARGET> split_PPE ;

static vector<split_PPE> split_PPE_predicate(TARGET & predicate){
  typedef MultiplicativeLinearCombination<GROUP, GROUP> DEGREE1 ;
  TARGET  d2 ;
  DEGREE1 d1 ;
  TARGET  d0 ;

  for(auto i = predicate.rbegin(); i != predicate.rend() ; i++){ // multiplication
    TARGETVAR & p = (TARGETVAR &)i->first  ;
    INTEGER   & c = (INTEGER &)i->second ;
    GROUP     & X = (GROUP &)p.first   ;
    GROUP     & Y = (GROUP &)p.second  ;
         if(( is_COMMITTED(X))&&( is_COMMITTED(Y))) d2 *= TARGET(p)^c    ; // e([X],[Y])^c
    else if((!is_COMMITTED(X))&&( is_COMMITTED(Y))) d1 *= DEGREE1(Y,X^c) ; // e(X^c,[Y])
    else if(( is_COMMITTED(X))&&(!is_COMMITTED(Y))) d1 *= DEGREE1(X,Y^c) ; // e([X],Y^c)
    else                                            d0 *= TARGET(p)^c ;
  }

  vector<split_PPE> V ;

  for(auto i=d2.rbegin(); i!=d2.rend(); i++){ // e([X],[Y])^c
    const TARGETVAR & B = i->first  ;
    const INTEGER   & c = i->second ;
    const GROUP     & x = B.first   ;
    const GROUP     & y = B.second  ;
    V.push_back(split_PPE(2, x, y, c, TARGET())) ;
  }

  for(auto i=d1.rbegin(); i!=d1.rend(); i++){ // e(b,[X])
    const GROUP & x = i->first  ;
    const GROUP & b = i->second ;
    V.push_back(split_PPE(1,x,b,INTEGER(1),TARGET())) ;
  }

  if(d0!=TARGET()) V.push_back(split_PPE(0,GROUP(),GROUP(),INTEGER(),d0)) ;

  return V ;
}

static excell * GS_PROOFWI_PPE_4_1_1 (excell * crs, excell * predicate, cell * L){
  GROUP phi1, phi2, phi3 ;
  vector<split_PPE> Vec = split_PPE_predicate(as_TARGET(predicate)) ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    GROUP   & Y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    TARGET  & Z           = get<4>(*i) ;
    switch(commit_type){
      case  2: break ; // e([X],[Y])^c
      case  1:         // e([X], Y )
      {
        string      X_s       = X.rbegin()->first ;
        char      * true_name = unique_symbol((char *)X_s.c_str()) ;
        excell  * & Com       = history[true_name].back() ;
        excell    * Opt       = Com->element(2) ;
        INTEGER   & s1        = as_INTEGER(Opt->element(6)) ;
        INTEGER   & s2        = as_INTEGER(Opt->element(7)) ;
        INTEGER   & s3        = as_INTEGER(Opt->element(8)) ;

        phi1 *= Y^s1 ;
        phi2 *= Y^s2 ;
        phi3 *= Y^s3 ;
      }
      break ;
      case  0: break ; // Z
      default: break ; // impossible
    }
  }
  char * Phi1 = new_symbol("phi1") ; REDUCE(Phi1, phi1) ;
  char * Phi2 = new_symbol("phi2") ; REDUCE(Phi2, phi2) ;
  char * Phi3 = new_symbol("phi3") ; REDUCE(Phi3, phi3) ;
  excell * PHI1 = EXLIST(GROUP,L,new GROUP(Phi1)) ;
  excell * PHI2 = EXLIST(GROUP,L,new GROUP(Phi2)) ;
  excell * PHI3 = EXLIST(GROUP,L,new GROUP(Phi3)) ;
  excell * Opt  = OPT_PROOF_(4_1_1, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,PHI1,PHI2,PHI3,Var) ;
}

static excell * GS_DUMMY_PROOFWI_PPE_4_1_1 (excell * crs, excell * predicate, cell * L){
  char * Phi1 = new_symbol("phi1") ;
  char * Phi2 = new_symbol("phi2") ;
  char * Phi3 = new_symbol("phi3") ;
  excell * PHI1 = EXLIST(GROUP,L,new GROUP(Phi1)) ; DECLARE(PHI1) ;
  excell * PHI2 = EXLIST(GROUP,L,new GROUP(Phi2)) ; DECLARE(PHI2) ;
  excell * PHI3 = EXLIST(GROUP,L,new GROUP(Phi3)) ; DECLARE(PHI3) ;
  excell * Opt  = OPT_PROOF_(4_1_1, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,PHI1,PHI2,PHI3,Var) ;
}

static excell * GS_VERIFY_PPE_4_1_1 (excell * crs, excell * predicate, excell * proof, cell * L){
  excell * Opt       = proof->element(2) ;
  TARGET left1, left2, left3, T ;

  vector<split_PPE> Vec = split_PPE_predicate(as_TARGET(predicate)) ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    GROUP   & Y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    TARGET  & Z           = get<4>(*i) ;
    switch(commit_type){
      case  2: break ; // e([X],[Y])^c
      case  1:         // e([X], Y )
      {
        string     X_s       = X.rbegin()->first ;
        char     * true_name = unique_symbol((char *)X_s.c_str()) ;
        excell * & Com       = history[true_name].back() ;
        GROUP    & c1        = as_GROUP(Com->element(3)) ;
        GROUP    & c2        = as_GROUP(Com->element(4)) ;
        GROUP    & c3        = as_GROUP(Com->element(5)) ;

        left1 *= PAIRING_RAW(c1,Y) ;
        left2 *= PAIRING_RAW(c2,Y) ;
        left3 *= PAIRING_RAW(c3,Y) ;
      }
      break ;
      case  0:         // Z
      {
        T /= Z ;
      }
      break ;
      default: break ; // impossible
    }
  }

  GROUP & phi1 = as_GROUP(proof->element(3)) ;
  GROUP & phi2 = as_GROUP(proof->element(4)) ;
  GROUP & phi3 = as_GROUP(proof->element(5)) ;
  GROUP   phi12= phi1 * phi2 ;

  GROUP & G  = as_GROUP(crs->element(3)) ;
  GROUP & U  = as_GROUP(crs->element(4)) ;
  GROUP & V  = as_GROUP(crs->element(5)) ;
  GROUP & W1 = as_GROUP(crs->element(6)) ;
  GROUP & W2 = as_GROUP(crs->element(7)) ;
  GROUP & W3 = as_GROUP(crs->element(8)) ;

  TARGET right1 = PAIRING_RAW(phi1 ,U) * PAIRING_RAW(phi3,W1)     ;
  TARGET right2 = PAIRING_RAW(phi2 ,V) * PAIRING_RAW(phi3,W2)     ;
  TARGET right3 = PAIRING_RAW(phi12,G) * PAIRING_RAW(phi3,W3) * T ;

  return EXLIST(LIST_LITERAL,L,EQ(left1,right1,L),EQ(left2,right2,L),EQ(left3,right3,L));
}

static excell * GS_VERIFY2_PPE_4_1_1 (excell * proof, cell * L){
  excell * Opt       = proof->element(2) ;
  excell * crs       = Opt->element(3) ;
  excell * predicate = Opt->element(4) ;
  return GS_VERIFY_PPE_4_1_1 (crs, predicate, proof, L) ;
}

static excell * GS_PROOFWI_PPE_4_1_2 (excell * crs, excell * predicate, cell * L){
  INTEGER ssc11, ssc12, ssc13, ssc21, ssc22, ssc23, ssc31, ssc32, ssc33 ;
  GROUP   Xsc1, Xsc2, Xsc3, As1, As2, As3 ;

  vector<split_PPE> Vec = split_PPE_predicate(as_TARGET(predicate)) ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    GROUP   & Y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    TARGET  & Z           = get<4>(*i) ;
    switch(commit_type){
      case  2:         // e([X],[Y])^c
      {
        string      X_s        = X.rbegin()->first ;
        string      Y_s        = Y.rbegin()->first ;
        char      * true_name1 = unique_symbol((char *)X_s.c_str()) ;
        char      * true_name2 = unique_symbol((char *)Y_s.c_str()) ;
        excell  * & Com1       = history[true_name1].back() ;
        excell  * & Com2       = history[true_name2].back() ;
        excell    * Opt1       = Com1->element(2) ;
        excell    * Opt2       = Com2->element(2) ;
        GROUP     & X_t        = as_GROUP(Opt1->element(5)) ;
        INTEGER   & s11        = as_INTEGER(Opt1->element(6)) ;
        INTEGER   & s12        = as_INTEGER(Opt1->element(7)) ;
        INTEGER   & s13        = as_INTEGER(Opt1->element(8)) ;
        GROUP     & Y_t        = as_GROUP(Opt2->element(5)) ;
        INTEGER   & s21        = as_INTEGER(Opt2->element(6)) ;
        INTEGER   & s22        = as_INTEGER(Opt2->element(7)) ;
        INTEGER   & s23        = as_INTEGER(Opt2->element(8)) ;
        ssc11 += s11 * c * s21 ;
        ssc12 += s11 * c * s22 ;
        ssc13 += s11 * c * s23 ;
        ssc21 += s12 * c * s21 ;
        ssc22 += s12 * c * s22 ;
        ssc23 += s12 * c * s23 ;
        ssc31 += s13 * c * s21 ;
        ssc32 += s13 * c * s22 ;
        ssc33 += s13 * c * s23 ;
        Xsc1  *= (X_t^( c * s21 )) + (Y_t^( c * s11 )) ;
        Xsc2  *= (X_t^( c * s22 )) + (Y_t^( c * s12 )) ;
        Xsc3  *= (X_t^( c * s23 )) + (Y_t^( c * s13 )) ;
      }
      break ; 
      case  1:         // e([X], Y )
      {
        string      X_s       = X.rbegin()->first ;
        char      * true_name = unique_symbol((char *)X_s.c_str()) ;
        excell  * & Com       = history[true_name].back() ;
        excell    * Opt       = Com->element(2) ;
        INTEGER   & s1        = as_INTEGER(Opt->element(6)) ;
        INTEGER   & s2        = as_INTEGER(Opt->element(7)) ;
        INTEGER   & s3        = as_INTEGER(Opt->element(8)) ;
        As1 *= Y^s1 ;
        As2 *= Y^s2 ;
        As3 *= Y^s3 ;
      }
      break ;
      case  0: break ; // Z
      default: break ; // impossible
    }
  }

  GROUP & G   = as_GROUP(crs->element(3)) ;
  GROUP & U   = as_GROUP(crs->element(4)) ;
  GROUP & V   = as_GROUP(crs->element(5)) ;
  GROUP & W1  = as_GROUP(crs->element(6)) ;
  GROUP & W2  = as_GROUP(crs->element(7)) ;
  GROUP & W3  = as_GROUP(crs->element(8)) ;

  char *  R1 = new_symbol("r1") ;
  char *  R2 = new_symbol("r2") ;
  char *  R3 = new_symbol("r3") ;

  INTEGER r1 = INTEGER(string(R1)) ;
  INTEGER r2 = INTEGER(string(R2)) ;
  INTEGER r3 = INTEGER(string(R3)) ;

  DECLARE(r1);
  DECLARE(r2);
  DECLARE(r3);

  GROUP phi11 = (U^(ssc11            )) * (W1^(ssc31+r2)) ;
  GROUP phi12 = (V^(      ssc21+r1   )) * (W2^(ssc31+r2)) ;
  GROUP phi13 = (G^(ssc11+ssc21+r1   )) * (W3^(ssc31+r2)) * As1 * Xsc1 ;
  GROUP phi21 = (U^(ssc12      -r1   )) * (W1^(ssc32+r3)) ;
  GROUP phi22 = (V^(      ssc22      )) * (W2^(ssc32+r3)) ;
  GROUP phi23 = (G^(ssc12+ssc22-r1   )) * (W3^(ssc32+r3)) * As2 * Xsc2 ;
  GROUP phi31 = (U^(ssc13      -r2   )) * (W1^(ssc33   )) ;
  GROUP phi32 = (V^(      ssc23   -r3)) * (W2^(ssc33   )) ;
  GROUP phi33 = (G^(ssc13+ssc23-r2-r3)) * (W3^(ssc33   )) * As3 * Xsc3 ;

  char * Phi11 = new_symbol("phi11") ; REDUCE(Phi11, phi11) ;
  char * Phi12 = new_symbol("phi12") ; REDUCE(Phi12, phi12) ;
  char * Phi13 = new_symbol("phi13") ; REDUCE(Phi13, phi13) ;
  char * Phi21 = new_symbol("phi21") ; REDUCE(Phi21, phi21) ;
  char * Phi22 = new_symbol("phi22") ; REDUCE(Phi22, phi22) ;
  char * Phi23 = new_symbol("phi23") ; REDUCE(Phi23, phi23) ;
  char * Phi31 = new_symbol("phi31") ; REDUCE(Phi31, phi31) ;
  char * Phi32 = new_symbol("phi32") ; REDUCE(Phi32, phi32) ;
  char * Phi33 = new_symbol("phi33") ; REDUCE(Phi33, phi33) ;

  excell * PHI11 = EXLIST(GROUP,L,new GROUP(Phi11)) ;
  excell * PHI12 = EXLIST(GROUP,L,new GROUP(Phi12)) ;
  excell * PHI13 = EXLIST(GROUP,L,new GROUP(Phi13)) ;
  excell * PHI21 = EXLIST(GROUP,L,new GROUP(Phi21)) ;
  excell * PHI22 = EXLIST(GROUP,L,new GROUP(Phi22)) ;
  excell * PHI23 = EXLIST(GROUP,L,new GROUP(Phi23)) ;
  excell * PHI31 = EXLIST(GROUP,L,new GROUP(Phi31)) ;
  excell * PHI32 = EXLIST(GROUP,L,new GROUP(Phi32)) ;
  excell * PHI33 = EXLIST(GROUP,L,new GROUP(Phi33)) ;
  excell * Opt  = OPT_PROOF_(4_1_2, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,
                PHI11, PHI12, PHI13,
		PHI21, PHI22, PHI23,
		PHI31, PHI32, PHI33, Var) ;
}

static excell * GS_DUMMY_PROOFWI_PPE_4_1_2 (excell * crs, excell * predicate, cell * L){
  char * Phi11 = new_symbol("phi11") ; 
  char * Phi12 = new_symbol("phi12") ;
  char * Phi13 = new_symbol("phi13") ;
  char * Phi21 = new_symbol("phi21") ;
  char * Phi22 = new_symbol("phi22") ;
  char * Phi23 = new_symbol("phi23") ;
  char * Phi31 = new_symbol("phi31") ;
  char * Phi32 = new_symbol("phi32") ;
  char * Phi33 = new_symbol("phi33") ;

  excell * PHI11 = EXLIST(GROUP,L,new GROUP(Phi11)) ; DECLARE(PHI11) ;
  excell * PHI12 = EXLIST(GROUP,L,new GROUP(Phi12)) ; DECLARE(PHI12) ;
  excell * PHI13 = EXLIST(GROUP,L,new GROUP(Phi13)) ; DECLARE(PHI13) ;
  excell * PHI21 = EXLIST(GROUP,L,new GROUP(Phi21)) ; DECLARE(PHI21) ;
  excell * PHI22 = EXLIST(GROUP,L,new GROUP(Phi22)) ; DECLARE(PHI22) ;
  excell * PHI23 = EXLIST(GROUP,L,new GROUP(Phi23)) ; DECLARE(PHI23) ;
  excell * PHI31 = EXLIST(GROUP,L,new GROUP(Phi31)) ; DECLARE(PHI31) ;
  excell * PHI32 = EXLIST(GROUP,L,new GROUP(Phi32)) ; DECLARE(PHI32) ;
  excell * PHI33 = EXLIST(GROUP,L,new GROUP(Phi33)) ; DECLARE(PHI33) ;
  excell * Opt  = OPT_PROOF_(4_1_2, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,
                PHI11, PHI12, PHI13,
		PHI21, PHI22, PHI23,
		PHI31, PHI32, PHI33, Var) ;
}

static excell * GS_VERIFY_PPE_4_1_2 (excell * crs, excell * predicate, excell * proof, cell * L){
  excell * Opt       = proof->element(2) ;

  TARGET CCc11, CCc12, CCc13, CCc22, CCc23, CCc33, CA1, CA2, CA3, T ;

  vector<split_PPE> Vec = split_PPE_predicate(as_TARGET(predicate)) ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    GROUP   & Y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    TARGET  & Z           = get<4>(*i) ;
    switch(commit_type){
      case  2:         // e([X],[Y])^c
      {
        string      X_s        = X.rbegin()->first ;
        string      Y_s        = Y.rbegin()->first ;
        char      * true_name1 = unique_symbol((char *)X_s.c_str()) ;
        char      * true_name2 = unique_symbol((char *)Y_s.c_str()) ;
        excell  * & Com1       = history[true_name1].back() ;
        excell  * & Com2       = history[true_name2].back() ;
        GROUP     & cx1        = as_GROUP(Com1->element(3)) ;
        GROUP     & cx2        = as_GROUP(Com1->element(4)) ;
        GROUP     & cx3        = as_GROUP(Com1->element(5)) ;
        GROUP     & cy1        = as_GROUP(Com2->element(3)) ;
        GROUP     & cy2        = as_GROUP(Com2->element(4)) ;
        GROUP     & cy3        = as_GROUP(Com2->element(5)) ;

	CCc11 *= (PAIRING_RAW(cx1,cy1)                       )^c ;
	CCc12 *= (PAIRING_RAW(cx1,cy2) * PAIRING_RAW(cx2,cy1))^c ;
	CCc13 *= (PAIRING_RAW(cx1,cy3) * PAIRING_RAW(cx3,cy1))^c ;
	CCc22 *= (PAIRING_RAW(cx2,cy2)                       )^c ;
	CCc23 *= (PAIRING_RAW(cx2,cy3) * PAIRING_RAW(cx3,cy2))^c ;
	CCc33 *= (PAIRING_RAW(cx3,cy3)                       )^c ;
      }
      break ; 
      case  1:         // e([X], Y )
      {
        string      X_s       = X.rbegin()->first ;
        char      * true_name = unique_symbol((char *)X_s.c_str()) ;
        excell  * & Com       = history[true_name].back() ;
        GROUP     & c1        = as_GROUP(Com->element(3)) ;
        GROUP     & c2        = as_GROUP(Com->element(4)) ;
        GROUP     & c3        = as_GROUP(Com->element(5)) ;

        CA1 *= PAIRING_RAW(c1,Y) ;
        CA2 *= PAIRING_RAW(c2,Y) ;
        CA3 *= PAIRING_RAW(c3,Y) ;
      }
      break ;
      case  0:
      {
        T /= Z ;
      }
      break ; // Z
      default: break ; // impossible
    }
  }

  GROUP & phi11 = as_GROUP(proof->element(3)) ;
  GROUP & phi12 = as_GROUP(proof->element(4)) ;
  GROUP & phi13 = as_GROUP(proof->element(5)) ;
  GROUP & phi21 = as_GROUP(proof->element(6)) ;
  GROUP & phi22 = as_GROUP(proof->element(7)) ;
  GROUP & phi23 = as_GROUP(proof->element(8)) ;
  GROUP & phi31 = as_GROUP(proof->element(9)) ;
  GROUP & phi32 = as_GROUP(proof->element(10)) ;
  GROUP & phi33 = as_GROUP(proof->element(11)) ;

  GROUP & G   = as_GROUP(crs->element(3)) ;
  GROUP & U   = as_GROUP(crs->element(4)) ;
  GROUP & V   = as_GROUP(crs->element(5)) ;
  GROUP & W1  = as_GROUP(crs->element(6)) ;
  GROUP & W2  = as_GROUP(crs->element(7)) ;
  GROUP & W3  = as_GROUP(crs->element(8)) ;

  TARGET left1 =       CCc11 ;
  TARGET left2 =       CCc12 ;
  TARGET left3 = CA1 * CCc13 ;
  TARGET left4 =       CCc22 ;
  TARGET left5 = CA2 * CCc23 ;
  TARGET left6 = CA3 * CCc33 ;

  TARGET right1 = PAIRING_RAW(U,phi11)       * PAIRING_RAW(W1,phi31) ;
  TARGET right2 = PAIRING_RAW(U,phi12)       * PAIRING_RAW(W1,phi32)
                * PAIRING_RAW(V,phi21)       * PAIRING_RAW(W2,phi31) ;
  TARGET right3 = PAIRING_RAW(U,phi13)       * PAIRING_RAW(W1,phi33)
                * PAIRING_RAW(G,phi11*phi12) * PAIRING_RAW(W3,phi31) ;
  TARGET right4 = PAIRING_RAW(V,phi22)       * PAIRING_RAW(W2,phi32) ;
  TARGET right5 = PAIRING_RAW(V,phi23)       * PAIRING_RAW(W2,phi33)
                * PAIRING_RAW(G,phi12*phi22) * PAIRING_RAW(W3,phi32) ;
  TARGET right6 = PAIRING_RAW(G,phi13*phi23) * PAIRING_RAW(W3,phi33) ;

  return EXLIST(LIST_LITERAL,L,
    EQ(left1, right1, L), EQ(left2, right2, L), EQ(left3, right3, L),
    EQ(left4, right4, L), EQ(left5, right5, L), EQ(left6, right6, L)
  ) ;
}

static excell* GS_VERIFY2_PPE_4_1_2 (excell* proof, cell* L) {
  excell * Opt       = proof->element(2) ;
  excell * crs       = Opt->element(3) ;
  excell * predicate = Opt->element(4) ;
  return GS_VERIFY_PPE_4_1_2 (crs, predicate, proof, L);
}

// ===================================================================================
// GS_PROOFWI_MSE
// ===================================================================================

typedef std::tuple<int, GROUP, INTEGER, INTEGER> split_MSE ;

static vector<split_MSE> split_MSE_predicate(GROUP & predicate) {
  typedef LinearCombination              <GROUP , INTEGER> DEGREE11 ;
  typedef LinearCombination              <string, INTEGER> DEGREE10 ;
  typedef MultiplicativeLinearCombination<string, GROUP  > DEGREE01 ;
  DEGREE11 d11 ;
  DEGREE10 d10 ;
  DEGREE01 d01 ;
  GROUP    d00 ;
  for(auto i = predicate.rbegin(); i != predicate.rend() ; i++){ // multiplication
    string    X_s = i->first  ;
    GROUP     X   = GROUP(X_s) ;
    INTEGER & a   = i->second ;

    if(is_COMMITTED(a)){ // ([X]^[y])^c

      for(auto j = a.rbegin(); j != a.rend() ; j++){ // addition

        INTEGER c = INTEGER(1) ;
        const Monomial<string, integer_> & mono        = j->first ;
        const integer_                   & coefficient = j->second ;
        c *= INTEGER(coefficient) ;
        if(is_COMMITTED(mono)){

          for(auto k = mono.rbegin(); k != mono.rend() ; k++){ // multiplication
            const string   & com      = k->first ;
            const integer_ & comindex = k->second ;
            if(! is_COMMITTED(com)) c *= INTEGER(com) ^ comindex ;
          }

          for(auto k = mono.rbegin(); k != mono.rend() ; k++){ // multiplication
            const string y_s = k->first ;
            if(is_COMMITTED(y_s)){
              if(is_COMMITTED(X_s)) d11 += DEGREE11(X^INTEGER(y_s), c) ;
	      else                  d01 *= DEGREE01(y_s, X^c) ;
	      break ;
            }
          }
        }else{ // [X]^a
	  INTEGER a = c * INTEGER(mono) ;
          if(is_COMMITTED(X_s)) d10 += DEGREE10(X_s, a) ;
          else                  d00 *= X^a ;
	}
      }
    }else{
      if(is_COMMITTED(X_s)) d10 += DEGREE10(X_s, a) ;
      else                  d00 *= X^a ;
    }
  }

  vector<split_MSE> V ;

  for(auto i=d11.rbegin(); i!=d11.rend(); i++){ // ([x]^[y])^c
    const GROUP   & g   = i->first  ;
    const INTEGER & c   = i->second ;
    auto            j   = g.begin() ;
    const string  & X_s = j->first  ;
    const INTEGER & y   = j->second ;
    V.push_back(split_MSE(3, GROUP(X_s), INTEGER(y), INTEGER(c))) ;
  }

  for(auto i=d01.rbegin(); i!=d01.rend(); i++){ // (x^[y])
    const string & y_s = i->first  ;
    const GROUP  & x   = i->second ;
    V.push_back(split_MSE(2, GROUP(x), INTEGER(y_s), INTEGER(1))) ;
  }

  for(auto i=d10.rbegin(); i!=d10.rend(); i++){ // ([x]^c)
    const string    X_s = i->first  ;
    const INTEGER & c   = i->second ;
    V.push_back(split_MSE(1, GROUP(X_s), INTEGER(1), INTEGER(c))) ;
  }

  if(d00!=GROUP()) V.push_back(split_MSE(0, d00, INTEGER(1), INTEGER(1))) ;
  return V ;
}

static excell* GS_PROOFWI_MSE_4_2_1(excell* crs, excell* predicate, cell* L){
  INTEGER   rho1, rho2, rho3 ;
  vector<split_MSE> Vec = split_MSE_predicate(as_GROUP(predicate)) ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    INTEGER & y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    switch(commit_type){
      case 3: break ; // ([X]^[y])^c
      case 2: break ; // X^[y]
      case 1:         // [X]^c
      {
        char     * true_name1 = as_char_star(X) ;
        excell * & Com1       = history[true_name1].back() ;
        excell   * Opt1       = Com1->element(2) ;
        INTEGER  & s1         = as_INTEGER(Opt1->element(6)) ;
        INTEGER  & s2         = as_INTEGER(Opt1->element(7)) ;
        INTEGER  & s3         = as_INTEGER(Opt1->element(8)) ;
        rho1 += c * s1 ;
        rho2 += c * s2 ;
        rho3 += c * s3 ;
      }
      break ;
      default: break ; // X^c
    }
  }
  char * Rho1 = new_symbol("rho1") ; REDUCE(Rho1, rho1) ;
  char * Rho2 = new_symbol("rho2") ; REDUCE(Rho2, rho2) ;
  char * Rho3 = new_symbol("rho3") ; REDUCE(Rho3, rho3) ;
  excell * Opt  = OPT_PROOF_(4_2_1, crs, predicate) ;
  excell * RHO1 = EXLIST(INTEGER,L,new INTEGER(Rho1)) ;
  excell * RHO2 = EXLIST(INTEGER,L,new INTEGER(Rho2)) ;
  excell * RHO3 = EXLIST(INTEGER,L,new INTEGER(Rho3)) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,RHO1, RHO2, RHO3, Var) ;
}

static excell* GS_DUMMY_PROOFWI_MSE_4_2_1(excell* crs, excell* predicate, cell* L){
  char * Rho1 = new_symbol("rho1") ;
  char * Rho2 = new_symbol("rho2") ;
  char * Rho3 = new_symbol("rho3") ;
  excell * Opt  = OPT_PROOF_(4_2_1, crs, predicate) ;
  excell * RHO1 = EXLIST(INTEGER,L,new INTEGER(Rho1)) ; DECLARE(RHO1) ;
  excell * RHO2 = EXLIST(INTEGER,L,new INTEGER(Rho2)) ; DECLARE(RHO2) ;
  excell * RHO3 = EXLIST(INTEGER,L,new INTEGER(Rho3)) ; DECLARE(RHO3) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,RHO1, RHO2, RHO3, Var) ;
}

static excell* GS_VERIFY_MSE_4_2_1 (excell * crs, excell * predicate, excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  vector<split_MSE> Vec = split_MSE_predicate(as_GROUP(predicate)) ;
  GROUP    left1, left2, left3 ;
  GROUP    T ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    INTEGER & y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;

    switch(commit_type){
      case 3: break ; // ([X]^[y])^c
      case 2: break ; // X^[y]
      case 1:         // [X]^c
      {
        char     * true_name1 = as_char_star(X) ;
        excell * & Com1       = history[true_name1].back() ;
        GROUP    & c1         = as_GROUP(Com1->element(3)) ;
        GROUP    & c2         = as_GROUP(Com1->element(4)) ;
        GROUP    & c3         = as_GROUP(Com1->element(5)) ;

        left1 *= c1^c ;
        left2 *= c2^c ;
        left3 *= c3^c ;
      }
      break ;
      default:        // X^c
        T /= X^c ;
      break ;
    }
  }
  INTEGER & rho1 = as_INTEGER(proof->element(3)) ;
  INTEGER & rho2 = as_INTEGER(proof->element(4)) ;
  INTEGER & rho3 = as_INTEGER(proof->element(5)) ;
  GROUP & G  = as_GROUP(crs->element(3)) ;
  GROUP & U  = as_GROUP(crs->element(4)) ;
  GROUP & V  = as_GROUP(crs->element(5)) ;
  GROUP & W1 = as_GROUP(crs->element(6)) ;
  GROUP & W2 = as_GROUP(crs->element(7)) ;
  GROUP & W3 = as_GROUP(crs->element(8)) ;
  GROUP right1 = (U^rho1) * (W1^rho3) ;
  GROUP right2 = (V^rho2) * (W2^rho3) ;
  GROUP right3 = (G^(rho1+rho2))*(W3^rho3) * T ;

  return EXLIST(LIST_LITERAL,L,EQ(left1,right1,L),EQ(left2,right2,L),EQ(left3,right3,L));
}

static excell* GS_VERIFY2_MSE_4_2_1 (excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  excell * crs       = Opt->element(3) ;
  excell * predicate = Opt->element(4) ;
  return GS_VERIFY_MSE_4_2_1 (crs, predicate, proof, L);
}

static excell* GS_PROOFWI_MSE_4_2_2(excell* crs, excell* predicate, cell* L){
  GROUP   phi1, phi2;
  vector<split_MSE> Vec = split_MSE_predicate(as_GROUP(predicate)) ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    INTEGER & y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;

    switch(commit_type){
      case 3:  break ; // ([X]^[y])^c
      case 2:          // X^[y]
      {
        char     * true_name2 = as_char_star(y) ;
        excell * & Com2       = history[true_name2].back() ;
        excell   * Opt2       = Com2->element(2) ;
        INTEGER  & r1         = as_INTEGER(Opt2->element(6)) ;
        INTEGER  & r2         = as_INTEGER(Opt2->element(7)) ;

        phi1 *= X^r1 ;
        phi2 *= X^r2 ;
      }
      break ;
      case 1:  break ; // [X]^c
      default: break ;
    }
  }
  char * Phi1 = new_symbol("phi1") ; REDUCE(Phi1, phi1) ;
  char * Phi2 = new_symbol("phi2") ; REDUCE(Phi2, phi2) ;
  excell * PHI1  = EXLIST(GROUP,L,new GROUP(Phi1)) ;
  excell * PHI2  = EXLIST(GROUP,L,new GROUP(Phi2)) ;
  excell * Opt   = OPT_PROOF_(4_2_2, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt, PHI1, PHI1, Var) ;
}

static excell* GS_DUMMY_PROOFWI_MSE_4_2_2(excell* crs, excell* predicate, cell* L){
  char * Phi1 = new_symbol("phi1") ;
  char * Phi2 = new_symbol("phi2") ;
  excell * PHI1  = EXLIST(GROUP,L,new GROUP(Phi1)) ; DECLARE(PHI1) ;
  excell * PHI2  = EXLIST(GROUP,L,new GROUP(Phi2)) ; DECLARE(PHI2) ;
  excell * Opt   = OPT_PROOF_(4_2_2, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt, PHI1, PHI1, Var) ;
}

static excell* GS_VERIFY_MSE_4_2_2 (excell * crs, excell * predicate, excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  TARGET   left1, left2, left3 ;
  GROUP    T ;
  vector<split_MSE> Vec = split_MSE_predicate(as_GROUP(predicate)) ;
  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    INTEGER & y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    switch(commit_type){
      case 3: break ; // ([X]^[y])^c
      case 2: // X^[y]
      {
        char     * true_name2 = as_char_star(y) ;
        excell * & Com2       = history[true_name2].back() ;
        GROUP    & d1         = as_GROUP(Com2->element(3)) ;
        GROUP    & d2         = as_GROUP(Com2->element(4)) ;
        GROUP    & d3         = as_GROUP(Com2->element(5)) ;
        left1 *= PAIRING(d1,X) ;
        left2 *= PAIRING(d2,X) ;
        left3 *= PAIRING(d3,X) ;
      }
      break ;
      case 1: break ; // [X]^c
      default:
        T /= X^c ;
      break ;
    }
  }
  GROUP  & phi1   = as_GROUP(proof->element(3)) ;
  GROUP  & phi2   = as_GROUP(proof->element(4)) ;
  GROUP  & G      = as_GROUP(crs->element(3)) ;
  GROUP  & U      = as_GROUP(crs->element(4)) ;
  GROUP  & V      = as_GROUP(crs->element(5)) ;
  GROUP  & W1     = as_GROUP(crs->element(6)) ;
  GROUP  & W2     = as_GROUP(crs->element(7)) ;
  GROUP  & W3     = as_GROUP(crs->element(8)) ;
  TARGET   right1 = PAIRING(W1,T)   * PAIRING(U,phi1) ;
  TARGET   right2 = PAIRING(W2,T)   * PAIRING(V,phi2) ;
  TARGET   right3 = PAIRING(W3*G,T) * PAIRING(G,phi1*phi2) ;

  return EXLIST(LIST_LITERAL,L,EQ(left1,right1,L),EQ(left2,right2,L),EQ(left3,right3,L)) ;
}

static excell* GS_VERIFY2_MSE_4_2_2 (excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  excell * crs       = Opt->element(3) ;
  excell * predicate = Opt->element(4) ;
  return GS_VERIFY_MSE_4_2_2 (crs, predicate, proof, L);
}

static excell* GS_PROOFWI_MSE_4_2_X(excell* crs, excell* predicate, cell* L){
  INTEGER as1  , as2  , as3  ;
  INTEGER syc1 , syc2 , syc3 ;
  INTEGER rcs11, rcs12, rcs13, rcs21, rcs22, rcs23 ;
  GROUP   Xcr1 , Xcr2 ;
  GROUP   Br1  , Br2  ;

  vector<split_MSE> Vec = split_MSE_predicate(as_GROUP(predicate)) ;

  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    INTEGER & y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;

    switch(commit_type){
      case 3: // ([X]^[y])^c
      {
        char     * true_name1 = as_char_star(X) ;
        char     * true_name2 = as_char_star(y) ;
        excell * & Com1       = history[true_name1].back() ;
        excell * & Com2       = history[true_name2].back() ;
        excell   * Opt1       = Com1->element(2) ;
        excell   * Opt2       = Com2->element(2) ;
        GROUP    & X          = as_GROUP  (Opt1->element(5)) ;
        INTEGER  & s1         = as_INTEGER(Opt1->element(6)) ;
        INTEGER  & s2         = as_INTEGER(Opt1->element(7)) ;
        INTEGER  & s3         = as_INTEGER(Opt1->element(8)) ;
        INTEGER  & y          = as_INTEGER(Opt2->element(5)) ;
        INTEGER  & r1         = as_INTEGER(Opt2->element(6)) ;
        INTEGER  & r2         = as_INTEGER(Opt2->element(7)) ;

        rcs11 += r1 * c * s1 ;
        rcs12 += r1 * c * s2 ;
        rcs13 += r1 * c * s3 ;

        rcs21 += r2 * c * s1 ;
        rcs22 += r2 * c * s2 ;
        rcs23 += r2 * c * s3 ;

        syc1  += s1 * y * c  ;
        syc2  += s2 * y * c  ;
        syc3  += s3 * y * c  ;

        Xcr1  *= X^(c * r1) ;
        Xcr2  *= X^(c * r2) ;
      }  
      break ;
      case 2: // X^[y]
      {
        char     * true_name2 = as_char_star(y) ;
        excell * & Com2       = history[true_name2].back() ;
        excell   * Opt2       = Com2->element(2) ;
        INTEGER  & r1         = as_INTEGER(Opt2->element(6)) ;
        INTEGER  & r2         = as_INTEGER(Opt2->element(7)) ;

        Br1 *= X^r1 ;
        Br2 *= X^r2 ;
      }
      break ;
      case 1: // [X]^c
      {
        char     * true_name1 = as_char_star(X) ;
        excell * & Com1       = history[true_name1].back() ;
        excell   * Opt1       = Com1->element(2) ;
        INTEGER  & s1         = as_INTEGER(Opt1->element(6)) ;
        INTEGER  & s2         = as_INTEGER(Opt1->element(7)) ;
        INTEGER  & s3         = as_INTEGER(Opt1->element(8)) ;

        as1 += c * s1 ;
        as2 += c * s2 ;
        as3 += c * s3 ;
      }
      break ;
      default:
      break ;
    }
  }

  GROUP & G   = as_GROUP(crs->element(3)) ;
  GROUP & U   = as_GROUP(crs->element(4)) ;
  GROUP & V   = as_GROUP(crs->element(5)) ;
  GROUP & W1  = as_GROUP(crs->element(6)) ;
  GROUP & W2  = as_GROUP(crs->element(7)) ;
  GROUP & W3  = as_GROUP(crs->element(8)) ;

  char *  R1 = new_symbol("r1") ;
  char *  R2 = new_symbol("r2") ;
  char *  R3 = new_symbol("r3") ;

  INTEGER r1 = INTEGER(string(R1)) ;
  INTEGER r2 = INTEGER(string(R2)) ;
  INTEGER r3 = INTEGER(string(R3)) ;

  DECLARE(r1);
  DECLARE(r2);
  DECLARE(r3);

  GROUP phi11 =              (W1^(as1 + syc1 + rcs13 + r2)) * (U^(rcs11   )) ;
  GROUP phi12 =              (W2^(as1 + syc1 + rcs13 + r2)) * (V^(rcs12+r1)) ;
  GROUP phi13 = Br1 * Xcr1 * (W3^(as1 + syc1 + rcs13 + r2)) * 
                                       (G^(as1 + syc1 + rcs11 + rcs12 + r1)) ;

  GROUP phi21 =              (W1^(as2 + syc2 + rcs23 + r3)) * (U^(rcs21-r1)) ;
  GROUP phi22 =              (W2^(as2 + syc2 + rcs23 + r3)) * (V^(rcs22   )) ;
  GROUP phi23 = Br2 * Xcr2 * (W3^(as2 + syc2 + rcs23 + r3)) *
                                       (G^(as2 + syc2 + rcs21 + rcs22 - r1)) ;

  GROUP phi31 =              (W1^(as3 + syc3)) * (U^(-r2)) ;
  GROUP phi32 =              (W2^(as3 + syc3)) * (V^(-r3)) ;
  GROUP phi33 =              (W3^(as3 + syc3)) * (G^(as3 + syc3 - r2 - r3)) ;

  char * Phi11 = new_symbol("phi11") ; REDUCE(Phi11, phi11) ;
  char * Phi12 = new_symbol("phi12") ; REDUCE(Phi12, phi12) ;
  char * Phi13 = new_symbol("phi13") ; REDUCE(Phi13, phi13) ;
  char * Phi21 = new_symbol("phi21") ; REDUCE(Phi21, phi21) ;
  char * Phi22 = new_symbol("phi22") ; REDUCE(Phi22, phi22) ;
  char * Phi23 = new_symbol("phi23") ; REDUCE(Phi23, phi23) ;
  char * Phi31 = new_symbol("phi31") ; REDUCE(Phi31, phi31) ;
  char * Phi32 = new_symbol("phi32") ; REDUCE(Phi32, phi32) ;
  char * Phi33 = new_symbol("phi33") ; REDUCE(Phi33, phi33) ;

  excell * PHI11 = EXLIST(GROUP,L,new GROUP(Phi11)) ;
  excell * PHI12 = EXLIST(GROUP,L,new GROUP(Phi12)) ;
  excell * PHI13 = EXLIST(GROUP,L,new GROUP(Phi13)) ;
  excell * PHI21 = EXLIST(GROUP,L,new GROUP(Phi21)) ;
  excell * PHI22 = EXLIST(GROUP,L,new GROUP(Phi22)) ;
  excell * PHI23 = EXLIST(GROUP,L,new GROUP(Phi23)) ;
  excell * PHI31 = EXLIST(GROUP,L,new GROUP(Phi31)) ;
  excell * PHI32 = EXLIST(GROUP,L,new GROUP(Phi32)) ;
  excell * PHI33 = EXLIST(GROUP,L,new GROUP(Phi33)) ;
  excell * Opt   = OPT_PROOF_(4_2_4, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;

  return EXLIST(LIST_LITERAL,L,Opt,
               PHI11, PHI12, PHI13,
               PHI21, PHI22, PHI23,
               PHI31, PHI32, PHI33, Var) ;
}

static excell* GS_DUMMY_PROOFWI_MSE_4_2_X(excell* crs, excell* predicate, cell* L){
  char * Phi11 = new_symbol("phi11") ;
  char * Phi12 = new_symbol("phi12") ;
  char * Phi13 = new_symbol("phi13") ;
  char * Phi21 = new_symbol("phi21") ;
  char * Phi22 = new_symbol("phi22") ;
  char * Phi23 = new_symbol("phi23") ;
  char * Phi31 = new_symbol("phi31") ;
  char * Phi32 = new_symbol("phi32") ;
  char * Phi33 = new_symbol("phi33") ;

  excell * PHI11 = EXLIST(GROUP,L,new GROUP(Phi11)) ; DECLARE(PHI11) ;
  excell * PHI12 = EXLIST(GROUP,L,new GROUP(Phi12)) ; DECLARE(PHI12) ;
  excell * PHI13 = EXLIST(GROUP,L,new GROUP(Phi13)) ; DECLARE(PHI13) ;
  excell * PHI21 = EXLIST(GROUP,L,new GROUP(Phi21)) ; DECLARE(PHI21) ;
  excell * PHI22 = EXLIST(GROUP,L,new GROUP(Phi22)) ; DECLARE(PHI22) ;
  excell * PHI23 = EXLIST(GROUP,L,new GROUP(Phi23)) ; DECLARE(PHI23) ;
  excell * PHI31 = EXLIST(GROUP,L,new GROUP(Phi31)) ; DECLARE(PHI31) ;
  excell * PHI32 = EXLIST(GROUP,L,new GROUP(Phi32)) ; DECLARE(PHI32) ;
  excell * PHI33 = EXLIST(GROUP,L,new GROUP(Phi33)) ; DECLARE(PHI33) ;
  excell * Opt   = OPT_PROOF_(4_2_4, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;

  return EXLIST(LIST_LITERAL,L,Opt,
               PHI11, PHI12, PHI13,
               PHI21, PHI22, PHI23,
               PHI31, PHI32, PHI33, Var) ;
}

static excell* GS_VERIFY_MSE_4_2_X (excell * crs, excell * predicate, excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;

  vector<split_MSE> Vec = split_MSE_predicate(as_GROUP(predicate)) ;

  GROUP    Ca1, Ca2, Ca3 ;
  TARGET   DCc11, DCc12, DCc13, DCc21, DCc22, DCc23, DCc31, DCc32, DCc33 ;
  TARGET   DB1, DB2, DB3 ;
  GROUP    T ;

  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    INTEGER & y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;

    switch(commit_type){
      case 3: // ([X]^[y])^c
      {
        char     * true_name1 = as_char_star(X) ;
        char     * true_name2 = as_char_star(y) ;
        excell * & Com1       = history[true_name1].back() ;
        excell * & Com2       = history[true_name2].back() ;
        GROUP    & c1         = as_GROUP(Com1->element(3)) ;
        GROUP    & c2         = as_GROUP(Com1->element(4)) ;
        GROUP    & c3         = as_GROUP(Com1->element(5)) ;
        GROUP    & d1         = as_GROUP(Com2->element(3)) ;
        GROUP    & d2         = as_GROUP(Com2->element(4)) ;
        GROUP    & d3         = as_GROUP(Com2->element(5)) ;

        DCc11 *= PAIRING(d1,c1^c) ;
        DCc12 *= PAIRING(d1,c2^c) ;
        DCc13 *= PAIRING(d1,c3^c) ;
        DCc21 *= PAIRING(d2,c1^c) ;
        DCc22 *= PAIRING(d2,c2^c) ;
        DCc23 *= PAIRING(d2,c3^c) ;
        DCc31 *= PAIRING(d3,c1^c) ;
        DCc32 *= PAIRING(d3,c2^c) ;
        DCc33 *= PAIRING(d3,c3^c) ;
      }  
      break ;
      case 2: // X^[y]
      {
        char     * true_name2 = as_char_star(y) ;
        excell * & Com2       = history[true_name2].back() ;
        GROUP    & d1         = as_GROUP(Com2->element(3)) ;
        GROUP    & d2         = as_GROUP(Com2->element(4)) ;
        GROUP    & d3         = as_GROUP(Com2->element(5)) ;

        DB1   *= PAIRING(X,d1) ;
        DB2   *= PAIRING(X,d2) ;
        DB3   *= PAIRING(X,d3) ;
      }
      break ;
      case 1: // [X]^c
      {
        char     * true_name1 = as_char_star(X) ;
        excell * & Com1       = history[true_name1].back() ;
        GROUP    & c1         = as_GROUP(Com1->element(3)) ;
        GROUP    & c2         = as_GROUP(Com1->element(4)) ;
        GROUP    & c3         = as_GROUP(Com1->element(5)) ;

        Ca1 *= c1^c ;
        Ca2 *= c2^c ;
        Ca3 *= c3^c ;
      }
      break ;
      default:
        T /= X^c ;
      break ;
    }
  }

  GROUP & G   = as_GROUP(crs->element(3)) ;
  GROUP & U   = as_GROUP(crs->element(4)) ;
  GROUP & V   = as_GROUP(crs->element(5)) ;
  GROUP & W1  = as_GROUP(crs->element(6)) ;
  GROUP & W2  = as_GROUP(crs->element(7)) ;
  GROUP & W3  = as_GROUP(crs->element(8)) ;

  GROUP & phi11 = as_GROUP(proof->element(3)) ;
  GROUP & phi12 = as_GROUP(proof->element(4)) ;
  GROUP & phi13 = as_GROUP(proof->element(5)) ;
  GROUP & phi21 = as_GROUP(proof->element(6)) ;
  GROUP & phi22 = as_GROUP(proof->element(7)) ;
  GROUP & phi23 = as_GROUP(proof->element(8)) ;
  GROUP & phi31 = as_GROUP(proof->element(9)) ;
  GROUP & phi32 = as_GROUP(proof->element(10)) ;
  GROUP & phi33 = as_GROUP(proof->element(11)) ;

  TARGET left1  = PAIRING(U,phi11) ;
  TARGET left2  = PAIRING(U,phi12) * PAIRING(V,phi21) ;
  TARGET left3  = PAIRING(U,phi13) ;
  TARGET left4  = PAIRING(V,phi22) ;
  TARGET left5  = PAIRING(V,phi23) ;
  TARGET left6  = TARGET() ;

  TARGET right1 = PAIRING(W1,Ca1/phi31) * DCc11 ;
  TARGET right2 = PAIRING(W1,Ca2/phi32) * PAIRING(W2,Ca1/phi31) * DCc12 * DCc21 ;
  TARGET right3 = PAIRING(W1,Ca3/phi33/T) * PAIRING(W3,Ca1/phi31)
                                  * PAIRING(G ,Ca1/phi11/phi21) * DB1 * DCc13 * DCc31 ;
  TARGET right4 = PAIRING(W2,Ca2/phi32) * DCc22 ;
  TARGET right5 = PAIRING(W2,Ca3/phi33/T) * PAIRING(W3,Ca2/phi32)
                                  * PAIRING(G ,Ca2/phi12/phi22) * DB2 * DCc23 * DCc32 ;
  TARGET right6 = PAIRING(W3,Ca3/phi33/T) * PAIRING(G ,Ca3/phi13/phi23/T)
                                  * DB3 * DCc33 ;

  return EXLIST(LIST_LITERAL,L,
    EQ(left1, right1, L), EQ(left2, right2, L), EQ(left3, right3, L),
    EQ(left4, right4, L), EQ(left5, right5, L), EQ(left6, right6, L)
  ) ;
}

static excell* GS_VERIFY2_MSE_4_2_X (excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  excell * crs       = Opt->element(3) ;
  excell * predicate = Opt->element(4) ;
  return GS_VERIFY_MSE_4_2_X (crs, predicate, proof, L);
}

static excell* GS_PROOFWI_MSE_4_2_3(excell* crs, excell* predicate, cell* L) {
  excell * proof = GS_PROOFWI_MSE_4_2_X(crs, predicate, L) ;
  excell * Opt = proof->element(2) ;
  Opt->cdr->cdr->car = OPT_PROOF_4_2_3 ;
  return proof ;
}

static excell* GS_DUMMY_PROOFWI_MSE_4_2_3(excell* crs, excell* predicate, cell* L) {
  excell * proof = GS_DUMMY_PROOFWI_MSE_4_2_X(crs, predicate, L) ;
  excell * Opt = proof->element(2) ;
  Opt->cdr->cdr->car = OPT_PROOF_4_2_3 ;
  return proof ;
}

static excell* GS_VERIFY_MSE_4_2_3 (excell * crs, excell * predicate, excell* proof, cell* L){
  return GS_VERIFY_MSE_4_2_X (crs, predicate, proof, L) ;
}

static excell* GS_VERIFY2_MSE_4_2_3 (excell* proof, cell* L){
  return GS_VERIFY2_MSE_4_2_X (proof, L) ;
}

static excell* GS_PROOFWI_MSE_4_2_4(excell* crs, excell* predicate, cell* L) {
  return GS_PROOFWI_MSE_4_2_X(crs, predicate, L) ;
}

static excell* GS_DUMMY_PROOFWI_MSE_4_2_4(excell* crs, excell* predicate, cell* L) {
  return GS_DUMMY_PROOFWI_MSE_4_2_X(crs, predicate, L) ;
}

static excell* GS_VERIFY_MSE_4_2_4 (excell * crs, excell * predicate, excell* proof, cell* L){
  return GS_VERIFY_MSE_4_2_X (crs, predicate, proof, L) ;
}

static excell* GS_VERIFY2_MSE_4_2_4 (excell* proof, cell* L){
  return GS_VERIFY2_MSE_4_2_X (proof, L) ;
}

// =======================================================================================
// GS_PROOFWI_QE
// =======================================================================================

typedef std::tuple<int, INTEGER, INTEGER, INTEGER> split_QE ;

static vector<split_QE> split_QE_predicate(INTEGER & predicate) {
  typedef SymmetricPairing <INTEGER         > D2BASE ;
  typedef LinearCombination<D2BASE , INTEGER> DEGREE2 ;
  typedef LinearCombination<INTEGER, INTEGER> DEGREE1 ;

  DEGREE2 d2 ;
  DEGREE1 d1 ;
  INTEGER d0 ;

  for(auto j = predicate.rbegin(); j != predicate.rend() ; j++){ // addition
    const Monomial<string, integer_> & mono = j->first ;
    INTEGER                            c    = INTEGER(j->second) ;
    vector<INTEGER> com ;
    if(is_COMMITTED(mono)){
      for(auto k = mono.rbegin(); k != mono.rend() ; k++){ // multiplication
        const string   & y       = k->first ;
        const integer_ & y_index = k->second ;
        if(is_COMMITTED(y)){
	       if(y_index == integer_(1)){ com.push_back(INTEGER(y)) ; }
          else if(y_index == integer_(2)){ com.push_back(INTEGER(y)) ; com.push_back(INTEGER(y)) ; }
          else /* impossible */ ;
        }
	else c *= INTEGER(y) ^ y_index ;
      }
           if(com.size() == 2) d2 += DEGREE2(D2BASE(com[0], com[1]), c) ;
      else if(com.size() == 1) d1 += DEGREE1(com[0], c) ;
      else /* impossible */ ;
    }else{
      d0 += c * INTEGER(mono) ;
    }
  }

  vector<split_QE> V ;

  for(auto i=d2.rbegin(); i!=d2.rend(); i++){ // c * [x] * [y]
    const D2BASE  & B   = i->first  ;
    const INTEGER & c   = i->second ;
    const INTEGER & x   = B.first   ;
    const INTEGER & y   = B.second  ;
    V.push_back(split_QE(2, c, x, y)) ;
  }

  for(auto i=d1.rbegin(); i!=d1.rend(); i++){ // c * [x]
    const INTEGER & x = i->first  ;
    const INTEGER & c = i->second ;
    V.push_back(split_QE(1, c, x, INTEGER(1))) ;
  }

  if(d0 != INTEGER()) V.push_back(split_QE(0, d0, INTEGER(1), INTEGER(1))) ;
  return V ;
}

static excell* GS_PROOFWI_QE_4_3_1 (excell* crs, excell* predicate, cell* L){
  vector<split_QE> Vec = split_QE_predicate(as_INTEGER(predicate)) ;
  INTEGER rho1, rho2 ;
  for(auto i = Vec.begin() ; i != Vec.end() ; i++){
    int     & commit_type = get<0>(*i) ;
    INTEGER & c           = get<1>(*i) ;
    INTEGER & y1          = get<2>(*i) ;
    INTEGER & y2          = get<3>(*i) ;
    switch(commit_type){
      case  2: break ; // c * [y1] * [y2]
      case  1:         // c * [y1]
      {
        char     * true_name1 = as_char_star(y1) ;
        excell * & Com1       = history[true_name1].back() ;
        excell   * Opt1       = Com1->element(2) ;
        INTEGER  & r1         = as_INTEGER(Opt1->element(6)) ;
        INTEGER  & r2         = as_INTEGER(Opt1->element(7)) ;
        rho1 += c * r1 ;
        rho2 += c * r2 ;
      }
      break ;
      case  0: break ; // c
      default: break ; // impossible
    }
  }
  excell * Opt  = OPT_PROOF_(4_3_1, crs, predicate) ;
  char * Rho1 = new_symbol("rho1") ; REDUCE(Rho1, rho1) ;
  char * Rho2 = new_symbol("rho2") ; REDUCE(Rho2, rho2) ;
  excell * RHO1 = EXLIST(INTEGER, L, new INTEGER(Rho1)) ;
  excell * RHO2 = EXLIST(INTEGER, L, new INTEGER(Rho2)) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,RHO1,RHO2, Var) ;
}

static excell* GS_DUMMY_PROOFWI_QE_4_3_1 (excell* crs, excell* predicate, cell* L){
  excell * Opt  = OPT_PROOF_(4_3_1, crs, predicate) ;
  char * Rho1 = new_symbol("rho1") ;
  char * Rho2 = new_symbol("rho2") ;
  excell * RHO1 = EXLIST(INTEGER, L, new INTEGER(Rho1)) ; DECLARE(RHO1) ;
  excell * RHO2 = EXLIST(INTEGER, L, new INTEGER(Rho2)) ; DECLARE(RHO2) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;
  return EXLIST(LIST_LITERAL,L,Opt,RHO1,RHO2, Var) ;
}

static excell* GS_VERIFY_QE_4_3_1 (excell* crs, excell* predicate, excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  vector<split_QE> Vec = split_QE_predicate(as_INTEGER(predicate)) ;
  GROUP DB1, DB2, DB3 ;
  INTEGER t ;
  for(auto i = Vec.begin() ; i != Vec.end() ; i++) {
    int     & commit_type = get<0>(*i) ;
    INTEGER & c           = get<1>(*i) ;
    INTEGER & y1          = get<2>(*i) ;
    INTEGER & y2          = get<3>(*i) ;
    switch(commit_type){
      case  2: break ; // c * [y1] * [y2]
      case  1:         // c * [y1]
      {
        char     * true_name1 = as_char_star(y1) ;
        excell * & Com1       = history[true_name1].back() ;
        GROUP    & d1         = as_GROUP(Com1->element(3)) ;
        GROUP    & d2         = as_GROUP(Com1->element(4)) ;
        GROUP    & d3         = as_GROUP(Com1->element(5)) ;
        DB1 *= d1^c ;
        DB2 *= d2^c ;
        DB3 *= d3^c ;
      }
      break ;
      case  0:         // c
      {
        t -= c ;
      }
      break ;
      default: break ; // impossible
    }
  }

  INTEGER & rho1 = as_INTEGER(proof->element(3)) ;
  INTEGER & rho2 = as_INTEGER(proof->element(4)) ;

  GROUP & G  = as_GROUP(crs->element(3)) ;
  GROUP & U  = as_GROUP(crs->element(4)) ;
  GROUP & V  = as_GROUP(crs->element(5)) ;
  GROUP & W1 = as_GROUP(crs->element(6)) ;
  GROUP & W2 = as_GROUP(crs->element(7)) ;
  GROUP & W3 = as_GROUP(crs->element(8)) ;

  return EXLIST(LIST_LITERAL,L
    ,EQ(DB1, (W1^t)*(U^rho1), L)
    ,EQ(DB2, (W2^t)*(V^rho2), L)
    ,EQ(DB3, (W3^t)*(G^(t+rho1+rho2)), L)
  ) ;
}

static excell* GS_VERIFY2_QE_4_3_1  (excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  excell * crs       = Opt->element(3) ;
  excell * predicate = Opt->element(4) ;
  return GS_VERIFY_QE_4_3_1 (crs, predicate, proof, L);
}

static excell* GS_PROOFWI_QE_4_3_2 (excell* crs, excell* predicate, cell* L){
  vector<split_QE> Vec = split_QE_predicate(as_INTEGER(predicate)) ;
  INTEGER br1, br2, yrc1, yrc2, rcr11, rcr12, rcr21, rcr22 ;

  for(auto i = Vec.begin() ; i != Vec.end() ; i++){
    int     & commit_type = get<0>(*i) ;
    INTEGER & c           = get<1>(*i) ;
    INTEGER & y1          = get<2>(*i) ;
    INTEGER & y2          = get<3>(*i) ;
    switch(commit_type){
      case  2: break ; // c * [y1] * [y2]
      {
        char     * true_name1 = as_char_star(y1) ;
        char     * true_name2 = as_char_star(y2) ;
        excell * & Com1       = history[true_name1].back() ;
        excell * & Com2       = history[true_name2].back() ;
        excell   * Opt1       = Com1->element(2) ;
        excell   * Opt2       = Com2->element(2) ;
        INTEGER  & y1         = as_INTEGER(Opt1->element(5)) ;
        INTEGER  & y2         = as_INTEGER(Opt2->element(5)) ;
        INTEGER  & r11        = as_INTEGER(Opt1->element(6)) ;
        INTEGER  & r21        = as_INTEGER(Opt2->element(6)) ;
        INTEGER  & r12        = as_INTEGER(Opt1->element(7)) ;
        INTEGER  & r22        = as_INTEGER(Opt2->element(7)) ;
        yrc1  += (y2 * r11 + y1 * r21) * c ;
        yrc2  += (y2 * r12 + y1 * r22) * c ;
        rcr11 += r11 * c * r21 ;
        rcr12 += r11 * c * r22 ;
        rcr21 += r12 * c * r21 ;
        rcr22 += r12 * c * r22 ;
      }
      break ;
      case  1:         // c * [y1]
      {
        char     * true_name1 = as_char_star(y1) ;
        excell * & Com1       = history[true_name1].back() ;
        excell   * Opt1       = Com1->element(2) ;
        INTEGER  & r1         = as_INTEGER(Opt1->element(6)) ;
        INTEGER  & r2         = as_INTEGER(Opt1->element(7)) ;
        br1 += c * r1 ;
        br2 += c * r2 ;
      }
      break ;
      case  0: break ; // c
      default: break ; // impossible
    }
  }

  GROUP & G  = as_GROUP(crs->element(3)) ;
  GROUP & U  = as_GROUP(crs->element(4)) ;
  GROUP & V  = as_GROUP(crs->element(5)) ;
  GROUP & W1 = as_GROUP(crs->element(6)) ;
  GROUP & W2 = as_GROUP(crs->element(7)) ;
  GROUP & W3 = as_GROUP(crs->element(8)) ;

  char *  Omega1 = new_symbol("omega1") ;
  INTEGER omega1 = INTEGER(string(Omega1)) ;
  DECLARE(omega1);

  GROUP phi11 = ((  W1)^(br1+yrc1)) * (U^(rcr11             )) ;
  GROUP phi12 = ((  W2)^(br1+yrc1)) * (V^(      rcr12+omega1)) ;
  GROUP phi13 = ((G*W3)^(br1+yrc1)) * (G^(rcr11+rcr12+omega1)) ;

  GROUP phi21 = ((  W1)^(br2+yrc2)) * (U^(rcr21      -omega1)) ;
  GROUP phi22 = ((  W2)^(br2+yrc2)) * (V^(      rcr22       )) ;
  GROUP phi23 = ((G*W3)^(br2+yrc2)) * (G^(rcr21+rcr22-omega1)) ;

  char * Phi11 = new_symbol("phi11") ; REDUCE(Phi11, phi11) ;
  char * Phi12 = new_symbol("phi12") ; REDUCE(Phi12, phi12) ;
  char * Phi13 = new_symbol("phi13") ; REDUCE(Phi13, phi13) ;
  char * Phi21 = new_symbol("phi21") ; REDUCE(Phi21, phi21) ;
  char * Phi22 = new_symbol("phi22") ; REDUCE(Phi22, phi22) ;
  char * Phi23 = new_symbol("phi23") ; REDUCE(Phi23, phi23) ;

  excell * PHI11 = EXLIST(GROUP,L,new GROUP(Phi11)) ;
  excell * PHI12 = EXLIST(GROUP,L,new GROUP(Phi12)) ;
  excell * PHI13 = EXLIST(GROUP,L,new GROUP(Phi13)) ;
  excell * PHI21 = EXLIST(GROUP,L,new GROUP(Phi21)) ;
  excell * PHI22 = EXLIST(GROUP,L,new GROUP(Phi22)) ;
  excell * PHI23 = EXLIST(GROUP,L,new GROUP(Phi23)) ;
  excell * Opt   = OPT_PROOF_(4_3_2, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;

  return EXLIST(LIST_LITERAL,L,Opt,
               PHI11, PHI12, PHI13,
               PHI21, PHI22, PHI23, Var) ;
}

static excell* GS_DUMMY_PROOFWI_QE_4_3_2 (excell* crs, excell* predicate, cell* L){
  char * Phi11 = new_symbol("phi11") ;
  char * Phi12 = new_symbol("phi12") ;
  char * Phi13 = new_symbol("phi13") ;
  char * Phi21 = new_symbol("phi21") ;
  char * Phi22 = new_symbol("phi22") ;
  char * Phi23 = new_symbol("phi23") ;

  excell * PHI11 = EXLIST(GROUP,L,new GROUP(Phi11)) ; DECLARE(PHI11) ;
  excell * PHI12 = EXLIST(GROUP,L,new GROUP(Phi12)) ; DECLARE(PHI12) ;
  excell * PHI13 = EXLIST(GROUP,L,new GROUP(Phi13)) ; DECLARE(PHI13) ;
  excell * PHI21 = EXLIST(GROUP,L,new GROUP(Phi21)) ; DECLARE(PHI21) ;
  excell * PHI22 = EXLIST(GROUP,L,new GROUP(Phi22)) ; DECLARE(PHI22) ;
  excell * PHI23 = EXLIST(GROUP,L,new GROUP(Phi23)) ; DECLARE(PHI23) ;
  excell * Opt   = OPT_PROOF_(4_3_2, crs, predicate) ;
  excell * Var  = PREDICATE_VARIABLES(predicate, L) ;

  return EXLIST(LIST_LITERAL,L,Opt,
               PHI11, PHI12, PHI13,
               PHI21, PHI22, PHI23, Var) ;
}

static excell* GS_VERIFY_QE_4_3_2 (excell* crs, excell* predicate, excell* proof, cell* L){
  excell * Opt       = proof->element(2) ;
  vector<split_QE> Vec = split_QE_predicate(as_INTEGER(predicate)) ;
  GROUP DB1, DB2, DB3 ;
  TARGET DDc11, DDc12, DDc13, DDc22, DDc23, DDc33 ;

  INTEGER t ;
  for(auto i = Vec.begin() ; i != Vec.end() ; i++){
    int     & commit_type = get<0>(*i) ;
    INTEGER & c           = get<1>(*i) ;
    INTEGER & y1          = get<2>(*i) ;
    INTEGER & y2          = get<3>(*i) ;
    switch(commit_type){
      case  2: break ; // c * [y1] * [y2]
      {
        char     * true_name1 = as_char_star(y1) ;
        char     * true_name2 = as_char_star(y2) ;
        excell * & Com1       = history[true_name1].back() ;
        excell * & Com2       = history[true_name2].back() ;
        GROUP    & d11        = as_GROUP(Com1->element(3)) ;
        GROUP    & d12        = as_GROUP(Com1->element(4)) ;
        GROUP    & d13        = as_GROUP(Com1->element(5)) ;
        GROUP    & d21        = as_GROUP(Com2->element(3)) ;
        GROUP    & d22        = as_GROUP(Com2->element(4)) ;
        GROUP    & d23        = as_GROUP(Com2->element(5)) ;

        DDc11 *= PAIRING(d11,d21)^c ;
        DDc12 *= PAIRING(d11,d22)^c ;
        DDc13 *= PAIRING(d11,d23)^c ;
        DDc22 *= PAIRING(d12,d22)^c ;
        DDc23 *= PAIRING(d12,d23)^c ;
        DDc33 *= PAIRING(d13,d23)^c ;
      }
      break ;
      case  1:         // c * [y1]
      {
        char     * true_name1 = as_char_star(y1) ;
        excell * & Com1       = history[true_name1].back() ;
        GROUP    & d1         = as_GROUP(Com1->element(3)) ;
        GROUP    & d2         = as_GROUP(Com1->element(4)) ;
        GROUP    & d3         = as_GROUP(Com1->element(5)) ;
        DB1 *= d1^c ;
        DB2 *= d2^c ;
        DB3 *= d3^c ;
      }
      break ;
      case  0:         // c
      {
        t -= c ;
      }
      break ; 
      default: break ; // impossible
    }
  }

  GROUP & G  = as_GROUP(crs->element(3)) ;
  GROUP & U  = as_GROUP(crs->element(4)) ;
  GROUP & V  = as_GROUP(crs->element(5)) ;
  GROUP & W1 = as_GROUP(crs->element(6)) ;
  GROUP & W2 = as_GROUP(crs->element(7)) ;
  GROUP & W3 = as_GROUP(crs->element(8)) ;

  GROUP & phi11 = as_GROUP(proof->element(3)) ;
  GROUP & phi12 = as_GROUP(proof->element(4)) ;
  GROUP & phi13 = as_GROUP(proof->element(5)) ;
  GROUP & phi21 = as_GROUP(proof->element(6)) ;
  GROUP & phi22 = as_GROUP(proof->element(7)) ;
  GROUP & phi23 = as_GROUP(proof->element(8)) ;

  TARGET left1  = PAIRING(U,phi11) ;
  TARGET left2  = PAIRING(U,phi12) * PAIRING(V,phi21) ;
  TARGET left3  = PAIRING(U,phi13) * PAIRING(G,phi11*phi21) ;
  TARGET left4  = PAIRING(V,phi22) ;
  TARGET left5  = PAIRING(V,phi23) * PAIRING(G,phi12*phi22) ;
  TARGET left6  =                    PAIRING(G,phi13*phi23) ;

  TARGET right1 = PAIRING((W1^-t             ) * DB1, W1  ) * DDc11 ;
  TARGET right2 = PAIRING((W1^-INTEGER(1)    ) * DB1, W2  ) *
                  PAIRING((W2^-INTEGER(1)    ) * DB2, W1  ) * (DDc12^INTEGER(2)) ;
  TARGET right3 = PAIRING((W1^-(INTEGER(2)*t)) * DB1, W3*G) *
                  PAIRING(                       DB3, W1  ) * (DDc13^INTEGER(2)) ;
  TARGET right4 = PAIRING((W2^-t             ) * DB2, W2  ) * DDc22 ;
  TARGET right5 = PAIRING((W2^-(INTEGER(2)*t)) * DB2, W3*G) *
                  PAIRING(                       DB3, W2  ) * (DDc23^INTEGER(2)) ;
  TARGET right6 = PAIRING(((W3*G)^-t         ) * DB3, W3*G) * DDc33 ;

  return EXLIST(LIST_LITERAL,L,
    EQ(left1, right1, L), EQ(left2, right2, L), EQ(left3, right3, L),
    EQ(left4, right4, L), EQ(left5, right5, L), EQ(left6, right6, L)
  ) ;

}

static excell* GS_VERIFY2_QE_4_3_2  (excell* proof, cell* L) {
  excell * Opt       = proof->element(2) ;
  excell * crs       = Opt->element(3) ;
  excell * predicate = Opt->element(4) ;
  return GS_VERIFY_QE_4_3_2 (crs, predicate, proof, L);
}

static excell * GS_PROOFWI_AND (excell * crs, excell * predicate, cell * L) {
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(
    excell * arg = (excell*)(predicate->pointer(2));
    arg;
    arg=(excell*)arg->cdr
  ){
    excell * X = GS_PROOFWI (crs, (excell*)(arg->car), L) ;
    if(!X) return NULL ;
    R = (excell *)INTEGRATE(R,X) ;
  }
  return R ;
}

static excell * GS_PROOFWI (excell * crs, excell * predicate, cell * L){
  BINARY_LIST_PROCESS(crs,predicate,L,GS_PROOFWI) ;
  if(is_LOGICAL_AND(predicate)) return GS_PROOFWI_AND (crs, predicate, L) ;
  excell * already_proven = ALREADY_PROVEN(crs, predicate) ;
  if(already_proven) return already_proven ;
  switch(COMMIT_type(predicate)){
    case 1: return GS_PROOFWI_PPE_4_1_1 (crs, predicate, L); // 4.1.1
    case 2: return GS_PROOFWI_PPE_4_1_2 (crs, predicate, L); // 4.1.2
    case 3: return GS_PROOFWI_MSE_4_2_1 (crs, predicate, L); // 4.2.1
    case 4: return GS_PROOFWI_MSE_4_2_2 (crs, predicate, L); // 4.2.2
    case 5: return GS_PROOFWI_MSE_4_2_3 (crs, predicate, L); // 4.2.3
    case 6: return GS_PROOFWI_MSE_4_2_4 (crs, predicate, L); // 4.2.4
    case 7: return GS_PROOFWI_QE_4_3_1  (crs, predicate, L); // 4.3.1
    case 8: return GS_PROOFWI_QE_4_3_2  (crs, predicate, L); // 4.3.2
  }
  return NULL ;
}

static excell * GS_VERIFY2_AND (excell * proof, cell * L) {
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(
    excell * arg = (excell*)(proof->pointer(2));
    arg;
    arg=(excell*)arg->cdr
  ){
    excell * X = GS_VERIFY2((excell*)(arg->car), L) ;
    if(!X) return NULL ;
    R = (excell *)INTEGRATE(R,X) ;
  }
  return R ;
}

static excell * GS_VERIFY2 (excell * proof, cell * L){
  UNARY_LIST_PROCESS(proof,L,GS_VERIFY2) ;
  if(! is_RAW_PROOF(proof) && is_LIST_LITERAL(proof)) return GS_VERIFY2_AND (proof, L) ;
  if(! is_RAW_PROOF(proof)) return NULL ;
  excell * Opt       = proof->element(2) ;
  excell * predicate = Opt->element(4) ;
  switch(COMMIT_type(predicate)){
    case 1: return GS_VERIFY2_PPE_4_1_1 (proof, L); // 4.1.1
    case 2: return GS_VERIFY2_PPE_4_1_2 (proof, L); // 4.1.2
    case 3: return GS_VERIFY2_MSE_4_2_1 (proof, L); // 4.2.1
    case 4: return GS_VERIFY2_MSE_4_2_2 (proof, L); // 4.2.2
    case 5: return GS_VERIFY2_MSE_4_2_3 (proof, L); // 4.2.3
    case 6: return GS_VERIFY2_MSE_4_2_4 (proof, L); // 4.2.4
    case 7: return GS_VERIFY2_QE_4_3_1  (proof, L); // 4.3.1
    case 8: return GS_VERIFY2_QE_4_3_2  (proof, L); // 4.3.2
  }
  return NULL ;
}

static excell * GS_DUMMY_PROOFWI (excell * crs, excell * predicate, cell * L) ;

static excell * GS_DUMMY_PROOFWI_AND (excell * crs, excell * predicate, cell * L) {
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(
    excell * arg = (excell*)(predicate->pointer(2));
    arg;
    arg=(excell*)arg->cdr
  ){
    excell * X = GS_DUMMY_PROOFWI (crs, (excell*)(arg->car), L) ;
    if(!X) return NULL ;
    R = (excell *)INTEGRATE(R,X) ;
  }
  return R ;
}

static excell * GS_DUMMY_PROOFWI (excell * crs, excell * predicate, cell * L){
  BINARY_LIST_PROCESS(crs,predicate,L,GS_DUMMY_PROOFWI) ;
  if(is_LOGICAL_AND(predicate)) return GS_DUMMY_PROOFWI_AND (crs, predicate, L) ;
  excell * already_proven = ALREADY_PROVEN(crs, predicate) ;
  if(already_proven) return already_proven ;
  switch(COMMIT_type(predicate)){
    case 1: return GS_DUMMY_PROOFWI_PPE_4_1_1 (crs, predicate, L); // 4.1.1
    case 2: return GS_DUMMY_PROOFWI_PPE_4_1_2 (crs, predicate, L); // 4.1.2
    case 3: return GS_DUMMY_PROOFWI_MSE_4_2_1 (crs, predicate, L); // 4.2.1
    case 4: return GS_DUMMY_PROOFWI_MSE_4_2_2 (crs, predicate, L); // 4.2.2
    case 5: return GS_DUMMY_PROOFWI_MSE_4_2_3 (crs, predicate, L); // 4.2.3
    case 6: return GS_DUMMY_PROOFWI_MSE_4_2_4 (crs, predicate, L); // 4.2.4
    case 7: return GS_DUMMY_PROOFWI_QE_4_3_1  (crs, predicate, L); // 4.3.1
    case 8: return GS_DUMMY_PROOFWI_QE_4_3_2  (crs, predicate, L); // 4.3.2
  }
  return NULL ;
}

#if 0
static excell * GS_VERIFYWI_AND (excell * crs, excell * predicate, excell * proof, cell * L){
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(
    excell * arg = (excell*)(predicate->pointer(2)), * pro = (excell*)(proof->pointer(2));
    arg;
    arg=(excell*)arg->cdr, pro = (excell*)pro->cdr
  ){
    excell * X = GS_VERIFYWI(crs, (excell*)(arg->car), (excell*)(pro->car), L) ;
    R = (excell *)INTEGRATE(R,X) ;
  }
  return R ;
}
#endif

static excell * GS_DUMMY_SETUP(cell * L){
  GROUP   g     = GROUP(string(new_symbol("g"))) ;
  GROUP   u     = GROUP(string(new_symbol("u"))) ;
  GROUP   v     = GROUP(string(new_symbol("v"))) ;
  GROUP   w1    = GROUP(string(new_symbol("w1"))) ;
  GROUP   w2    = GROUP(string(new_symbol("w2"))) ;
  GROUP   w3    = GROUP(string(new_symbol("w3"))) ;

  excell * G    = EXLIST(GROUP, L, new GROUP(g)) ;
  excell * U    = EXLIST(GROUP, L, new GROUP(u)) ;
  excell * V    = EXLIST(GROUP, L, new GROUP(v)) ;
  excell * W1   = EXLIST(GROUP, L, new GROUP(w1)) ;
  excell * W2   = EXLIST(GROUP, L, new GROUP(w2)) ;
  excell * W3   = EXLIST(GROUP, L, new GROUP(w3)) ;
  excell * Opt  = OPTION_CRS(L);
  excell * Crs  = EXLIST(LIST_LITERAL,L,Opt,G,U,V,W1,W2,W3) ;
                  COMMIT_G(Crs,G) ;
  return Crs ;
}

static excell * GS_VERIFYWI (excell * crs, excell * predicate, excell * proof, cell * L){
  if(is_LIST_VARIABLE(proof) && (lastvalue(proof) == NULL) ){
    excell * dummy = GS_DUMMY_PROOFWI(crs, predicate, L) ;
    ASSIGN(dummy, proof, L) ;
    ASSIGN(proof, dummy, L) ;
    return GS_VERIFYWI (crs, predicate, dummy, L) ;
  }
  TERNARY_LIST_PROCESS(crs, predicate, proof,L, GS_VERIFYWI) ;
  return GS_VERIFY2(proof, L);
#if 0
  TERNARY_LIST_PROCESS(crs, predicate, proof,L, GS_VERIFYWI) ;
  if(is_LOGICAL_AND(predicate)) return GS_VERIFYWI_AND (crs, predicate, proof, L) ;
  switch(COMMIT_type(predicate)){
    case 1: return GS_VERIFY_PPE_4_1_1 (crs, predicate, proof, L); // 4.1.1
    case 2: return GS_VERIFY_PPE_4_1_2 (crs, predicate, proof, L); // 4.1.2
    case 3: return GS_VERIFY_MSE_4_2_1 (crs, predicate, proof, L); // 4.2.1
    case 4: return GS_VERIFY_MSE_4_2_2 (crs, predicate, proof, L); // 4.2.2
    case 5: return GS_VERIFY_MSE_4_2_3 (crs, predicate, proof, L); // 4.2.3
    case 6: return GS_VERIFY_MSE_4_2_4 (crs, predicate, proof, L); // 4.2.4
    case 7: return GS_VERIFY_QE_4_3_1  (crs, predicate, proof, L); // 4.3.1
    case 8: return GS_VERIFY_QE_4_3_2  (crs, predicate, proof, L); // 4.3.2
  }
  return NULL ;
#endif
}

// =====================================================================================
// GS_PROOFZK
// =====================================================================================

static excell * GS_PROOFZK_PPE_5_1   (excell * crs, excell * predicate, cell * L){
  vector<split_PPE>   Vec         = split_PPE_predicate(as_TARGET(predicate)) ;
  split_PPE         & T           = Vec.back();
  int               & commit_type = get<0>(T) ;
  TARGET            & Z           = get<4>(T) ;

  if(commit_type != 0) return GS_PROOFWI(crs, predicate, L) ;
  if(is_variable(Z)  ) return NULL ;

  GROUP             & G             = as_GROUP(crs->element(3)) ;
  TARGET              P                       ;
  excell            * new_predicate = EXLIST(LOGICAL_AND, L) ;

  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    GROUP   & Y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    TARGET  & Z           = get<4>(*i) ;
    switch(commit_type){
      case  1:
      case  2:
      {
        P *= PAIRING_RAW(X,Y)^c ;
      }
      break ;
      case  0:
      {
        for(auto j = Z.begin(); j != Z.end(); j++){
	  const TARGETVAR & T     = j->first  ;
	  const GROUP     * X     = &T.first  ;
	  const GROUP     * Y     = &T.second ;
	  const INTEGER   & c     = j->second ;
                GROUP     & g     = as_GROUP( crs->element(3) ) ;
                GROUP     & com_G = COMMIT_GROUP_G(crs, L) ;

          if(find_COMMIT(crs, *X)){ const GROUP * T = X ; X = Y ; Y = T ; }
          const GROUP & com_Y = COMMIT_GROUP_X(crs, (GROUP&)*Y, L) ;
          P *= PAIRING_RAW(*X, com_Y)^c ;

          if( (*X != g) && (*Y != g) ){
	    TARGET Q     = PAIRING_RAW(g, com_Y) * PAIRING_RAW((*Y)^-INTEGER(1), com_G) ;
            new_predicate  = (excell*)INTEGRATE(new_predicate, EXLIST(TARGET, L, new TARGET(Q))) ;
          }
        }
      }
      break ;
      default: break ; // impossible
    }
  }
  new_predicate = (excell*)INTEGRATE(new_predicate, EXLIST(TARGET, L, new TARGET(P))) ;
  return GS_PROOFWI(crs, new_predicate, L) ;
}

static excell * GS_PROOFZK_MSE_5_2   (excell * crs, excell * predicate, cell * L, int c_type){
  vector<split_MSE>   Vec         = split_MSE_predicate(as_GROUP(predicate)) ;
  split_MSE         & T           = Vec.back();
  int               & commit_type = get<0>(T) ;
  GROUP             & X           = get<1>(T) ;
  INTEGER           & y           = get<2>(T) ;
  INTEGER           & c           = get<3>(T) ;

  if(commit_type != 0) return GS_PROOFWI(crs, predicate, L) ; // 5.2.1

  GROUP               P                       ;
  excell            * new_predicate = (c_type == 3) ? EXLIST(LOGICAL_AND, L) : NULL ;

  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ; // 1
    GROUP   & X           = get<1>(*i) ; // [X]
    INTEGER & y           = get<2>(*i) ; // 1
    INTEGER & c           = get<3>(*i) ; // c
    switch(commit_type){
      case 3: // ([X]^[y])^c
      case 2: // X^[y]
      case 1: // [X]^c
      {
          P *= X^(y*c) ;
      }
      break ;
      case 0: // B
      {
        if(c_type == 3){ /* 4.2.1 GROUP MSEs */               // 5.2.2
          GROUP  & g     = as_GROUP( crs->element(3) ) ;
          GROUP  & com_G = COMMIT_GROUP_G(crs, L) ;
          GROUP  & com_X = COMMIT_GROUP_X(crs, X, L) ;
          TARGET   Q     = PAIRING_RAW(g, com_X) * PAIRING_RAW(X^-INTEGER(1), com_G) ;
          P *= com_X ;
          new_predicate = (excell*)INTEGRATE(new_predicate, EXLIST(TARGET, L, new TARGET(Q))) ;
        }else{                                                // 5.2.3
          INTEGER & com_1 = COMMIT_INTEGER_1(crs, L) ;
          P *= X^com_1 ;
        }
      }
      break ;
    }
  }
  if(c_type == 3) new_predicate = (excell*)INTEGRATE(new_predicate, EXLIST(GROUP, L, new GROUP(P))) ;
  else            new_predicate = EXLIST(GROUP, L, new GROUP(P)) ;
  return GS_PROOFWI(crs, new_predicate, L) ;
}

static excell * GS_PROOFZK_QE_5_3 (excell * crs, excell * predicate, cell * L){
  vector<split_QE> Vec = split_QE_predicate(as_INTEGER(predicate)) ;
  split_QE & T          = Vec.back();
  int     & commit_type = get<0>(T) ;
  INTEGER & c           = get<1>(T) ;
  INTEGER & y1          = get<2>(T) ;
  INTEGER & y2          = get<3>(T) ;
  INTEGER   P                       ;

  if(commit_type != 0) return GS_PROOFWI(crs, predicate, L) ; // 5.3.1

  for(auto i = Vec.begin() ; i != Vec.end() ; i++){
    int     & commit_type = get<0>(*i) ;
    INTEGER & c           = get<1>(*i) ;
    INTEGER & y1          = get<2>(*i) ;
    INTEGER & y2          = get<3>(*i) ;
    switch(commit_type){
      case  2: // c * [y1] * [y2]
      case  1: // c * [y1]
      {
        P += c * y1 * y2 ;
      }
      break ;
      case  0:
      {
        INTEGER & com_1 = COMMIT_INTEGER_1(crs, L) ;
        P += c * com_1 ;
      }
      break ; // c
      default: break ; // impossible
    }
  }
  excell * new_predicate = EXLIST(INTEGER, L, new INTEGER(P)) ;
  return GS_PROOFWI(crs, new_predicate, L) ;
}

static excell * GS_PROOFZK_AND (excell * crs, excell * predicate, cell * L) {
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(
    excell * arg = (excell*)(predicate->pointer(2));
    arg;
    arg=(excell*)arg->cdr
  ){
    excell * X = GS_PROOFZK (crs, (excell*)(arg->car), L) ;
    if(!X) return NULL ;
    R = (excell *)INTEGRATE(R,X) ;
  }
  return R ;
}

static excell * GS_PROOFZK (excell * crs, excell * predicate, cell * L){
  BINARY_LIST_PROCESS(crs,predicate,L,GS_PROOFZK) ;
  if(is_LOGICAL_AND(predicate)) return GS_PROOFZK_AND (crs, predicate, L) ;
  excell * already_proven = ALREADY_PROVEN_ZK(crs, predicate) ;
  if(already_proven) return already_proven ;
  int c_type = COMMIT_type(predicate) ;
  switch(c_type){
    case 1: 
    case 2: return GS_PROOFZK_PPE_5_1   (crs, predicate, L); // 5.1
    break ;
    case 3: 
    case 4: 
    case 5: 
    case 6: return GS_PROOFZK_MSE_5_2   (crs, predicate, L, c_type); // 5.2
    break ;
    case 7: 
    case 8: return GS_PROOFZK_QE_5_3    (crs, predicate, L); // 5.3
    break ;
  }
  return NULL ;
}

// ======================================================================================
// VERIFY
// ======================================================================================

static excell * GS_DUMMY_PROOFZK (excell * crs, excell * predicate, cell * L) ;

static excell * GS_DUMMY_PROOFZK_AND (excell * crs, excell * predicate, cell * L) {
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(
    excell * arg = (excell*)(predicate->pointer(2));
    arg;
    arg=(excell*)arg->cdr
  ){
    excell * X = GS_DUMMY_PROOFZK (crs, (excell*)(arg->car), L) ;
    if(!X) return NULL ;
    R = (excell *)INTEGRATE(R,X) ;
  }
  return R ;
}

static excell * GS_DUMMY_PROOFZK_PPE_5_1   (excell * crs, excell * predicate, cell * L){
  vector<split_PPE>   Vec         = split_PPE_predicate(as_TARGET(predicate)) ;
  split_PPE         & T           = Vec.back();
  int               & commit_type = get<0>(T) ;
  TARGET            & Z           = get<4>(T) ;

  if(commit_type != 0) return GS_DUMMY_PROOFWI(crs, predicate, L) ;
  if(is_variable(Z)  ) return NULL ;

  GROUP             & G             = as_GROUP(crs->element(3)) ;
  TARGET              P                       ;
  excell            * new_predicate = EXLIST(LOGICAL_AND, L) ;

  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ;
    GROUP   & X           = get<1>(*i) ;
    GROUP   & Y           = get<2>(*i) ;
    INTEGER & c           = get<3>(*i) ;
    TARGET  & Z           = get<4>(*i) ;
    switch(commit_type){
      case  1:
      case  2:
      {
        P *= PAIRING_RAW(X,Y)^c ;
      }
      break ;
      case  0:
      {
        for(auto j = Z.begin(); j != Z.end(); j++){
	  const TARGETVAR & T     = j->first  ;
	  const GROUP     * X     = &T.first  ;
	  const GROUP     * Y     = &T.second ;
	  const INTEGER   & c     = j->second ;
                GROUP     & g     = as_GROUP( crs->element(3) ) ;
                GROUP     & com_G = COMMIT_GROUP_G(crs, L) ;
          if(find_COMMIT(crs, *X)){ const GROUP * T = X ; X = Y ; Y = T ; }
          const GROUP & com_Y = COMMIT_GROUP_X(crs, (GROUP&)*Y, L) ;
          P *= PAIRING_RAW(*X, com_Y)^c ;
          if( (*X != g) && (*Y != g) ){
	    TARGET Q     = PAIRING_RAW(g, com_Y) * PAIRING_RAW((*Y)^-INTEGER(1), com_G) ;
            new_predicate  = (excell*)INTEGRATE(new_predicate, EXLIST(TARGET, L, new TARGET(Q))) ;
          }
        }
      }
      break ;
      default: break ; // impossible
    }
  }
  new_predicate = (excell*)INTEGRATE(new_predicate, EXLIST(TARGET, L, new TARGET(P))) ;
  return GS_DUMMY_PROOFWI(crs, new_predicate, L) ;
}

static excell * GS_DUMMY_PROOFZK_MSE_5_2   (excell * crs, excell * predicate, cell * L, int c_type){
  vector<split_MSE>   Vec         = split_MSE_predicate(as_GROUP(predicate)) ;
  split_MSE         & T           = Vec.back();
  int               & commit_type = get<0>(T) ;
  GROUP             & X           = get<1>(T) ;
  INTEGER           & y           = get<2>(T) ;
  INTEGER           & c           = get<3>(T) ;

  if(commit_type != 0) return GS_DUMMY_PROOFWI(crs, predicate, L) ; // 5.2.1

  GROUP               P                       ;
  excell            * new_predicate = (c_type == 3) ? EXLIST(LOGICAL_AND, L) : NULL ;

  for(auto i = Vec.begin(); i != Vec.end(); i++){
    int     & commit_type = get<0>(*i) ; // 1
    GROUP   & X           = get<1>(*i) ; // [X]
    INTEGER & y           = get<2>(*i) ; // 1
    INTEGER & c           = get<3>(*i) ; // c
    switch(commit_type){
      case 3: // ([X]^[y])^c
      case 2: // X^[y]
      case 1: // [X]^c
      {
          P *= X^(y*c) ;
      }
      break ;
      case 0: // B
      {
        if(c_type == 3){ /* 4.2.1 GROUP MSEs */               // 5.2.2
          GROUP  & g     = as_GROUP( crs->element(3) ) ;
          GROUP  & com_G = COMMIT_GROUP_G(crs, L) ;
          GROUP  & com_X = COMMIT_GROUP_X(crs, X, L) ;
          TARGET   Q     = PAIRING_RAW(g, com_X) * PAIRING_RAW(X^-INTEGER(1), com_G) ;
          P *= com_X ;
          new_predicate = (excell*)INTEGRATE(new_predicate, EXLIST(TARGET, L, new TARGET(Q))) ;
        }else{                                                // 5.2.3
          INTEGER & com_1 = COMMIT_INTEGER_1(crs, L) ;
          P *= X^com_1 ;
        }
      }
      break ;
    }
  }
  if(c_type == 3) new_predicate = (excell*)INTEGRATE(new_predicate, EXLIST(GROUP, L, new GROUP(P))) ;
  else            new_predicate = EXLIST(GROUP, L, new GROUP(P)) ;
  return GS_DUMMY_PROOFWI(crs, new_predicate, L) ;
}

static excell * GS_DUMMY_PROOFZK_QE_5_3 (excell * crs, excell * predicate, cell * L){
  vector<split_QE> Vec = split_QE_predicate(as_INTEGER(predicate)) ;
  split_QE & T          = Vec.back();
  int     & commit_type = get<0>(T) ;
  INTEGER & c           = get<1>(T) ;
  INTEGER & y1          = get<2>(T) ;
  INTEGER & y2          = get<3>(T) ;
  INTEGER   P                       ;

  if(commit_type != 0) return GS_DUMMY_PROOFWI(crs, predicate, L) ; // 5.3.1

  for(auto i = Vec.begin() ; i != Vec.end() ; i++){
    int     & commit_type = get<0>(*i) ;
    INTEGER & c           = get<1>(*i) ;
    INTEGER & y1          = get<2>(*i) ;
    INTEGER & y2          = get<3>(*i) ;
    switch(commit_type){
      case  2: // c * [y1] * [y2]
      case  1: // c * [y1]
      {
        P += c * y1 * y2 ;
      }
      break ;
      case  0:
      {
        INTEGER & com_1 = COMMIT_INTEGER_1(crs, L) ;
        P += c * com_1 ;
      }
      break ; // c
      default: break ; // impossible
    }
  }
  excell * new_predicate = EXLIST(INTEGER, L, new INTEGER(P)) ;
  return GS_DUMMY_PROOFWI(crs, new_predicate, L) ;
}

static excell * GS_DUMMY_PROOFZK (excell * crs, excell * predicate, cell * L){
  BINARY_LIST_PROCESS(crs,predicate,L,GS_DUMMY_PROOFZK) ;
  if(is_LOGICAL_AND(predicate)) return GS_DUMMY_PROOFZK_AND (crs, predicate, L) ;
  int c_type = COMMIT_type(predicate) ;
  switch(c_type){
    case 1: 
    case 2: return GS_DUMMY_PROOFZK_PPE_5_1   (crs, predicate, L); // 5.1
    break ;
    case 3: 
    case 4: 
    case 5: 
    case 6: return GS_DUMMY_PROOFZK_MSE_5_2   (crs, predicate, L, c_type); // 5.2
    break ;
    case 7: 
    case 8: return GS_DUMMY_PROOFZK_QE_5_3    (crs, predicate, L); // 5.3
    break ;
  }
  return NULL ;
}

#if 0
static excell * GS_VERIFYZK_AND (excell * crs, excell * predicate, excell * proof, cell * L){
  excell * R = EXLIST(LIST_LITERAL, L) ;
  for(
    excell * arg = (excell*)(predicate->pointer(2)), * pro = (excell*)(proof->pointer(2));
    arg;
    arg=(excell*)arg->cdr, pro = (excell*)pro->cdr
  ){
    excell * X = GS_VERIFYZK(crs, (excell*)(arg->car), (excell*)(pro->car), L) ;
    R = (excell *)INTEGRATE(R,X) ;
  }
  return R ;
}
#endif

excell * GENERATE_CRS_if_listvar (excell * crs, cell * L){
  if(is_LIST_VARIABLE(crs) && (lastvalue(crs) == NULL) ){
    excell * dummy = GS_DUMMY_SETUP(L) ;
    ASSIGN(dummy, crs, L) ;
    ASSIGN(crs, dummy, L) ;
    return dummy ;
  }else return crs ;
}

static excell * GS_VERIFYZK (excell * crs, excell * predicate, excell * proof, cell * L){
  if(is_LIST_VARIABLE(proof) && (lastvalue(proof) == NULL) ){
    excell * dummy = GS_DUMMY_PROOFZK(crs, predicate, L) ;
    ASSIGN(dummy, proof, L) ;
    ASSIGN(proof, dummy, L) ;
    return GS_VERIFYZK (crs, predicate, dummy, L) ;
  }
  TERNARY_LIST_PROCESS(crs, predicate, proof,L, GS_VERIFYZK) ;
  return GS_VERIFY2 (proof, L) ;
}

// ================================================================================
// DATA FORMAT
// ================================================================================

// -------------------------------------
// CRS
// -------------------------------------
//        0            1 2   3 4 5 6  7  8
// EXLIST(LIST_LITERAL,L,Opt,G,U,V,W1,W2,W3) ;
// -------------------------------------
//        0       1  2        3             4              5                 6
// EXLIST(OPTION, L, OPT_CRS, commit_stack, proof_history, proof_history_zk, true_name)
// -------------------------------------

// -------------------------------------
// COMMIT
// -------------------------------------
//        0            1 2   3  4  5
// EXLIST(LIST_LITERAL,L,Opt,C1,C2,C3) ;
// -------------------------------------
//        0       1  2           3                   4    5  6   7   8
// EXLIST(OPTION, L, OPT_COMMIT, OPT_GROUP_COMMIT  , crs, X, S1, S2, S3) ;
// EXLIST(OPTION, L, OPT_COMMIT, OPT_INTEGER_COMMIT, crs, X, R1, R2) ;
// -------------------------------------

// -------------------------------------
// PROOF_x_y_z
// -------------------------------------
//        0            1 2   3    4    5
// EXLIST(LIST_LITERAL,L,Opt,PHI1,PHI2,PHI3,...,Vars) ;
// -------------------------------------
//        0       1  2                3    4
// EXLIST(OPTION, L, OPT_PROOF_x_y_z, crs, predicate) ;
// -------------------------------------
//
// ------------------------------------------
// Major TYPE
// ------------------------------------------
// typedef boost::multiprecision::cpp_int                     integer_  ;
// typedef Polynomial       <std::string, integer_, integer_> INTEGER   ;
// typedef Monomial         <std::string, INTEGER           > GROUP     ;
// typedef SymmetricPairing <GROUP                          > TARGETVAR ;
// typedef Monomial         <TARGETVAR, INTEGER             > TARGET    ;

// ------------------------------------------
// split_PPE
// ------------------------------------------
// split_PPE(2, [X]    , [Y]    , c         , TARGET())) ; // e([X],[Y])^c
// split_PPE(1, [X]    , B      , INTEGER(1), TARGET())) ; // e(B,[X])
// split_PPE(0, GROUP(), GROUP(), INTEGER() , T       )) ; // T
// ------------------------------------------
// split_MSE
// ------------------------------------------
// split_MSE(3, [X], [y]       , c         )) ; // ([X]^[y])^c
// split_MSE(2, X  , [y]       , INTEGER(1))) ; // (X^[y])
// split_MSE(1, [X], INTEGER(1), c         )) ; // ([X]^c)
// split_MSE(0, X  , INTEGER(1), INTEGER(1))) ; // X
// ------------------------------------------
// split_QE
// ------------------------------------------
// split_QE(2, c, x         , y         )) ; // c * [x] * [y]
// split_QE(1, c, x         , INTEGER(1))) ; // c * [x]
// split_QE(0, c, INTEGER(1), INTEGER(1))) ; // c
// ------------------------------------------
