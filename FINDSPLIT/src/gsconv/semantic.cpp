#include "lisp.h"
#include "ast.h"
#include "symbol_stack.h"
#include "semantic.h"

#include <iostream>
#include <vector>
#include <stdint.h>
#include <boost/unordered_map.hpp>
#include <string.h>

#include "operation.h"
#include "preamble.h"

using namespace std   ;
using namespace boost ;

extern int reconstruct ;
extern unordered_map<string, int> group_of ;
extern vector<string> reconstruct_stack ;
static inline string reconstruct_stack_pop_back(){
  if(reconstruct_stack.size()>0){
    string s = reconstruct_stack.back() ;
    reconstruct_stack.pop_back() ;
    return s ;
  }else{
    cout << "reconstruct_stack error" << endl ;
    return "" ;
  }
}

/*
static int nest = 0 ;
static inline void indent(){
//  putchar('%');
  for(int i=0;i<2*nest;i++) putchar(' ') ;
}
static inline void print_reconstruct_stack(){
  static int c = 0 ;
  c++ ;
  indent();
  cout << "recon " << c << " : ------------------------" << endl ;
  for(int i=0;i<reconstruct_stack.size();i++){
    indent();
    cout << "recon " << c << " : " << reconstruct_stack[i] << endl ;
  }
}
*/

static cell * unexpected(const string & LOCATION){
  cerr << "unexpected error in " + LOCATION + "()" << endl ;
  return NIL() ;
}

static void semantic_error(cell * L, const string & message) {
  cell * lineno = L->cdr ;
  int    n      = (intptr_t)lineno->car ;
  cerr << "line " << n << " semantic error: "
       << message << "." << endl ;
  exit(1);
}

vector<identifier_stack_entry> identifier_stack ;
static vector<char *>                 type_stack       ;
static vector<char *>                 crs_stack        ;
static vector<int>                    frame_stack      ;

static void print_identifier_stack(){
  for(int i = identifier_stack.size()-1; i>=0; i--){
    printf("%s,",identifier_stack[i].symbol) ;
  }
  printf("\n");
}

int find_identifier(char * symbol){

  int R = -1 ;

  if(symbol){
    for(int i = identifier_stack.size()-1; i>=0; i--){
      if(!strcmp(symbol,identifier_stack[i].symbol)){
        R = i ;
        break ;
      }
    }
  }

  return R ;
}

int find_identifier(cell * var){
  cell * opt1 = var->cdr ;
  cell * opt2 = opt1->cdr ;
  cell * opt3 = opt2->cdr ;
  cell * arg1 = opt3->cdr ;
  char * symbol = arg1->car ;
  return find_identifier(symbol) ;
}

void push_identifier(char * symbol, char * type_name, cell * note){
  identifier_stack_entry e = {
    symbol, type_name, note, note?LIST(note):NULL
  } ;
  identifier_stack.push_back(e);
//  printf("%% push_identifier(%s, %s, %p)\n", symbol, type_name, note) ;
}

void push_identifier(cell * var, char * type_name, cell * note){
  cell * opt1 = var->cdr ;
  cell * opt2 = opt1->cdr ;
  cell * opt3 = opt2->cdr ;
  cell * arg1 = opt3->cdr ;
  char * symbol = arg1->car ;
  push_identifier(symbol, type_name, note);
}

int top_of_identifier_stack(){
  return identifier_stack.size() ;
}

void restore_identifier_stack(int n){
  while(identifier_stack.size() > n){
    identifier_stack.pop_back() ;
  }
}

void push_frame(){
  frame_stack.push_back(top_of_identifier_stack());
  printf("%% push_frame="); print_identifier_stack() ;
}

void pop_frame(){
  printf("%% pop_frame="); print_identifier_stack() ;
  int tos = frame_stack.back() ;
  restore_identifier_stack(tos) ;
  frame_stack.pop_back() ;
  printf("%% pop_frame="); print_identifier_stack() ;
}

int top_of_frame(){
  return frame_stack.size()?frame_stack.back():0 ;
}

// static cell * process_all(cell * L) ;

#define DEBUG_INTERPRETER

#ifdef DEBUG_INTERPRETER
// #  define debug_printf(...)  fprintf(stderr, __VA_ARGS__)
#  define debug_printf(...)  printf(__VA_ARGS__)
#else 
#  define debug_printf(...)
#endif

// =====================================================================
// real process for literal
// =====================================================================

static cell * process_TYPE(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  const char * type_name = symbol_to_type_name(arg0->car) ;
  if(reconstruct)reconstruct_stack.push_back(string(arg0->car)) ;
  return LIST(type_name) ;
}

static cell * process_IDENTIFIER(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0 = lineno->cdr ;
  int i = find_identifier(arg0->car) ;
  if(reconstruct)reconstruct_stack.push_back(string(arg0->car)) ;
  if(i<0){
    return LIST(TYPE_IDENTIFIER, NULL,NULL,NULL, arg0->car ) ;
  }else{
// printf("process_IDENTIFIER(%s) -> %d (%s)\n",arg0->car, i, identifier_stack[i].type_name);
    cell * R = LIST(identifier_stack[i].type_name, NULL,NULL,NULL, arg0->car) ;
// OPERATION_PRINT("process_IDENTIFIER return R=[",R,"];\n") ;
    return R ;
  }
}

