#include "driver.h"

namespace findsplit {
  using namespace std   ;
  using namespace boost ;

// ============================================================
// Check
// ============================================================

  void driver::Check(integer_t limit_) {
    driver & g = *this ;
    g.check_limit = limit_ ;

    if(limit_ >= 0){
      result_t result = g.save_result() ;
      g.CoinFlipping(limit_, be_quiet) ;
      g.load_result(result) ;
  
      g.Assign(g.minimum.assignment) ; // restore
  
      if( ((montecarlo &)g).score < g.minimum) {
        warnning("check result indicates a failure of optimization") ;
        g.status     = cannot_optimize ;
        g.is_optimal = false ;
      }
    }
  }

// ============================================================
// to_G0, to_G1
// ============================================================

  pairing_graph & driver::to_G(int i, const char * title){
    if(! is_generated[i]){
      is_generated[i] = true ;
  
      driver & g = (*this) ;
      graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
      graph_traits<pairing_graph>::adjacency_iterator ai, ai_end ;
      boost::unordered_map<vertex_t, vertex_t> gv_to_g0v ;
    
      G[i].graph_title = title ;
      G[i].format = g.format ;
      G[i].number_to_vertex.resize(g.num_nodes) ;
      G[i].show_constraint = g.show_constraint ;
    
      for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
        if(g[*vi].is_G[i]){
          vertex_t v0 = add_vertex(g[*vi],G[i]) ;
          gv_to_g0v[*vi] = v0 ;
        }
      }
      for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
        if(g[*vi].is_G[i]){
          vertex_t v0s = gv_to_g0v[*vi] ;
          for(tie(ai,ai_end)=adjacent_vertices(*vi,g);ai!=ai_end;ai++){
            if(g[*ai].is_G[i]){
              vertex_t v0d = gv_to_g0v[*ai] ;
              add_edge(v0s, v0d, G[i]) ;
            }
          }
        }
      }
      for(tie(vi, vi_end) = vertices(G[i]); vi != vi_end; vi++){
        G[i].symbol_to_number[ G[i][*vi].symbol ] = G[i][*vi].number ;
        G[i].number_to_vertex[ G[i][*vi].number ] = *vi ;
        G[i].num_nodes++ ;
      }
    }
    return G[i] ;
  }

// ============================================================
// print
// ============================================================
  
  ostream & operator<<(ostream& out, driver & g){
    out << (optimizer  &) g ;
    out << (naive      &) g ;
    out << (crypto2014 &) g ;
    out << (bruteforce &) g ;
    out << (backtrack  &) g ;
    out << (montecarlo &) g ;
    out << (linear     &) g ;
    if(g.format != tango){
      out << "/*" << endl ;
      out << "== driver ==" << endl ;
      if(g.is_split){
        out << "-- original result --" << endl ;
        out << g.minimum ;
        out << "-- check result --" << endl ;
        out << ((montecarlo&)g).score ;
        out << "-- num_check --" << endl ;
        out << "check_limit=" << g.check_limit << endl ;
      }
      out << "*/" << endl ;
    }
    return out ;
  }

}

#ifdef DRIVER_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  driver g ;
  g.SetRatio(2.0) ;

  string argn = argc < 2 ? "3" : argv[1] ;
  /**/ if(argn == "1") g.LoadSolution         ("G.sol") ;
  else if(argn == "2") g.CoinFlipping         (     22) ;
  else if(argn == "3") g.NaiveBruteForce      (     22) ;
  else if(argn == "4") g.BackTrack            (     22) ;
  else if(argn == "5") g.BruteForce           (     22) ;
  else if(argn == "6") g.BruteForceCrypto2014 (     22) ;
  else {               g.IntegerPrograming    (   cout) ;
                       return EXIT_SUCCESS ;
  }

  cout << g ;
  cout << g.to_G0() ;
  cout << g.to_G1() ;
  return EXIT_SUCCESS ;
}

#endif
