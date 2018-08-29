#ifndef DECIDE_H
#define DECIDE_H
#include "semantic.h"

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct decide_base {
    list_and_vector_t prohibited     ;
    list_and_vector_t pairing        ;
    list_and_vector_t bottom         ;
    list_and_vector_t variable       ;
  
    list_and_vector_t dependent      ;
    list_and_vector_t independent    ;
  
    int critical_pairing_start = -1  ;
    vector<bitvector_t> relation     ;
    vector<bitvector_t> echelon_form ;
  } ;
  
  struct decide :
    semantic_analyzer,
    decide_base
  {
    real_t interval_to_decide = 0 ;
    real_t interval_to_gauss  = 0 ;
    decide() ;
  // ===
    inline const int num_prohibited() const { return prohibited.size() ; }
    inline const int num_bottom() const { return bottom.size() ; }
    inline const int num_variables() const { return variable.size() ; }
    inline const int num_dependent() const { return dependent.size() ; }
    inline const int num_independent() const {return independent.size();}
    inline const int dimension() const { return num_variables() + 1 ; }
    inline const int constant_term() const { return num_variables() ; }
    inline const bool is_inconsistent() const {
      return (dependent.size() == 0) ? false :
             (dependent.back() == constant_term()) ;
    }
    inline const bool is_consistent() const {return ! is_inconsistent();}
  
  } ;
  
  extern ostream & operator<<(ostream& out,const decide & g);
  
  inline static void fatal_error(const string & message){
    cerr << "fatal error: " << message << "." << endl ;
    exit(1);
  }
  
  inline static void warnning(const string & message){
    cerr << "warnning: " << message << "." << endl ;
  }
  
  inline static void unexpected_error(
    const string & LOCATION,
    const string & description = ""
  ){
    cerr << "unexpected error in " << LOCATION << "()"
         << ((description == "")? "" : ": ") << description
         << ((description == "")? "" : ".") << endl ;
    exit(1);
  }

}

#endif /* DECIDE_H */