static cell * process_NUMBER (cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  if(reconstruct)reconstruct_stack.push_back(string(arg0->car)) ;
  return LIST(TYPE_i,NULL,NULL,NULL) ;
}

static cell * process_STRING_INLINE(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cout << arg0->car << endl ;
  return NIL();
}

static string reconstruct_INLINE(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;

  string s = "inline \"" ;
  int n = strlen(arg0->car) ;
  for(int i=0;i<n;i++){
    char c = arg0->car[i] ;
    switch(c){
      case '"': s += '\\' ;
      default : s += c ;
    }
  }
  s += "\"" ;
  return s ;
}

// =====================================================================
// real process
// =====================================================================

static cell * process_INLINE(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  if(reconstruct){
    reconstruct_stack.push_back(reconstruct_INLINE((cell*)arg0->car)) ;
    return NIL();
  }else{
    return process_STRING_INLINE((cell*)arg0->car) ;
  }
}

static cell * process_DECL(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * var    = process_all((cell*)arg0->car) ;
  if(reconstruct){
    string var_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(var_s) ;
  }
  int i = find_identifier(var) ;
  if(i>=top_of_frame()){
    semantic_error(L,
      string("redefine variable: ") + 
      identifier_stack[i].type_name + " " +
      identifier_stack[i].symbol 
    ) ;
  }else if(i>=0){
    printf("%% overlaid variable: %s %s -> %s %s */\n",
      identifier_stack[i].type_name, identifier_stack[i].symbol,
      type_stack.back(), first_arg(var)
    );
  }
  push_identifier(var, type_stack.back()) ;
  var->car = type_stack.back() ;

/*
  if(reconstruct){
    string s ;
    if(var->car == TYPE_i  ) s = "integer "+first_arg(var)+";" ; else
    if(var->car == TYPE_tc ) s = "target  "+first_arg(var)+";" ; else
    if(var->car == TYPE_crs) s = "crs     "+first_arg(var)+";" ; else
    if(var->car == TYPE_g  ){
      int gtype = group_of[first_arg(var)] ;
      switch(gtype){
        case  1: s = "group0  "+first_arg(var)+";" ; break ;
        case  2: s = "group1  "+first_arg(var)+";" ; break ;
        case  3: s = "duplex  "+first_arg(var)+";" ; break ;
        default: s = "// unused "+first_arg(var)+";" ; break;
      }
    }
    reconstruct_stack.push_back(s);
  }
*/

  return var ;
}

void register_value(int i, cell * val){
  if(i>=0){
    identifier_stack[i].note = val ;
#if 0
    if(identifier_stack[i].history){
      identifier_stack[i].history =
        ast_append_lisp(identifier_stack[i].history, LIST(val)) ;
    }else{
      identifier_stack[i].history = LIST(val) ;
    }
#endif
  }
}

void register_value(char * symbol, cell * val){
  int i = find_identifier(symbol) ;
  register_value(i, val) ;
}

void register_value(cell * var, cell * val){
  int i = find_identifier(var) ;
  register_value(i, val) ;
}

static cell * ASSIGN_VAR_RAW(cell * L, cell * var, cell * val, int reduce){
  int i = find_identifier(var) ;
  if(i<0){
    printf("%% WARNING: variable %s is not declared.\n",first_arg(var)) ;
    push_identifier(var, TYPE_IDENTIFIER) ;
    i = find_identifier(var) ;
  }

  if(type_of(var) == TYPE_IDENTIFIER){
    if(
      identifier_stack[i].type_name == TYPE_IDENTIFIER
    ){
      char * type_name = val->car ;
      if(type_name == TYPE_t) type_name = TYPE_tc ;
      identifier_stack[i].type_name = type_name ;
//      printf("%% change type of %s: %s -> %s\n", first_arg(var), type_of(var), type_name);
      var->car = type_name ;
    }else{
      var->car = identifier_stack[i].type_name ;
    }
  }

  if(type_of(val) == TYPE_IDENTIFIER){
    int j = find_identifier(val) ;
    if(j<0){
      printf("%% WARNING: variable %s is not declared.\n",first_arg(val)) ;
      push_identifier(val, TYPE_IDENTIFIER) ;
      j = find_identifier(val) ;
    }
    if(
      identifier_stack[j].type_name == TYPE_IDENTIFIER
    ){
      char * type_name = var->car ;
      if(type_name == TYPE_t) type_name = TYPE_tc ;
      identifier_stack[j].type_name = type_name ;
      val->car = type_name ;
    }else{
      val->car = identifier_stack[j].type_name ;
    }
  }

  if( ! same_category(var, val) ){
    semantic_error(L, 
      string("invalid assignment for variable: ") +
      var->car + " " + identifier_stack[i].symbol + " = " + val->car 
    );
  }

  if(i>=0) register_value(i,val) ;

  if(reduce){
    if(var->car == TYPE_g){
      OPERATION_PRINT_RAW(val); printf(" -> ") ;
      OPERATION_PRINT_RAW(var); printf("\n") ;
    }
  }

  if(i>=0) return LIST(identifier_stack[i].type_name,NULL,NULL,NULL,identifier_stack[i].symbol) ;
  else return var ;

}

