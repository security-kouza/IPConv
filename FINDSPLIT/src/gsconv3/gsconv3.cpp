#include "lisp.h"
#include "ast.h"
#include "reserved_word.h"
#include "semantic.h"

#include <iostream>
#include <vector>
#include <stdint.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/unordered_map.hpp>
#include <string.h>

#include "declaration.h"
#include "operation.h"
#include "preamble.h"

#include <iostream>
#include <fstream>
#include <string>
// #include <regex>

#define YY_BUF_SIZE 32768

extern "C" {
  extern int yyparse(void) ;
  extern cell * AST ;
  extern FILE * yyin ;

  typedef struct yy_buffer_state * YY_BUFFER_STATE ;
  extern YY_BUFFER_STATE yy_create_buffer( FILE *file, int size) ;
  extern void yy_switch_to_buffer( YY_BUFFER_STATE new_buffer) ;
  extern void yy_delete_buffer( YY_BUFFER_STATE buffer ) ;
  extern void yy_flush_buffer(YY_BUFFER_STATE buffer) ;
  extern YY_BUFFER_STATE yy_scan_string(char * str);
}

extern "C" {
#include "syntactic.h"
}

using namespace std ;
using namespace boost ;

#if 0
int parse_dot(const char * filename){
  int phase = 0 ;
  int Dup   = 0 ;
  int G0    = 0 ;
  int G1    = 0 ;
  ifstream dotfile(filename) ;
  if (dotfile.is_open()){
    phase++ ; // 1
    smatch split ;
    string line ;
  
    static const regex begin ("^-- vertex list --"     ) ;
    static const regex end   ("^-- adjacency list --"  ) ;
    static const regex symbol("[^ 	]+"            ) ;
    static const regex g0g1  ("is_G\\[0\\],is_G\\[1\\]") ;
    static const regex g0    ("is_G\\[0\\]"            ) ;
    static const regex g1    ("is_G\\[1\\]"            ) ;
    static const regex pair  ("^p[0-9]+\\[[01]\\] :"   ) ;

    while(getline(dotfile,line))if(regex_search(line,begin)){phase++; break ;} // 2
    while(getline(dotfile,line)){
      if(regex_search(line,end)){phase++; break;} // 3
      regex_search(line,split,symbol); 
      const string & vname = split[0];
            if(regex_search(line,pair)){
        // pairing node
      }else if(regex_search(line,g0g1)){
        group_of[vname] = 3 ; Dup++ ;
      }else if(regex_search(line,g0)){
        group_of[vname] = 1 ; G0++ ;
      }else if(regex_search(line,g1)){
        group_of[vname] = 2 ; G1++ ;
      }
    }
    dotfile.close();
  }
  if(phase == 3){
    printf("// success to load %s. duplex %d, group0 %d, group1 %d (regular nodes)\n",filename,Dup,G0,G1);
    return 1 ;
  }else{
    printf("// fail to load %s. phase = %d\n",filename,phase) ;
    return 0 ;
  }
}
#else

static int is_pair(const string & s){
  if(s[0] != 'p') return 0 ;
  int n = s.find('[') ;
  if(n==string::npos || n<=1) return 0 ;
  string t = s.substr(n) ;
  if( (t != "[0]") && (t != "[1]") ) return 0 ;
  string u = s.substr(1,n-1) ;
  int m = u.size() ;
  for(int i=0;i<m;i++) if((u[i] > '9') || (u[i] < '0')) return 0 ;
  return 1;
}

int parse_dot(const char * filename){
  int phase = 0 ;
  int Dup   = 0 ;
  int G0    = 0 ;
  int G1    = 0 ;
  ifstream dotfile(filename) ;
  if (dotfile.is_open()){
    phase++ ; // 1
    string line ;
  
    static const string begin ("-- vertex list --"     ) ;
    static const string end   ("-- adjacency list --"  ) ;
    static const string g0g1  ("is_G[0],is_G[1]"       ) ;
    static const string g0    ("is_G[0]"               ) ;
    static const string g1    ("is_G[1]"               ) ;

    while(getline(dotfile,line))if(line.find(begin) == 0){phase++; break ;} // 2
    while(getline(dotfile,line)){
      if(line.find(end) == 0){phase++; break;} // 3
      const string vname = line.substr(0,line.find(' ')) ;
            if(is_pair(vname)){
        // pairing node
      }else if(line.find(g0g1) != string::npos){
        group_of[vname] = 3 ; Dup++ ;
      }else if(line.find(g0) != string::npos){
        group_of[vname] = 1 ; G0++ ;
      }else if(line.find(g1) != string::npos){
        group_of[vname] = 2 ; G1++ ;
      }
    }
    dotfile.close();
  }
  if(phase == 3){
    printf("// success to load %s. duplex %d, group0 %d, group1 %d (regular nodes)\n",filename,Dup,G0,G1);
    return 1 ;
  }else{
    printf("// fail to load %s. phase = %d\n",filename,phase) ;
    return 0 ;
  }
}

