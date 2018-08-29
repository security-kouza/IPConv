#include "decide.h"

namespace findsplit {
using namespace std   ;
using namespace boost ;

// ============================================================
// MakeAncestorMatrix
// ============================================================

static void Anc0(pairing_graph & g, vertex_t v, bitvector_t & results){
  int i = g[v].number ;
  results[i] = 1 ;
  graph_traits<pairing_graph>::in_edge_iterator ei, ei_end ;
  for(tie(ei, ei_end)=in_edges(v,g);ei != ei_end; ei++){
    vertex_t va = source(*ei,g) ;
    int a = g[va].number ;
    if(!results[a])Anc0(g,va,results);
  }
}

static void MakeAncestorMatrix(pairing_graph & g){
  for(int i=0;i<g.num_nodes; i++){
    vertex_t v = g.number_to_vertex [i] ;
    bitvector_t & results = g[v].ancestor.vector ;
    results.resize(g.num_nodes,0) ;
    Anc0(g, v, results) ;
//    results[i] = 0 ;
    g[v].ancestor.vector_to_list() ;
  }
}

// ============================================================
// MakeDescendantMatrix
// ============================================================

static void Desc0(pairing_graph & g, vertex_t v, bitvector_t & results){
  int i = g[v].number ;
  results[i] = 1 ;
  graph_traits<pairing_graph>::out_edge_iterator ei, ei_end ;
  for(tie(ei, ei_end)=out_edges(v,g);ei != ei_end; ei++){
    vertex_t va = target(*ei,g) ;
    int a = g[va].number ;
    if(!results[a])Desc0(g,va,results);
  }
}

static void MakeDescendantMatrix(pairing_graph & g){
  for(int i=0;i<g.num_nodes; i++){
    vertex_t v = g.number_to_vertex [i] ;
    bitvector_t & results = g[v].descendant.vector ;
    results.resize(g.num_nodes,0) ;
    Desc0(g, v, results) ;
    results[i] = 0 ;
    g[v].descendant.vector_to_list() ;
  }
}

// ============================================================
// DetectBottom
// ============================================================

static bool is_leaf(pairing_graph & g, vertex_t v){
  graph_traits<pairing_graph>::adjacency_iterator ai, ai_end ;
  tie(ai, ai_end) = adjacent_vertices(v, g) ;
  return ai_end == ai ;
}

static void DetectBottom(pairing_graph & g){
  graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    if(is_leaf(g,*vi)){
      g[*vi].is_leaf = true ;
      if(! g[*vi].is_pairing){
        g[*vi].is_bottom = true ;
      }
    }
  }
}

// ============================================================
// DetectCycle
// ============================================================

static void DetectCycle0(pairing_graph & g, vertex_t vt){
  g[vt].is_cycle = false ;
  graph_traits<pairing_graph>::in_edge_iterator ei, ei_end ;
  for(tie(ei, ei_end)=in_edges(vt,g);ei != ei_end; ei++){
    vertex_t va = source(*ei,g) ;
    if(g[va].is_cycle) DetectCycle0(g, va) ;
  }
}

static void DetectCycle(pairing_graph & g){
  graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    if(g[*vi].is_leaf) DetectCycle0(g,*vi) ;
  }
}

// ============================================================
// AddCycleToBottom
// ============================================================
//
// mark representative node of cycle as a bottom node
//

static void AddCycleToBottom(pairing_graph & g){
  for(int i=0;i<g.num_nodes;i++){
    vertex_t v = g.number_to_vertex[i] ;
    if(g[v].is_cycle){
      const intvector_t & anc = g[v].ancestor.list ;
      for(int j=0;j<anc.size();j++){
        vertex_t va = g.number_to_vertex[anc[j]] ;
        g[va].is_bottom = false ;
      }
      g[v].is_bottom = true ;
    }
  }
}

// ============================================================
// MarkCritical
// ============================================================

static void MarkCritical(decide & g){
  g.prohibited.vector.resize(g.num_nodes,0) ;
  g.pairing.vector.resize(g.num_nodes,0) ;
  g.bottom.vector.resize(g.num_nodes,0) ;
  g.variable.vector.resize(g.num_nodes,0) ;

  for(int i = 0 ; i < g.num_nodes ; i++){
    vertex_t v = g.number_to_vertex[i] ;
    if(g[v].is_prohibited) g.prohibited.set(i);
    if(g[v].is_bottom) g.bottom.set(i);
    if(g[v].is_prohibited || g[v].is_bottom){
      g[v].var_id = g.num_variables();
      g.variable.set(i);
      g[v].vartype = independent_variable ;
    }
  }
  g.critical_pairing_start = g.variable.size() ;
  for(int i = 0 ; i < g.num_nodes ; i++){
    vertex_t v = g.number_to_vertex[i] ;
    if(g[v].is_pairing){
      g.pairing.set(i) ;
      g[v].var_id = g.num_variables();
      g.variable.set(i);
      g[v].vartype = independent_variable ;
    }
  }
}