static cell * ASSIGN_VAR(cell * L, cell * var, cell * val){
  return ASSIGN_VAR_RAW(L, var, val, 1) ;
}

static cell * process_ASSIGN(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * var    = process_all((cell*)arg0->car) ;
  cell * val    = process_all((cell*)arg1->car) ;

  if(reconstruct){
    string val_s = reconstruct_stack_pop_back() ;
    string var_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(var_s+"="+val_s) ;
  }

  return ASSIGN_VAR(L, var, val) ;
}

static cell * process_INIT(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * var    = process_all((cell*)arg0->car) ;
  cell * val    = process_all((cell*)arg1->car) ;

  if(reconstruct){
    string val_s = reconstruct_stack_pop_back() ;
    string var_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(var_s+"="+val_s) ;
  }

  push_identifier(var, type_stack.back()) ;

  ASSIGN_VAR(L, var, val) ;
  return NIL() ;
}

static cell * process_INIT_LIST(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * type   = process_all((cell*)arg0->car) ;
  string type_s = (reconstruct) ? reconstruct_stack_pop_back() : "" ;
  int    group  = (type_s == "group") ;
  string arg_s  = "" ;

  type_stack.push_back(type->car) ;
  int current_gtype = -1 ;
  for(arg0=arg0->cdr;arg0;arg0=arg0->cdr){
    process_all((cell*)arg0->car) ;
    if(reconstruct){
      if(group){
        string s = reconstruct_stack_pop_back() ;
        int gtype = group_of[s] ;
        if(gtype == current_gtype){
	  arg_s += ", " + s ;
	}else{
	  current_gtype = gtype ;
          switch(gtype){
            case  1: s = "group0 "+ s ; break ;
            case  2: s = "group1 "+ s ; break ;
            case  3: s = "duplex "+ s ; break ;
            default: s = "unused "+ s ; break ;
          }
          if(arg_s == "") arg_s  =       s ;
          else            arg_s += "; " + s ;
	}
      }else{
        if(arg_s == "") arg_s  =       reconstruct_stack_pop_back() ;
        else            arg_s += "," + reconstruct_stack_pop_back() ;
      }
    }
  }
  if(reconstruct){
    if(group){
      reconstruct_stack.push_back(arg_s) ;
    }else{
      reconstruct_stack.push_back(type_s+" "+arg_s) ;
    }
  }
  type_stack.pop_back() ;
  return NIL() ;
}


static cell * process_UPLUS(cell * L) {
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * val    = process_all((cell*)arg0->car) ;

  if(reconstruct){
    string val_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back("+" + val_s) ;
  }

  if ( ! is_integer(val) )
    semantic_error(L, string("invalid unary plus operation for ") + val->car );

  return val ;
}

static cell * process_UMINUS (cell * L) {
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * val    = process_all((cell*)arg0->car) ;

  if(reconstruct){
    string val_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back("-" + val_s) ;
  }

  if ( ! is_integer(val) )
    semantic_error(L, string("invalid unary minus operation for ") + val->car );

  return val ;
}

static cell * process_ADD(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * val0   = process_all((cell*)arg0->car) ;
  cell * val1   = process_all((cell*)arg1->car) ;

  if(reconstruct){
    string val1_s = reconstruct_stack_pop_back() ;
    string val0_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(val0_s + "+" + val1_s) ;
  }

  return OPERATION_ADD(val0,val1,L) ;
}

static cell * process_MUL(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * val0   = process_all((cell*)arg0->car) ;
  cell * val1   = process_all((cell*)arg1->car) ;

  if(reconstruct){
    string val1_s = reconstruct_stack_pop_back() ;
    string val0_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(val0_s + "*" + val1_s) ;
  }

  return OPERATION_MUL(val0,val1,L) ;
}

static cell * process_POW(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * val0   = process_all((cell*)arg0->car) ;
  cell * val1   = process_all((cell*)arg1->car) ;

  if(reconstruct){
    string val1_s = reconstruct_stack_pop_back() ;
    string val0_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(val0_s + "^" + val1_s) ;
  }

  return OPERATION_POW(val0,val1,L) ;
}

static cell * process_LOGICALAND (cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * val0   = process_all((cell*)arg0->car) ;
  cell * val1   = process_all((cell*)arg1->car) ;

  if(reconstruct){
    string val1_s = reconstruct_stack_pop_back() ;
    string val0_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(val0_s + "&&" + val1_s) ;
  }

  return OPERATION_LOGICALAND(val0,val1,L) ;
}

