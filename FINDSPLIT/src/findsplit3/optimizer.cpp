#include "optimizer.h"
#include "mysignal.h"

namespace findsplit {

  using namespace std   ;
  using namespace boost ;

// ============================================================
// SetRatio
// ============================================================

  void optimizer::SetRatio(real_t ratio_){
    optimizer & g    = *this ;
    g.ratio          = ratio_ ;
    g.ratio_min      = (ratio_ < 1) ? ratio_ : 1 ;
    g.ratio_G0       = 1      - g.ratio_min ;
    g.ratio_G1       = ratio_ - g.ratio_min ;

    g.digit_vector   = realvector_t(num_priorities,1) ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].priority){
        if(g[vi].is_explicit()){
          g.digit_vector[g[vi].priority-1] += g[vi].weight * (1+g.ratio) ;
        }
      }
    }
    for(int i=g.num_priorities-2;i>=0;i--)
      g.digit_vector[i] *= g.digit_vector[i+1] ;
  }


// ============================================================
// Assign
// ============================================================
  
  static void clear_all_nodes(optimizer & g){
    for(int i=0;i<g.num_nodes;i++){
      vertex_t v = g.number_to_vertex[i] ;
      g[v].is_G[0] = false ;
      g[v].is_G[1] = false ;
    }
  }
  
  static void solve_assignment(optimizer & g){
    bitvector_t & value = g.current.assignment ;
    for(int i=g.num_dependent()-1;i>=0;i--){
      int j=g.dependent[i] ;
      bitvector_t mask = g.echelon_form[i] ;
      mask[j] = 0 ;
      mask &= value ;
      value[j] = mask.count() & 1 ;
    }
  }

  static int _assign_all_nodes(optimizer & g){
    for(int i=0;i<g.num_variables();i++){
      bool value = g.current.assignment[i] ;
      int j = g.variable[i] ;
      vertex_t vj = g.number_to_vertex[j] ;
      const intvector_t & anc = g[vj].ancestor.list ;
      for(int k=0;k<anc.size();k++){
        vertex_t v = g.number_to_vertex[ anc[k] ] ;
        if(g[v].is_normal()){
          g[v].is_G[value] = true  ;
        }else{
          if(g[v].is_not_visited()){
            g[v].is_G[value] = true  ;
            g[v].value       = value ;
          }else{
	    if(g[v].value != value){
	      return inconsistent_assignment ;
	    }
          }
        }
      }
    }
    for(int i=0;i<g.num_nodes;i++){
      vertex_t v = g.number_to_vertex[i] ;
      if(g[v].is_not_visited()){
        return exists_unvisited_node ;
      }
    }
    return success_to_assign ;
  }

  inline static void assign_all_nodes(optimizer & g){
    int result_code = _assign_all_nodes(g) ;
    if(result_code == inconsistent_assignment)
      unexpected_error("assign_all_nodes", "inconsistent assignment") ;
    else if(result_code == exists_unvisited_node)
      unexpected_error("assign_all_nodes", "there exists unvisited node") ;
  }

  static void evaluate_score(optimizer & g){
    realvector_t & score = g.current.score_vector ;
    for(int i=0;i<g.num_priorities;i++)score[i]=0;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t v = g.number_to_vertex[i] ;
      if(!(g[v].is_pairing||g[v].is_constraint)){
        score[ g[v].priority ] +=
          g[v].weight * (g[v].is_G[0] + g.ratio * g[v].is_G[1]) ;
//	  if(g[v].is_G[0]) cerr << g[v].symbol << ".is_G0" << endl ;
//	  if(g[v].is_G[1]) cerr << g[v].symbol << ".is_G1" << endl ;
      }
    }
  }

  inline static void huge_score(optimizer & g){
    g.current.score_vector = realvector_t(g.num_priorities, huge_real) ;
  }

  void optimizer::Assign(){
    optimizer & g = *this ;
    clear_all_nodes(g);
    bitvector_t b = g.current.assignment ;
//    cerr << "before solve = " << b << endl ;
    solve_assignment(g);
    bitvector_t a = g.current.assignment ;
//    cerr << "after  solve = " << a << endl ;
//    cerr << "a ^ b        = " << (a ^ b) << endl ;
    assign_all_nodes(g);
    evaluate_score(g);
  }

  int optimizer::AssignCrypto2014(){
    optimizer & g = *this ;
    clear_all_nodes(g);
    int r = _assign_all_nodes(g);
    if(r)huge_score(g);
    else evaluate_score(g);
    return r ;
  }

  void optimizer::Assign(const assignvector_t & assignment){
    optimizer & g = *this ;
    g.current.assignment = assignment ;
    Assign();
  }