// ============================================================
// MakeCriticalAncestor
// ============================================================

static void MakeCriticalAncestor(decide & g){
  for(int k = 0 ; k < g.num_nodes ; k++){
    vertex_t v = g.number_to_vertex[k] ;
    g[v].critical_ancestor.vector.resize(g.num_variables(),0) ;
    for(int j = 0 ; j < g.num_variables() ; j++){
      int l = g.variable[j] ;
      if(k!=l){
        int value = g[v].ancestor.vector[l] ;
        if(value)g[v].critical_ancestor.set(j) ;
      }
    }
  }
}

// ============================================================
// MakeCriticalDescendant
// ============================================================

static void MakeCriticalDescendant (decide & g){
  for(int k = 0 ; k < g.num_nodes ; k++){
    vertex_t v = g.number_to_vertex[k] ;
    g[v].critical_descendant.vector.resize(g.num_variables(),0) ;
    for(int j = 0 ; j < g.num_variables() ; j++){
      int l = g.variable[j] ;
      int value = g[v].descendant.vector[l] ;
      if(value)g[v].critical_descendant.set(j) ;
    }
  }
}

// ============================================================
// MakeRelation
// ============================================================

static void MakeRelation (decide & g){
  int i,j ;
  bitvector_t zero = bitvector_t(g.dimension(), 0) ;
  vector<bitvector_t> & relation = g.relation ;

  for(i = g.critical_pairing_start ; i < g.num_variables() ; i+=2){
    j = i+1 ;
    relation.push_back(zero) ;
    relation.back()[g.constant_term()]=1;
    relation.back()[i]=1;
    relation.back()[j]=1;
  }

  for(i = 0 ; i < g.num_variables() ; i++){
    int k = g.variable[i] ;
    vertex_t v = g.number_to_vertex[k] ;
    const intvector_t & anc = g[v].critical_ancestor.list ;
    for(j = 0 ; j < anc.size(); j++){
      relation.push_back(zero) ;
      relation.back()[i]=1 ;
      relation.back()[ anc[j] ]=1 ;
    }
  }

  for(i = 0 ; i < g.num_variables() ; i++){
    int j = g.variable[i] ;
    vertex_t v = g.number_to_vertex[j] ;
    if(g[v].is_fix){
      relation.push_back(zero) ;
      relation.back()[i]=1 ;
      relation.back()[g.constant_term()]=g[v].value ;
    }
    int k ;
    for(k = 0 ; k < g[v].same_as.size() ; k++){
      relation.push_back(zero) ;
      relation.back()[i]=1 ;
      vertex_t w = g.number_to_vertex[ g[v].same_as[k] ] ;
      relation.back()[g[w].var_id]=1 ;
    }
    for(k = 0 ; k < g[v].not_same_as.size() ; k++){
      relation.push_back(zero) ;
      relation.back()[i]=1 ;
      vertex_t w = g.number_to_vertex[g[v].not_same_as[k] ] ;
      relation.back()[g[w].var_id]=1 ;
      relation.back()[g.constant_term()]=1 ;
    }
  }
}

// ============================================================
// GaussianElimination
// ============================================================

template <typename T>
static void swap(T & a, T & b){
  if(&a != &b){
    T c = a ;
    a = b ;
    b = c ;
  }
}

static void GaussianElimination (decide & g){
  int i,j ;
  vector<bitvector_t> & M = g.echelon_form ; M = g.relation ;
  int r = g.dimension() ;
  int l = g.relation.size() ;

  g.dependent.vector.resize(g.num_variables()) ;

  int t = -1 ;

  for(i=0; i<l; i++){
    t++ ; if(t >= r) return ;
    j = i ;
    while(M[j][t]==0){
      j++ ;
      if(j >= l ){
        t++ ; if(t >= r) return ;
        j = i ;
      }
    }
    swap(M[i],M[j]) ;
    g.dependent.push_back(t) ;
    // M[i] /= M[i][t] ; if not GF(2)
    for(j=i+1;j<l;j++){
      if(M[j][t])M[j]^=M[i];
    }
  }
}

// ============================================================
// ListIndependentVariable
// ============================================================

static void ListIndependentVariable (decide & g){
  if( g.is_consistent() ){

    g.independent.vector.resize(g.num_variables()) ;

    for(int i=0;i<g.num_dependent();i++){
      int j = g.variable[ g.dependent[i] ] ;
      vertex_t v = g.number_to_vertex[j] ;
      g[v].vartype = dependent_variable ;
    }

    for(int i = 0; i < g.num_variables() ; i++){
      int j = g.variable[i] ;
      vertex_t v = g.number_to_vertex[j] ;
      if(g[v].vartype == independent_variable)
        g.independent.push_back(i) ;
    }

  }
}

// ============================================================
// DetectTrivial
// ============================================================

