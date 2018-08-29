#include "semantic.h"

#include <iostream>
#include <string>
#include <boost/graph/property_iter_range.hpp>
#include <boost/algorithm/string.hpp>
#include <stdint.h>
#include "ast_node_def.h"

namespace findsplit {
using namespace std   ;
using namespace boost ;

#ifdef SEMANTIC_DEBUG
static const bool semantic_debug = false ;
#else
static const bool semantic_debug = false ;
#endif

static void semantic_error(cell * L, const string & message)
{
  cell * lineno = L->cdr ;
  int    n      = (intptr_t)lineno->car ;
  cerr << "line " << n << " semantic error: "
       << message << "." << endl ;
  exit(1);
}

static const string decl_statement
  = "declaration statement 'SYMBOL[,...]'"        ;
static const string arrow_statement
  = "dependency statement 'SYMBOL[,...]->SYMBOL'" ;
static const string colon_statement
  = "assignment statement 'SYMBOL[,...]:NUMBER'"  ;
static const string colonsn_statement
  = "assignment statement 'SYMBOL[,...]:[~]SYMBOL'"  ;
static const string pairing_statement
  = "pairing statement '(SYMBOL,SYMBOL)'"  ;

// ============================================================
// Tools
// ============================================================

inline static void unexpected_error(
  cell * L, 
  const string & LOCATION
){
  semantic_error(L, "unexpected error in " + LOCATION + "()") ;
}

inline static void not_allowed_error(
  cell * L, 
  const string & STATEMENT,
  const string & BLOCK
){
  semantic_error(L, STATEMENT + " is not allowed in " + BLOCK + " block");
}

static int error_if_not_defined(
  const string  & description,
  const string  & name,
  cell * L,
  semantic_analyzer & g
){
  int i ;
  if(g.not_found(name,i)){
    semantic_error(L, description + " '" + name + "' is not defined");
  }
  return i ;
}

static inline vertex_t symbol_to_vertex_with_register(
  const string & name,
  semantic_analyzer & g
){
  return g.symbol_to_vertex(name) ;
}

static vertex_t symbol_to_vertex_without_register(
  const string & name,
  cell * L,
  semantic_analyzer & g
){
  int i = error_if_not_defined("symbol name", name, L, g) ;
  return g.number_to_vertex[i] ;
}

// ============================================================
// Dependencies
// ============================================================

static void process_Dependencies_decl_symlist(
  cell * L,
  char * comment_,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    string comment = comment_ ;
    size_t begin_index = comment.find('$',0) ;
    size_t end_index ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr){
      string name     = M->car ;
      string texlabel = ""     ;

      if(begin_index != string::npos){
        end_index = comment.find('$',begin_index+1) ;
        if(end_index != string::npos){
	  size_t length = end_index - begin_index + 1 ;
	  texlabel = comment.substr(begin_index,length) ;
          begin_index = comment.find('$',end_index+1) ;
        }
      }

      g.register_symbol(name, texlabel) ;
    }
  }else unexpected_error(L,"process_Dependencies_decl_symlist");
}

static void process_Dependencies_decl(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * comment     = symbol_list->cdr ;
  process_Dependencies_decl_symlist(
    (cell*)symbol_list->car, comment->car, g) ;
}

static void process_Dependencies_arrow_register(
  cell * L,
  const vertex_t & dst,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr){
      string   src_name = M->car ;
      vertex_t src      = symbol_to_vertex_with_register(src_name,g) ;
      if(semantic_debug){
        cout << "add_edge(" << 
	  g[src].number << "," << g[dst].number << ")" << endl ;
      }
      add_edge(src, dst, g) ;
    }
  }else unexpected_error(L,"process_Dependencies_arrow_register");
}

static void process_Dependencies_arrow(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * symbol      = symbol_list->cdr ;
  string     dst_name    = symbol->car ;
  vertex_t   dst         = symbol_to_vertex_with_register(dst_name,g) ;
  process_Dependencies_arrow_register((cell*)symbol_list->car, dst, g) ;
}

static void process_Dependencies_colon(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colon_statement, "[Dependencies]") ;
}

static void process_Dependencies_colons(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Dependencies]") ;
}

