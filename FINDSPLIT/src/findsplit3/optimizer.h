#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "decide.h"
#include <iostream>
#include <math.h>
#include <tgmath.h>

#ifdef __CYGWIN__
static inline long double powl(long double base, long double e){
  return (long double)pow((double)base,(double)e) ;
}
#endif

namespace findsplit {
  using namespace std ;
  using namespace boost ;

  struct assignvector_t : bitvector_t {
    inline assignvector_t(){} // bad constructor
    inline assignvector_t(int n):bitvector_t(n){(*this)[n-1]=1;}
    inline assignvector_t(decide & g):assignvector_t(g.dimension()){}
  } ;

  struct score_t {
    realvector_t   score_vector ;
    assignvector_t assignment   ;
    inline score_t(){} // bad constructor
    inline score_t(decide & g, const real_t & r):
      score_vector(g.num_priorities, r),
      assignment(g.dimension()){}
    inline real_t & operator[](int i){return score_vector[i] ;}
  } ;

  struct score_storage_t {
    score_t score ;
  } ;

  struct state_t {
    vector<bitvector_t> is_G ;
    score_t score ;
  } ;

  template<typename T>
  inline int compare(vector<T> a, vector<T> b){
    for(int i=0;i<a.size();i++){
      /**/ if(a[i]<b[i]) return -1 ;
      else if(a[i]>b[i]) return  1 ;
    }
    return 0 ;
  }

  inline int compare(const score_t & a, const score_t & b){
    /**/ if(a.score_vector < b.score_vector) return -1 ;
    else if(a.score_vector > b.score_vector) return  1 ;
    else return 0 ;
  }

  template<typename T>
  inline bool operator<(T a,T b){return compare(a,b)<0;}
  template<typename T>
  inline bool operator<=(T a,T b){return compare(a,b)<=0;}
  template<typename T>
  inline bool operator>(T a,T b){return compare(a,b)>0;}
  template<typename T>
  inline bool operator>=(T a,T b){return compare(a,b)>=0;}
  template<typename T>
  inline bool operator==(T a,T b){return compare(a,b)==0;}

  inline ostream & operator<<(ostream& out, score_t & s){
    out << "score_vector=" << s.score_vector << endl
        << "assignment=" ; bit_print(out, s.assignment) ;
    return out ;
  }

  template<typename T>
  inline static const T inner_product(vector<T> a, vector<T> b){
    T c = 0;
    for(int i=a.size()-1;i>=0;i--)c += a[i]*b[i] ;
    return c ;
  }

  enum status_t {
    success         = 0,
    cannot_split    = 1,
    cannot_optimize = 2
  } ;

  struct result_t {
    score_t   current       ;
    score_t   minimum       ;
    integer_t num_draw      ;
    integer_t limit         ;
    integer_t maxcount      ;
    integer_t count         ;
    status_t  status        ;
    bool_t    is_split      ;
    bool_t    is_optimal    ;
    real_t    progress_rate ;
  } ;

  struct optimizer_base : decide {
    real_t        ratio            = 1.0 ;
    real_t        ratio_min        = 1.0 ;
    real_t        ratio_G0         = 1.0 ;
    real_t        ratio_G1         = 1.0 ;
    realvector_t  digit_vector     = realvector_t(num_priorities,1);
    score_t       current          = score_t(*this,0);
    score_t       minimum          = score_t(*this,huge_real);
    integer_t     num_draw         = 1 ;
    integer_t     limit            = 20 ;
    integer_t     maxcount         = 1LL << 20 ;
    integer_t     count            = 0 ;
    status_t      status           = success ;
    bool_t        is_split         = false ;
    bool_t        is_optimal       = false ;
    real_t        progress_rate    = -1 ;

    inline const real_t total_score() const {
      return inner_product(digit_vector, current.score_vector) ;
    }

    inline const result_t save_result() const {
      result_t result = {
        current,minimum,num_draw,limit,maxcount,
        count,status,is_split,is_optimal,progress_rate} ;
      return result ;
    }