// ====================================================================
// save_state
// ====================================================================

  state_t optimizer::save_state(
    const int           i   // variable index
  ){
    const optimizer   & g   = *this ;
    const int           j   = g.variable[i] ;
    const vertex_t      v   = g.number_to_vertex[j] ;
    const intvector_t & anc = g[v].ancestor.list ;

    state_t state = {
      {bitvector_t(anc.size()), bitvector_t(anc.size())},
      g.current
    } ;

    for(int b=0;b<2;b++)
      for(int k=0;k<anc.size();k++){
        vertex_t va = g.number_to_vertex[ anc[k] ] ;
        state.is_G[b][k] = g[va].is_G[b] ;
      }
    return state ;
  }

// ====================================================================
// load_state
// ====================================================================

  void optimizer::load_state(
    const state_t & state,
    const int       i      // variable index
  ){
    optimizer & g = *this ;
    const int j = g.variable[i] ;
    vertex_t v = g.number_to_vertex[j] ;
    const intvector_t & anc = g[v].ancestor.list      ;

    for(int b=0;b<2;b++)
      for(int j=0;j<anc.size();j++){
        vertex_t va = g.number_to_vertex[ anc[j] ] ;
        g[va].is_G[b] = state.is_G[b][j] ;
      }
    g.current = state.score ;
  }

// ====================================================================
// prefetch_score, increment_score
// ====================================================================

  enum{ do_not_increment = false, do_increment = true } ;

  inline static void _prefetch_score(
          optimizer        & g,
    const int            k,    // variable index
    const bool           b,    // value of variable[k]
          score_t      & score,
    const bool           increment
  ){
    const int            j            = g.variable[k]          ;
    const vertex_t       v            = g.number_to_vertex [j] ;
    const intvector_t  & anc          = g[v].ancestor.list     ;
          bitvector_t  & assign       = score.assignment       ;
          realvector_t & score_vector = score.score_vector     ;

    assign[k] = b ;

    for(int i = 0; i < anc.size(); i++){
      vertex_t va = g.number_to_vertex[ anc[i] ] ;

      if(b){
        if(g[va].is_G[1]==0){
          if(!(g[va].is_pairing||g[va].is_constraint))
            score[ g[va].priority ] += g[va].weight *
              (g.ratio_G1 + g.ratio_min * g[va].is_visited()) ;
          if(increment)g[va].is_G[1]=1 ;
        }
      }else{
        if(g[va].is_G[0]==0){
          if(!(g[va].is_pairing||g[va].is_constraint))
            score[ g[va].priority ] += g[va].weight *
              (g.ratio_G0 + g.ratio_min * g[va].is_visited()) ;
          if(increment)g[va].is_G[0]=1 ;
        }
      }
    }
  }

  score_t optimizer::prefetch_score(
    const int         k, // variable index
    const bool        b  // value of variable[k]
  ){
    optimizer   & g     = *this     ;
    score_t   score = g.current ;
    _prefetch_score(g,k,b,score,do_not_increment) ;
    return score ;
  }

  void optimizer::increment_score(
    const int         k, // variable index
    const bool        b  // value of variable[k]
  ){
    optimizer   & g     = *this     ;
    score_t & score = g.current ;
    _prefetch_score(g,k,b,score,do_increment) ;
  }