static void process_Dependencies_colonn(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Dependencies]") ;
}

static void process_Dependencies_pairing(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno  = L->cdr ;
  cell     * symbol0 = lineno->cdr ;
  cell     * symbol1 = symbol0->cdr ;
  cell     * comment = symbol1->cdr ;

  string name0 = symbol0->car ;
  string name1 = symbol1->car ;

//  error_if_not_defined("1st argument of pairing", name0, L, g) ;
//  error_if_not_defined("2nd argument of pairing", name1, L, g) ;

  g.register_pairing(name0, name1) ;
}

// ============================================================
// Pairings
// ============================================================

static void process_Pairings_decl_symlist(
  cell * L,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    const string message =
      decl_statement + " must have 2 SYMBOLs in [Pairings] block" ;
  
    cell * arg0 = lineno->cdr ; if(!arg0)semantic_error(L, message);
    cell * arg1 = arg0->cdr   ; if(!arg1)semantic_error(L, message);
    cell * arg2 = arg1->cdr   ; if( arg2)semantic_error(L, message);
  
    string name0 = arg0->car ;
    string name1 = arg1->car ;
  
    error_if_not_defined("1st argument of pairing", name0, L, g) ;
    error_if_not_defined("2nd argument of pairing", name1, L, g) ;
  
    g.register_pairing(name0, name1) ;
  }else unexpected_error(L,"process_Pairings_decl_symlist");
}

static void process_Pairings_decl(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * comment     = symbol_list->cdr ;
  process_Pairings_decl_symlist((cell*)symbol_list->car, g) ;
}

static void process_Pairings_arrow(cell * L, semantic_analyzer & g){
  not_allowed_error(L, arrow_statement, "[Pairings]");
}

static void process_Pairings_colon(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colon_statement, "[Pairings]");
}

static void process_Pairings_colons(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Pairings]");
}

static void process_Pairings_colonn(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Pairings]");
}

static void process_Pairings_pairing(cell * L, semantic_analyzer & g){
  not_allowed_error(L, pairing_statement, "[Pairings]");
}

// ============================================================
// Prohibits
// ============================================================

static void process_Prohibits_decl_symlist(
  cell * L,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr) {
      string name = M->car ;
      vertex_t v = symbol_to_vertex_without_register(name,L,g) ;
      g[v].is_prohibited = true ;
    }
  }else unexpected_error(L,"process_Prohibits_decl_symlist");
}

static void process_Prohibits_decl(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * comment     = symbol_list->cdr ;
  process_Prohibits_decl_symlist((cell*)symbol_list->car, g) ;
}

static void process_Prohibits_arrow(cell * L, semantic_analyzer & g){
  not_allowed_error(L, arrow_statement, "[Prohibits]");
}

static void process_Prohibits_colon_register(
  cell * L,
  const int n,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr){
      string name = M->car ;
      vertex_t v = symbol_to_vertex_without_register(name,L,g) ;
      g[v].is_prohibited = true ;
      g[v].is_fix        = true ;
      g[v].value         = n    ;
    }
  }else unexpected_error(L,"process_Prohibits_colon");
}

static void process_Prohibits_colon(
  cell * L,
  semantic_analyzer & g
){
  cell   * lineno      = L->cdr ;
  cell   * symbol_list = lineno->cdr ;
  cell   * number      = symbol_list->cdr ;
  string   nstr        = number->car ;
  int      n           = stoi(nstr) ;
  if((n!=0) && (n!=1))
    semantic_error(L,"assignment value must be in [0,1] in [Prohibits] block");
  process_Prohibits_colon_register((cell*)symbol_list->car, n, g) ;
}

static void process_Prohibits_colons_register(
  cell * L,
  int number,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr){
      string name = M->car ;
      vertex_t v = symbol_to_vertex_without_register(name,L,g) ;
      g[v].is_prohibited = true ;
      g[v].same_as.push_back(number);
    }
  }else unexpected_error(L,"process_Prohibits_colons");
}

static void process_Prohibits_colons(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * symbol      = symbol_list->cdr ;
  string     name        = symbol->car ;
  vertex_t   v           = symbol_to_vertex_without_register(name,L,g) ;
  g[v].is_prohibited     = true ;
  process_Prohibits_colons_register((cell*)symbol_list->car, g[v].number, g) ;
}

