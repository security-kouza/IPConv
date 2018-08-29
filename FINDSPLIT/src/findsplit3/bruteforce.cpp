#include "bruteforce.h"
#include "mysignal.h"

namespace findsplit {

  using namespace std   ;
  using namespace boost ;

  static void BruteForceRecursive(bruteforce &,int,int);

  static void BruteForceRecursiveWithoutPruningTest(
    bruteforce & g,
    int k, // index of variable to assign
    int b, // assignment value
    int d, // index of dependent
    int i  // index of independent
  ){
    state_t state = g.save_state(k) ;
    g.increment_score(k,b) ;
    BruteForceRecursive(g,d,i);
    g.load_state(state,k) ;
  }

  inline static void BruteForceRecursiveWithoutPruningTestWithCount(
    bruteforce & g,
    int k, // index of variable to assign
    int b, // assignment value
    int d, // index of dependent
    int i  // index of independent
  ){
    if(((g.maxcount>=0)&&(g.count>=g.maxcount))||exit_by_signal){
      if(g.progress_rate < 0) g.progress_rate = g._progress_rate();
      return ;
    }
    g.count++ ;
    BruteForceRecursiveWithoutPruningTest(g,k,b,d,i);
  }

  static void BruteForceRecursive(
    bruteforce & g,
    int d, // current   index of dependent
    int i  // current   index of independent
  ){
    if((d < 0) && (i < 0)){
      g._OptimizeOne();
    }else{
      if(
        ((d < 0) && (i >=0))||
        ((d >=0) && (i >=0) && (g.dependent[d] < g.independent[i]))
      ){
        const int k = g.independent[i] ;
        BruteForceRecursiveWithoutPruningTestWithCount(g,k,0, d,i-1);
        BruteForceRecursiveWithoutPruningTestWithCount(g,k,1, d,i-1);
      }else{
        /*
        ((d >=0) && (i < 0))||
        ((d >=0) && (i >=0) && (g.dependent[d] >= g.independent[i]))
        */
        const int k = g.dependent[d] ;
        bool b = g.solve_dependent(d) ;
        BruteForceRecursiveWithoutPruningTest(g,k,b, d-1,i);
      }
    }
  }

  void bruteforce::BruteForce(integer_t limit_) {
    bruteforce & g = *this ;
    g.BeginOptimize(limit_) ;
    BruteForceRecursive(g,g.num_dependent()-1,g.num_independent()-1) ;
    g.EndOptimize();
    g.score = g.minimum ;
  }

  ostream & operator<<(ostream& out, bruteforce & g) {
    if(g.format != tango){
      out << "/*" << endl ;
      out << "== bruteforce ==" << endl ;
      if(g.is_split){
        out << "maxcount=" << g.maxcount << endl ;
        out << g.score ;
      }else{
        out << "fail to split." << endl ;
      }
      out << "*/" << endl ;
    }
    return out ;
  }

}

// ====================================================================
// main
// ====================================================================

#ifdef BRUTEFORCE_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  bruteforce g ;
  g.SetRatio(2.0) ;
  g.BruteForce(22) ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