static void DetectTrivial(decide & g){
  if( g.is_consistent() ){
    for(int k = 0; k < g.num_nodes; k++){
      vertex_t v = g.number_to_vertex[k] ;
      int is_trivial = 0 ;
      if(g[v].is_normal()){
        const bitvector_t & is_descendant = g[v].descendant.vector ;
        for(int i = g.critical_pairing_start ; i < g.num_variables() ; i+=2){
          int j = i+1 ;
          int I = g.variable[i] ;
          int J = g.variable[j] ;
          if(is_descendant[I] && is_descendant[J]) is_trivial = 1 ;
        }
      }
      if(is_trivial) g[v].is_trivial = true ;
    }
  }
}

// ============================================================
// decide::decide
// ============================================================

decide::decide(){
  real_t start = now() ;
  decide & g = (*this) ;
  MakeAncestorMatrix(g) ;
  MakeDescendantMatrix(g);
  DetectBottom(g);
  DetectCycle(g);
  AddCycleToBottom(g);
  MarkCritical(g);
  MakeCriticalAncestor(g);
  MakeCriticalDescendant(g);
  MakeRelation(g);
  real_t start_gauss = now() ;
  GaussianElimination(g);
  real_t end_gauss = now() ;
  ListIndependentVariable(g);

  DetectTrivial(g);

  real_t end = now() ;
  interval_to_decide = end - start ;
  interval_to_gauss  = end_gauss - start_gauss ;
}

// ============================================================
// print
// ============================================================

ostream & operator<<(ostream& out, const decide & g){
  graph_traits<pairing_graph>::vertex_iterator vi, vi_end ;
  graph_traits<pairing_graph>::adjacency_iterator ai, ai_end ;

  if(g.format != tango){
    out << (semantic_analyzer &) g ;
  }else{
    out << (pairing_graph &) g ;
  }

  if(g.format != tango){
  
    out << "/*" << endl ;
    out << "== decide ==" << endl ;
    out << "interval_to_decide  = " << g.interval_to_decide << endl ;
    out << "interval_to_gauss   = " << g.interval_to_gauss  << endl ;
    out << "-- ancestor matrix --" << endl ;
    for(int i = 0; i < g.num_nodes; i++){
      vertex_t v = g.number_to_vertex[i] ;
      bit_print(out, g[v].ancestor.vector, " " + g[v].symbol) ;
    }
    out << "-- descendant matrix --" << endl ;
    for(int i = 0; i < g.num_nodes; i++){
      vertex_t v = g.number_to_vertex[i] ;
      bit_print(out, g[v].descendant.vector, " " + g[v].symbol) ;
    }
    out << "-- flags --" << endl ;
    list_and_vector_print(out, g.prohibited, " prohibited") ;
    list_and_vector_print(out, g.pairing   , " pairing"   ) ;
    list_and_vector_print(out, g.bottom    , " bottom"    ) ;
    list_and_vector_print(out, g.variable  , " variable"  ) ;

    out << "-- critical ancestor matrix --" << endl ;
    for(int i = 0; i < g.num_variables() ; i++){
      int j = g.variable[i] ;
      vertex_t v = g.number_to_vertex[j] ;
      bit_print(out, g[v].critical_ancestor.vector, " " + g[v].symbol);
    }

    out << "-- critical ancestor list --" << endl ;
    for(int i = 0 ; i < g.num_variables() ; i++){
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      const intvector_t & anc = g[vi].critical_ancestor.list ;
      out << g[vi].symbol << " --> " ;
      for(int j = 0 ; j < anc.size(); j++){
        vertex_t vj = g.number_to_vertex[ g.variable[ anc[j] ] ] ;
        out << g[vj].symbol << "," ;
      }
      out << endl ;
    }

    out << "-- relation --" << endl ;
    for(int i = 0; i < g.relation.size() ; i++){
      bit_print(out, g.relation[i]);
    }
  
    out << "-- echelon_form --" << endl ;
    for(int i=0;i<g.num_dependent();i++){
      bit_print(out, g.echelon_form[i]);
    }
    out << "--" << endl ;
    for(int i=g.num_dependent();i<g.echelon_form.size();i++){
      bit_print(out, g.echelon_form[i]);
    }
  
    out << "-- is_consistent --" << endl ;
    out << "is_consistent=" << g.is_consistent() << endl ;
  
    if(g.is_consistent()){
  
      out << "-- dependent variable --" << endl ;
      for(int i = 0; i < g.num_dependent() ; i++){
        int j = g.variable[ g.dependent[i] ] ;
        vertex_t v = g.number_to_vertex[j] ;
        out << g[v].symbol << "," ;
      }
      out << endl ;
      out << "num_dependent=" << g.num_dependent() << endl ;
      out << "-- independent variable --" << endl ;
      for(int i = 0; i < g.num_independent() ; i++){
        int j = g.variable[ g.independent[i] ] ;
        vertex_t v = g.number_to_vertex[j] ;
        out << g[v].symbol << "," ;
      }
      out << endl ;
      out << "num_independent=" << g.num_independent() << endl ;
    }
    out << "*/" << endl ;
  }
  return out ;
}

}

#ifdef DECIDE_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  decide g ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