static cell * process_EQ(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;

  cell * left   = process_all((cell*)arg0->car) ;
  cell * right  = process_all((cell*)arg1->car) ;

  cell * R = NULL ;

  if(reconstruct){
    string right_s = reconstruct_stack_pop_back() ;
    string left_s  = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(left_s + "==" + right_s) ;
  }

  if(is_integer(left) && is_integer(right)){
    R = OPERATION_ADD(left,right,L) ;
  }else if(
    (is_group(left) && is_group(right)) ||
    (is_target(left) && is_target(right))
  ){
    R = OPERATION_MUL(left,right,L) ;
  }else if(
    (is_group(left) || is_target(left)) && is_integer(right) && is_null(right)
  ){
    R = left ;
  }else if(
    (is_group(right) || is_target(right)) && is_integer(left) && is_null(left)
  ){
    R = right ;
  }
  if(R){
    return R ;
  }else{
    semantic_error(L, 
      string("invalid operate: ") + type_of(left) + " == " + type_of(right)
    );
  }
  return NULL ; // OK
}

static cell * process_SQUAREBRACKETS(cell * L) {
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * val    = process_all((cell*)arg0->car) ;

  if(reconstruct){
    string val_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back("[" + val_s + "]") ;
  }

  if ( ! ( (val->car == TYPE_i) || (val->car == TYPE_g) ) )
    semantic_error(L, string("invalid commitment for ") + val->car );

  if(is_single(val)) val = DUPLICATE(val) ;
  else               val = REDUCE(val,"com") ;

  if ( val->car == TYPE_i ) val->car = TYPE_ic ;
  if ( val->car == TYPE_g ) val->car = TYPE_gc ;

  return val ;
}

static cell * process_DOT(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;

  cell * left   = process_all((cell*)arg0->car) ;
  cell * right  = process_all((cell*)arg1->car) ;

  if(reconstruct){
    string right_s = reconstruct_stack_pop_back() ;
    string left_s  = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(left_s + "." + right_s) ;
  }

  if( is_crs(left) && ( is_ic(right) || is_gc(right) ) && is_single(right) ){
//    printf("%% %s.[%s]\n", first_arg(left), first_arg(right) );
    int k = find_identifier( first_arg(left) ) ;
    if(k>=0){
      cell * crs = identifier_stack[k].note ;

      if(!crs || is_single(crs)){
        semantic_error(L, string("unassigned crs: ") + first_arg(left));
      }

      if( is_ic(right) ) return integer_commit(crs, first_arg(right)) ;
      else               return group_commit(crs, first_arg(right)) ;
    }
  }else if( is_crs(left) && ( is_i(right) || is_g(right) || is_IDENTIFIER(right) ) && is_single(right) ) {
//    printf("%% %s.%s\n", first_arg(left), first_arg(right) );
    int k = find_identifier( first_arg(left) ) ;
    if(k>=0){
      cell * crs = identifier_stack[k].note ;

      if(!crs || is_single(crs)){
        semantic_error(L, string("unassigned crs: ") + first_arg(left));
      }

      string field = string(first_arg(right)) ;
      cell * L = crs ;
      L=L->cdr ; // type_name
      L=L->cdr ; // opt1
      L=L->cdr ; // opt2
      L=L->cdr ; // opt3
  
//      return LIST(TYPE_g, NULL, NULL, NULL, "debug") ;
  
      char * g  = L->car ; L=L->cdr ;
      char * u  = L->car ; L=L->cdr ;
      char * v  = L->car ; L=L->cdr ;
      char * w1 = L->car ; L=L->cdr ;
      char * w2 = L->car ; L=L->cdr ;
      char * w3 = L->car ; L=L->cdr ;
  
      if(field == "g")  return LIST(TYPE_g, NULL, NULL, NULL, g) ;
      if(field == "u")  return LIST(TYPE_g, NULL, NULL, NULL, u) ;
      if(field == "v")  return LIST(TYPE_g, NULL, NULL, NULL, v) ;
      if(field == "w1") return LIST(TYPE_g, NULL, NULL, NULL, w1) ;
      if(field == "w2") return LIST(TYPE_g, NULL, NULL, NULL, w2) ;
      if(field == "w3") return LIST(TYPE_g, NULL, NULL, NULL, w3) ;
    }
  }else if( is_commit(left) && ( is_i(right) || is_g(right) || is_IDENTIFIER(right) ) && is_single(right) ) {
    string field = string(first_arg(right)) ;
    cell * L = left ;
    L=L->cdr ; // type_name
    L=L->cdr ; // opt1
    L=L->cdr ; // opt2
    L=L->cdr ; // opt3
    char * c1 = L->car ; L=L->cdr ;
    char * c2 = L->car ; L=L->cdr ;
    char * c3 = L->car ; L=L->cdr ;
    char * z  = L->car ; L=L->cdr ;

    if(field == "c1") return LIST(TYPE_g, NULL, NULL, NULL, c1) ;
    if(field == "c2") return LIST(TYPE_g, NULL, NULL, NULL, c2) ;
    if(field == "c3") return LIST(TYPE_g, NULL, NULL, NULL, c3) ;

    if(field == "d1") return LIST(TYPE_g, NULL, NULL, NULL, c1) ;
    if(field == "d2") return LIST(TYPE_g, NULL, NULL, NULL, c2) ;
    if(field == "d3") return LIST(TYPE_g, NULL, NULL, NULL, c3) ;

    if(field == "z" ) return LIST(TYPE_g, NULL, NULL, NULL, z ) ;
  }
  semantic_error(L, 
    string("invalid field reference: ") + type_of(left) + "." + type_of(right)
  );
  return NULL ; // OK
}

