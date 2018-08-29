#include "symbol_stack.h"
#include "syntactic.h"
#include <iostream>
// #include <unordered_set>
#include <vector>

using namespace std ;

typedef struct {
  string symbol      ;
  int    symbol_type ;
  char * type_name   ;
} symbol_stack_entry ;

static vector<symbol_stack_entry> symbol_stack ;

int find(const string & x, const vector<symbol_stack_entry> & y){
  for(int i = y.size()-1; i>=0; i--){
    if(x==y[i].symbol)return i ;
  }
  return -1;
}

extern "C" {

  int symbol_stack_pointer(){
    return symbol_stack.size() ;
  }

  void push_symbol(char * symbol, int symbol_type, char * type_name){
    symbol_stack_entry e = {string(symbol), symbol_type, type_name} ;
    symbol_stack.push_back(e);
  }

  void restore_symbol_stack(int n){
    while(symbol_stack.size() > n)
      symbol_stack.pop_back() ;
  }

  int symbol_to_symbol_type(char * symbol){
    int i = find(symbol, symbol_stack) ;
    if(i >= 0){
      return symbol_stack[i].symbol_type ;
    }else{
//      cerr << "IDENTIFIER: " << symbol << endl ;
      return IDENTIFIER ;
    }
  }

  const char * symbol_to_type_name(char * symbol){
    int i = find(symbol, symbol_stack) ;
    if(i >= 0){
      return symbol_stack[i].type_name ;
    }else{
      return "" ;
    }
  }

}

#if 0
int main(){
//  unordered_set<string> symbol_set ;
  cout << symbol_stack.size() << endl ;
  return 0 ;
}
#endif
