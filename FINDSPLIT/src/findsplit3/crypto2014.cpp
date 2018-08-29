#include "crypto2014.h"
#include "mysignal.h"

namespace findsplit {

  using namespace std   ;
  using namespace boost ;

/*
  inline static void indicator(ostream & out,const int n,const string & s){
    for(int i=0;i<n;i++)cout << " " ;
    cout << s << endl ;
    cout << "n = " << n << endl ;
  }
*/

  static void BruteForceCrypto2014Recursive(crypto2014 & g, int n){
    if(((g.maxcount>=0)&&(g.count>=g.maxcount))||exit_by_signal){
      if(g.progress_rate<0)g.progress_rate=g._progress_rate_crypto2014();
      return ;
    }
    g.count++ ;
    if(n < g.num_variables() ){
      vertex_t v = g.number_to_vertex[ g.variable[n] ] ;
      if(g[v].is_pairing){
        g.current.assignment[n  ] = 0 ;
        g.current.assignment[n+1] = 1 ;

//        bit_print(cout, g.current.assignment) ; indicator(cout,n,"^^") ;

        BruteForceCrypto2014Recursive(g, n+2);
        g.current.assignment[n  ] = 1 ;
        g.current.assignment[n+1] = 0 ;

//        bit_print(cout, g.current.assignment) ; indicator(cout,n,"^^") ;

        BruteForceCrypto2014Recursive(g, n+2);
      }else{
        g.current.assignment[n  ] = 0 ;

//        bit_print(cout, g.current.assignment) ; indicator(cout,n,"^") ;

        BruteForceCrypto2014Recursive(g, n+1);
        g.current.assignment[n  ] = 1 ;

//        bit_print(cout, g.current.assignment) ; indicator(cout,n,"^") ;

        BruteForceCrypto2014Recursive(g, n+1);
      }
    }else{
      if(g.AssignCrypto2014() == success_to_assign){
        g._OptimizeOne();
        g.status   = cannot_optimize ;
        g.is_split = true ;
      }
    }
  }

  void crypto2014::BruteForceCrypto2014(integer_t limit_) {
    crypto2014 & g = *this ;
    g.BeginOptimizeCrypto2014(limit_) ;
    BruteForceCrypto2014Recursive(g,0) ;
    g.EndOptimizeCrypto2014() ;
    g.score = g.minimum ;
  }

  ostream & operator<<(ostream& out, crypto2014 & g) {
    if(g.format != tango){
      out << "/*" << endl ;
      out << "== crypto2014 ==" << endl ;
      if(g.is_split){
        out << g.score ;
      }else{
        out << "fail to split." << endl ;
      }
      out << "*/" << endl ;
    }
    return out ;
  }

}

#ifdef CRYPTO2014_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  crypto2014 g ;
  g.SetRatio(2.0) ;
  g.BruteForceCrypto2014(22) ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