static void process_Prohibits_colonn_register(
  cell * L,
  int number,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr){
      string name = M->car ;
      vertex_t v = symbol_to_vertex_without_register(name,L,g) ;
      g[v].is_prohibited = true ;
      g[v].not_same_as.push_back(number);
    }
  }else unexpected_error(L,"process_Prohibits_colonn");
}

static void process_Prohibits_colonn(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * symbol      = symbol_list->cdr ;
  string     name        = symbol->car ;
  vertex_t   v           = symbol_to_vertex_without_register(name,L,g) ;
  g[v].is_prohibited     = true ;
  process_Prohibits_colonn_register((cell*)symbol_list->car, g[v].number, g) ;
}

static void process_Prohibits_pairing(cell * L, semantic_analyzer & g){
  not_allowed_error(L, pairing_statement, "[Prohibits]");
}

// ============================================================
// Constraints
// ============================================================

static void process_Constraints_decl_symlist(
  cell * L,
  vertex_t c,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr) {
      string name = M->car ;
      vertex_t v = symbol_to_vertex_without_register(name,L,g) ;
      add_edge(v, c, g) ;
    }
  }else unexpected_error(L,"process_Constraints_decl_symlist");
}

static void process_Constraints_decl(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * comment     = symbol_list->cdr ;
  vertex_t c = g.register_constraint() ;
  g[c].is_prohibited = true ;
  g[c].is_constraint = true ;
  process_Constraints_decl_symlist((cell*)symbol_list->car, c, g) ;
}

static void process_Constraints_arrow(cell * L, semantic_analyzer & g){
  not_allowed_error(L, arrow_statement, "[Constraints]");
}

static void process_Constraints_colon(
  cell * L,
  semantic_analyzer & g
){
  cell   * lineno      = L->cdr ;
  cell   * symbol_list = lineno->cdr ;
  cell   * number      = symbol_list->cdr ;
  string   nstr        = number->car ;
  int      n           = stoi(nstr) ;
  if((n!=0) && (n!=1))
    semantic_error(L,"assignment value must be in [0,1] in [Constraints] block");
  vertex_t c = g.register_constraint() ;
  g[c].is_prohibited = true ;
  g[c].is_fix        = true ;
  g[c].value         = n    ;
  g[c].is_constraint = true ;
  process_Constraints_decl_symlist((cell*)symbol_list->car, c, g) ;
}

static void process_Constraints_colons(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Constraints]");
}

static void process_Constraints_colonn(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Constraints]");
}

static void process_Constraints_pairing(
  cell * L, semantic_analyzer & g){
  not_allowed_error(L, pairing_statement, "[Constraints]");
}

// ============================================================
// Priority
// ============================================================

static void process_Priority_decl_symlist(
  cell * L,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr) {
      string name = M->car ;
      vertex_t v = symbol_to_vertex_without_register(name,L,g) ;
      g[v].priority = g.num_priorities ;
    }
    g.num_priorities++ ;
  }else unexpected_error(L,"process_Priority_decl_symlist");
}

static void process_Priority_decl(
  cell * L,
  semantic_analyzer & g
){
  cell     * lineno      = L->cdr ;
  cell     * symbol_list = lineno->cdr ;
  cell     * comment     = symbol_list->cdr ;
  process_Priority_decl_symlist((cell*)symbol_list->car, g) ;
}

static void process_Priority_arrow(cell * L, semantic_analyzer & g){
  not_allowed_error(L, arrow_statement, "[Priority]");
}

static void process_Priority_colon(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colon_statement, "[Priority]");
}
static void process_Priority_colons(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Priority]");
}
static void process_Priority_colonn(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Priority]");
}

static void process_Priority_pairing(cell * L, semantic_analyzer & g){
  not_allowed_error(L, pairing_statement, "[Priority]");
}

// ============================================================
// Weight
// ============================================================

static void process_Weight_decl(cell * L, semantic_analyzer & g){
  not_allowed_error(L, decl_statement, "[Weight]");
}