static cell * process_FUNC_DECL(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * arg2   = arg1->cdr ;
//cell * type_name = process_all((cell*)arg0->car) ;
  cell * symbol    = process_all((cell*)arg1->car) ;
//                   process_all((cell*)arg2->car) ;
  if(reconstruct){
    string symbol_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(symbol_s + "()") ;
  }
  push_identifier(symbol, TYPE_function, L) ;

  return symbol ;
}

static cell * FUNC_CALL_apply(
  cell * function_template,
  cell * expression_list
){
  cell * L              = function_template ;
  cell * lineno         = L->cdr ;
  cell * arg0           = lineno->cdr ;
  cell * arg1           = arg0->cdr ;
  cell * arg2           = arg1->cdr ;
  cell * type_cell      = process_all((cell*)arg0->car) ;
  cell * func_name_cell = process_all((cell*)arg1->car) ;
  cell * var_list       = process_all((cell*)arg2->car) ;
  char * func_name      = first_arg(func_name_cell) ;
  char * type_name      = type_cell->car ;
  string func_name_str  = string(func_name) ;
  int    is_special     = (func_name_str == "e")             ||
                          (func_name_str == "GS_setup")      ||
                          (func_name_str == "GS_proofwi")    ||
                          (func_name_str == "GS_proofzk")    ||
                          (func_name_str == "set_dl_known")  ||
                          (func_name_str == "set_dl_unknown") ;

  string arg_s = "" ;

  if(reconstruct){
    string var_list_s       = reconstruct_stack_pop_back() ;
    string func_name_cell_s = reconstruct_stack_pop_back() ;
    string type_cell_s      = reconstruct_stack_pop_back() ;
  }

// ----------------------------------------------------
         L         = expression_list ;
         lineno    = L->cdr ;
  cell * val_list  = lineno->cdr ;
  cell * tmp_list  = NULL ;

  int    ellipsis = 0 ; 
  cell * VAR_LIST = var_list ;
  cell * VAL_LIST = val_list ;

  for(;VAL_LIST;VAR_LIST=VAR_LIST?VAR_LIST->cdr:VAR_LIST,VAL_LIST=VAL_LIST->cdr){
    cell * val = process_all((cell*)VAL_LIST->car) ;
    if(reconstruct){
      if(arg_s == "") arg_s  =       reconstruct_stack_pop_back() ;
      else            arg_s += "," + reconstruct_stack_pop_back() ;
    }
    char * var_name = NULL ;
    cell * var      = NULL ;
    if(!ellipsis){
      if(!VAR_LIST) semantic_error(expression_list,
        string("illegal function call ") + func_name + "(), too long arguments list" ) ;
      cell * var_template = (cell*)VAR_LIST->car ;
      if( type_of(var_template) == TYPE_ELLIPSIS ) ellipsis = 1 ;
      else{
        var_name = new_symbol(first_arg(var_template),type_of(var_template),NULL) ;
        var      = LIST(type_of(var_template),NULL,NULL,NULL,var_name) ;
      }
    }
    if(ellipsis){
      var_name = new_symbol("va_arg",type_of(val),NULL) ;
      var      = LIST(type_of(val),NULL,NULL,NULL,var_name) ;
    }
    if(is_special || is_single(val)){
      ASSIGN_VAR_RAW(expression_list, var, val, 0) ;
    }else{
      ASSIGN_VAR_RAW(expression_list, var, val, 1) ;
    }
    tmp_list = INTEGRATE(tmp_list,var) ;
  }

  if(VAR_LIST && (string("...") != first_arg((cell*)VAR_LIST)))
    semantic_error(expression_list, 
      string("illegal function call ") + func_name + "(), too short arguments list" ) ;

         L = expression_list ;
  cell * R = NULL ;

// ============================================
// special function
// ============================================

  if(string(func_name) == "e"){
    char * arg1 = first_arg((cell*)tmp_list->car) ;
    char * arg2 = first_arg((cell*)tmp_list->cdr->car) ;
    int i = find_identifier(arg1) ;
    int j = find_identifier(arg2) ;
    R = OPERATION_PAIRING(
      identifier_stack[i].note,
      identifier_stack[j].note,
      L
    ) ;
  }else if(string(func_name) == "GS_setup"){
    char * arg1 = first_arg((cell*)tmp_list->car) ;
    int i = find_identifier(arg1) ;
    R = OPERATION_GS_SETUP(
      identifier_stack[i].note,
      L
    ) ;
  }else if(string(func_name) == "GS_proofwi"){
    char * arg1 = first_arg((cell*)tmp_list->car) ;
    int i = find_identifier(arg1) ;
    if(is_single(identifier_stack[i].note)){
      i = find_identifier( first_arg(identifier_stack[i].note) ) ;
    }
    char * arg2 = first_arg((cell*)tmp_list->cdr->car) ;
    int j = find_identifier(arg2) ;

    cell * crs = identifier_stack[i].note ;
    if(!crs || is_single(crs)){
      semantic_error(L,
        string("unassigned crs: ") + identifier_stack[i].symbol
      );
    }
    R = OPERATION_GS_PROOFWI(
      identifier_stack[i].note,
      identifier_stack[j].note,
      L
    ) ;
  }else if(string(func_name) == "GS_proofzk"){
    char * arg1 = first_arg((cell*)tmp_list->car) ;
    char * arg2 = first_arg((cell*)tmp_list->cdr->car) ;
    int i = find_identifier(arg1) ;
    int j = find_identifier(arg2) ;
    int k = find_identifier( first_arg(identifier_stack[i].note) ) ;
    cell * crs = identifier_stack[k].note ;

    if(!crs || is_single(crs)){
      semantic_error(L,
        string("unassigned crs: ") + identifier_stack[k].symbol
      );
    }

    R = OPERATION_GS_PROOFZK(
      identifier_stack[k].note,
      identifier_stack[j].note,
      L
    ) ;
  }else if(string(func_name) == "set_dl_known"){
    char * arg1 = first_arg((cell*)tmp_list->car) ;
    char * arg2 = first_arg((cell*)tmp_list->cdr->car) ;
    int i = find_identifier(arg1) ;
    int j = find_identifier(arg2) ;

    R = SET_DL_KNOWN(
      identifier_stack[i].note,
      identifier_stack[j].note,
      L
    );
  }else if(string(func_name) == "set_dl_unknown"){
    char * arg1 = first_arg((cell*)tmp_list->car) ;
    char * arg2 = first_arg((cell*)tmp_list->cdr->car) ;
    int i = find_identifier(arg1) ;
    int j = find_identifier(arg2) ;
    R = SET_DL_UNKNOWN(
      identifier_stack[i].note,
      identifier_stack[j].note,
      L
    ) ;
  }
  if(!R){
    char * r = new_symbol(func_name,type_name,NULL) ;
    R = LIST(type_name,NULL,NULL,NULL,r) ;
  }

  if(reconstruct){
    reconstruct_stack.push_back(func_name_str + "(" + arg_s + ")") ;
  }

  return R ;
}

