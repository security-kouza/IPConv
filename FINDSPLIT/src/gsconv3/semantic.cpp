#include <iostream>
#include <vector>
#include <stdint.h>
#include <boost/unordered_map.hpp>
#include <stdlib.h>
#include <string.h>

#include "lisp.h"
#include "ast.h"
#include "reserved_word.h"
#include "preamble.h"
#include "semantic.h"
#include "yacclex.h"
#include "declaration.h"
#include "ast2str.h"
#include "operation.h"

using namespace std   ;
using namespace boost ;

unordered_map<string, int> prohibited_stack ;
unordered_map<string, int> group0_stack     ;
unordered_map<string, int> group1_stack     ;
unordered_map<string, int> duplex_stack     ;

vector<crs_stack_entry> crs_stack ;

#define YY_BUF_SIZE 32768

extern "C" {
  extern int yyparse(void) ;
  extern cell * AST ;

  typedef struct yy_buffer_state * YY_BUFFER_STATE ;
  extern YY_BUFFER_STATE yy_create_buffer( FILE *file, int size) ;
  extern void yy_switch_to_buffer( YY_BUFFER_STATE new_buffer) ;
  extern void yy_delete_buffer( YY_BUFFER_STATE buffer ) ;
  extern void yy_flush_buffer(YY_BUFFER_STATE buffer) ;
  extern YY_BUFFER_STATE yy_scan_string(char * str);

//  yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));

}

cell * yyparse(char * s){
  cell * R = NULL ;
  YY_BUFFER_STATE str_buffer = yy_scan_string(s);
  yy_switch_to_buffer(str_buffer) ;
  if(yyparse()==0)R = AST ;
  yy_flush_buffer(str_buffer) ;
  yy_delete_buffer(str_buffer) ;
  return R ;
}

static YYLTYPE dummy_location ;
cell * dummy_LOCATION = LIST(&dummy_location) ;

static string location(cell * L){
  YYLTYPE * location = (YYLTYPE *) L->element(1) ;
  string s = "line "                     ;
  s += to_string(location->first_line)   ;
  s += ", column "                       ;
  s += to_string(location->first_column) ;
  return s ;
}

static void unexpected_error(cell * L, const string & message) {
  cerr << "unexpected error at " << location(L) << ": " << message << endl
       << "abstruct syntax tree node: " << L->car << endl ;
  exit(1) ;
}

void print_reconstruct_stack() ;

void warning(cell * L, const string & message) {
  cerr << "warning at " << location(L) << ": " << message << endl ;
}

void semantic_error(cell * L, const string & message) {
  cerr << "semantic error at " << location(L) << ": " << message << endl ;
  exit(1);
}

int nest = 0 ;
static inline void indent(){
  for(int i=0;i<nest;i++)cout << " " ;
}

string replace_all(
  const string & a, // with
  const string & b, // in
  const string & c
){
  string result = c ;
  size_t index  = 0 ;
  size_t a_len  = a.size() ;
  size_t b_len  = b.size() ;
  while((index = result.find(a,index))!=string::npos){
    result.replace(index, a_len, b);
    index += b_len ;
  }
  return result ;
}


vector<string> reconstruct_stack ;

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

void clear_reconstruct_stack(){ reconstruct_stack = {} ; }

void print_reconstruct_stack(){
  for(auto i = reconstruct_stack.begin(); i != reconstruct_stack.end(); i++){
    cout << replace_all("$","_",*i) << endl ;
  }
}

// ===================================================================================

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

static inline string double_quote(const string & s){
  string d = "\"" ;
  for(auto c : s){
         if(unlikely(c=='"' ))d+="\\\"" ;
    else if(unlikely(c=='\\'))d+="\\\\" ;
    else d+=c;
  }
  d+="\"" ;
  return d ;
}

static inline string single_quote(const string & s){
  string d = "'" ;
  for(auto c : s){
         if(unlikely(c=='\'' ))d+="\\'" ;
    else if(unlikely(c=='\\'))d+="\\\\" ;
    else d+=c;
  }
  d+="'" ;
  return d ;
}

// ===================================================================================

static excell * process_LITERAL(cell * L, int flag) {
  const char * s = (char *)L->element(2) ;
  switch(flag){
    case  1: reconstruct_stack.push_back(double_quote(string(s))); break;
    case  2: reconstruct_stack.push_back(single_quote(string(s))); break;
    default: reconstruct_stack.push_back(             string(s) ); break;
  }
  return NULL ;
}

static excell * process_STRING                  (cell * L){ return process_LITERAL(L,1); }
static excell * process_CHAR                    (cell * L){ return process_LITERAL(L,2); }