static void process_Weight_arrow(cell * L, semantic_analyzer & g){
  not_allowed_error(L, arrow_statement, "[Weight]");
}

static void process_Weight_colon_register(
  cell * L,
  const real_t n,
  semantic_analyzer & g
){
  /* */ if (L->car == AST_SYMLIST) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr){
      string name = M->car ;
      vertex_t v = symbol_to_vertex_without_register(name,L,g) ;
      g[v].weight = n ;
    }
  }else unexpected_error(L,"process_Weight_colon");
}

static void process_Weight_pairing(cell * L, semantic_analyzer & g){
  not_allowed_error(L, pairing_statement, "[Weight]");
}

static void process_Weight_colon(cell * L, semantic_analyzer & g){
  cell   * lineno      = L->cdr ;
  cell   * symbol_list = lineno->cdr ;
  cell   * number      = symbol_list->cdr ;
  string   nstr        = number->car ;
//  int n           = stoi(nstr) ;
  real_t   n           = stold(nstr) ;
  process_Weight_colon_register((cell*)symbol_list->car, n, g) ;
}

static void process_Weight_colons(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Weight]");
}

static void process_Weight_colonn(cell * L, semantic_analyzer & g){
  not_allowed_error(L, colonsn_statement, "[Weight]");
}

// ============================================================
// BLOCK
// ============================================================

static void process_Dependencies(cell * L, semantic_analyzer & g){
  /**/ if (L->car == AST_DECL   ) process_Dependencies_decl   (L,g);
  else if (L->car == AST_ARROW  ) process_Dependencies_arrow  (L,g);
  else if (L->car == AST_COLON  ) process_Dependencies_colon  (L,g);
  else if (L->car == AST_COLONS ) process_Dependencies_colons (L,g);
  else if (L->car == AST_COLONN ) process_Dependencies_colonn (L,g);
  else if (L->car == AST_PAIRING) process_Dependencies_pairing(L,g);
  else if (L->car == AST_COMMENT) ;
  else unexpected_error(L,"process_Dependencies");
}

static void process_Pairings(cell * L, semantic_analyzer & g){
  /**/ if (L->car == AST_DECL   ) process_Pairings_decl   (L,g);
  else if (L->car == AST_ARROW  ) process_Pairings_arrow  (L,g);
  else if (L->car == AST_COLON  ) process_Pairings_colon  (L,g);
  else if (L->car == AST_COLONS ) process_Pairings_colons (L,g);
  else if (L->car == AST_COLONN ) process_Pairings_colonn (L,g);
  else if (L->car == AST_PAIRING) process_Pairings_pairing(L,g);
  else if (L->car == AST_COMMENT) ;
  else unexpected_error(L,"process_Pairings");
}

static void process_Prohibits(cell * L, semantic_analyzer & g){
  /**/ if (L->car == AST_DECL   ) process_Prohibits_decl   (L,g);
  else if (L->car == AST_ARROW  ) process_Prohibits_arrow  (L,g);
  else if (L->car == AST_COLON  ) process_Prohibits_colon  (L,g);
  else if (L->car == AST_COLONS ) process_Prohibits_colons (L,g);
  else if (L->car == AST_COLONN ) process_Prohibits_colonn (L,g);
  else if (L->car == AST_PAIRING) process_Prohibits_pairing(L,g);
  else if (L->car == AST_COMMENT) ;
  else unexpected_error(L,"process_Prohibits");
}

static void process_Constraints(cell * L, semantic_analyzer & g){
  /**/ if (L->car == AST_DECL   ) process_Constraints_decl   (L,g);
  else if (L->car == AST_ARROW  ) process_Constraints_arrow  (L,g);
  else if (L->car == AST_COLON  ) process_Constraints_colon  (L,g);
  else if (L->car == AST_COLONS ) process_Constraints_colons (L,g);
  else if (L->car == AST_COLONN ) process_Constraints_colonn (L,g);
  else if (L->car == AST_PAIRING) process_Constraints_pairing(L,g);
  else if (L->car == AST_COMMENT) ;
  else unexpected_error(L,"process_Prohibits");
}