static cell * process_FUNC_CALL(cell * L){
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * symbol = process_all((cell*)arg0->car) ;
  if(reconstruct){
    string symbol_s = reconstruct_stack_pop_back() ;
  }
  int i = find_identifier(symbol) ;
  if(i>=0){
    if(identifier_stack[i].type_name == TYPE_function){
      return FUNC_CALL_apply(identifier_stack[i].note, (cell*)arg1->car) ;
    }
  }
  semantic_error(L, string("illegal function call: ") + first_arg(symbol) + "()" );
  return NULL ;
}

static cell * process_FUNC_DEF(cell * L) {
  cell * lineno = L->cdr ;
  cell * arg0   = lineno->cdr ;
  cell * arg1   = arg0->cdr ;
  cell * arg2   = arg1->cdr ;
  cell * name   = process_all((cell*)arg0->car) ;
  cell * decl   = process_all((cell*)arg1->car) ;
  cell * body   = process_all((cell*)arg2->car) ;

  if(reconstruct){
    string body_s = reconstruct_stack_pop_back() ;
    string decl_s = reconstruct_stack_pop_back() ;
    string name_s = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(name_s + "(" + decl_s + ")" + body_s + "\n") ;
  }
  return NULL ;
}

static cell * process_ARG_LIST(cell * L){
  cell * lineno = L->cdr ;
  cell * arg    = lineno->cdr ;
  cell * R      = NIL() ;
  string arg_s = "" ;

  for(;arg;arg=arg->cdr->cdr) {
    cell * type_cell = (cell*)arg->car ;      // LIST(AST_TYPE      ,line,$1)
    cell * var_cell  = (cell*)arg->cdr->car ; // LIST(AST_IDENTIFIER,line,$1)
    const char * type_name = symbol_to_type_name(type_cell->cdr->cdr->car) ;
    char       * var_name  = var_cell->cdr->cdr->car ;
    R                      = INTEGRATE(R, LIST(type_name,NULL,NULL,NULL,var_name)) ;
    if(reconstruct){
      if(arg_s == "") arg_s  =       string(type_cell->cdr->cdr->car) + " " + var_name ;
      else            arg_s += "," + string(type_cell->cdr->cdr->car) + " " + var_name ;
    }
  }
  if(reconstruct){
    reconstruct_stack.push_back(arg_s) ;
  }
  return R ;
}

// =====================================================================
// stub process
// =====================================================================

