#include "linear.h"
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace findsplit {
  using namespace std   ;
  using namespace boost ;

  static const string lp_symbol_CPLEX(const string & s) {
    return replace_all("]",")",replace_all("[","(",s)) ;
  }
  
  static const string lp_symbol_LP_SOLVE(const string & s) {
    return "x" + s ;
  }

// ============================================================
// IntegerPrograming
// ============================================================

  void linear::IntegerPrograming(const string & filename) {
    linear & g = *this ;
    ofstream fout(filename.c_str()) ;
    if(!fout.good())fatal_error("fail to open " + filename) ;
    g.lp_filename = filename ;
    IntegerPrograming(fout);
    fout.close() ;
  }

  void linear::IntegerPrograming(ostream & out){
    linear & g = *this ;
  
    const string minimize      = (g.lp_mode == lp_solve) ? "min:"  : "minimize\n $findsplit:";
    const string subject_to    = (g.lp_mode == lp_solve) ? ""      : "subject to\n" ;
    const string binary        = (g.lp_mode == lp_solve) ? "bin "  : "binary\n"     ;
    const string end           = (g.lp_mode == lp_solve) ? ""      : "end\n"        ;
    const string semicolon     = (g.lp_mode == lp_solve) ? ";"     : ""             ;
    const string begin_comment = (g.lp_mode == lp_solve) ? "/* "   : "\\ "          ;
    const string end_comment   = (g.lp_mode == lp_solve) ? " */"   : ""             ;
    const string (*lp_symbol)(const string &)
                               = (g.lp_mode == lp_solve) ? lp_symbol_LP_SOLVE
                                                         : lp_symbol_CPLEX ;

    /* MINIMIZE */
  
    out << minimize << " " ;

    int flag = 0 ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      long double c0 = g[vi].weight * g.digit_vector[g[vi].priority] ;
      long double c1 = c0 * g.ratio ;
      if(g[vi].is_explicit()){
        if(c0 != 0){
          if(flag)out << " + " ; else flag=1;
          if(c0 != 1) out << c0 << " " ;
          out << lp_symbol(g[vi].symbol) << ".is_G0" ;
        }
        if(c1 != 0){
          if(flag)out << " + " ; else flag=1;
          if(c1 != 1) out << c1 << " " ;
          out << lp_symbol(g[vi].symbol) << ".is_G1" ;
        }
      }
    }
    out << semicolon << endl ;
  
    /* SUBJECT TO */
  
    out << subject_to ;
    string sum ;
    string sub ;
    string or_ ;
    string nor ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_normal()){
        const intvector_t & critical_descendant
          = g[vi].critical_descendant.list ;
        const int num_critical_descendant = critical_descendant.size();
        if( num_critical_descendant >= 1 ){ // maybe duplicated
          sum="";
          sub="";
          or_="";
          nor="";
          flag=0;
          for(int j=0; j<num_critical_descendant; j++){
            int k = g.variable[ critical_descendant[j] ] ;
            vertex_t vj = g.number_to_vertex[k] ;
            if(flag){
              sum += " + " ;
              or_ += "|" ;
              nor += "|" ;
            } else flag=1;
            sum +=         lp_symbol(g[vj].symbol) ;
            sub += " - " + lp_symbol(g[vj].symbol) ;
            or_ +=         lp_symbol(g[vj].symbol) ;
            nor += "!"   + lp_symbol(g[vj].symbol) ;
          }
  
          out << begin_comment
  	    << lp_symbol(g[vi].symbol) << ".is_G0 = " << nor 
  	    << end_comment << endl ;
  
  /*
          out << sub << " - "
  	    << lp_symbol(g[vi].symbol) << ".is_G0 >= -"
  	    << num_critical_descendant 
  	    << semicolon << endl ;
  */
  
          for(int j=0; j<num_critical_descendant; j++){
            int k = g.variable[ critical_descendant[j] ] ;
            vertex_t vj = g.number_to_vertex[k] ;
  	  out << " "
  	      << lp_symbol(g[vj].symbol) << " + "
  	      << lp_symbol(g[vi].symbol) << ".is_G0 >= 1"
  	      << semicolon << endl ;
          }
  
          if(g.ratio != 0){
            out << begin_comment
                << lp_symbol(g[vi].symbol) << ".is_G1 = " << or_
  	      << end_comment << endl ;
   /* 
            out << " " << sum << " - "
                << lp_symbol(g[vi].symbol) << ".is_G1 >= 0"
                << semicolon << endl ;
   */ 
            for(int j=0; j<num_critical_descendant; j++){
              int k = g.variable[ critical_descendant[j] ] ;
              vertex_t vj = g.number_to_vertex[k] ;
              out << " - " << lp_symbol(g[vj].symbol)
                  << " + " << lp_symbol(g[vi].symbol) << ".is_G1 >= 0"
                  << semicolon << endl ;
            }
          }
        }
      }
    }
  
    out << begin_comment
        << "-- relation --"
        << end_comment << endl ;
  
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_explicit()){
        if(g[vi].is_critical()){
          out << " "
              << lp_symbol(g[vi].symbol) << ".is_G0 + "
              << lp_symbol(g[vi].symbol) << " = 1"
              << semicolon << endl ;
          if(g.ratio != 0){
            out << " "
                << lp_symbol(g[vi].symbol) << ".is_G1 - "
                << lp_symbol(g[vi].symbol) << " = 0"
                << semicolon << endl ;
          }
        }
      }
    }
  
    for(int i = g.critical_pairing_start ; i < g.num_variables() ; i+=2){
      int j = i+1 ;
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      vertex_t vj = g.number_to_vertex[ g.variable[j] ] ;
      out << " "
          << lp_symbol(g[vi].symbol) << " + "
          << lp_symbol(g[vj].symbol) << " = 1"
          << semicolon << endl ;
    }
  
    for(int i = 0 ; i < g.num_variables() ; i++){
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      const intvector_t & anc = g[vi].critical_ancestor.list ;
      for(int j = 0 ; j < anc.size(); j++){
        vertex_t vj = g.number_to_vertex[ g.variable[ anc[j] ] ] ;
        out << " "
  	  << lp_symbol(g[vi].symbol) << " - "
            << lp_symbol(g[vj].symbol) << " = 0"
            << semicolon << endl ;
      }
    }
  
    out << begin_comment
        << "-- constraints of prohibited node --"
        << end_comment << endl ;
  
    for(int i = 0 ; i < g.num_variables() ; i++){
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      if(g[vi].is_fix){
        out << " " 
            << lp_symbol(g[vi].symbol) << " = " << g[vi].value
            << semicolon << endl ;
      }
      int j ;
      for(j = 0 ; j < g[vi].same_as.size() ; j++){
        vertex_t vj = g.number_to_vertex[ g[vi].same_as[j] ] ;
        out << " "
  	    << lp_symbol(g[vi].symbol) << " - "
            << lp_symbol(g[vj].symbol) << " = 0"
            << semicolon << endl ;
      }
      for(j = 0 ; j < g[vi].not_same_as.size() ; j++){
        vertex_t vj = g.number_to_vertex[ g[vi].not_same_as[j] ] ;
        out << " "
            << lp_symbol(g[vi].symbol) << " + "
            << lp_symbol(g[vj].symbol) << " = 1"
            << semicolon << endl ;
      }
    }

    out << begin_comment
        << "-- trivial node --"
        << end_comment << endl ;

    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_trivial){
        out << " "
            << lp_symbol(g[vi].symbol) << ".is_G0 = 1"
            << semicolon << endl ;
        out << " "
            << lp_symbol(g[vi].symbol) << ".is_G1 = 1"
            << semicolon << endl ;
      }
    }

    /* BINARY */
  
    out << binary << " " ;
  
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_explicit()){
        out << lp_symbol(g[vi].symbol) << ".is_G0 " ;
        if(g.ratio != 0)
          out << lp_symbol(g[vi].symbol) << ".is_G1 " ;
        if(g[vi].is_critical())
          out << lp_symbol(g[vi].symbol) << " " ;
      }else{
        out << lp_symbol(g[vi].symbol) << " " ;
      }
    }
    out << semicolon << endl ;
  
    /* END */
  
    out << end ;
  }

