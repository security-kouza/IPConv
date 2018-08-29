#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "lisp.h"
#include "operation.h"

extern excell * process_all           (cell * L                             ) ;
extern excell * process_IDENTIFIER_wo (cell * L                             ) ;
extern void     semantic_error        (cell * L, const std::string & message) ;
extern cell   * dummy_LOCATION ;
extern void     clear_reconstruct_stack() ;
extern void     print_reconstruct_stack() ;

struct crs_stack_entry {
  excell * crs ;
  int dummycom ;
} ;

extern std::vector<crs_stack_entry> crs_stack ;
extern boost::unordered_map<std::string, int> prohibited_stack ;
extern boost::unordered_map<std::string, int> group0_stack ;
extern boost::unordered_map<std::string, int> group1_stack ;
extern boost::unordered_map<std::string, int> duplex_stack ;

// extern std::vector<char *> prohibited_stack ;

#endif /* SEMANTIC_H */