static cell * process_STUB(cell * L){
  cell * lineno = L->cdr ;
  cell * arg    = lineno->cdr ;
  cell * R      = NIL() ;
  static int num_indent = 0 ;

  string arg_s = "" ;

  int    n      = 0 ;
  string header = L->car ;
  string lp     = "(" ;
  string delim  = "," ;
  string rp     = ")" ;

  if(reconstruct){
    if(L->car == AST_PROG){
      header = ""   ;
      lp     = ""   ;
      delim  = ";\n" ;
      rp     = ";\n" ;
    }
    if(L->car == AST_STAT_LIST){
      string indent0 = "" ;
      string indent1 = "" ;
      for(int i=0;i<num_indent;i++)indent0 += "  " ;
      indent1 = indent0 + "  " ;

      num_indent++ ;
      header = "";
      lp     = "{\n" + indent1 ;
      delim  = ";\n" + indent1 ;
      rp     = ";\n" + indent0 + "}" ;
    }
    if(L->car == AST_EXP_LIST){
      header = "";
      lp     = "(" ;
      delim  = "," ;
      rp     = ")" ;
    }
  }

  for(;arg;arg=arg->cdr){
    cell * val = process_all((cell*)arg->car) ;
    if(reconstruct){
      if(arg_s == "") arg_s  =         reconstruct_stack_pop_back() ;
      else            arg_s += delim + reconstruct_stack_pop_back() ;
      n++ ;
    }
    R = val ;
  }
  if(reconstruct){
    if((n>1)||(header !="")){
      reconstruct_stack.push_back(header + lp + arg_s + rp) ;
    }else{
      reconstruct_stack.push_back(arg_s) ;
    }
    if(L->car == AST_STAT_LIST){
      num_indent-- ;
    }
  }
  return R ;
}

// =====================================================================
// dispatch process
// =====================================================================

