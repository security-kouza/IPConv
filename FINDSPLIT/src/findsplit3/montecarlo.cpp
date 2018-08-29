#include "montecarlo.h"
#include "mysignal.h"
#include <math.h>

namespace findsplit {
using namespace std   ;
using namespace boost ;

struct random_bit_t { // same as nrand48()
  const uinteger_t a      = 0x5DEECE66D ;
  const uinteger_t c      = 0xB ;
        uinteger_t seed   = 0xCB0D7033330E ; // srand48(0xCB0D7033)
        uinteger_t buffer = 1 ;
  inline bool generator(){
    if(buffer == 1){
      seed   = (a * seed + c) & 0xFFFFFFFFFFFFULL ;
      buffer = seed | 0x1000000000000ULL;
    }
    bool ret = buffer & 1 ;
    buffer >>= 1 ;
    return ret ;
  }
} ;

void CoinTossing(montecarlo & g){
  assignvector_t random_assignment = assignvector_t(g);
  random_bit_t   random_bit ;
  const integer_t & n = g.maxcount ; 
        integer_t & i = g.count ; 

  if(g.num_independent()==0){
    g.OptimizeOne(random_assignment) ;
  }else{
    for(i=0;(n<=0)||(i<n);){
      if(exit_by_signal){
        g.progress_rate = g._progress_rate() ;
        return ;
      }
      for(integer_t j = 0; j < g.num_independent() ; j++, i++){
        random_assignment[ g.independent[j] ] = random_bit.generator() ;
      }
      g.OptimizeOne(random_assignment) ;
    }
  }
}

void montecarlo::CoinFlipping(integer_t limit_, bool omit_warnings){
  montecarlo & g = *this ;
  g.BeginOptimize(limit_) ;
  CoinTossing(g) ;
  g.EndOptimize(omit_warnings) ;
  g.score = g.minimum ;
}

ostream & operator<<(ostream& out, montecarlo & g) {
  if(g.format != tango){
    out << "/*" << endl ;
    out << "== montecarlo ==" << endl ;
    if(g.is_split){
      out << g.score ;
    }else{
      out << "fail to split." << endl;
    }
    out << "*/" << endl ;
  }
  return out ;
}

}

#ifdef MONTECARLO_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  montecarlo g ;
  g.SetRatio(2.0) ;
  g.CoinFlipping(100000) ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