#endif

vector< pair<char *, excell *> > sorted_hist ;
boost::unordered_map <char *, char *> type_map ;
vector< char *                 > type_hist ;

// ====================================================================

// ここから

struct  transition_graph_vertex_property ;
typedef adjacency_list <
  setS, // setS, hash_setS : for single edges
        // listS, vecS     : for parallel edges
  vecS,
  bidirectionalS,
  transition_graph_vertex_property
> transition_graph_0 ;

struct  transition_graph_vertex_property {
  int    found    ;
  int    is_group ;
  char * symbol   ;
} ;

typedef graph_traits<transition_graph_0>::vertex_descriptor vertex_t ;
typedef graph_traits<transition_graph_0>::edge_descriptor   edge_t   ;

struct  transition_graph : transition_graph_0 {
  boost::unordered_map <char *, vertex_t> symbol_to_vertex_table ;

  int found(char * symbol){
    auto i = symbol_to_vertex_table.find(symbol) ;
    return (i != symbol_to_vertex_table.end()) ;
  }

  vertex_t symbol_to_vertex(char * symbol){
    auto i = symbol_to_vertex_table.find(symbol) ;
    if(i == symbol_to_vertex_table.end()){
      transition_graph & g = (*this) ;
      vertex_t v = add_vertex(g) ;
      g[v].found    = 0      ;
      g[v].is_group = 0      ;
      g[v].symbol   = symbol ;
      return symbol_to_vertex_table[symbol] = v ;
    }else{
      return i->second ;
    }
  }


} ;

transition_graph TG ;

static void freshen(){
  transition_graph & g = TG ;
  graph_traits<transition_graph>::vertex_iterator vi, vi_end ;

  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    g[*vi].found = 0 ;
  }
}

static int is_group(vertex_t v){
  transition_graph & g = TG ;

  g[v].found = 1 ;

  if(g[v].is_group){
    return 1 ;
  }else{
    graph_traits<transition_graph>::in_edge_iterator  ei, ei_end ;
    graph_traits<transition_graph>::out_edge_iterator eo, eo_end ;
    for(tie(ei, ei_end)=in_edges(v,g);ei != ei_end; ei++){
      vertex_t u = source(*ei,g) ;
      if(!g[u].found){
        if(is_group(u)){
          g[v].is_group = 1 ;
          return 1 ;
        }
      }
    }
    for(tie(eo, eo_end)=out_edges(v,g);eo != eo_end; eo++){
      vertex_t u = target(*eo,g) ;
      if(!g[u].found){
        if(is_group(u)){
          g[v].is_group = 1 ;
          return 1 ;
        }
      }
    }
    return 0 ;
  }
}

static void make_closure(){
  transition_graph & g = TG ;
  graph_traits<transition_graph>::vertex_iterator vi, vi_end ;

  for(tie(vi, vi_end) = vertices(g); vi != vi_end; vi++){
    freshen();
    is_group(*vi);
  }
}

static char * resolve_type(excell * X, unsigned long long n){
  if ( X->car == TYPE_LIST_VARIABLE) {
    if(n){
      char * name = as_LIST_VARIABLE(X) ;
      hist_entry & h = history[name] ;
      for(int i=h.size()-1; i>=0; i--){
        unsigned long long d = h.date[i] ;
        if (d<n){
          char * R = resolve_type(h[i], d) ;
          if(R != TYPE_LIST_VARIABLE) return R ;
        }
      }
      return TYPE_LIST_VARIABLE ;
    }else return TYPE_LIST_VARIABLE ;
  }else if ( X->car == TYPE_IMPOSSIBLE ) {
    return resolve_type(X->element(3), n) ;
  }else return X->car ;
}

// ====================================================================

