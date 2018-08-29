#include "naive.h"
#include "mysignal.h"

namespace findsplit {

  using namespace std   ;
  using namespace boost ;

  static void NaiveBruteForceRecursive(naive & g, int n){
    if(((g.maxcount>=0)&&(g.count>=g.maxcount))||exit_by_signal){
      if(g.progress_rate < 0) g.progress_rate = g._progress_rate() ;
      return ;
    }
    g.count++ ;
    if(n < g.num_independent() ){
      g.current.assignment[ g.independent[n] ] = 0 ;
      NaiveBruteForceRecursive(g, n+1);
      g.current.assignment[ g.independent[n] ] = 1 ;
      NaiveBruteForceRecursive(g, n+1);
    }else{
      g.Assign() ;
      g._OptimizeOne();
    }
  }

  void naive::NaiveBruteForce(integer_t limit_) {
    naive & g = *this ;
    g.BeginOptimize(limit_) ;
    NaiveBruteForceRecursive(g,0) ;
    g.EndOptimize() ;
    g.score = g.minimum ;
  }

  ostream & operator<<(ostream& out, naive & g) {
    if(g.format != tango){
      out << "/*" << endl ;
      out << "== naive ==" << endl ;
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

#ifdef NAIVE_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  naive g ;
  g.SetRatio(2.0) ;
  g.NaiveBruteForce(22) ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