// ============================================================
// IntegerProgramingOmitTrivial
// ============================================================

  void linear::IntegerProgramingOmitTrivial(const string & filename) {
    linear & g = *this ;
    ofstream fout(filename.c_str()) ;
    if(!fout.good())fatal_error("fail to open " + filename) ;
    g.lp_filename = filename ;
    IntegerProgramingOmitTrivial(fout);
    fout.close() ;
  }

  void linear::IntegerProgramingOmitTrivial(ostream & out){
    linear & g = *this ;
  
    const string minimize      = (g.lp_mode == lp_solve) ? "min:"  : "minimize\n $findsplit:";
    const string subject_to    = (g.lp_mode == lp_solve) ? ""      : "subject to\n" ;
    const string binary        = (g.lp_mode == lp_solve) ? "bin "  : "binary\n"     ;
    const string end           = (g.lp_mode == lp_solve) ? ""      : "end\n"        ;
    const string semicolon     = (g.lp_mode == lp_solve) ? ";"     : ""             ;
    const string begin_comment = (g.lp_mode == lp_solve) ? "/* "   : "\\ "          ;
    const string end_comment   = (g.lp_mode == lp_solve) ? " */"   : ""             ;
    const string (*lp_symbol)(const string &)
                               = (g.lp_mode == lp_solve) ? lp_symbol_LP_SOLVE
                                                         : lp_symbol_CPLEX ;

    /* MINIMIZE */
  
    out << minimize << " " ;

    int flag = 0 ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      long double c0 = g[vi].weight * g.digit_vector[g[vi].priority] ;
      long double c1 = c0 * g.ratio ;
      if(g[vi].is_explicit()){
        if(c0 != 0){
          if(flag)out << " + " ; else flag=1;
          if(c0 != 1) out << c0 << " " ;
          out << lp_symbol(g[vi].symbol) << ".is_G0" ;
        }
        if(c1 != 0){
          if(flag)out << " + " ; else flag=1;
          if(c1 != 1) out << c1 << " " ;
          out << lp_symbol(g[vi].symbol) << ".is_G1" ;
        }
      }
    }
    out << semicolon << endl ;
  
    /* SUBJECT TO */
  
    out << subject_to ;
    string sum ;
    string sub ;
    string or_ ;
    string nor ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_normal()){
        const intvector_t & critical_descendant
          = g[vi].critical_descendant.list ;
        const int num_critical_descendant = critical_descendant.size();
        if( num_critical_descendant >= 1 ){ // maybe duplicated
          sum="";
          sub="";
          or_="";
          nor="";
          flag=0;
          for(int j=0; j<num_critical_descendant; j++){
            int k = g.variable[ critical_descendant[j] ] ;
            vertex_t vj = g.number_to_vertex[k] ;
            if(flag){
              sum += " + " ;
              or_ += "|" ;
              nor += "|" ;
            } else flag=1;
            sum +=         lp_symbol(g[vj].symbol) ;
            sub += " - " + lp_symbol(g[vj].symbol) ;
            or_ +=         lp_symbol(g[vj].symbol) ;
            nor += "!"   + lp_symbol(g[vj].symbol) ;
          }
  
          out << begin_comment
  	    << lp_symbol(g[vi].symbol) << ".is_G0 = " << nor 
  	    << end_comment << endl ;
  
  /*
          out << sub << " - "
  	    << lp_symbol(g[vi].symbol) << ".is_G0 >= -"
  	    << num_critical_descendant 
  	    << semicolon << endl ;
  */
  
          for(int j=0; j<num_critical_descendant; j++){
            int k = g.variable[ critical_descendant[j] ] ;
            vertex_t vj = g.number_to_vertex[k] ;
  	  out << " "
  	      << lp_symbol(g[vj].symbol) << " + "
  	      << lp_symbol(g[vi].symbol) << ".is_G0 >= 1"
  	      << semicolon << endl ;
          }
  
          if(g.ratio != 0){
            out << begin_comment
                << lp_symbol(g[vi].symbol) << ".is_G1 = " << or_
  	      << end_comment << endl ;
   /* 
            out << " " << sum << " - "
                << lp_symbol(g[vi].symbol) << ".is_G1 >= 0"
                << semicolon << endl ;
   */ 
            for(int j=0; j<num_critical_descendant; j++){
              int k = g.variable[ critical_descendant[j] ] ;
              vertex_t vj = g.number_to_vertex[k] ;
              out << " - " << lp_symbol(g[vj].symbol)
                  << " + " << lp_symbol(g[vi].symbol) << ".is_G1 >= 0"
                  << semicolon << endl ;
            }
          }
        }
      }
    }
  
    out << begin_comment
        << "-- relation --"
        << end_comment << endl ;
  
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_explicit()){
        if(g[vi].is_critical()){
          out << " "
              << lp_symbol(g[vi].symbol) << ".is_G0 + "
              << lp_symbol(g[vi].symbol) << " = 1"
              << semicolon << endl ;
          if(g.ratio != 0){
            out << " "
                << lp_symbol(g[vi].symbol) << ".is_G1 - "
                << lp_symbol(g[vi].symbol) << " = 0"
                << semicolon << endl ;
          }
        }
      }
    }
  
    for(int i = g.critical_pairing_start ; i < g.num_variables() ; i+=2){
      int j = i+1 ;
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      vertex_t vj = g.number_to_vertex[ g.variable[j] ] ;
      out << " "
          << lp_symbol(g[vi].symbol) << " + "
          << lp_symbol(g[vj].symbol) << " = 1"
          << semicolon << endl ;
    }
  
    for(int i = 0 ; i < g.num_variables() ; i++){
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      const intvector_t & anc = g[vi].critical_ancestor.list ;
      for(int j = 0 ; j < anc.size(); j++){
        vertex_t vj = g.number_to_vertex[ g.variable[ anc[j] ] ] ;
        out << " "
  	  << lp_symbol(g[vi].symbol) << " - "
            << lp_symbol(g[vj].symbol) << " = 0"
            << semicolon << endl ;
      }
    }
  
    out << begin_comment
        << "-- constraints of prohibited node --"
        << end_comment << endl ;
  
    for(int i = 0 ; i < g.num_variables() ; i++){
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      if(g[vi].is_fix){
        out << " " 
            << lp_symbol(g[vi].symbol) << " = " << g[vi].value
            << semicolon << endl ;
      }
      int j ;
      for(j = 0 ; j < g[vi].same_as.size() ; j++){
        vertex_t vj = g.number_to_vertex[ g[vi].same_as[j] ] ;
        out << " "
  	    << lp_symbol(g[vi].symbol) << " - "
            << lp_symbol(g[vj].symbol) << " = 0"
            << semicolon << endl ;
      }
      for(j = 0 ; j < g[vi].not_same_as.size() ; j++){
        vertex_t vj = g.number_to_vertex[ g[vi].not_same_as[j] ] ;
        out << " "
            << lp_symbol(g[vi].symbol) << " + "
            << lp_symbol(g[vj].symbol) << " = 1"
            << semicolon << endl ;
      }
    }