static void setup_sorted_hist(){
  sorted_hist.resize (history_date) ;
  type_hist  .resize (history_date) ;
  for(
    auto i =history.begin();
         i!=history.end();
         i++
  ){
    char * true_name     = i->first  ;
    hist_entry & h = i->second ;
    for(auto j=0; j<h.size(); j++){
      excell * e = h[j] ;
      unsigned long long d = h.date[j] ;
      sorted_hist[d] = make_pair(true_name, e) ;
    }
  }

// setup tansition graph

  for(unsigned long long i=0; i<history_date; i++){
    char   * true_name = sorted_hist[i].first  ;
    excell * value     = sorted_hist[i].second ;
    vertex_t v = TG.symbol_to_vertex(true_name) ;
    if(is_GROUP(value)){
      TG[v].is_group = 1 ;
      GROUP val_g = as_GROUP(value) ;
      for (auto j = val_g.begin(); j != val_g.end(); j++) {
        char * variable = unique_symbol((char *)j->first.c_str())  ;
        vertex_t u = TG.symbol_to_vertex(variable) ;
        TG[u].is_group = 1 ;
        add_edge(u,v,TG) ;
      }
    }else if(is_LIST_VARIABLE(value)){
      char * variable = as_LIST_VARIABLE(value) ;
      vertex_t u = TG.symbol_to_vertex(variable) ;
      add_edge(u,v,TG) ;
    }
  }

  make_closure() ;

  for(unsigned long long i=0; i<history_date; i++){
    char   * true_name = sorted_hist[i].first  ;
    excell * value     = sorted_hist[i].second ;
    if(TG.found(true_name)){
      vertex_t v = TG.symbol_to_vertex(true_name) ;
      if(TG[v].is_group){
        type_hist[i] = TYPE_GROUP ;
      }
    }
  }

// ここも

  for(unsigned long long i=0; i<history_date; i++){
    char   * true_name = sorted_hist[i].first  ;
    excell * value     = sorted_hist[i].second ;
    if(type_hist[i]==NULL){

      char * type = resolve_type(value, i) ;
      hist_entry & h = history[true_name] ;
      for(int j=0; j<h.size(); j++){
        unsigned long long d = h.date[j] ;
        type_hist[d] = type ;
      }

    }
  }

  for(unsigned long long i=0; i<history_date; i++){
    if(type_hist[i] == TYPE_LIST_VARIABLE){
      char   * true_name = sorted_hist[i].first  ;
      excell * value     = sorted_hist[i].second ;
      hist_entry & h = history[unique_symbol((char*)as_string(value).c_str())] ;
      for(int j=0; j<h.size(); j++){
        unsigned long long d = h.date[j] ;
        if(type_hist[d] != TYPE_LIST_VARIABLE){
          type_hist[i] = type_hist[d] ;
          break ;
        }
      }
    }
  }

}

static char * mold(char * X){
  if ( X == TYPE_INTEGER      ) return "integer " ;
  if ( X == TYPE_GROUP        ) return "group "   ;
  if ( X == TYPE_TARGET       ) return "target "  ;
  if ( X == TYPE_LIST_VARIABLE) return "listv "    ;
  if ( X == TYPE_LIST_LITERAL ) return "listl "    ;
  if ( X == TYPE_OPTION       ) return "option "  ;
  if ( X == TYPE_LOGICAL_AND  ) return "integer "  ;
  return "unknown " ;
}