// static 
cell * process_all(cell * L){
/*
  if(reconstruct){
    indent();
    printf("begin %s\n",L->car);
    print_reconstruct_stack() ;
  }
  nest ++ ;
*/
  cell * R = NIL() ;
  if(0);
// LITERAL
  else if (L->car == AST_IDENTIFIER    ) R = process_IDENTIFIER    (L) ;
  else if (L->car == AST_TYPE          ) R = process_TYPE          (L) ;
  else if (L->car == AST_NUMBER        ) R = process_NUMBER        (L) ;
  else if (L->car == AST_STRING        ) R = NIL()                     ; // process_STRING   (L) ;
// NULLARY
  else if (L->car == AST_BREAK         ) R = NIL()                     ; // process_BREAK    (L) ;
  else if (L->car == AST_CONTINUE      ) R = NIL()                     ; // process_CONTINUE (L) ;
// UNARY
  else if (L->car == AST_DECL          ) R = process_DECL          (L) ;
  else if (L->car == AST_INLINE        ) R = process_INLINE        (L) ;
  else if (L->car == AST_SQUAREBRACKETS) R = process_SQUAREBRACKETS(L) ;
  else if (L->car == AST_UPLUS         ) R = process_UPLUS         (L) ;
  else if (L->car == AST_UMINUS        ) R = process_UMINUS        (L) ;
#if 0
  else if (L->car == AST_ADDRESSOF     ) R = process_STUB          (L) ; // process_ADDRESSOF(L);
  else if (L->car == AST_DEREFERENCEOF ) R = process_STUB          (L) ; // process_DEREFERENCEOF(L) ;
  else if (L->car == AST_NEG           ) R = process_STUB          (L) ; // process_NEG    (L);
  else if (L->car == AST_NOT           ) R = process_STUB          (L) ; // process_NOT    (L);
  else if (L->car == AST_PREMM         ) R = process_STUB          (L) ; // process_PREMM  (L);
  else if (L->car == AST_PREPP         ) R = process_STUB          (L) ; // process_PREPP  (L);
  else if (L->car == AST_RETURN        ) R = process_STUB          (L) ; // process_RETURN (L);
  else if (L->car == AST_POSTPP        ) R = process_STUB          (L) ; // process_POSTPP (L);
  else if (L->car == AST_POSTMM        ) R = process_STUB          (L) ; // process_POSTMM (L);
#endif
// BINARY
  else if (L->car == AST_INIT          ) R = process_INIT      (L) ;
  else if (L->car == AST_ASSIGN        ) R = process_ASSIGN    (L) ;
  else if (L->car == AST_ADD           ) R = process_ADD       (L) ;
  else if (L->car == AST_SUB           ) R = process_ADD       (L) ;
  else if (L->car == AST_MUL           ) R = process_MUL       (L) ;
  else if (L->car == AST_DIV           ) R = process_MUL       (L) ;
  else if (L->car == AST_POW           ) R = process_POW       (L) ;
  else if (L->car == AST_FUNC_CALL     ) R = process_FUNC_CALL (L) ;
  else if (L->car == AST_EQ            ) R = process_EQ        (L) ;
  else if (L->car == AST_LOGICALAND    ) R = process_LOGICALAND(L) ;
  else if (L->car == AST_DOT           ) R = process_DOT       (L) ;
#if 0
  else if (L->car == AST_ARROW         ) R = process_STUB (L)  ; // process_ARROW    (L);
  else if (L->car == AST_ASSIGN        ) R = process_ASSIGN(L)  ; // process_ASSIGN   (L);
  else if (L->car == AST_ADDASSIGN     ) R = process_ASSIGN(L)  ; // process_ADDASSIGN(L);
  else if (L->car == AST_SUBASSIGN     ) R = process_ASSIGN(L)  ; // process_SUBASSIGN(L);
  else if (L->car == AST_MULASSIGN     ) R = process_ASSIGN(L)  ; // process_MULASSIGN(L);
  else if (L->car == AST_DIVASSIGN     ) R = process_ASSIGN(L)  ; // process_DIVASSIGN(L);
  else if (L->car == AST_MODASSIGN     ) R = process_ASSIGN(L)  ; // process_MODASSIGN(L);
  else if (L->car == AST_SHLASSIGN     ) R = process_ASSIGN(L)  ; // process_SHLASSIGN (L);
  else if (L->car == AST_SHRASSIGN     ) R = process_ASSIGN(L)  ; // process_SHRASSIGN (L);
  else if (L->car == AST_ANDASSIGN     ) R = process_ASSIGN(L)  ; // process_ANDASSIGN (L);
  else if (L->car == AST_XORASSIGN     ) R = process_ASSIGN(L)  ; // process_XORASSIGN (L);
  else if (L->car == AST_ORASSIGN      ) R = process_ASSIGN(L)  ; // process_ORASSIGN  (L);
  else if (L->car == AST_POWASSIGN     ) R = process_ASSIGN(L)  ; // process_POWASSIGN (L);
  else if (L->car == AST_BITAND        ) R = process_EQ(L)  ; // process_BITAND        (L);
  else if (L->car == AST_BITOR         ) R = process_EQ(L)  ; // process_BITOR         (L);
  else if (L->car == AST_BITXOR        ) R = process_EQ(L)  ; // process_BITXOR        (L);
  else if (L->car == AST_DIV           ) R = process_STUB (L)  ; // process_DIV       (L);
  else if (L->car == AST_DO            ) R = process_STUB (L)  ; // process_DO        (L);
  else if (L->car == AST_GE            ) R = process_EQ(L)  ; // process_GE         (L) ;
  else if (L->car == AST_GT            ) R = process_EQ(L)  ; // process_GT         (L) ;
  else if (L->car == AST_IF            ) R = process_STUB (L)  ; // process_IF     (L) ;
  else if (L->car == AST_INIT          ) R = process_INIT(L)    ; // process_INIT   (L) ;
  else if (L->car == AST_LE            ) R = process_EQ(L)  ; // process_LE         (L) ;
  else if (L->car == AST_LOGICALOR     ) R = process_EQ(L)  ; // process_LOGICALOR  (L) ;
  else if (L->car == AST_LT            ) R = process_EQ(L)  ; // process_LT         (L) ;
  else if (L->car == AST_MOD           ) R = process_EQ(L)  ; // process_MOD        (L) ;
  else if (L->car == AST_NE            ) R = process_EQ(L)  ; // process_NE         (L) ;
  else if (L->car == AST_SHL           ) R = process_EQ(L)  ; // process_SHL        (L) ;
  else if (L->car == AST_SHR           ) R = process_EQ(L)  ; // process_SHR        (L) ;
  else if (L->car == AST_WHILE         ) R = process_STUB (L)  ; // process_WHILE  (L) ;
#endif
// TERNARY
  else if (L->car == AST_FUNC_DECL     ) R = process_FUNC_DECL(L) ;
  else if (L->car == AST_FUNC_DEF      ) R = process_FUNC_DEF(L)  ;
  else if (L->car == AST_IF_ELSE       ) R = process_STUB (L)     ; // process_IF_ELSE (L);
  else if (L->car == AST_TERNARY       ) R = process_STUB (L)     ; // process_TERNARY (L);
// QUATERNARY
  else if (L->car == AST_FOR           ) R = process_STUB (L)     ; // process_FOR (L) ;
// N_ARY
  else if (L->car == AST_ARG_LIST      ) R = process_ARG_LIST(L)  ;
  else if (L->car == AST_EXP_LIST      ) R = process_STUB (L)     ; // process_EXP_LIST (L);
  else if (L->car == AST_INIT_LIST     ) R = process_INIT_LIST(L) ; // process_INIT_LIST(L);
  else if (L->car == AST_PROG          ) R = process_STUB (L)     ; // process_PROG     (L);
  else if (L->car == AST_STAT_LIST     ) R = process_STUB (L)     ; // process_STAT_LIST(L);
  else                                   R = process_STUB (L)     ;
//else                                   unexpected("process_all") ;

//  if (L->car == AST_IDENTIFIER    )
//  OPERATION_PRINT("process_ALL accept R=[",R,"]\n");
/*
  nest -- ;
  if(reconstruct){
    indent();
    printf("end %s\n",L->car);
    print_reconstruct_stack() ;
  }
*/
  return R ;
}

extern "C" {
#include "syntactic.h"
}

#ifdef SEMANTIC_DEBUG

extern "C" int yyparse(void) ;
extern "C" cell * AST ;

#include "type_define.txt"

int main(int argc, char **argv) {

#include "symbol_define.txt"
  if(yyparse() == 0){
    process_all(PREAMBLE) ;
    process_all(AST) ;
  }else{
    // syntactic error
    exit(1) ;
  }
  return EXIT_SUCCESS ;
}

#endif