/*
    out << begin_comment
        << "-- trivial node --"
        << end_comment << endl ;

    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_trivial){
        out << " "
            << lp_symbol(g[vi].symbol) << ".is_G0 = 1"
            << semicolon << endl ;
        out << " "
            << lp_symbol(g[vi].symbol) << ".is_G1 = 1"
            << semicolon << endl ;
      }
    }
*/

    /* BINARY */
  
    out << binary << " " ;
  
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_explicit()){
        out << lp_symbol(g[vi].symbol) << ".is_G0 " ;
        if(g.ratio != 0)
          out << lp_symbol(g[vi].symbol) << ".is_G1 " ;
        if(g[vi].is_critical())
          out << lp_symbol(g[vi].symbol) << " " ;
      }else{
        out << lp_symbol(g[vi].symbol) << " " ;
      }
    }
    out << semicolon << endl ;
  
    /* END */
  
    out << end ;
  }

// ============================================================
// IntegerProgramingCrypto2016
// ============================================================

  void linear::IntegerProgramingCrypto2016(const string & filename) {
    linear & g = *this ;
    ofstream fout(filename.c_str()) ;
    if(!fout.good())fatal_error("fail to open " + filename) ;
    g.lp_filename = filename ;
    IntegerProgramingCrypto2016(fout);
    fout.close() ;
  }

  void linear::IntegerProgramingCrypto2016(ostream & out){
    linear & g = *this ;
  
    const string minimize      = (g.lp_mode == lp_solve) ? "min:"  : "minimize\n $findsplit:";
    const string subject_to    = (g.lp_mode == lp_solve) ? ""      : "subject to\n" ;
    const string binary        = (g.lp_mode == lp_solve) ? "bin "  : "binary\n"     ;
    const string end           = (g.lp_mode == lp_solve) ? ""      : "end\n"        ;
    const string semicolon     = (g.lp_mode == lp_solve) ? ";"     : ""             ;
    const string begin_comment = (g.lp_mode == lp_solve) ? "/* "   : "\\ "          ;
    const string end_comment   = (g.lp_mode == lp_solve) ? " */"   : ""             ;
    const string (*lp_symbol)(const string &)
                               = (g.lp_mode == lp_solve) ? lp_symbol_LP_SOLVE
                                                         : lp_symbol_CPLEX ;

    /* MINIMIZE */
  
    out << minimize << " " ;

    int flag = 0 ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      long double c0 = g[vi].weight * g.digit_vector[g[vi].priority] ;
      long double c1 = c0 * g.ratio ;
      if(g[vi].is_explicit()){
        if(c0 != 0){
          if(flag)out << " + " ; else flag=1;
          if(c0 != 1) out << c0 << " " ;
          out << lp_symbol(g[vi].symbol) << ".is_G0" ;
        }
        if(c1 != 0){
          if(flag)out << " + " ; else flag=1;
          if(c1 != 1) out << c1 << " " ;
          out << lp_symbol(g[vi].symbol) << ".is_G1" ;
        }
      }
    }
    out << semicolon << endl ;
  
    /* SUBJECT TO */
  
    out << subject_to ;
    string sum ;
    string sub ;
    string or_ ;
    string nor ;
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_normal()){
        const intvector_t & critical_descendant
          = g[vi].critical_descendant.list ;
        const int num_critical_descendant = critical_descendant.size();
        if( num_critical_descendant >= 1 ){ // maybe duplicated
          sum="";
          sub="";
          or_="";
          nor="";
          flag=0;
          for(int j=0; j<num_critical_descendant; j++){
            int k = g.variable[ critical_descendant[j] ] ;
            vertex_t vj = g.number_to_vertex[k] ;
            if(flag){
  	    sum += " + " ;
  	    or_ += "|" ;
  	    nor += "|" ;
            } else flag=1;
            sum +=         lp_symbol(g[vj].symbol) ;
            sub += " - " + lp_symbol(g[vj].symbol) ;
            or_ +=         lp_symbol(g[vj].symbol) ;
            nor += "!"   + lp_symbol(g[vj].symbol) ;
          }
  
          out << begin_comment
  	    << lp_symbol(g[vi].symbol) << ".is_G0 = " << nor 
  	    << end_comment << endl ;
  
          out << sub << " - "
  	    << lp_symbol(g[vi].symbol) << ".is_G0 >= -"
  	    << num_critical_descendant 
  	    << semicolon << endl ;
  
          for(int j=0; j<num_critical_descendant; j++){
            int k = g.variable[ critical_descendant[j] ] ;
            vertex_t vj = g.number_to_vertex[k] ;
  	  out << " "
  	      << lp_symbol(g[vj].symbol) << " + "
  	      << lp_symbol(g[vi].symbol) << ".is_G0 >= 1"
  	      << semicolon << endl ;
          }
  
          if(g.ratio != 0){
            out << begin_comment
                << lp_symbol(g[vi].symbol) << ".is_G1 = " << or_
  	      << end_comment << endl ;
    
            out << " " << sum << " - "
                << lp_symbol(g[vi].symbol) << ".is_G1 >= 0"
                << semicolon << endl ;
    
            for(int j=0; j<num_critical_descendant; j++){
              int k = g.variable[ critical_descendant[j] ] ;
              vertex_t vj = g.number_to_vertex[k] ;
              out << " - " << lp_symbol(g[vj].symbol)
                  << " + " << lp_symbol(g[vi].symbol) << ".is_G1 >= 0"
                  << semicolon << endl ;
            }
          }
        }
      }
    }
  
    out << begin_comment
        << "-- relation --"
        << end_comment << endl ;
  
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_explicit()){
        if(g[vi].is_critical()){
          out << " "
              << lp_symbol(g[vi].symbol) << ".is_G0 + "
              << lp_symbol(g[vi].symbol) << " = 1"
              << semicolon << endl ;
          if(g.ratio != 0){
            out << " "
                << lp_symbol(g[vi].symbol) << ".is_G1 - "
                << lp_symbol(g[vi].symbol) << " = 0"
                << semicolon << endl ;
          }
        }
      }
    }
  
    for(int i = g.critical_pairing_start ; i < g.num_variables() ; i+=2){
      int j = i+1 ;
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      vertex_t vj = g.number_to_vertex[ g.variable[j] ] ;
      out << " "
          << lp_symbol(g[vi].symbol) << " + "
          << lp_symbol(g[vj].symbol) << " = 1"
          << semicolon << endl ;
    }
  
    for(int i = 0 ; i < g.num_variables() ; i++){
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      const intvector_t & anc = g[vi].critical_ancestor.list ;
      for(int j = 0 ; j < anc.size(); j++){
        vertex_t vj = g.number_to_vertex[ g.variable[ anc[j] ] ] ;
        out << " "
  	  << lp_symbol(g[vi].symbol) << " - "
            << lp_symbol(g[vj].symbol) << " = 0"
            << semicolon << endl ;
      }
    }
  
    out << begin_comment
        << "-- constraints of prohibited node --"
        << end_comment << endl ;
  
    for(int i = 0 ; i < g.num_variables() ; i++){
      vertex_t vi = g.number_to_vertex[ g.variable[i] ] ;
      if(g[vi].is_fix){
        out << " " 
            << lp_symbol(g[vi].symbol) << " = " << g[vi].value
            << semicolon << endl ;
      }
      int j ;
      for(j = 0 ; j < g[vi].same_as.size() ; j++){
        vertex_t vj = g.number_to_vertex[ g[vi].same_as[j] ] ;
        out << " "
  	    << lp_symbol(g[vi].symbol) << " - "
            << lp_symbol(g[vj].symbol) << " = 0"
            << semicolon << endl ;
      }
      for(j = 0 ; j < g[vi].not_same_as.size() ; j++){
        vertex_t vj = g.number_to_vertex[ g[vi].not_same_as[j] ] ;
        out << " "
            << lp_symbol(g[vi].symbol) << " + "
            << lp_symbol(g[vj].symbol) << " = 1"
            << semicolon << endl ;
      }
    }
  
    /* BINARY */
  
    out << binary << " " ;
  
    for(int i=0;i<g.num_nodes;i++){
      vertex_t vi = g.number_to_vertex[i] ;
      if(g[vi].is_explicit()){
        out << lp_symbol(g[vi].symbol) << ".is_G0 " ;
        if(g.ratio != 0)
          out << lp_symbol(g[vi].symbol) << ".is_G1 " ;
        if(g[vi].is_critical())
          out << lp_symbol(g[vi].symbol) << " " ;
      }else{
        out << lp_symbol(g[vi].symbol) << " " ;
      }
    }
    out << semicolon << endl ;
  
    /* END */
  
    out << end ;
  }