    inline void load_result(const result_t & result) {
      current       = result.current       ;
      minimum       = result.minimum       ;
      num_draw      = result.num_draw      ;
      limit         = result.limit         ;
      maxcount      = result.maxcount      ;
      count         = result.count         ;
      status        = result.status        ;
      is_split      = result.is_split      ;
      is_optimal    = result.is_optimal    ;
      progress_rate = result.progress_rate ;
    }

  } ;

//  enum {unoptimal = false, optimal = true } ;

  enum {
    success_to_assign = 0, 
    inconsistent_assignment = 1,
    exists_unvisited_node = 2
  } ;

  struct optimizer : optimizer_base {
    inline optimizer(){
      if(this->is_inconsistent()){
        warnning("there exists no split for this graph") ;
        status = cannot_split ;
      }else{
        SetRatio(1.0);
      }
    }
    void SetRatio(const real_t ratio_) ;
// ========================================================
    void Assign();
    void Assign(const assignvector_t & assignment) ;
    int  AssignCrypto2014();
// ========================================================
    state_t save_state(const int i) ; // i = variable index
    void    load_state(const state_t & state, const int i) ;
    score_t prefetch_score (const int  k, const bool b) ;
    void    increment_score(const int  k, const bool b) ;
            // k = variable index, b = value of variable[k]
    inline  bool solve_dependent(int d) ; // d = index of dependent
// ========================================================
    enum {be_quiet = true} ;
    void BeginOptimize(integer_t limit_ = 22);
    void EndOptimize  (bool omit_warnings = false) ;
    void BeginOptimizeCrypto2014 (integer_t limit_ = 22);
    void EndOptimizeCrypto2014  (bool omit_warnings = false) ;
    void _OptimizeOne();
    void OptimizeOne(assignvector_t & assignment);

    inline real_t _progress_rate(integer_t base){
      real_t rate = 0 ;
      real_t d = powl(2.0,base+1) - 2 ;
      if(d == 0) rate = count     ;
      else       rate = count / d ;
      return rate ;
    }
    inline real_t _progress_rate(){
      return _progress_rate(num_independent());
    }
    inline real_t _progress_rate_crypto2014(){
      return _progress_rate(num_pairings+num_prohibited()+num_bottom());
    }
    inline real_t _progress_rate_backtrack(const score_t & score){
      real_t digit = 1;
      real_t sum   = 0;
      for(int i=num_independent()-1;i>=0;i--){
        digit /= 2 ;
        sum += digit * score.assignment [ independent[i] ] ;
      }
      return sum ;
    }
/*
    inline real_t _progress_rate_backward(const score_t & score){
      real_t digit = 1;
      real_t sum   = 0;
      for(int i=num_independent()-1;i>=0;i--){
        digit /= 2 ;
        sum += digit * score.assignment [ independent[i] ] ;
      }
      return sum ;
    }

    inline real_t _progress_rate_forward(const score_t & score){
      real_t digit = 1;
      real_t sum   = 0;
      for(int i=0;i<num_independent();i++){
        digit /= 2 ;
        sum += digit * score.assignment [ independent[i] ] ;
      }
      return sum ;
    }


    inline real_t _progress_rate_crypto2014(const score_t & score){
      real_t digit = 1;
      real_t sum   = 0;
      const optimizer & g = *this ;
      for(int i=0;i<g.num_variables();i++){
        vertex_t v = g.number_to_vertex[ g.variable[i] ] ;
        digit /= 2 ;
        sum += digit * score.assignment [i] ;
        if(g[v].is_pairing)i++ ;
      }
      return sum ;
    }
*/

  } ;

  ostream & operator<<(ostream& out, optimizer & g) ;

  inline  bool optimizer::solve_dependent(int d){ // d = index of dependent
    const optimizer   & g     = *this                ;
    const int           k     = g.dependent[d]       ;
    const bitvector_t & value = g.current.assignment ;
          bitvector_t   mask  = g.echelon_form[d]    ;
    mask[k] = 0 ;
    mask &= value ;
    return mask.count() & 1 ;
  }
}

#endif /* OPTIMIZER_H */