// ====================================================================
// BeginOptimize
// ====================================================================

  void init_current(optimizer & g){
    for(int i=0;i<g.num_priorities;i++) g.current[i] = 0 ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_explicit()){
        g.current[g[vi].priority] += g[vi].weight * g.ratio_min ;
      }
    }
  }

  void optimizer::BeginOptimize(integer_t limit_){
    current    = score_t(*this,0) ;
    minimum    = score_t(*this,huge_real) ;
    num_draw   = 1 ;

    optimizer & g = *this ;
    g.limit = limit_ ;
    if(g.limit >= 0){
      g.maxcount = 1 ;
      g.maxcount <<= (g.limit+1) ;
      if(g.maxcount <= 0) g.maxcount = -1 ;
    }else{
      g.maxcount = -1 ;
    }
//    cerr << "limit=" << g.limit << endl ;
//    cerr << "maxcount=" << g.maxcount << endl ;

    init_current(g) ;

    count      = 0 ;
    status     = success ;
    is_split   = true ;
    is_optimal = false ;
  }

  void optimizer::BeginOptimizeCrypto2014(integer_t limit_){
    current    = score_t(*this,0) ;
    minimum    = score_t(*this,huge_real) ;
    num_draw   = 1 ;

    optimizer & g = *this ;
    g.limit = limit_ ;
    if(g.limit >= 0){
      g.maxcount = 1 ;
      g.maxcount <<= g.limit ;
      if(g.maxcount <= 0) g.maxcount = -1 ;
    }else{
      g.maxcount = -1 ;
    }

    init_current(g) ;

    count      = 0 ;
    status     = cannot_split ;
    is_split   = false ;
    is_optimal = false ;
  }

// ============================================================
// EndOptimize
// ============================================================

  void optimizer::EndOptimize(bool omit_warnings){
    Assign(minimum.assignment);
    is_split = true ;
    if(((maxcount>=0)&&(count>=maxcount))||exit_by_signal){
      status = cannot_optimize ;
      is_optimal = false ;
      if(! omit_warnings){
        if(exit_by_signal){
          warnning("exit by signal") ;
        }else{
          warnning("number of bit assignment exceeds " +
                   to_string(maxcount) +
                   ". use IP solver or --unlimit");
        }
      }
      if(progress_rate<0)progress_rate=_progress_rate() ;
    }else{
      status     = success ;
      is_optimal = true ;
      progress_rate = 1 ;
    }
  }

  void optimizer::EndOptimizeCrypto2014(bool omit_warnings){
    if(status != cannot_split){
      Assign(minimum.assignment) ;
      is_split = true ;
    }else{
      is_split = false ;
    }
    if(((maxcount>=0)&&(count>=maxcount))||exit_by_signal){
      if(status != cannot_split) status = cannot_optimize ;
      is_optimal = false ;
      if(! omit_warnings){
        if(exit_by_signal){
          warnning("exit by signal") ;
        }else{
          warnning("number of bit assignment exceeds " +
                   to_string(maxcount) +
                   ". use IP solver or --unlimit");
        }
      }
      if(progress_rate<0)progress_rate=_progress_rate_crypto2014() ;
    }else{
      status     = success ;
      is_optimal = true ;
      progress_rate = 1 ;
    }
  }

// ====================================================================
// OptimizeOne
// ====================================================================

  inline static void OptimizeStep(optimizer & g){
    if(g.current < g.minimum){
      g.minimum  = g.current ;
      g.num_draw = 1 ;
    }else if(g.current == g.minimum){
      g.num_draw++ ;
    }
  }

  void optimizer::_OptimizeOne(){
    optimizer & g = *this ;
    OptimizeStep(g) ;
  }

  void optimizer::OptimizeOne(assignvector_t & assignment) {
    optimizer & g = *this ;
    g.Assign(assignment) ;
    OptimizeStep(g) ;
  }

// ============================================================
// print
// ============================================================

  ostream & operator<<(ostream& out, optimizer & g){
    out << (decide &)g ;
    if(g.format != tango){
      out << "/*" << endl ;
      out << "== optimizer ==" << endl ;
      if(g.is_split){
        out << "-- current --"                    << endl
            << g.current  
            << "-- minimum --"                    << endl
            << g.minimum
            << "-- etc --"                        << endl
            << "ratio="        << g.ratio         << endl
            << "digit_vector=" << g.digit_vector  << endl
            << "total_score="  << g.total_score() << endl
            << "num_draw="     << g.num_draw      << endl
            << "limit="        << g.limit         << endl
            << "maxcount="     << g.maxcount      << endl
            << "count="        << g.count         << endl
            << "is_split="     << g.is_split      << endl
            << "is_optimal="   << g.is_optimal    << endl
            << "progress_rate="<< g.progress_rate << endl ;
      }else{
        out << "fail to split." << endl ;
      }
      out << "*/" << endl ;
    }
    return out ;
  }

}

#ifdef OPTIMIZER_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  optimizer g ;
  assignvector_t assignment = assignvector_t(g) ;
  g.SetRatio(2.0) ;
  g.BeginOptimize() ;
  g.OptimizeOne(assignment) ;
  g.EndOptimize() ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