int main(int argc, char ** argv){
  unsigned long long begin_history_date ;
  yyin  = fdopen(0,"r") ;
  yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
  switch(argc){
    case 1:
      if(yyparse() == 0){
        cell * A = AST ;
        process_all(PREAMBLE) ;

        begin_history_date = history_date ; 

        clear_reconstruct_stack() ;
        reduction_history = {""} ;
	weight = {} ;

        gsconv_input  = "scheme(){\n" ;
        process_all(A) ;
        gsconv_input += "}\n" ;

        // ====================================================

#if 1
        gsconv_input  = "scheme(){\n" ; // reset

        setup_sorted_hist() ;

        boost::unordered_map<string, int   > check ;
        boost::unordered_map<string, char *> check_type ;
        boost::unordered_map<string, int   > check_use  ;

        for(auto i=begin_history_date; i<history_date ; i++){
          char   * true_name = sorted_hist[i].first  ;
          string   true_name_s = string(true_name) ;
          excell * value     = sorted_hist[i].second ;
          string   value_s   = as_string(value) ;
          char   * type      = type_hist[i] ;

// ================================
          if(value_s != true_name_s){
            if( type == TYPE_GROUP ) {
              if(value_s.find('~')==string::npos){
                if(is_GROUP(value)){
                  GROUP val_g = as_GROUP(value) ;
                  for (auto j = val_g.begin(); j != val_g.end(); j++) {
                    const string & variable = j->first ;
                    if(check[variable] == 0){
                      stringstream ss ;
                      ss << "group " << variable << " ;" << endl ;
                      string s = ss.str() ;
                      if(s.find('~')==string::npos){
                        gsconv_input += s ;
                        check_use[variable]++ ;
                      }
                      check[variable]++ ;
                      check_type[variable] = TYPE_GROUP ;
                    }
                  }
                }else if(is_LIST_VARIABLE(value)){
                  const string variable = as_LIST_VARIABLE(value) ;
                  if(check[variable] == 0){
                    stringstream ss ;
                    ss << "group " << variable << " ;" << endl ;
                    string s = ss.str() ;
                    if(s.find('~')==string::npos){
                      gsconv_input += s ;
                      check_use[variable]++ ;
                    }
                    check[variable]++ ;
                    check_type[variable] = TYPE_GROUP ;
                  }
                }
              }
            }
          }
// ================================

          if (
              ( type == TYPE_INTEGER )||
              ( type == TYPE_GROUP   )||
              ( type == TYPE_TARGET  )){
            string type_name = "" ;
            if(check[true_name_s] == 0){
              type_name = mold(type) ;
            }
            if(value_s == true_name_s){
              if(type_name != ""){
                stringstream ss ;
                ss << type_name << true_name << " ;" << endl ;
                string s = ss.str() ;
                if(s.find('~')==string::npos) gsconv_input += s ;
              }
            }else{
              stringstream ss ;
              ss << type_name << true_name << "=" << value << " ;" << endl ;
              string s = ss.str() ;
              if(s.find('~')==string::npos){
                if((type == TYPE_INTEGER)||(s.find("=1 ;")==string::npos)){
                  gsconv_input += s ;
                  check_use[true_name_s]++ ;
                }
              }
            }
            check[true_name_s]++ ;
            check_type[true_name_s] = type ;
          }
        }

        gsconv_input += "}\n" ;

#endif

        // ====================================================

        gsconv_input += "inline \"[Prohibits]\n" ;
	string s = "" ;

	if(prohibited_stack.size()){
          for(
            auto i=prohibited_stack.begin();
            i!=prohibited_stack.end();
            i++
          ){
            if(check[i->first]){
              if(s=="") s += i->first ;
              else s+= string(",") + i->first ;
            }
          }
          s += ";\n" ;
	}

	if(group0_stack.size()){
          for(
            auto i=group0_stack.begin();
            i!=group0_stack.end();
            i++
          ){
            if(check[i->first]){
              if(s=="") s += i->first ;
              else s+= string(",") + i->first ;
            }
          }
          s += " = 0 ;\n" ;
        }

	if(group1_stack.size()){
          for(
            auto i=group1_stack.begin();
            i!=group1_stack.end();
            i++
          ){
            if(check[i->first]){
              if(s=="") s += i->first ;
              else s+= string(",") + i->first ;
            }
          }
  
          s += " = 1 ;\n" ;
        }

	for(
	  auto i=exclusion.begin();
	  i!=exclusion.end();
	  i++
	){
	  s += i->first ;
          s += "!=" ;
	  s += i->second ;
	  s += ";\n" ;
	}

        gsconv_input += s ;
        gsconv_input += "\" ;\n" ;

        // ====================================================
        // [Weight]
        // ====================================================

        int weight_flag = 0 ;

	s = "" ;
        for(auto i=check.begin();
            i!=check.end();
            i++
        ){
          if((check_type[i->first] == TYPE_GROUP)&&(check_use[i->first])){
            if(i->second){
              if(weight[i->first] != 0){
                s += i->first + "=" + to_string(weight[i->first]) + "\n" ;
                weight_flag = 1 ;
              }else{
                s += i->first + "=0 %" + to_string(check_use[i->first]) + "\n" ;
              }
            }
          }
        }

        if(weight_flag){
          gsconv_input += "inline \"[Weight]\n" ;
          gsconv_input += s ;
          gsconv_input += "\" ;\n" ;
        }

        // ====================================================

        gsconv_input += "inline \"[Priority]\n" ;
        for(
          auto i =priority.begin();
               i!=priority.end();
               i++
        ){
          const boost::unordered_map<string, int> & vec = i->second ;
          s = "" ;
          for(
            auto j=vec.begin();
                 j!=vec.end();
                 j++
          ){
            if(check[j->first]){
              if(s=="") s += j->first ;
              else s+= string(",") + j->first ;
	    }
          }
          gsconv_input += s + "\n" ;
        }
        gsconv_input += "\" ;\n" ;

        // ====================================================

        cout << gsconv_input << endl ;

        // ====================================================

      }else{
        // syntactic error
        exit(1) ;
      }
      break ;

    case 2:
      reconstruction_mode = 1 ;
      if(!parse_dot(argv[1])) exit(2) ; // dot error
      if(yyparse() == 0){
        cell * A = AST ;
        process_all(PREAMBLE) ;

        clear_reconstruct_stack() ;
        reduction_history = {""} ;
	weight = {} ;
        process_all(A) ;
        print_reconstruct_stack() ;
      }else{
        // syntactic error
        exit(1) ;
      }
      break ;

    default:
      // option error
      exit(3) ;
      break ;
  }

  return EXIT_SUCCESS ;
}
