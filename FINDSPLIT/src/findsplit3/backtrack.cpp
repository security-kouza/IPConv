#include "backtrack.h"
#include "mysignal.h"

namespace findsplit {

  using namespace std   ;
  using namespace boost ;

  static void BackTrackRecursive(backtrack &,int,int);

  static void BackTrackRecursiveWithPruningTest(
    backtrack & g,
    int k, // index of variable to assign
    int b, // assignment value
    int d, // index of dependent
    int i  // index of independent
  ){
    score_t score = g.prefetch_score(k,b) ;
    if(score < g.minimum){
      state_t state = g.save_state(k) ;
      g.increment_score(k,b) ;
      BackTrackRecursive(g,d,i);
      g.load_state(state,k) ;
    }
  }

  inline static void BackTrackRecursiveWithPruningTestWithCount(
    backtrack & g,
    int k, // index of variable to assign
    int b, // assignment value
    int d, // index of dependent
    int i  // index of independent
  ){
    if(((g.maxcount>=0)&&(g.count>=g.maxcount))||exit_by_signal){
      if(g.progress_rate < 0)
        g.progress_rate = g._progress_rate_backtrack(g.current);
      return ;
    }
    g.count++ ;
    BackTrackRecursiveWithPruningTest(g,k,b,d,i);
  }

  static void BackTrackRecursive(
    backtrack & g,
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
        int k = g.independent[i] ;
        BackTrackRecursiveWithPruningTestWithCount(g,k,0, d,i-1);
        BackTrackRecursiveWithPruningTestWithCount(g,k,1, d,i-1);
      }else{
        /*
        ((d >=0) && (i < 0))||
        ((d >=0) && (i >=0) && (g.dependent[d] >= g.independent[i]))
        */
        int k = g.dependent[d] ;
        bool b = g.solve_dependent(d) ;
        BackTrackRecursiveWithPruningTest(g,k,b, d-1,i);
      }
    }
  }

  void backtrack::BackTrack(integer_t limit_){
    backtrack & g = *this ;
    g.BeginOptimize(limit_) ;
    BackTrackRecursive(g,g.num_dependent()-1,g.num_independent()-1) ;
    g.EndOptimize() ;
    g.score = g.minimum ;
  }

  ostream & operator<<(ostream& out, backtrack & g) {
    if(g.format != tango){
      out << "/*" << endl ;
      out << "== backtrack ==" << endl ;
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

// ====================================================================
// main
// ====================================================================

#ifdef BACKTRACK_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  backtrack g ;
  g.SetRatio(2.0) ;
  g.BackTrack(22) ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
