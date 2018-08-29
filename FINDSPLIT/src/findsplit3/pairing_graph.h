#ifndef PAIRING_GRAPH_H
#define PAIRING_GRAPH_H

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/dynamic_bitset.hpp>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifdef __CYGWIN__
namespace findsplit {
  using namespace std;
  static string my_to_string( int value ){
    string s = "" ;
    string d = "" ;
    if(value<0){
      value = -value ;
      s = "-" ;
    }else if(value==0){
      d = "0" ;
    }
    while(value){
      static char const * const t[] = {
        "0", "1", "2", "3", "4",
        "5", "6", "7", "8", "9"
      } ;
      d = t[value % 10] + d ;
      value /= 10 ;
    }
    return s+d ;
  }
}
#define to_string my_to_string
#endif

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  typedef bool                       bool_t       ;
  typedef long long                  integer_t    ;
  const   integer_t                  huge_integer = LLONG_MAX ;

  typedef unsigned long long         uinteger_t   ;
  // typedef __uint128_t             uinteger_t   ;// => numeric_limits

  typedef long double                real_t       ;
  const   real_t                     huge_real    = HUGE_VALL ;

  typedef dynamic_bitset<uinteger_t> bitvector_t  ;
  typedef vector<integer_t>          intvector_t  ;
  typedef vector<real_t>             realvector_t ;
  
  struct pairing_graph_vertex_property ;
  
  typedef adjacency_list<
    setS, // setS, hash_setS : for single edges
          // listS, vecS     : for parallel edges
    vecS,
    bidirectionalS,
    pairing_graph_vertex_property
  > pairing_graph_base_0 ;
  
  typedef graph_traits<pairing_graph_base_0>::vertex_descriptor vertex_t ;
  typedef graph_traits<pairing_graph_base_0>::edge_descriptor edge_t ;
  
  template <typename T>
  ostream& operator<<(ostream& out, const vector<T> & v){
    const int n = v.size() ;
    out << "[" ;
    for(int i = 0; i < n; i++){
      if(i) out << "," ;
      out << v[i] ;
    }
    out << "]" ;
    return out ;
  }
  
  struct list_and_vector_t {
    intvector_t list   = intvector_t(0) ;
    bitvector_t vector = bitvector_t(0) ;
  
    inline void set(const int i){
      vector[i] = 1;
      list.push_back(i) ;
    }
    inline void vector_to_list(){
      const int n = vector.size();
      for(int i=0; i<n; i++)if(vector[i])list.push_back(i);
    }
  
  //  inline integer_t & operator[](const int i) {return list[i];}
  //  inline integer_t & back(){return list.back();}
    inline const integer_t operator[](const int i) const {return list[i];}
    inline const integer_t back() const {return list.back();}
    inline const int size() const { return list.size() ; }
    inline void push_back(const int i){ set(i); }
  } ;
  
  enum { dontcare, dependent_variable, independent_variable } ;
  
  struct pairing_graph_vertex_property {
    int       number        = -1       ;
    string    symbol        = ""       ;
    string    texlabel      = ""       ;
    bool      is_pairing    = false    ;
    bool      is_bottom     = false    ;
    bool      is_prohibited = false    ;
    bool      is_constraint = false    ;
    bool      is_cycle      = true     ;
    bool      is_fix        = false    ;
    bool      is_leaf       = false    ;
    bool      is_G[2]       = {false,false} ;
    bool      is_trivial    = false    ;

    int       value         = -1       ;
    real_t    weight        = 1        ;
    int       priority      = -1       ;
    int       vartype       = dontcare ;
    int       pairing_id    = -1       ;
    int       pairing_subid = -1       ;
    int       pair_id       = -1       ;
    int       var_id        = -1       ;

    intvector_t same_as     = intvector_t(0) ;
    intvector_t not_same_as = intvector_t(0) ;

    list_and_vector_t ancestor ;
    list_and_vector_t descendant ;
    list_and_vector_t critical_ancestor ;
    list_and_vector_t critical_descendant ;


    inline const bool is_critical() const {
      return (is_pairing || is_bottom || is_prohibited) ; }
    inline const bool is_normal() const { return (!is_critical()) ; }
    inline const bool is_implicit() const {
      return (is_pairing || is_constraint) ; }
    inline const bool is_explicit() const { return (!is_implicit()) ; }
    inline const bool is_duplicated() const {return (is_G[0]&&is_G[1]);}
    inline const bool is_visited()    const {return (is_G[0]||is_G[1]);}
    inline const bool is_not_visited() const {return (!is_visited());}
  } ;

  enum {raw, tango, tex} ;

  struct pairing_graph_base_1 {
    int    num_nodes      = 0     ;
    int    num_pairings   = 0     ;
    int    num_constraints= 0     ;
    int    num_priorities = 0     ;
    string graph_title    = "all" ;
    int    format         = tex   ;
    bool_t no_dup         = false ;
    bool_t show_constraint= true  ;
    boost::unordered_map<string, int> symbol_to_number ;
    vector<vertex_t>           number_to_vertex ;
  } ;
  
  struct pairing_graph :
    pairing_graph_base_0,
    pairing_graph_base_1
  {
      static int devnull ;

      int not_found(const string & symbol, int & i = devnull) ;
  
      inline void register_alias(const string & symbol){
        if(not_found(symbol))
          symbol_to_number[symbol] = num_nodes ;
      }
  
      vertex_t register_symbol(
        const string & symbol,
        const string & texlabel = ""
      ) ;
  
      inline vertex_t symbol_to_vertex(const string & symbol){
        return register_symbol(symbol);
      }
  
      void register_pairing(
        const string & symbol0,
        const string & symbol1
      );
  
      vertex_t register_constraint() ;
  
  } ;
  
  extern ostream & operator<<(
    ostream&, const pairing_graph_vertex_property &) ;
  extern ostream & operator<<(ostream&, const pairing_graph &) ;
  extern string replace_all(
    const string & a, // with
    const string & b, // in
    const string & c
  );

  inline static void bit_print(
    ostream& out,
    const bitvector_t & b,
    const string & label = ""
  ){
    for(int i=0;i<b.size();i++) out << b[i] ;
    out << label << endl ;
  }

  inline static void list_and_vector_print(
    ostream& out,
    const list_and_vector_t & lv,
    const string & label = ""
  ){
    bit_print(out, lv.vector, label) ;
    out << lv.list << endl ;
  }

