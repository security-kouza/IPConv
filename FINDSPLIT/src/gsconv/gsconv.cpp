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


#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "type_define.txt"

extern "C" int yyparse(void) ;
extern "C" cell * AST ;

extern "C" {
#include "syntactic.h"
}

using namespace std ;
using namespace boost ;

int reconstruct = 0 ;
unordered_map<string, int> group_of ;
vector<string> reconstruct_stack ;

int parse_dot(const char * filename){
  ifstream dotfile(filename) ;
  if (!dotfile.is_open()) return 0 ;

  smatch split ;
  string line ;
  int Dup = 0 ;
  int G0  = 0 ;
  int G1  = 0 ;

  static const regex begin ("^-- vertex list --"     ) ;
  static const regex end   ("^-- adjacency list --"  ) ;
  static const regex symbol("[^ 	]+"           ) ;
  static const regex g0g1  ("is_G\\[0\\],is_G\\[1\\]") ;
  static const regex g0    ("is_G\\[0\\]"            ) ;
  static const regex g1    ("is_G\\[1\\]"            ) ;

  while(getline(dotfile,line))if(regex_search(line,begin))break ;
  while(getline(dotfile,line)){
    if(regex_search(line,end)) break ;
    regex_search(line,split,symbol); 
    const string & vname = split[0];
    /* */ if(regex_search(line,g0g1)){
      group_of[vname] = 3 ; Dup++ ;
    }else if(regex_search(line,g0)){
      group_of[vname] = 1 ; G0++ ;
    }else if(regex_search(line,g1)){
      group_of[vname] = 2 ; G1++ ;
    }
  }
  printf("// duplex %d, group0 %d, group1 %d\n",Dup,G0,G1);
  dotfile.close();
  return 1 ;
}

int main(int argc, char ** argv){
#include "symbol_define.txt"
  switch(argc){
    case 1:
      if(yyparse() == 0){
        process_all(PREAMBLE) ;
        process_all(AST) ;
      }else{
        // syntactic error
        exit(1) ;
      }
      break ;

    case 2:
      if(!parse_dot(argv[1])) exit(2) ; // dot error
      if(yyparse() == 0){
        process_all(PREAMBLE) ;
        reconstruct = 1 ;
        process_all(AST) ;
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

  if(reconstruct){
    for(int i=0;i<reconstruct_stack.size();i++){
      cout << "recon: " << reconstruct_stack[i] << endl ;
    }
  }

  return EXIT_SUCCESS ;
}