#define define_is_(X) \
static int is_ ## X(declaration_stack_entry & dse){                         \
  YYLTYPE & loc = dummy_location ;                                          \
  static cell * label_3 = LIST(AST_IDENTIFIER,&loc,unique_symbol( #X ));    \
  static cell * X ## _  = LIST(AST_DECLARATION_SPECIFIERS,&loc,label_3);    \
  static cell * label_5 = LIST(AST_NULL,&loc);                              \
  static cell * label_6 = LIST(AST_NEW_IDENTIFIER,&loc,unique_symbol("x")); \
  static cell * x_      = LIST(AST_DECLARATOR,&loc,label_5,label_6);        \
  return same_declaration(X ## _, dse.declaration_specifiers) &&            \
         same_declaration(x_    , dse.init_declarator       ) ;             \
}

define_is_(integer) ;
define_is_(group) ;
define_is_(target) ;
define_is_(list) ;
define_is_(prohibited) ;
define_is_(group0) ;
define_is_(group1) ;
define_is_(duplex) ;

int true_IDENTIFIER = 1 ;

static excell * process_IDENTIFIER_RAW (cell * L, int push_back){
  char * name = (char *)L->element(2);
  int i = symbol_to_declaration_stack_entry(name) ;
  excell * R = NULL ;
  if(i>=0){
    char * true_name = declaration_stack[i].true_name ;

    if(true_IDENTIFIER)reconstruct_stack.push_back(string(true_name)) ;
    else               reconstruct_stack.push_back(string(name)) ;

    if(is_integer(declaration_stack[i])){
      R = (excell *)LIST(TYPE_INTEGER , L->element(1), new INTEGER(string(true_name)) ) ;
      if(push_back){
        gsconv_input  += string("integer ") + true_name + " ;\n" ;
	gsconv_declared[true_name] = 1 ;
        history[true_name].push_back(R);
      }
    }else if(is_group(declaration_stack[i])){
      R = (excell *)LIST(TYPE_GROUP , L->element(1), new GROUP(string(true_name)) ) ;
      if(push_back){
        gsconv_input  += string("group ") + true_name + " ;\n" ;
        weight_candidate[string(true_name)] = 0 ;
	gsconv_declared[true_name] = 1 ;
        history[true_name].push_back(R);
      }
    }else if(is_prohibited(declaration_stack[i])){
      R = (excell *)LIST(TYPE_GROUP , L->element(1), new GROUP(string(true_name)) ) ;
      if(push_back){
        gsconv_input  += string("group ") + true_name + " ;\n" ;
        weight_candidate[string(true_name)] = 0 ;
	gsconv_declared[true_name] = 1 ;
        history[true_name].push_back(R);
      }
      prohibited_stack[true_name] = 1 ;
    }else if(is_group0(declaration_stack[i])){
      R = (excell *)LIST(TYPE_GROUP , L->element(1), new GROUP(string(true_name)) ) ;
      if(push_back){
        gsconv_input  += string("group ") + true_name + " ;\n" ;
        weight_candidate[string(true_name)] = 0 ;
	gsconv_declared[true_name] = 1 ;
        history[true_name].push_back(R);
      }
      group0_stack[true_name] = 1 ;
    }else if(is_group1(declaration_stack[i])){
      R = (excell *)LIST(TYPE_GROUP , L->element(1), new GROUP(string(true_name)) ) ;
      if(push_back){
        gsconv_input  += string("group ") + true_name + " ;\n" ;
        weight_candidate[string(true_name)] = 0 ;
	gsconv_declared[true_name] = 1 ;
        history[true_name].push_back(R);
      }
      group1_stack[true_name] = 1 ;
    }else if(is_duplex(declaration_stack[i])){
      R = (excell *)LIST(TYPE_GROUP , L->element(1), new GROUP(string(true_name)) ) ;
      if(push_back){
        gsconv_input  += string("group ") + true_name + " ;\n" ;
        weight_candidate[string(true_name)] = 0 ;
	gsconv_declared[true_name] = 1 ;
        history[true_name].push_back(R);
      }
      duplex_stack[true_name] = 1 ;
    }else if(is_target(declaration_stack[i])){
      R = (excell *)LIST(TYPE_TARGET, L->element(1), new TARGET(TARGETVAR(string(true_name)))) ;
      if(push_back){
        gsconv_input  += string("target ") + true_name + " ;\n" ;
	gsconv_declared[true_name] = 1 ;
        history[true_name].push_back(R) ;
      }
    }else if(is_list(declaration_stack[i])){
      R = (excell *)LIST(TYPE_LIST_VARIABLE, L->element(1), true_name) ;
    }else if(declaration_stack[i].type & FUNC_DEFINITION){
      R = (excell *)LIST(TYPE_LIST_VARIABLE, L->element(1), true_name) ;
    }else{
      R = (excell *)LIST(TYPE_LIST_VARIABLE, L->element(1), true_name) ;
    }
  }else{
//    if(! is_reserved_word(name)) warning(L, string("undefined symbol ") + name ) ;
    reconstruct_stack.push_back(string(name));
    R = (excell *)LIST(TYPE_LIST_VARIABLE, L->element(1), name) ;
  }
  return R ;
}

static excell * process_IDENTIFIER      (cell * L){ return process_IDENTIFIER_RAW(L, 0); }
static excell * process_NEW_IDENTIFIER  (cell * L){ return process_IDENTIFIER_RAW(L, 1); }

static excell * process_NUMBER (cell * L){
  process_LITERAL(L,0); 
  char * name = (char *)L->element(2);

#ifndef USE_LONG_LONG
  excell * R = (excell *)LIST(TYPE_INTEGER , L->element(1), new INTEGER(integer_(name)) ) ;
#else
  excell * R = (excell *)LIST(TYPE_INTEGER , L->element(1), new INTEGER(strtoll(name,NULL,0)) ) ;
#endif

  return R ;
}

static excell * process_TYPE_QUALIFIER          (cell * L){ return process_LITERAL(L,0); }
static excell * process_STORAGE_CLASS_SPECIFIER (cell * L){ return process_LITERAL(L,0); }

// ===================================================================================

static excell * process_NULLARY(const char * const prefix){
  reconstruct_stack.push_back(string(prefix));
  return NULL ;
}

static excell * process_NULLARY_OPERATION(
  cell * L,
  const char * const prefix,
  excell * (*operation_X)(cell * L)
){
  reconstruct_stack.push_back(string(prefix)) ;
  return operation_X(L) ;
}

static excell * process_BREAK     (cell * L){ return process_NULLARY("break ;"); }
static excell * process_CONTINUE  (cell * L){ return process_NULLARY("continue ;"); }
static excell * process_NULL      (cell * L){ return process_NULLARY(""); }

// ===================================================================================

static excell * process_UNARY(
  cell * L,
  const char * const prefix,
  const char * const postfix
){
  cell * arg0 = L->element(2) ;
  excell * R = process_all(arg0) ;
  string s = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( string(prefix) + s + postfix ) ;
  return R ;
}

static excell * process_UNARY_OPERATION(
  cell * L,
  const char * const prefix,
  const char * const postfix,
  excell * (*operation_X)(excell * X, cell * L),
  int check = 1
){
  cell * arg0 = L->element(2) ;

  excell * A = process_all(arg0) ;
  string s0 = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( string(prefix) + s0 + postfix ) ;

  excell * R = operation_X(A, L) ;
  if(check) if(!R) semantic_error (L, 
    string("invalid operands: ")
    + prefix + " (" + type_of(A) + " " + s0 + ") "
    + postfix
  );
  return R ;
}

static excell * process_ADDRESSOF         (cell * L){ return process_UNARY(L,"&",""); }

static int brackets_expand = 1 ;

static excell * process_BRACKETS (cell * L) {
  const char * const prefix  = "[" ;
  const char * const postfix = "]" ;
  if(crs_stack.size() == 0) semantic_error(L, string("no crs specified for ") + ast2str(L)) ;
  excell * crs  = crs_stack.back().crs      ;
  int dummycom  = crs_stack.back().dummycom ;
  cell   * arg0 = L->element(2) ;
  excell * X    = process_all(arg0) ;
  string   s0   = reconstruct_stack_pop_back() ;

  if(brackets_expand) reduction_history.push_back("") ;

  excell * R    = dummycom ? operation_GS_DUMMY_COMMIT(crs, X, L) : operation_GS_COMMIT(crs, X, L) ;
  string   s ;

  if(brackets_expand){
    string   s1   = reduction_history.back() ;
             s1   = "  " + replace_all("\n","\n  ",s1) ;
    string   s2   = as_string(R) ;
    reduction_history.pop_back() ;
    s = string("\n/* special macro: ") + prefix + s0 + postfix + " */\n"
             + "({\n" + s1 + s2 + ";\n" + "})" ;
  }else{
    s = string(prefix) + s0 + postfix ;
  }

  reconstruct_stack.push_back(s) ;

  if(R) return R ;
  else semantic_error (L,
    string("invalid operands: ")
    + prefix + type_of(X) + " " + s0 + postfix
  );
  return NULL ;
}

static excell * process_COMPOUND_STATEMENT(cell * L){
  declaration_frame_stack_push();
  excell * R = process_UNARY(L,"{\n","}");
  declaration_frame_stack_pop();
  return R ;
}

static excell * process_CURLY_BRACES (cell * L){ return process_UNARY(L,"{","}"); }
static excell * process_DEFAULT      (cell * L){ return process_UNARY(L,"default: ",""); }
static excell * process_DEREFERENCEOF(cell * L){ return process_UNARY(L,"*",""); }
static excell * process_EXP_STAT     (cell * L){ return process_UNARY(L,"",";"); }
static excell * process_GOTO         (cell * L){ return process_UNARY(L,"goto ",";"); }
static excell * process_INLINE       (cell * L){ return process_UNARY(L,"inline ",";"); }
static excell * process_NEG          (cell * L){ return process_UNARY(L,"~",""); }
static excell * process_NOT          (cell * L){ return process_UNARY(L,"!",""); }

static excell * process_PARENTHESIS  (cell * L){
  cell * arg0 = L->element(2) ;
  excell * R = process_all(arg0) ;
  string s = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( string("(") + s + ")" ) ;
  return R ;
}

static excell * process_POSTPP       (cell * L){ return process_UNARY(L,"","++"); }
static excell * process_POSTMM       (cell * L){ return process_UNARY(L,"","--"); }
static excell * process_PREMM        (cell * L){ return process_UNARY(L,"--",""); }
static excell * process_PREPP        (cell * L){ return process_UNARY(L,"++",""); }
static excell * process_RETURN       (cell * L){ return process_UNARY(L,"return ",";"); }
static excell * process_SIZEOF_TYPE  (cell * L){ return process_UNARY(L,"sizeof(",")"); }
static excell * process_SIZEOF_EXP   (cell * L){ return process_UNARY(L,"sizeof(",")"); }
static excell * process_UPLUS(cell * L){
  return process_UNARY_OPERATION(L,"+","",operation_UPLUS,0) ;
}
static excell * process_UMINUS(cell * L){
  return process_UNARY_OPERATION(L,"-","",operation_UMINUS,0) ;
}

// ===================================================================================

static excell * process_BINARY(
  cell * L,
  const char * const prefix,
  const char * const infix,
  const char * const postfix
){
  cell * arg0 = L->element(2) ;
  cell * arg1 = L->element(3) ;

  process_all(arg0) ;
  process_all(arg1) ;
  string s1 = reconstruct_stack_pop_back() ;
  string s0 = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( string(prefix) + s0 + infix + s1 + postfix ) ;
  return NULL ;
}

enum process_XARY_OPERATION_flag {
  POP_BACK       =  1, // tweak declaration stack to initialize a variable
  PUSH_CRS       =  2, // push crs
  DOT_FLAG       =  4, // push crs and resolve .xxx identifier
  SUPPRESS       =  8, // suppress brackets expand
  DUMMYCOM       = 16, // dummy commit
  PUSH_REDUCE    = 32, // push reduce_history
  REFLECT_REDUCE = 64, // reflect reduce_history to reconstruct_stack
  WITHOUT_REDUCE = 32,
  WITH_REDUCE    = 32 + 64,
  SKIPERROR      = 128
} ;

static excell * process_BINARY_OPERATION_CORE(
  cell * L,
  const char * const prefix,
  const char * const infix,
  const char * const postfix,
  excell * (*operation_X)(excell * X, excell * Y, cell * L),
  int flag = 0
){
  int pop_back = flag & POP_BACK ;
  int push_crs = flag & PUSH_CRS ;
  int dot      = flag & DOT_FLAG ;
  int suppress = flag & SUPPRESS ;
  int dummycom = flag & DUMMYCOM ;
  int skiperror= flag & SKIPERROR;

  int save_brackets_expand = brackets_expand ;

  cell * arg0 = L->element(2) ;
  cell * arg1 = L->element(3) ;

  excell * A = process_all(arg0) ;
  declaration_stack_entry dse ;
  if(pop_back){
    dse = declaration_stack.back() ;
    declaration_stack.pop_back() ;
  }
  if(dot) push_crs = 1 ;
  if(push_crs) crs_stack.push_back({GENERATE_CRS_if_listvar(A,L), dummycom});
  if(suppress) brackets_expand = 0 ;
  excell * B = process_all(arg1) ;
  if(suppress) brackets_expand = save_brackets_expand ;

  if(pop_back){
    declaration_stack.push_back(dse) ;
  }
  string s1 = reconstruct_stack_pop_back() ;
  string s0 = reconstruct_stack_pop_back() ;
  excell * R = operation_X(A, B, L) ;
  if(push_crs) crs_stack.pop_back();
  if(true_IDENTIFIER && dot && R && ! operation_is_COMMIT(R)) s1 = as_string(R) ;
  string s = string(prefix) + s0 + infix + s1 + postfix  ;
  reconstruct_stack.push_back(s) ;

  if(skiperror) return R ;
  else if(R)    return R ;
  else
  semantic_error (L, 
    string("invalid operands: ")
    + prefix + " (" + type_of(A) + " " + s0 + ") "
    + infix  + " (" + type_of(B) + " " + s1 + ") "
    + postfix
  );
  return NULL ;
}

static excell * process_BINARY_OPERATION(
  cell * L,
  const char * const prefix,
  const char * const infix,
  const char * const postfix,
  excell * (*operation_X)(excell * X, excell * Y, cell * L),
  int flag = 0
){
  int push_reduce    = flag & PUSH_REDUCE    ;
  int reflect_reduce = flag & REFLECT_REDUCE ;
  string t = "" ;

  if(push_reduce) reduction_history.push_back("") ;
  excell * R = process_BINARY_OPERATION_CORE(L, prefix, infix, postfix, operation_X, flag) ;
  if(push_reduce){
    t = reduction_history.back() ;
    reduction_history.pop_back() ;
  }
  if(reflect_reduce){
    if(t!=""){
      string s = reconstruct_stack_pop_back() ;
      s = "({\n  " + replace_all("\n","\n  ",t+as_string(R)) + ";\n})" ;
      reconstruct_stack.push_back(s) ;
    }
  }
  return R ;
}

static excell * process_TERNARY_OPERATION(
  cell * L,
  const char * const prefix,
  const char * const infix1,
  const char * const infix2,
  const char * const postfix,
  excell * (*operation_X)(excell * X, excell * Y, excell * Z, cell * L),
  int flag = 0
){
  int pop_back = flag & POP_BACK ; // unused
  int push_crs = flag & PUSH_CRS ;
  int dot      = flag & DOT_FLAG ; // unused
  int suppress = flag & SUPPRESS ;
  int dummycom = flag & DUMMYCOM ;

  int save_brackets_expand = brackets_expand ;

  cell * arg0 = L->element(2) ;
  cell * arg1 = L->element(3) ;
  cell * arg2 = L->element(4) ;

  excell * A = process_all(arg0) ;

  if(push_crs) crs_stack.push_back({GENERATE_CRS_if_listvar(A,L),dummycom});
  if(suppress) brackets_expand = 0 ;
  excell * B = process_all(arg1) ;
  if(suppress) brackets_expand = save_brackets_expand ;

  excell * C = process_all(arg2) ;

  string s2 = reconstruct_stack_pop_back() ;
  string s1 = reconstruct_stack_pop_back() ;
  string s0 = reconstruct_stack_pop_back() ;

  excell * R = operation_X(A, B, C, L) ;
  if(push_crs) crs_stack.pop_back();

  if(true_IDENTIFIER && dot && R && ! operation_is_COMMIT(R)) s1 = as_string(R) ;
  string s = string(prefix) + s0 + infix1 + s1 + infix2 + s2 + postfix  ;

  reconstruct_stack.push_back(s) ;

  if(R) return R ;
  else semantic_error (L,
    string("invalid operands: ")
    + prefix + " (" + type_of(A) + " " + s0 + ") "
    + infix1 + " (" + type_of(B) + " " + s1 + ") "
    + infix2 + " (" + type_of(C) + " " + s2 + ") "
    + postfix
  );
  return NULL ;
}

static excell * process_ABSTRACT_DECLARATOR(cell * L){ return process_BINARY(L,"","",""); }
static excell * process_ARRAY_DECLARATOR   (cell * L){ return process_BINARY(L,"","[","]"); }

static excell * process_ARRAY_REF          (cell * L){
  return process_BINARY_OPERATION(L,"","[","]",operation_ARRAY_REF); 
}

static excell * process_ASSIGN(cell * L){
  return process_BINARY_OPERATION(L,"","=","",operation_ASSIGN, WITHOUT_REDUCE) ;
}

static excell * process_INIT  (cell * L){
  process_BINARY_OPERATION(L,"","=","",operation_ASSIGN, WITHOUT_REDUCE | POP_BACK) ;
  return NULL ;
}

static excell * process_DECLARATION (cell * L){
  cell * loc  = L   ->cdr ;
  cell * arg0 = loc ->cdr ;
  cell * arg1 = arg0->cdr ;
  cell * declaration_specifiers = (cell*)arg0->car ;

  int sp = declaration_stack_pointer();
  declaration_specifiers_stack_push(declaration_specifiers);
//  excell * R = process_BINARY(L,""," ",";");
  excell * R = process_BINARY(L,"","",";");
  declaration_specifiers_stack_pop();

  if( is_typedef(declaration_specifiers) ){
    int n = declaration_stack_pointer();
    for(int i=sp;i<n;i++) declaration_stack[i].type |= TYPEDEF_DECLARATOR ;
  }

  return R ;
}

static excell * process_DECLARATOR     (cell * L){
  const char * const prefix  = "" ;
  const char * const infix   = "" ;
  const char * const postfix = "" ;
  cell * arg0 = L->element(2) ;
  cell * arg1 = L->element(3) ;

               process_all(arg0) ; string s0 = reconstruct_stack_pop_back() ;
  excell * R = process_all(arg1) ; string s1 = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( string(prefix) + s0 + infix + s1 + postfix ) ;

  if(arg0->car == AST_NULL) return R ;
  else                      return NULL ; // pointer/reference
}

static excell * process_FUNC_DECLARATOR(cell * L){
  const char * const prefix  = "" ;
  const char * const infix   = "(" ;
  const char * const postfix = ")" ;
  cell * arg0 = L->element(2) ;
  cell * arg1 = L->element(3) ;

  int sp = declaration_stack_pointer();
  declaration_frame_stack_push();
  process_all(arg0) ; 
  process_all(arg1) ; 
  string s1 = reconstruct_stack_pop_back() ;
  string s0 = reconstruct_stack_pop_back() ;
  int n = declaration_stack_pointer();

  if(is_group((cell *)declaration_specifiers_stack.back())){
    string return_symbol = s0 + "$return" ;
    weight_candidate[return_symbol] = 0 ;
  }
  if(is_prohibited((cell *)declaration_specifiers_stack.back())){
    string return_symbol = s0 + "$return" ;
    prohibited_stack[return_symbol] = 1 ;
    weight_candidate[return_symbol] = 0 ;
  }
  if(is_group0((cell *)declaration_specifiers_stack.back())){
    string return_symbol = s0 + "$return" ;
    group0_stack[return_symbol] = 1 ;
    weight_candidate[return_symbol] = 0 ;
  }
  if(is_group1((cell *)declaration_specifiers_stack.back())){
    string return_symbol = s0 + "$return" ;
    group1_stack[return_symbol] = 1 ;
    weight_candidate[return_symbol] = 0 ;
  }
  if(is_duplex((cell *)declaration_specifiers_stack.back())){
    string return_symbol = s0 + "$return" ;
    duplex_stack[return_symbol] = 1 ;
    weight_candidate[return_symbol] = 0 ;
  }
  for(int i=sp;i<n;i++){
    declaration_stack[sp-1].option.push_back(declaration_stack[i]) ;
    if(
      is_group(declaration_stack[i].declaration_specifiers) ||
      is_prohibited(declaration_stack[i].declaration_specifiers) ||
      is_group0(declaration_stack[i].declaration_specifiers) ||
      is_group1(declaration_stack[i].declaration_specifiers) ||
      is_duplex(declaration_stack[i].declaration_specifiers)
    ){
      weight_candidate[ string(declaration_stack[i].true_name) ] = 1 ;
    }
  }
  declaration_frame_stack_pop();
  reconstruct_stack.push_back( string(prefix) + s0 + infix + s1 + postfix ) ;

  return NULL ;
}

static excell * process_ADD(cell * L){ return process_BINARY_OPERATION(L,"","+","",operation_ADD) ; }
static excell * process_SUB(cell * L){ return process_BINARY_OPERATION(L,"","-","",operation_SUB) ; }
static excell * process_MUL(cell * L){ return process_BINARY_OPERATION(L,"","*","",operation_MUL) ; }
static excell * process_DIV(cell * L){ return process_BINARY_OPERATION(L,"","/","",operation_DIV //, WITH_REDUCE
) ; }
static excell * process_POW(cell * L){ return process_BINARY_OPERATION(L,"","^","",operation_POW //, WITH_REDUCE
) ; }
static excell * process_EQ (cell * L){ return process_BINARY_OPERATION(L,"","==","",operation_EQ //, WITHOUT_REDUCE
) ; }
static excell * process_NE (cell * L){ return process_BINARY_OPERATION(L,"","!=","",operation_EQ //, WITHOUT_REDUCE
) ; }
static excell * process_DOT(cell * L){ return process_BINARY_OPERATION(L,"",".","",operation_DOT, DOT_FLAG) ; }
static excell * process_LOGICALAND(cell * L){
  return process_BINARY_OPERATION(L,"","&&","",operation_LOGICAL_AND) ;
}
static excell * process_ARROW          (cell * L){ return process_BINARY(L,"","->",""); }
static excell * process_ADDASSIGN      (cell * L){ return process_BINARY(L,"","+=",""); }
static excell * process_SUBASSIGN      (cell * L){ return process_BINARY(L,"","-=",""); }
static excell * process_MULASSIGN      (cell * L){ return process_BINARY(L,"","*=",""); }
static excell * process_DIVASSIGN      (cell * L){ return process_BINARY(L,"","/=",""); }
static excell * process_MODASSIGN      (cell * L){ return process_BINARY(L,"","%=",""); }
static excell * process_SHLASSIGN      (cell * L){ return process_BINARY(L,"","<<=",""); }
static excell * process_SHRASSIGN      (cell * L){ return process_BINARY(L,"",">>=",""); }
static excell * process_ANDASSIGN      (cell * L){ return process_BINARY(L,"","&=",""); }
static excell * process_XORASSIGN      (cell * L){ return process_BINARY(L,"","(+)=",""); }
static excell * process_ORASSIGN       (cell * L){ return process_BINARY(L,"","|=",""); }

static int is_group_variant(string & type){
  return (type == "group") || (type == "prohibited") ;
}

static excell * process_PARAMETER_DECLARATION(cell * L){
  cell * declaration_specifiers = L->element(2) ;
  cell * declarator             = L->element(3) ;
  if(declarator->car == AST_DECLARATOR){
    declaration_specifiers_stack_push(declaration_specifiers);
    declaration_stack_push(declarator) ;
    declaration_specifiers_stack_pop();
  }
  const char * const prefix  = "" ;
  const char * const infix   = " " ;
  const char * const postfix = "" ;
  cell * arg0 = L->element(2) ;
  cell * arg1 = L->element(3) ;

  process_all(arg0) ;
  process_all(arg1) ;
  string init   = reconstruct_stack_pop_back() ;
  string type   = reconstruct_stack_pop_back() ;
  int    group  = reconstruction_mode && is_group_variant(type) ;
  if(group){
    string name  = init.substr(0,init.find("=")) ;
    if(name.find("(") != string::npos){
      name = name.substr(0,name.find("(")) ;
      name += "$return" ;
    }
    int    gtype = group_of[name] ;
    switch(gtype){
      case  1: type = "group0" ; break ;
      case  2: type = "group1" ; break ;
      case  3: type = "duplex" ; break ;
      default: type = "unused" ; break ;
    }
  }
  reconstruct_stack.push_back( string(prefix) + type + infix + init + postfix ) ;
  return NULL ;
}

static excell * process_POWASSIGN      (cell * L){ return process_BINARY(L,"","^=",""); }
static excell * process_BITAND         (cell * L){ return process_BINARY(L,"","&",""); }
static excell * process_BITOR          (cell * L){ return process_BINARY(L,"","|",""); }
static excell * process_BITXOR         (cell * L){ return process_BINARY(L,"","(+)",""); }
static excell * process_DO             (cell * L){
  return process_BINARY(L,"do ","while(",");");
}

static cell * first_ast(cell * L, char * name){
  if(L->car == name) return L ;
  if(is_literal(L))  return NULL ;
  for(cell * arg = L->pointer(2); arg; arg=arg->cdr){
    cell * t = first_ast((cell *)(arg->car), name) ;
    if(t) return t ;
  }
  return NULL ;
}

cell * convert_return (cell * L, char * id, char * lb) {
  cell * R = NULL ;
  if(L){
    cell * loc = L  ->cdr ;
    cell * arg = loc->cdr ;
    if(is_literal(L)){
      R = LIST(L->car, loc->car, arg->car) ;
    }else if(L->car == AST_RETURN){
      cell * right = convert_return((cell*)arg->car, id, lb) ;
      cell * left  = LIST(AST_IDENTIFIER, loc->car, id) ;
      cell * label = LIST(AST_IDENTIFIER, loc->car, lb) ;
      cell * exp0  = LIST(AST_ASSIGN    , loc->car, left, right) ;
      cell * exp1  = LIST(AST_EXP_LIST  , loc->car, exp0) ;
      cell * stat0 = LIST(AST_EXP_STAT  , loc->car, exp1) ;
      cell * stat1 = LIST(AST_GOTO      , loc->car, label) ;
      cell * stat2 = LIST(AST_STAT_LIST , loc->car, stat0, stat1) ;
      R = LIST(AST_COMPOUND_STATEMENT   , loc->car, stat2) ;
    }else{
      R = LIST(L->car, loc->car) ;
      for(;arg;arg=arg->cdr) {
        R = INTEGRATE(R, convert_return((cell*)arg->car, id, lb)) ;
      }
    }
  }
  return R ;
}

cell * convert_return2 (cell * L, char * id){
  cell * R = NULL ;
  if(L){
    cell * loc = L  ->cdr ;
    cell * arg = loc->cdr ;
    if(is_literal(L)){
      R = LIST(L->car, loc->car, arg->car) ;
    }else if(L->car == AST_RETURN){
      cell * right = convert_return2((cell*)arg->car, id) ;
      cell * left  = LIST(AST_IDENTIFIER, loc->car, id) ;
      cell * exp0  = LIST(AST_ASSIGN    , loc->car, left, right) ;
      cell * exp1  = LIST(AST_EXP_LIST  , loc->car, exp0) ;
      cell * stat0 = LIST(AST_EXP_STAT  , loc->car, exp1) ;
      cell * left2 = LIST(AST_IDENTIFIER, loc->car, id) ;
      cell * exp2  = LIST(AST_EXP_LIST  , loc->car, left2) ;
      cell * stat1 = LIST(AST_RETURN    , loc->car, exp2) ;
      cell * stat2 = LIST(AST_STAT_LIST , loc->car, stat0, stat1) ;
      R = LIST(AST_COMPOUND_STATEMENT   , loc->car, stat2) ;
    }else{
      R = LIST(L->car, loc->car) ;
      for(;arg;arg=arg->cdr) {
        R = INTEGRATE(R, convert_return2((cell*)arg->car, id)) ;
      }
    }
  }
  return R ;
}

cell * replace_func(cell * L, char * id){
  cell * R = NULL ;
  if(L){
    cell * loc = L  ->cdr ;
    cell * arg = loc->cdr ;
    if(is_literal(L)){
      R = LIST(L->car, loc->car, arg->car) ;
    }else if(L->car == AST_FUNC_DECLARATOR){
      R = LIST(AST_NEW_IDENTIFIER, loc->car, id) ;
    }else{
      R = LIST(L->car, loc->car) ;
      for(;arg;arg=arg->cdr) {
        R = INTEGRATE(R, replace_func((cell*)arg->car, id)) ;
      }
    }
  }
  return R ;
}

cell * convert_to_macro (cell * L, int i){
#define dse (declaration_stack[i])
  cell * loc            = L->cdr;
  cell * name           = L->element(2) ;
  cell * expression_opt = L->element(3) ;
  char * base           = init_declarator_to_symbol(dse.init_declarator) ;
  char * id             = new_symbol(base) ;
  char * lb             = new_symbol(base) ;
  cell * R              = LIST(AST_STAT_LIST, loc->car) ;
  { // return variable
    cell * declaration_specifiers ;

    cell * dse_ds = copy_ast(dse.declaration_specifiers) ;
    remove_macro( dse_ds ) ;
    string s = ast2str( dse_ds ) ;

    if(s==""){
      cell * type_specifier =  LIST(AST_IDENTIFIER, loc->car, unique_symbol("list")) ;
      declaration_specifiers = LIST(AST_DECLARATION_SPECIFIERS,loc->car, type_specifier) ;
      free_ast(dse_ds) ;
    }else{
      declaration_specifiers = dse_ds ;
    }

    cell * init_declarator        = replace_func(dse.init_declarator, id) ;
    cell * init_declarator_list   = LIST(AST_INIT_DECLARATOR_LIST,loc->car, init_declarator) ;
    cell * declaration            = LIST(AST_DECLARATION,loc->car, declaration_specifiers, init_declarator_list) ;
    R                             = INTEGRATE(R,declaration) ;
  }
  { // arguments
    cell * init_declarator= dse.init_declarator ;
    cell * parameter_list = first_ast(init_declarator, AST_PARAMETER_LIST) ;
    cell * exp_list       = first_ast(expression_opt , AST_EXP_LIST      ) ;
    cell * var            = parameter_list->pointer(2) ;
    cell * val            = exp_list      ->pointer(2) ;
    cell * loc            = L->cdr ;
    for(;var && val;var=var->cdr,val=val->cdr){
      cell * parameter_declaration  = var->element(0) ;
      cell * declaration_specifiers = copy_ast(parameter_declaration->element(2)) ;
      cell * declarator             = copy_ast(parameter_declaration->element(3)) ;
      cell * initializer            = copy_ast(val->element(0)) ;
      cell * init_declarator        = LIST(AST_INIT, loc->car, declarator, initializer) ;
      cell * init_declarator_list   = LIST(AST_INIT_DECLARATOR_LIST,loc->car, init_declarator) ;
      cell * declaration            = LIST(AST_DECLARATION,loc->car, declaration_specifiers, init_declarator_list) ;
      R                             = INTEGRATE(R,declaration) ;
    }
    if(var) semantic_error(L, string("too few arguments to " ) + ast2str(init_declarator));
    if(val) semantic_error(L, string("too many arguments to ") + ast2str(init_declarator));
  }
  { // body
    cell * L = dse.compound_statement ;
    cell * loc = L->cdr ;
    cell * body  = convert_return (L, id, lb) ;
    cell * ret   = LIST(AST_IDENTIFIER, loc->car, id) ;
    cell * label = LIST(AST_IDENTIFIER, loc->car, lb) ;
    cell * exp0  = LIST(AST_EXP_LIST  , loc->car, ret) ;
    cell * stat0 = LIST(AST_EXP_STAT  , loc->car, exp0) ;
    cell * tail  = LIST(AST_LABEL, loc->car, label, stat0) ;
    R = INTEGRATE(R, body) ;
    R = INTEGRATE(R, tail) ;
  }
  R = LIST(AST_COMPOUND_STATEMENT, loc->car, R) ;
  R = LIST(AST_PARENTHESIS       , loc->car, R) ;
  return R ;
#undef dse
}

static excell * macro_expand(cell * L, int i){
  cell * loc            = L->element(1) ;
  cell * name           = L->element(2) ;
  cell * expression_opt = L->element(3) ;
  string s0 = ast2str(L) ;
  cell * t = convert_to_macro (L, i) ;
  excell * R = process_all(t) ;
  free_ast(t) ;
  string s1 = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( 
    string("\n/* macro expand: ") + s0 + " */\n" + s1 ) ;
  return R ;
}

static excell * func_expand(cell * L, int i){
#define dse (declaration_stack[i])
  cell * name           = L->element(2) ;
  cell * expression_opt = L->element(3) ;
  cell * init_declarator= dse.init_declarator ;

  string arguments = "" ;

  { // arguments
    cell * exp_list       = first_ast(expression_opt,AST_EXP_LIST) ;
    cell * val            = exp_list->pointer(2) ;
    cell * loc            = L->cdr ;

    int n = dse.option.size() ;
    int j ;

    for(j=0; val && (j<n) ; val=val->cdr, j++){

      declaration_stack.push_back(dse.option[j]) ;
      cell * var = LIST(AST_IDENTIFIER, loc->car, dse.option[j].symbol) ;

      excell * left  = process_all(var) ;
      string s = reconstruct_stack_pop_back() ;
      if(arguments == "") arguments  =                s ;
      else                arguments += string(", ") + s ;
      declaration_stack.pop_back();

      excell * right = process_all((cell*)val->car) ;
      s = reconstruct_stack_pop_back() ;
      arguments += string("=") + s ;

      excell * R = operation_ASSIGN(left, right, L) ;
    }
    if(j<n) semantic_error(L, string("too few arguments to " ) + ast2str(init_declarator));
    if(val) semantic_error(L, string("too many arguments to ") + ast2str(init_declarator));
  }
  excell * R = NULL ;
  { // function body
    string s0 = ast2str(L) ;
    R = process_all(name) ; 

    char * true_name     = (char *) R->element(2) ;
    char * return_symbol = unique_symbol((char*)(string(true_name) + "$return").c_str()) ;
    R->pointer(2)->car = return_symbol ;

    string s1 = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back(
      // string("\n/* func expand: ") + s0 + " */\n" + 
      s1 + "(" + arguments + ")") ;
  }
  return R ;
#undef dse
}

// =================================================================================
// is_function_X
// =================================================================================


static int is_function_(char * declaration, int i){
#define dse (declaration_stack[i])
  cell * prog = yyparse(declaration) ;
  cell * declaration_specifiers = first_ast(prog, AST_DECLARATION_SPECIFIERS) ;
  cell * init_declarator        = first_ast(prog, AST_DECLARATOR) ;
  return same_declaration(declaration_specifiers, dse.declaration_specifiers, 1) &&
         same_declaration(init_declarator       , dse.init_declarator       , 1) ;
#undef dse
}

static int is_COM        (int i){ return is_function_("list COM(list proof) ;", i) ; }
static int is_PI         (int i){ return is_function_("list PI (list proof) ;", i) ; }
static int is_pairing    (int i){ return is_function_("target e(group g0, group g1) ;", i) ; }
static int is_option_crs (int i){ return is_function_("list option_crs() ;", i) ; }
static int is_GS_proofwi (int i){ return is_function_("list GS_proofwi(list crs, list predicate) ;", i) ; }
static int is_GS_proofzk (int i){ return is_function_("list GS_proofzk(list crs, list predicate) ;", i) ; }
static int is_GS_verifywi(int i){ return is_function_("list GS_verifywi(list crs, list predicate, list proof) ;", i) ; }
static int is_GS_verifyzk(int i){ return is_function_("list GS_verifyzk(list crs, list predicate, list proof) ;", i) ; }
static int is_GS_commit  (int i){ return is_function_("list GS_commit (list crs, group g) ;", i) ; }
static int is_GS_commit_g(int i){ return is_function_("list GS_commit_g(list crs) ;", i) ; }
static int is_GS_verify2 (int i){ return is_function_("list GS_verify2(list proof) ;", i) ; }
static int is_GS_setupB2 (int i){ return is_function_("list GS_setupB2(group g) ;", i) ; }
static int is_GS_setupH2 (int i){ return is_function_("list GS_setupH2(group g) ;", i) ; }
static int is_statistics (int i){ return is_function_("list statistics(list x) ;", i) ; }
static int is_setweight  (int i){ return is_function_("list setweight(integer n, list group_variables) ;", i) ; }
static int is_setpriority(int i){ return is_function_("list setpriority(integer n, list group_variables) ;", i) ; }
static int is_exclude    (int i){ return is_function_("list exclude(list X, list Y) ;", i) ; }

// =================================================================================
// do_function_X
// =================================================================================

static excell * do_nullary_function_(cell * L, excell* (*func)(cell * L)){
  cell * exp_list = first_ast(L, AST_EXP_LIST) ;
  cell * val      = exp_list->pointer(2) ;
  if(val) semantic_error(L, string("too many arguments: ") + ast2str(L));
  return process_NULLARY_OPERATION(L,"",func) ;
}
static excell * do_unary_function_  (cell * L, excell* (*func)(excell * X, cell * L)){
  cell * exp_list = first_ast(L, AST_EXP_LIST) ;
  cell * val      = exp_list->pointer(2) ;
  if(!val    ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(val->cdr) semantic_error(L, string("too many arguments: ") + ast2str(L));
  return process_UNARY_OPERATION(L,"","",func,0) ;
}
static excell* do_binary_function_ (
  cell* L,excell* (*func)(excell* X, excell* Y, cell* L), int flag=0
){
  cell * exp_list = first_ast(L, AST_EXP_LIST) ;
  cell * val      = exp_list->pointer(2) ;
  if(!val         ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(!(val->cdr)  ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(val->cdr->cdr) semantic_error(L, string("too many arguments: ") + ast2str(L));
  return process_BINARY_OPERATION(L,"",",","",func,flag) ;
}

static excell* do_binary_function_crs_push (cell* L,excell* (*func)(excell* X, excell* Y, cell* L)){
  cell * exp_list = first_ast(L, AST_EXP_LIST) ;
  cell * val      = exp_list->pointer(2) ;
  if(!val         ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(!(val->cdr)  ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(val->cdr->cdr) semantic_error(L, string("too many arguments: ") + ast2str(L));
  return process_BINARY_OPERATION(L,"",",","",func, PUSH_CRS | SUPPRESS) ;
}

static excell* do_ternary_function_crs_push (cell* L,excell* (*func)(excell* X, excell* Y, excell * Z, cell* L)){
  cell * exp_list = first_ast(L, AST_EXP_LIST) ;
  cell * val      = exp_list->pointer(2) ;
  if(!val              ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(!(val->cdr)       ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(!(val->cdr->cdr)  ) semantic_error(L, string("too few arguments: " ) + ast2str(L));
  if(val->cdr->cdr->cdr) semantic_error(L, string("too many arguments: ") + ast2str(L));
  return process_TERNARY_OPERATION(L,"",",",",","",func, PUSH_CRS | SUPPRESS | DUMMYCOM ) ;
}

static excell* do_option_crs (cell* L){return do_nullary_function_(L,operation_OPTION_CRS);}
static excell* do_COM        (cell* L){return do_unary_function_  (L,operation_COM);}
static excell* do_PI         (cell* L){return do_unary_function_  (L,operation_PI);}
static excell* do_pairing    (cell* L){return do_binary_function_ (L,operation_PAIRING   );}
static excell* do_GS_commit  (cell* L){return do_binary_function_ (L,operation_GS_COMMIT );}
static excell* do_GS_commit_g(cell* L){return do_unary_function_ (L,operation_GS_COMMIT_G);}
static excell* do_GS_proofwi (cell* L){return do_binary_function_crs_push (L,operation_GS_PROOFWI);}
static excell* do_GS_proofzk (cell* L){return do_binary_function_crs_push (L,operation_GS_PROOFZK);}
static excell* do_GS_verifywi(cell* L){return do_ternary_function_crs_push(L,operation_GS_VERIFYWI);}
static excell* do_GS_verifyzk(cell* L){return do_ternary_function_crs_push(L,operation_GS_VERIFYZK);}

static excell* do_GS_setupB2 (cell* L){return do_unary_function_  (L,operation_GS_SETUPB2);}
static excell* do_GS_setupH2 (cell* L){return do_unary_function_  (L,operation_GS_SETUPH2);}
static excell* do_GS_verify2 (cell* L){return do_unary_function_  (L,operation_GS_VERIFY2);}
static excell* do_statistics (cell* L){return do_unary_function_  (L,operation_STATISTICS);}
static excell* do_setweight  (cell* L){return do_binary_function_ (L,operation_SETWEIGHT,SKIPERROR);}
static excell* do_setpriority(cell* L){return do_binary_function_ (L,operation_SETPRIORITY,SKIPERROR);}
static excell* do_exclude    (cell* L){return do_binary_function_ (L,operation_EXCLUDE,SKIPERROR);}

// =================================================================================
// function_X
// =================================================================================

static excell * do_function_(cell * L, excell* (*func)(cell * L)){
  const char * const prefix  = ""  ;
  const char * const infix   = "(" ;
  const char * const postfix = ")" ;
  cell   * arg0 = L->element(2) ;
  cell   * arg1 = L->pointer(3) ? L->element(3) : L ;
  process_all(arg0) ;
  string   s0   = reconstruct_stack_pop_back() ;
  excell * R    = func(arg1) ;
  string   s1   = reconstruct_stack_pop_back() ;
  string   s    = string(prefix) + s0 + infix + s1 + postfix  ;
  reconstruct_stack.push_back(s) ;
  return R ;
}

static excell * option_crs(cell * L){ return do_function_(L, do_option_crs) ; }
static excell * pairing   (cell * L){ return do_function_(L, do_pairing   ) ; }
static excell * COM       (cell * L){ return do_function_(L, do_COM       ) ; }
static excell * PI        (cell * L){ return do_function_(L, do_PI        ) ; }
static excell * GS_commit (cell * L){ return do_function_(L, do_GS_commit ) ; }
static excell * GS_commit_g(cell * L){ return do_function_(L, do_GS_commit_g ) ; }

static excell * do_function_with_expand(cell * L, excell* (*func)(cell * L)){
  reduction_history.push_back("") ;
  excell * R  = do_function_(L, func) ; 
  string   s0 = reconstruct_stack_pop_back() ;
  string   s1 = reduction_history.back() ;
  string   s2 = as_string(R) ;
  s1 = "  " + replace_all("\n","\n  ",s1) ;
  string   s  = s1 + s2 + ";\n" ;
  reduction_history.pop_back() ;
  reconstruct_stack.push_back( string("\n/* expand: ") + s0 + " */\n({\n" + s + "})") ;
  return R ;
}
static excell * GS_proofwi (cell * L){ return do_function_with_expand(L, do_GS_proofwi ) ; }
static excell * GS_proofzk (cell * L){ return do_function_with_expand(L, do_GS_proofzk ) ; }
static excell * GS_verifywi(cell * L){ return do_function_with_expand(L, do_GS_verifywi) ; }
static excell * GS_verifyzk(cell * L){ return do_function_with_expand(L, do_GS_verifyzk) ; }

// ------------------------------------------------------------------------------------------
static excell * GS_setupB2(cell * L){ return do_function_(L, do_GS_setupB2) ; }
static excell * GS_setupH2(cell * L){ return do_function_(L, do_GS_setupH2) ; }
static excell * GS_verify2(cell * L){ return do_function_with_expand(L, do_GS_verify2) ; }
static excell * statistics(cell * L){ return do_function_with_expand(L, do_statistics) ; }
static excell * setweight (cell * L){ return do_function_with_expand(L, do_setweight ) ; }
static excell * setpriority(cell * L){ return do_function_with_expand(L, do_setpriority) ; }
static excell * exclude   (cell * L){ return do_function_with_expand(L, do_exclude) ; }

// =================================================================================
//  FUNC_CALL
// =================================================================================

static excell * process_FUNC_CALL(cell * L){
  string s = ast2str(L->element(2));
  int i = symbol_to_declaration_stack_entry((char*)s.c_str()) ;
  excell * R = NULL ;
  if(i>=0){
    if(declaration_stack[i].type & MACRO_DEFINITION){
      R = macro_expand(L, i) ;
    }else if(declaration_stack[i].type & FUNC_DEFINITION){
      R = func_expand(L, i) ;
    }else if(declaration_stack[i].type & FUNC_DECLARATOR){
      string s = "" ;
           if(is_pairing    (i)) R = pairing    (L) ;
      else if(is_COM        (i)) R = COM        (L) ;
      else if(is_PI         (i)) R = PI         (L) ;
      else if(is_GS_commit  (i)) R = GS_commit  (L) ;
      else if(is_GS_commit_g(i)) R = GS_commit_g(L) ;
      else if(is_option_crs (i)) R = option_crs (L) ;
      else if(is_GS_proofwi (i)) R = GS_proofwi (L) ;
      else if(is_GS_proofzk (i)) R = GS_proofzk (L) ;
      else if(is_GS_verifywi(i)) R = GS_verifywi(L) ;
      else if(is_GS_verifyzk(i)) R = GS_verifyzk(L) ;
      else if(is_GS_setupB2 (i)) R = GS_setupB2 (L) ; // obsolete
      else if(is_GS_setupH2 (i)) R = GS_setupH2 (L) ; // obsolete
      else if(is_GS_verify2 (i)) R = GS_verify2 (L) ; // obsolete
      else if(is_statistics (i)) R = statistics (L) ;
      else if(is_setweight  (i)) R = setweight  (L) ;
      else if(is_setpriority(i)) R = setpriority(L) ;
      else if(is_exclude    (i)) R = exclude    (L) ;
      else                       R = func_expand(L, i) ;
    }else{
      semantic_error(L, s + " is not a function") ;
    }
  }else{
    R = process_BINARY(L,"","(",")");
    string s0 = reconstruct_stack_pop_back() ;
    reconstruct_stack.push_back( string("\n/* not found: [") + s + "] */\n" + s0 ) ;
  }
  return R ;
}

static excell * process_GE          (cell * L){ return process_BINARY(L,"",">=",""); }
static excell * process_GT          (cell * L){ return process_BINARY(L,"",">",""); }
static excell * process_IF          (cell * L){ return process_BINARY(L,"if","",""); }
static excell * process_LE          (cell * L){ return process_BINARY(L,"","<=",""); }
static excell * process_LOGICALOR   (cell * L){ return process_BINARY(L,"","||",""); }
static excell * process_LT          (cell * L){ return process_BINARY(L,"","<",""); }
static excell * process_MOD         (cell * L){ return process_BINARY(L,"","%",""); }

// static excell * process_NE          (cell * L){ return process_BINARY(L,"","!=",""); }

static excell * process_POINTER     (cell * L){ return process_BINARY(L,"*","",""); }
static excell * process_REFERENCE   (cell * L){ return process_BINARY(L,"&","",""); }
static excell * process_SHL         (cell * L){ return process_BINARY(L,"","<<",""); }
static excell * process_SHR         (cell * L){ return process_BINARY(L,"",">>",""); }
static excell * process_WHILE       (cell * L){ return process_BINARY(L,"while(",")",""); }
static excell * process_TYPE_NAME   (cell * L){ return process_BINARY(L,"","",""); }
static excell * process_CAST        (cell * L){ return process_BINARY(L,"(",")",""); }

static excell * process_LABEL       (cell * L){
  const char * const prefix  = ""   ;
  const char * const infix   = ": " ;
  const char * const postfix = ""   ;
  cell * arg0 = L->element(2) ;
  cell * arg1 = L->element(3) ;
  process_all(arg0) ;
  string s0 = reconstruct_stack_pop_back() ;
  excell * R = process_all(arg1) ;
  string s1 = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( string(prefix) + s0 + infix + s1 + postfix ) ;
  return R ;
}

static excell * process_LABELED_INIT(cell * L){ return process_BINARY(L,"",": ",""); }
static excell * process_SWITCH      (cell * L){ return process_BINARY(L,"switch","",""); }
static excell * process_CASE        (cell * L){ return process_BINARY(L,"case ",": ",""); }
static excell * process_BIT_FIELD   (cell * L){ return process_BINARY(L,"",": ",""); }

static excell * process_SU(
  cell * L,
  const char * const prefix,
  const char * const infix,
  const char * const postfix,
  int                declarator_type
){
  cell * loc  = L   ->cdr ;
  cell * arg0 = loc ->cdr ;
  cell * arg1 = arg0->cdr ;

  cell * struct_                 = (cell*)arg0->car ;
  cell * struct_declaration_list = (cell*)arg1->car ;

  cell * declaration_specifiers  = LIST(AST_DECLARATION_SPECIFIERS,&dummy_location,L) ;

  string s = prefix ;

  int sp = declaration_stack_pointer();

  declaration_specifiers_stack_push(declaration_specifiers);
  declaration_stack_push(struct_);
  declaration_stack[sp].type = declarator_type ;
  declaration_specifiers_stack_pop ();

  process_all(struct_) ;
  string s0 = reconstruct_stack_pop_back() ;

  declaration_frame_stack_push() ;

  process_all(struct_declaration_list) ;
  string s1 = reconstruct_stack_pop_back() ;

  int n = declaration_stack_pointer();
  for(int i=sp+1;i<n;i++) declaration_stack[sp].option.push_back(declaration_stack[i]);

  declaration_frame_stack_pop() ;

  reconstruct_stack.push_back( string(prefix) + s0 + infix + s1 + postfix ) ;
  return NULL ;
}

static excell * process_STRUCT(cell * L){
  return process_SU(L,"struct ","{\n","}",STRUCT_DECLARATOR); }
static excell * process_UNION (cell * L){
  return process_SU(L,"union ","{\n","}",UNION_DECLARATOR); }

static excell * process_E(
  cell * L,
  const char * const prefix,
  const char * const infix,
  const char * const postfix
){
  cell * loc  = L   ->cdr ;
  cell * arg0 = loc ->cdr ;
  cell * arg1 = arg0->cdr ;

  cell * enum_           = (cell*)arg0->car ;
  cell * enumerator_list = (cell*)arg1->car ;

  cell * declaration_specifiers = LIST(AST_DECLARATION_SPECIFIERS,&dummy_location,L);

  int sp = declaration_stack_pointer();

  declaration_specifiers_stack_push(declaration_specifiers);
  declaration_stack_push(enum_);
  declaration_stack[sp].type = ENUM_DECLARATOR ;
  declaration_specifiers_stack_pop ();

  process_all(enum_) ;
  string s0 = reconstruct_stack_pop_back() ;

  declaration_specifiers_stack_push(declaration_specifiers) ;

  process_all(enumerator_list) ;
  string s1 = reconstruct_stack_pop_back() ;

  int n = declaration_stack_pointer();
  for(int i=sp+1;i<n;i++) declaration_stack[i].type = ENUM_CONST ;

  declaration_specifiers_stack_pop();

  reconstruct_stack.push_back( string(prefix) + s0 + infix + s1 + postfix ) ;
  return NULL ;
}

static excell * process_ENUM (cell * L){ return process_E(L,"enum ","{","}"); }

// ===================================================================================

static excell * process_TERNARY(
  const cell * const L,
  const char * const prefix,
  const char * const infix1,
  const char * const infix2,
  const char * const postfix
){
  const cell * const loc  = L   ->cdr ;
  const cell * const arg0 = loc ->cdr ;
  const cell * const arg1 = arg0->cdr ;
  const cell * const arg2 = arg1->cdr ;

  process_all((cell*)arg0->car) ;
  process_all((cell*)arg1->car) ;
  process_all((cell*)arg2->car) ;
  string s2 = reconstruct_stack_pop_back() ;
  string s1 = reconstruct_stack_pop_back() ;
  string s0 = reconstruct_stack_pop_back() ;
  reconstruct_stack.push_back( prefix + s0 + infix1 + s1 + infix2 + s2 + postfix ) ;
  return NULL ;
}

static excell * process_IF_ELSE(cell * L){ return process_TERNARY(L,"if","","else",""); }
static excell * process_TERNARY(cell * L){ return process_TERNARY(L,"","?",":",""); }

// ===================================================================================

static excell * process_QUATERNARY(
  const cell * const L,
  const char * const prefix,
  const char * const infix1,
  const char * const infix2,
  const char * const infix3,
  const char * const postfix
){
  const cell * const loc  = L   ->cdr ;
  const cell * const arg0 = loc ->cdr ;
  const cell * const arg1 = arg0->cdr ;
  const cell * const arg2 = arg1->cdr ;
  const cell * const arg3 = arg2->cdr ;

  process_all((cell*)arg0->car) ;
  process_all((cell*)arg1->car) ;
  process_all((cell*)arg2->car) ;
  process_all((cell*)arg3->car) ;
  string s3 = reconstruct_stack_pop_back() ;
  string s2 = reconstruct_stack_pop_back() ;
  string s1 = reconstruct_stack_pop_back() ;
  string s0 = reconstruct_stack_pop_back() ;

  reconstruct_stack.push_back(
    prefix + s0 + infix1 + s1 + infix2 + s2 + infix3 + s3 + postfix
  ) ;
  return NULL ;
}

static excell * process_FOR     (cell * L){
  return process_QUATERNARY(L,"for(",";",";",")",""); 
}
static excell * process_FUNC_DEF(cell * L){
  cell * loc  = L   ->cdr ;
  cell * arg0 = loc ->cdr ;
  cell * arg1 = arg0->cdr ;
  cell * arg2 = arg1->cdr ;
  cell * arg3 = arg2->cdr ;

  cell * declaration_specifiers = (cell*)arg0->car ;
  cell * declarator             = (cell*)arg1->car ;
  cell * declaration_list       = (cell*)arg2->car ; /* K&R */ 
  cell * compound_statement     = (cell*)arg3->car ;

  record_reduction(
    string("// begin: ") + ast2str(declaration_specifiers) + " " + ast2str(declarator)
  ) ;

  int sp = declaration_stack_pointer() ;
  declaration_specifiers_stack_push(declaration_specifiers) ;
  declaration_stack_push(declarator) ;
#define dse (declaration_stack[sp])
  for(int i = dse.chain; i != -1; i = declaration_stack[i].chain){
    if(declaration_stack[i].compound_statement){
      semantic_error(L, string("duplicate function definition: ") + dse.symbol + "()" ) ;
    }
  }

//  declaration_stack_entry & dse = declaration_stack.back() ; // <== BUG CODE
  dse.declaration_list   = declaration_list ;
  dse.compound_statement = compound_statement ;

//  COUT(dse.compound_statement) ;
//  COUT(dse) ;

  declaration_frame_stack_push();

  process_all(declaration_specifiers) ;
  process_all(declarator            ) ;
  for(auto i : declaration_stack[sp].option) declaration_stack.push_back(i) ;
  process_all(declaration_list      ) ;
//  process_all((cell*)arg3->car) ;

  char * return_symbol = unique_symbol((char*)(string(dse.true_name) + "$return").c_str()) ;

  cell * body  = convert_return2 ((cell*)arg3->car, return_symbol) ;
  process_all(body) ;
  free_ast(body) ;

  string s3 = reconstruct_stack_pop_back() ;
  string s2 = reconstruct_stack_pop_back();
  string s1 = reconstruct_stack_pop_back() ;
  string s0 = reconstruct_stack_pop_back() ;

  declaration_frame_stack_pop();

  int is_macro_ = is_macro(declaration_specifiers) ;
  {
//   declaration_stack_entry & dse = declaration_stack.back() ; // <== BUG CODE
    if(is_macro_)dse.type |= MACRO_DEFINITION ;
    else         dse.type |= FUNC_DEFINITION  ;
  }
#undef dse
  declaration_specifiers_stack_pop();

  if(is_macro_){
    if(! declaration_specifiers->cdr->cdr)
      reconstruct_stack.push_back(string("")+s0+""+s1+"\n"+s2+""+s3+"") ;
    else
      reconstruct_stack.push_back(string("")+s0+" "+s1+"\n"+s2+""+s3+"") ;
  }else{
    if(! declaration_specifiers->cdr->cdr)
      reconstruct_stack.push_back(string("")+s0+""+s1+"\n"+s2+""+s3+"") ;
    else
      reconstruct_stack.push_back(string("")+s0+" "+s1+"\n"+s2+""+s3+"") ;
  }

  record_reduction(
    string("// end: ") + ast2str(declaration_specifiers) + " " + ast2str(declarator) + "\n"
  ) ;

  return NULL ;
}

// ===================================================================================

// ===================================================================================

static excell * process_LIST(cell * L) {
  cell * loc = L  ->cdr ;
  cell * arg = loc->cdr ;
  string s = "" ;
  for(;arg;arg=arg->cdr){
    process_all((cell*)arg->car) ;
    if(s == "") s = reconstruct_stack_pop_back() ;
    else        s += "," + reconstruct_stack_pop_back() ;
  }
  reconstruct_stack.push_back( string(L->car+4) + "(" + s + ")" ) ;
  return NULL ;
}

static excell * process_PROG(cell * L) {
  cell * loc = L  ->cdr ;
  cell * arg = loc->cdr ;
  string s = "" ;
  for(;arg;arg=arg->cdr){
    process_all((cell*)arg->car) ;
    if(s == "") s = reconstruct_stack_pop_back() ;
    else        s += "\n" + reconstruct_stack_pop_back() ;
  }
  reconstruct_stack.push_back(s) ;
  return NULL ;
}

static excell * process_DECLARATION_LIST (cell * L) {
  cell * loc = L  ->cdr ;
  cell * arg = loc->cdr ;
  string s = "" ;
  for(;arg;arg=arg->cdr){
    process_all((cell*)arg->car) ;
    s += "  " ;
    s += reconstruct_stack_pop_back() ;
    s += "\n" ;
  }
  reconstruct_stack.push_back(s);
  return NULL ;
}

static excell * process_STAT_LIST (cell * L) {
  cell * loc = L  ->cdr ;
  cell * arg = loc->cdr ;
  string s = "", t ;
  excell * R = NULL ;
  for(;arg;arg=arg->cdr){
    R = process_all((cell*)arg->car) ;
    t = reconstruct_stack_pop_back() ;
    t = "  " + replace_all("\n","\n  ",t) ;
    s += t + "\n" ;
  }
  reconstruct_stack.push_back(s) ;
  return R ;
}

static excell * process_DECLARATION_SPECIFIERS (cell * L) {
  cell * loc = L  ->cdr ;
  cell * arg = loc->cdr ;
  string s = "" ;
  for(;arg;arg=arg->cdr){
    process_all((cell*)arg->car) ;
    if(s == "") s = reconstruct_stack_pop_back() ;
    else        s += " " + reconstruct_stack_pop_back() ;
  }
  reconstruct_stack.push_back(s) ;
  return NULL ;
}

static excell * process_TYPE_QUALIFIER_LIST (cell * L) {
  return process_DECLARATION_SPECIFIERS (L);
}
static excell * process_SPECIFIER_QUALIFIER_LIST(cell * L) {
  return process_DECLARATION_SPECIFIERS (L);
}

static excell * process_EXP_LIST(cell * L) {
  cell * loc = L  ->cdr ;
  cell * arg = loc->cdr ;
  string s = "" ;
  int single = 0 ;
  if(arg) if(! (arg->cdr)) single = 1 ;
  excell * R = NULL ;
  if(single){
    R = process_all((cell*)arg->car) ;
    s = reconstruct_stack_pop_back() ;
  }else{
    R = (excell*)LIST(TYPE_LIST_LITERAL, L->element(1)) ;
    int i ;
    for(;arg;arg=arg->cdr){
      excell * T = process_all((cell*)arg->car) ;
      if(s == "") s  =        reconstruct_stack_pop_back() ;
      else        s += ", " + reconstruct_stack_pop_back() ;
      R = (excell*)INTEGRATE(R,T) ;
    }
  }
  reconstruct_stack.push_back(s) ;
  return R ;
}

static excell * process_PARAMETER_LIST       (cell * L) { return process_EXP_LIST(L); }
static excell * process_IDENTIFIER_LIST      (cell * L) { return process_EXP_LIST(L); }
static excell * process_INITIALIZER_LIST     (cell * L) { return process_EXP_LIST(L); }

static excell * process_INIT_DECLARATOR_LIST (cell * L) {
  cell * loc = L  ->cdr ;
  cell * arg = loc->cdr ;
  string type_s = reconstruct_stack_pop_back()    ;
                  reconstruct_stack.push_back("") ;
  int    group  = reconstruction_mode && is_group_variant(type_s) ;
  string s = "" ;
  int current_gtype = -1 ;

  for(;arg;arg=arg->cdr){
    cell * M = (cell*)arg->car ;
    declaration_stack_push((cell*)arg->car) ;
    process_all((cell*)arg->car) ;
    if(group){
      string declarator = reconstruct_stack_pop_back() ;
      string name       = declarator.substr(0,declarator.find("=")) ;

      if(name.find("(") != string::npos){
        name = name.substr(0,name.find("(")) ;
        name += "$return" ;
      }

      int gtype = group_of[name] ;

      if(gtype == current_gtype){
        s += ", " + declarator ;
      }else{
        current_gtype = gtype ;
        switch(gtype){
          case  1: declarator = "group0 "+ declarator ; break ;
          case  2: declarator = "group1 "+ declarator ; break ;
          case  3: declarator = "duplex "+ declarator ; break ;
          default: declarator = "unused "+ declarator ; break ;
        }
        if(s == "") s  =        declarator ;
        else        s += "; " + declarator ;
      }
    }else{
      if(s == "") s =         reconstruct_stack_pop_back() ;
      else        s += ", " + reconstruct_stack_pop_back() ;
    }
  }
  if(!group) s = type_s + " " + s ;

  reconstruct_stack.push_back(s) ;
  return NULL ;
}

static unordered_map<char const * const, excell * (*)(cell *)> table = {
  {AST_ABSTRACT_DECLARATOR,       process_ABSTRACT_DECLARATOR},
  {AST_ADD,                       process_ADD},
  {AST_ADDASSIGN,                 process_ADDASSIGN},
  {AST_ADDRESSOF,                 process_ADDRESSOF},
  {AST_ANDASSIGN,                 process_ANDASSIGN},
  {AST_ARRAY_DECLARATOR,          process_ARRAY_DECLARATOR},
  {AST_ARRAY_REF,                 process_ARRAY_REF},
  {AST_ARROW,                     process_ARROW},
  {AST_ASM,                       process_LIST},
  {AST_ASM_INIT,                  process_LIST},
  {AST_ASM_IN_OUT,                process_LIST},
  {AST_ASSIGN,                    process_ASSIGN},
  {AST_BITAND,                    process_BITAND},
  {AST_BIT_FIELD,                 process_BIT_FIELD},
  {AST_BITOR,                     process_BITOR},
  {AST_BITXOR,                    process_BITXOR},
  {AST_BREAK,                     process_BREAK},
  {AST_CASE,                      process_CASE},
  {AST_CAST,                      process_CAST},
  {AST_CHAR,                      process_CHAR},
  {AST_COMPOUND_STATEMENT,        process_COMPOUND_STATEMENT},
  {AST_CONTINUE,                  process_CONTINUE},
  {AST_CURLY_BRACES,              process_CURLY_BRACES},
  {AST_DECLARATION,               process_DECLARATION},
  {AST_DECLARATION_LIST,          process_DECLARATION_LIST},
  {AST_DECLARATION_SPECIFIERS,    process_DECLARATION_SPECIFIERS},
  {AST_DECLARATOR,                process_DECLARATOR},
  {AST_DEFAULT,                   process_DEFAULT},
  {AST_DEREFERENCEOF,             process_DEREFERENCEOF},
  {AST_DIV,                       process_DIV},
  {AST_DIVASSIGN,                 process_DIVASSIGN},
  {AST_DO,                        process_DO},
  {AST_DOT,                       process_DOT},
  {AST_ENUM,                      process_ENUM},
  {AST_EQ,                        process_EQ},
  {AST_EXP_LIST,                  process_EXP_LIST},
  {AST_EXP_STAT,                  process_EXP_STAT},
  {AST_FOR,                       process_FOR},
  {AST_FUNC_CALL,                 process_FUNC_CALL},
  {AST_FUNC_DECLARATOR,           process_FUNC_DECLARATOR},
  {AST_FUNC_DEF,                  process_FUNC_DEF},
  {AST_GE,                        process_GE},
  {AST_GOTO,                      process_GOTO},
  {AST_GT,                        process_GT},
  {AST_IDENTIFIER,                process_IDENTIFIER},
  {AST_IDENTIFIER_LIST,           process_IDENTIFIER_LIST},
  {AST_IF,                        process_IF},
  {AST_IF_ELSE,                   process_IF_ELSE},
  {AST_INIT,                      process_INIT},
  {AST_INITIALIZER_LIST,          process_INITIALIZER_LIST},
  {AST_INIT_DECLARATOR_LIST,      process_INIT_DECLARATOR_LIST},
  {AST_INLINE,                    process_INLINE},
  {AST_LABEL,                     process_LABEL},
  {AST_LABELED_INIT,              process_LABELED_INIT},
  {AST_LE,                        process_LE},
  {AST_LIST,                      process_LIST},
  {AST_LOGICALAND,                process_LOGICALAND},
  {AST_LOGICALOR,                 process_LOGICALOR},
  {AST_LT,                        process_LT},
  {AST_MOD,                       process_MOD},
  {AST_MODASSIGN,                 process_MODASSIGN},
  {AST_MUL,                       process_MUL},
  {AST_MULASSIGN,                 process_MULASSIGN},
  {AST_NE,                        process_NE},
  {AST_NEG,                       process_NEG},
  {AST_NEW_IDENTIFIER,            process_NEW_IDENTIFIER},
  {AST_NOT,                       process_NOT},
  {AST_NULL,                      process_NULL},
  {AST_NUMBER,                    process_NUMBER},
  {AST_ORASSIGN,                  process_ORASSIGN},
  {AST_PARAMETER_DECLARATION,     process_PARAMETER_DECLARATION},
  {AST_PARAMETER_LIST,            process_PARAMETER_LIST},
  {AST_PARENTHESIS,               process_PARENTHESIS},
  {AST_POINTER,                   process_POINTER},
  {AST_POSTMM,                    process_POSTMM},
  {AST_POSTPP,                    process_POSTPP},
  {AST_POW,                       process_POW},
  {AST_POWASSIGN,                 process_POWASSIGN},
  {AST_PREMM,                     process_PREMM},
  {AST_PREPP,                     process_PREPP},
  {AST_PROG,                      process_PROG},
  {AST_REFERENCE,                 process_REFERENCE},
  {AST_RETURN,                    process_RETURN},
  {AST_SHL,                       process_SHL},
  {AST_SHLASSIGN,                 process_SHLASSIGN},
  {AST_SHR,                       process_SHR},
  {AST_SHRASSIGN,                 process_SHRASSIGN},
  {AST_SIZEOF_EXP,                process_SIZEOF_EXP},
  {AST_SIZEOF_TYPE,               process_SIZEOF_TYPE},
  {AST_BRACKETS,                  process_BRACKETS},
  {AST_STAT_LIST,                 process_STAT_LIST},
  {AST_STORAGE_CLASS_SPECIFIER,   process_STORAGE_CLASS_SPECIFIER},
  {AST_STRING,                    process_STRING},
  {AST_STRUCT,                    process_STRUCT},
  {AST_SUB,                       process_SUB},
  {AST_SUBASSIGN,                 process_SUBASSIGN},
  {AST_SWITCH,                    process_SWITCH},
  {AST_TERNARY,                   process_TERNARY},
  {AST_TYPE_NAME,                 process_TYPE_NAME},
  {AST_TYPE_QUALIFIER,            process_TYPE_QUALIFIER},
  {AST_TYPE_QUALIFIER_LIST,       process_TYPE_QUALIFIER_LIST},
  {AST_UMINUS,                    process_UMINUS},
  {AST_UNION,                     process_UNION},
  {AST_UPLUS,                     process_UPLUS},
  {AST_WHILE,                     process_WHILE},
  {AST_XORASSIGN,                 process_XORASSIGN},
} ;

excell * process_all(cell * L) {
//  cout << L->car << endl ;
  auto r = table.find(L->car) ;
  if(r != table.end()) return r->second(L) ;
  else unexpected_error(L, "unknown node symbol") ;
  return NULL ;
}

extern "C" {
#include "syntactic.h"
}

#ifdef SEMANTIC_DEBUG

extern "C" int yyparse(void) ;
extern "C" cell * AST ;
extern "C" FILE * yyin ;

int main(int argc, char **argv) {
#include "symbol_define.txt"
  if(argc > 1) true_IDENTIFIER = 0 ;
  yyin  = fdopen(0,"r") ;
  yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
  if(yyparse() == 0){
    cell * A = AST ;
    process_all(PREAMBLE) ;

    clear_reconstruct_stack() ;
    reduction_history = {""} ;
    gsconv_input  = "scheme(){\n" ;
    process_all(A) ;
    gsconv_input += "}\n" ;

    cout << "-- reconstruction buffer --" << endl ;
    print_reconstruct_stack() ;
//    cout << "-- reduction history --" << endl ;
//    for(auto i : reduction_history) cout << i << endl ;
    cout << "-- gsconv input --" << endl ;
    cout << gsconv_input << endl ;
  }else{
    exit(1) ; // syntactic error
  }
  return EXIT_SUCCESS ;
}

#endif
