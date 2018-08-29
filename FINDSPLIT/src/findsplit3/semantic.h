#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "pairing_graph.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct semantic_analyzer : pairing_graph {
    real_t interval_to_load = 0 ;
    semantic_analyzer() ;
  } ;

  extern ostream & operator<<(ostream& out, const semantic_analyzer & g) ;

#ifdef __CYGWIN__
#include <cstdlib>
  static inline int stoi(const string& s){
    return strtol(s.c_str(),0,10);
  }

  static inline long double strtold(
    const char * nptr,
          char ** endptr
  ){
    return (long double)strtod(nptr, endptr);
  }

  static inline long double stold (const string& s){
    return strtold(s.c_str(),0);
  }

  static inline long double stoll (const string& s){
    return strtoll(s.c_str(),0,10);
  }

  static inline long long int llroundl (long double x){
    return llround((double)x) ;
  }
#endif
}

#endif /* SEMANTIC_H */