static void process_Priority(cell * L, semantic_analyzer & g){
  /**/ if (L->car == AST_DECL   ) process_Priority_decl   (L,g);
  else if (L->car == AST_ARROW  ) process_Priority_arrow  (L,g);
  else if (L->car == AST_COLON  ) process_Priority_colon  (L,g);
  else if (L->car == AST_COLONS ) process_Priority_colons (L,g);
  else if (L->car == AST_COLONN ) process_Priority_colonn (L,g);
  else if (L->car == AST_PAIRING) process_Priority_pairing(L,g);
  else if (L->car == AST_COMMENT) ;
  else unexpected_error(L,"process_Priority");
}

static void process_Weight(cell * L, semantic_analyzer & g){
  /**/ if (L->car == AST_DECL   ) process_Weight_decl   (L,g);
  else if (L->car == AST_ARROW  ) process_Weight_arrow  (L,g);
  else if (L->car == AST_COLON  ) process_Weight_colon  (L,g);
  else if (L->car == AST_COLONS ) process_Weight_colons (L,g);
  else if (L->car == AST_COLONN ) process_Weight_colonn (L,g);
  else if (L->car == AST_PAIRING) process_Weight_pairing(L,g);
  else if (L->car == AST_COMMENT) ;
  else unexpected_error(L,"process_Weight");
}

static void process_block(cell * L, semantic_analyzer & g){
  if (L->car == AST_BLOCK) {
    cell   * lineno = L->cdr ;
    cell   * symbol = lineno->cdr ;
    string   s      = symbol->car ;
    void (*proc)(cell*, semantic_analyzer & g) = NULL ;
    /**/ if(iequals(s,"Dependencies"))proc = process_Dependencies ;
    else if(iequals(s,"Pairings"    ))proc = process_Pairings ;
    else if(iequals(s,"Prohibits"   ))proc = process_Prohibits ;
    else if(iequals(s,"Constraints" ))proc = process_Constraints ;
    else if(iequals(s,"Priority"    ))proc = process_Priority ;
    else if(iequals(s,"Weight"      ))proc = process_Weight ;
    else semantic_error(L, "unknown block [" + s + "]") ;
    if(proc)
      for(cell * M = symbol->cdr ; M ; M = M->cdr)
        proc((cell*)M->car,g) ;
  }else unexpected_error(L,"process_block");
}

// ============================================================
// PROG
// ============================================================

static void process_prog(cell * L, semantic_analyzer & g){
  if (L->car == AST_PROG) {
    cell * lineno = L->cdr ;
    for(cell * M = lineno->cdr ; M ; M = M->cdr)
      process_block((cell*)M->car, g) ;
  }else unexpected_error(L,"process_prog");
}

// ============================================================
// post_process_prog
// ============================================================

static void SetMinimumPriority(semantic_analyzer & g){
  bool flag = false ;
  graph_traits<semantic_analyzer>::vertex_iterator vi, vi_end ;
  for(tie(vi,vi_end)=vertices(g); vi!=vi_end; vi++){
    if(g[*vi].priority == -1){
      g[*vi].priority = g.num_priorities ;
      flag = true ;
    }
  }
  if(flag) g.num_priorities++ ;
}

static void post_process_prog(semantic_analyzer & g){
  SetMinimumPriority(g) ;
}

// ============================================================
// semantic_analyzer()
// ============================================================

extern "C" int yyparse(void) ;
extern "C" cell * AST ;

semantic_analyzer::semantic_analyzer(){
  real_t start = now() ;
  semantic_analyzer & g = (*this) ;
  if(yyparse() == 0){
    process_prog(AST, g) ;
    post_process_prog(g) ;
  }else{
    // syntactic error
    exit(1) ;
  }
  real_t end = now() ;
  interval_to_load = end - start ;
}

ostream & operator<<(ostream& out, const semantic_analyzer & g){
  return
    out << (const pairing_graph &) g 
        << "/* "                       << endl
	<< "== semantic_analyzer =="   << endl
	<< "interval_to_load    = " << g.interval_to_load << endl
        << " */"                       << endl ;
}

}

#ifdef SEMANTIC_DEBUG

using namespace findsplit ;

int main(int argc, char **argv) {
  semantic_analyzer g ;
  cout << g ;
  return EXIT_SUCCESS ;
}

#endif