// ==============================================================
// LoadSolution
// ==============================================================
  
  static const string sol_symbol_CPLEX(const string & s) {
    return replace_all(")","]",replace_all("(","[",s)) ;
  }
  
  static const string sol_symbol_LP_SOLVE(const string & s) {
    return s.substr(1) ;
  }

  static void getlineGLPSOL(istream & in, string & s){
    getline(in,s) ;
    if(s.find('*')==string::npos){
      int n = s.length();
      int i,j;
      for(i=0;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t'))break;
      if((s[i]>='0')&&(s[i]<='9')){
        for(;i<n;i++)if((s[i]==' ')||(s[i]=='\t'))break;
        for(;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t'))break;
        if(
          ((s[i]>='A')&&(s[i]<='Z'))||
          ((s[i]>='a')&&(s[i]<='z'))||
          (s[i]=='!')||(s[i]=='"')||(s[i]=='#')||(s[i]=='$')||(s[i]=='%')||(s[i]=='&')||
          (s[i]=='(')||(s[i]==')')||(s[i]=='/')||(s[i]==',')||(s[i]==';')||(s[i]=='?')||
          (s[i]=='@')||(s[i]=='_')||(s[i]=='`')||(s[i]=='\'')||(s[i]=='{')||(s[i]=='}')||
          (s[i]=='|')||(s[i]=='~')
        ){
          for(;i<n;i++)if(
	    !(
              ((s[i]>='A')&&(s[i]<='Z'))||
              ((s[i]>='a')&&(s[i]<='z'))||
              (s[i]=='!')||(s[i]=='"')||(s[i]=='#')||(s[i]=='$')||(s[i]=='%')||(s[i]=='&')||
              (s[i]=='(')||(s[i]==')')||(s[i]=='/')||(s[i]==',')||(s[i]==';')||(s[i]=='?')||
              (s[i]=='@')||(s[i]=='_')||(s[i]=='`')||(s[i]=='\'')||(s[i]=='{')||(s[i]=='}')||
              (s[i]=='|')||(s[i]=='~')||
              ((s[i]>='0')&&(s[i]<='9'))||(s[i]=='.')
	    )
	  )break;
	  if(i==n){
            string t ;
            getline(in,t) ;
            s += t ;
	  }
        }
      }
    }
//    cout << "getlineGLPSOL:" << s << endl ;
  }

  static intvector_t LoadSolutionGLPSOL(linear & g, istream& in){
    const string (* const sol_symbol)(const string &) = sol_symbol_CPLEX ;
    string s ;
    intvector_t solution_assignment(g.num_nodes, -1) ;
    while (true) {
      getlineGLPSOL(in,s) ;
      if(in.eof())break;
      int n = s.length();
      int i,j;
      for(i=0;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t'))break;
      if((s[i]>='0')&&(s[i]<='9')){
        for(;i<n;i++)if((s[i]==' ')||(s[i]=='\t'))break;
        for(;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t'))break;
        for(j=0;i+j<n;j++)if((s[i+j]==' ')||(s[i+j]=='\t'))break ;
        string var = sol_symbol(s.substr(i,j)) ;
        if(var.find('.')==string::npos){
          i+=j ;
          for(;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t')&&(s[i]!='*'))break;
          if((s[i]>='0')&&(s[i]<='9')){
            for(j=0;i+j<n;j++)if((s[i+j]<'0')||(s[i+j]>'9'))break ;
            string val = s.substr(i,j) ;
  	  int num_val = stoi(val) ;
  	  if((num_val==0)||(num_val==1)){
              boost::unordered_map<string, int>::const_iterator t = 
                                            g.symbol_to_number.find(var);
              if(t != g.symbol_to_number.end()){
                solution_assignment[t->second] = num_val ;
              }
  	  }
  	}
        }
      }
    }
    return solution_assignment ;
  }

  static intvector_t LoadSolutionLP_SOLVE(linear & g, istream& in){
    const string (* const sol_symbol)(const string &) = sol_symbol_LP_SOLVE ;
    string s ;
    intvector_t solution_assignment(g.num_nodes, -1) ;
    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      if(s == "Actual values of the variables:")break;
    }
    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      int n = s.length();
      int i,j;
      for(i=0;i<n;i++)if((s[i]==' ')||(s[i]=='\t'))break ;
      string var = sol_symbol(s.substr(0,i)) ;
      if(var.find('.')==string::npos){
        for(;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t')&&(s[i]!='*'))break;
        if((s[i]>='0')&&(s[i]<='9')){
          for(j=0;i+j<n;j++)if((s[i+j]<'0')||(s[i+j]>'9'))break ;
          string val = s.substr(i,j) ;
  	int num_val = stoi(val) ;
  	if((num_val==0)||(num_val==1)){
            boost::unordered_map<string, int>::const_iterator t = 
                                          g.symbol_to_number.find(var);
            if(t != g.symbol_to_number.end()){
              solution_assignment[t->second] = num_val ;
            }
  	}
        }
      }
    }
    return solution_assignment ;
  }

  static intvector_t LoadSolutionSCIP(linear & g, istream& in){
    const string (* const sol_symbol)(const string &) = sol_symbol_CPLEX ;
    string s ;
    intvector_t solution_assignment(g.num_nodes, 0) ;
    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      if(s.find("objective value:") != string::npos)break;
    }
    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      if(s.find("==========") != string::npos)break;
      int n = s.length();
      int i,j;
      for(i=0;i<n;i++)if((s[i]==' ')||(s[i]=='\t'))break ;
      string var = sol_symbol(s.substr(0,i)) ;
      if(var.find('.')==string::npos){
        for(;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t')&&(s[i]!='*'))break;
        if(((s[i]>='0')&&(s[i]<='9'))||(s[i]=='.')){
          for(j=0;i+j<n;j++)if(((s[i+j]<'0')||(s[i+j]>'9'))&&(s[i+j]!='.'))break ;
          string val = s.substr(i,j) ;
          real_t    num_val_r = strtold(val.c_str(), NULL) ;
          integer_t num_val   = llroundl(num_val_r) ;
//	  cerr << var << " = " << val << " -> " << num_val_r << " -> " << num_val << endl ;
          if((num_val==0)||(num_val==1)){
            boost::unordered_map<string, int>::const_iterator t = 
                                          g.symbol_to_number.find(var);
            if(t != g.symbol_to_number.end()){
              solution_assignment[t->second] = num_val ;
            }
          }
        }
      }
    }
    return solution_assignment ;
  }

  static intvector_t LoadSolutionCBC(linear & g, istream& in){
    const string (* const sol_symbol)(const string &) = sol_symbol_CPLEX ;
    string s ;
    intvector_t solution_assignment(g.num_nodes, 0) ;
    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      if(s.find("Optimal - objective value ") != string::npos)break;
    }


    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      if(s.find("Total time (CPU seconds):") != string::npos)break;
      int n = s.length();
      int i,j,k;
      for(j=0;j<n;j++)if((s[j]!=' ')&&(s[j]!='\t')&&!((s[j]>='0')&&(s[j]<='9')))break ;
      for(i=j;i<n;i++)if((s[i]==' ')||(s[i]=='\t'))break ;
      string var = sol_symbol(s.substr(j,i-j)) ;
      if(var.find('.')==string::npos){
        for(;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t')&&(s[i]!='*'))break;
        if(((s[i]>='0')&&(s[i]<='9'))||(s[i]=='.')){
          for(j=0;i+j<n;j++)if(((s[i+j]<'0')||(s[i+j]>'9'))&&(s[i+j]!='.'))break ;
          string val = s.substr(i,j) ;
          real_t    num_val_r = strtold(val.c_str(), NULL) ;
          integer_t num_val   = llroundl(num_val_r) ;
//	  cerr << var << " = " << val << " -> " << num_val_r << " -> " << num_val << endl ;
          if((num_val==0)||(num_val==1)){
            boost::unordered_map<string, int>::const_iterator t = 
                                          g.symbol_to_number.find(var);
            if(t != g.symbol_to_number.end()){
              solution_assignment[t->second] = num_val ;
            }
          }
        }
      }
    }
    return solution_assignment ;
  }

  static intvector_t LoadSolutionGUROBI(linear & g, istream& in){
    const string (* const sol_symbol)(const string &) = sol_symbol_CPLEX ;
    string s ;
    intvector_t solution_assignment(g.num_nodes, 0) ;
    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      if(s.find("# Objective value = ") != string::npos)break;
    }

    while (true) {
      getline(in,s) ;
      if(in.eof())break;
      int n = s.length();
      int i,j,k;
      j=0;
      for(i=j;i<n;i++)if((s[i]==' ')||(s[i]=='\t'))break ;
      string var = sol_symbol(s.substr(j,i-j)) ;
      if(var.find('.')==string::npos){
        for(;i<n;i++)if((s[i]!=' ')&&(s[i]!='\t')&&(s[i]!='*'))break;
        if(((s[i]>='0')&&(s[i]<='9'))||(s[i]=='.')){
          for(j=0;i+j<n;j++)if(((s[i+j]<'0')||(s[i+j]>'9'))&&(s[i+j]!='.'))break ;
          string val = s.substr(i,j) ;
          real_t    num_val_r = strtold(val.c_str(), NULL) ;
          integer_t num_val   = llroundl(num_val_r) ;
//	  cerr << var << " = " << val << " -> " << num_val_r << " -> " << num_val << endl ;
          if((num_val==0)||(num_val==1)){
            boost::unordered_map<string, int>::const_iterator t = 
                                          g.symbol_to_number.find(var);
            if(t != g.symbol_to_number.end()){
              solution_assignment[t->second] = num_val ;
            }
          }
        }
      }
    }
    return solution_assignment ;
  }

  void linear::LoadSolution(const string & filename) {
    linear & g = *this ;
    ifstream fin(filename.c_str()) ;
    if(!fin.good())fatal_error("fail to open " + filename) ;
    g.solution_filename = filename ;
    LoadSolution(fin);
  }

  void linear::LoadSolution(istream & in) {
    real_t start = now() ;

    linear & g = *this ;
    assignvector_t assignment = assignvector_t(g.dimension());
    intvector_t int_assignment ;

    /**/ if(lp_mode == gurobi  )int_assignment = LoadSolutionGUROBI  (g,in);
    else if(lp_mode == glpsol  )int_assignment = LoadSolutionGLPSOL  (g,in);
    else if(lp_mode == lp_solve)int_assignment = LoadSolutionLP_SOLVE(g,in);
    else if(lp_mode == scip    )int_assignment = LoadSolutionSCIP    (g,in);
    else if(lp_mode == cbc     )int_assignment = LoadSolutionCBC     (g,in);

    for(int i=0;i<g.num_variables();i++){
      int j = g.variable[i] ;
      if((int_assignment[j] != 0) && (int_assignment[j] != 1)){
        vertex_t v = g.number_to_vertex[j] ;
        fatal_error(
	  "symbol " + g[v].symbol + " (=" + to_string(int_assignment[j])
	  + ", lp_mode == " + to_string(lp_mode) +
	  ") in {0,1} is not found "
	);
      }
      assignment[i] = int_assignment[j] ;
    }
    BeginOptimize();
    OptimizeOne(assignment);
    EndOptimize();
    g.score      = g.minimum ;

    real_t end = now() ;
    g.interval_to_loadsol = end - start ;
  }

  static int system2(
    const char * const file,
    const char * const argv[],
    const char * redirect
  ){
    cerr << "try to execute: " ;
    for(int i=0;argv[i];i++)cerr << argv[i] << " " ;
    if(redirect) cerr << "> " << redirect ;
    cerr << endl ;
    if(pid_t pid = fork()){
      while(true){
        int status ;
        wait(&status) ;
        /**/ if(WIFEXITED(status))   return WEXITSTATUS(status) ;
        else if(WIFSIGNALED(status)) return 256+WTERMSIG(status) ;
      }
    }else{
      if(redirect){
        close(1) ;
        int fd = creat(redirect, 0644) ;
        if(fd < 0) exit(1) ;
        if(fd != 1){
          dup2(fd,1);
          close(fd) ;
        }
      }
      execvp((const char *)file, (char * const *)argv) ;
      exit(1);
    }
  }

  static int ExecuteSolverGUROBI(
    const string & lp_filename,
    const string & solution_filename
  ){
    /* gurobi_cl ResultFile=G.sol G.lp > /dev/null */
    const string ResultFile   = "ResultFile=" + solution_filename ;
    const char * const argv[] = {
      "gurobi_cl", ResultFile.c_str(), lp_filename.c_str(), NULL
    } ;
    return system2(argv[0], argv, "/dev/null") ;
  }

  static int ExecuteSolverGLPSOL(
    const string & lp_filename,
    const string & solution_filename
  ){
    /* glpsol --lp G.lp -o G.sol > /dev/null */
    const char * const argv[] = {
      "glpsol", "--lp", lp_filename.c_str(), "-o", solution_filename.c_str(), NULL
    } ;
    return system2(argv[0], argv, "/dev/null") ;
  }

  static int ExecuteSolverLP_SOLVE(
    const string & lp_filename,
    const string & solution_filename
  ){
    /* lp_solve G.lp > G.sol */
    const char * const argv[] = { "lp_solve", lp_filename.c_str(), NULL } ;
    return system2(argv[0], argv, solution_filename.c_str()) ;
  }

  static int ExecuteSolverSCIP(
    const string & lp_filename,
    const string & solution_filename
  ){
    /* scip -f G.lp > G.sol */
    const char * const argv[] = { "scip", "-f", lp_filename.c_str(), NULL } ;
    return system2(argv[0], argv, solution_filename.c_str()) ;
  }

  static int ExecuteSolverCBC(
    const string & lp_filename,
    const string & solution_filename
  ){
    /* cbc G.lp solve solu G.sol */
    /* cbc G.lp solve solu stdout > G.sol */
    const char * const argv[] = {
      "cbc", lp_filename.c_str(), "solve", "solu", "stdout", NULL
    } ;
    return system2(argv[0], argv, solution_filename.c_str()) ;
  }

  int linear::ExecuteSolverX(void) {
    int error = 0 ;
    real_t start = now() ;
    switch(lp_mode){
      case gurobi  : error = ExecuteSolverGUROBI  (lp_filename, solution_filename); break ;
      case glpsol  : error = ExecuteSolverGLPSOL  (lp_filename, solution_filename); break ;
      case lp_solve: error = ExecuteSolverLP_SOLVE(lp_filename, solution_filename); break ;
      case scip    : error = ExecuteSolverSCIP    (lp_filename, solution_filename); break ;
      case cbc     : error = ExecuteSolverCBC     (lp_filename, solution_filename); break ;
      case nosolver: error = 0 ; break ;
      default      : error = 1 ; break ;
    }
    real_t end = now() ;
    interval_to_solve = end - start ;

    if(!error){
      if(lp_mode != nosolver) LoadSolution(solution_filename) ;
      else status = cannot_split ;
    }
    return error ;
  }

  void linear::ExecuteSolver(void) {
    int error = 0 ;

    real_t start = now() ;
    IntegerPrograming(lp_filename) ;
    real_t end = now() ;
    interval_to_convert = end - start ;

    if(lp_mode != anysolver){
      error = ExecuteSolverX() ;
    }else{
      for(lp_mode = anysolver + 1; lp_mode < nosolver; lp_mode++){
        if(lp_mode == lp_solve) IntegerPrograming(lp_filename) ;
        error = ExecuteSolverX() ;
        if(!error)break ;
      }
    }
    if(error){
      fatal_error("fail to execute IP solver");
    }
  }

  void linear::ExecuteSolverOmitTrivial(void) {
    int error = 0 ;

    real_t start = now() ;
    IntegerProgramingOmitTrivial(lp_filename) ;
    real_t end = now() ;
    interval_to_convert = end - start ;

    if(lp_mode != anysolver){
      error = ExecuteSolverX() ;
    }else{
      for(lp_mode = anysolver + 1; lp_mode < nosolver; lp_mode++){
        if(lp_mode == lp_solve) IntegerPrograming(lp_filename) ;
        error = ExecuteSolverX() ;
        if(!error)break ;
      }
    }
    if(error){
      fatal_error("fail to execute IP solver");
    }
  }

  void linear::ExecuteSolverCrypto2016(void) {
    int error = 0 ;

    real_t start = now() ;
    IntegerProgramingCrypto2016(lp_filename) ;
    real_t end = now() ;
    interval_to_convert = end - start ;

    if(lp_mode != anysolver){
      error = ExecuteSolverX() ;
    }else{
      for(lp_mode = anysolver + 1; lp_mode < nosolver; lp_mode++){
        if(lp_mode == lp_solve) IntegerPrograming(lp_filename) ;
        error = ExecuteSolverX() ;
        if(!error)break ;
      }
    }
    if(error){
      fatal_error("fail to execute IP solver");
    }
  }

  ostream & operator<<(ostream& out, linear & g) {
    if(g.format != tango){
      out << "/*" << endl ;
      out << "== Solution Loader ==" << endl ;
      out << "interval_to_convert = " << g.interval_to_convert << endl ;
      if(g.is_split){
        out << "interval_to_solve   = " << g.interval_to_solve   << endl ;
        out << "interval_to_loadsol = " << g.interval_to_loadsol << endl ;
        out << "solution_filename=" << g.solution_filename << endl ;
        out << g.score ;
      }else{
        out << "fail to split." << endl ;
      }
      out << "*/" << endl ;
    }
    return out ;
  }
}

#ifdef LINEAR_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  linear g ;
  g.SetRatio(2.0) ;
  g.LoadSolution("G.sol") ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