#if 0
  inline static real_t now() {
    struct timeval tv ;
    gettimeofday(&tv,NULL) ;
    return tv.tv_sec + tv.tv_usec * 1e-6 ;
  }
#endif

  inline static real_t now() {
    struct rusage    ru                ;
    struct timeval & ut  = ru.ru_utime ;
    struct timeval & st  = ru.ru_stime ;
    real_t           ret               ;

    getrusage(RUSAGE_SELF, &ru);
    ret  = ut.tv_sec + ut.tv_usec * 1e-6 ;
    ret += st.tv_sec + st.tv_usec * 1e-6 ;
    getrusage(RUSAGE_CHILDREN, &ru);
    ret += ut.tv_sec + ut.tv_usec * 1e-6 ;
    ret += st.tv_sec + st.tv_usec * 1e-6 ;

    return ret ;
  }

  inline void add_edge(vertex_t u, vertex_t v, pairing_graph & g){
    if(u == v){
      cerr << "WARNING: ignore loop edge (" << g[u].symbol << ")" << endl ;
    }else{
//      cerr << "normal edge (" << g[u].symbol << "," <<
//                                 g[v].symbol << ")" << endl ;
      boost::add_edge(u, v, g) ;
    }
  }

}

#endif /* PAIRING_GRAPH_H */
