#ifndef AST2STR_H
#define AST2STR_H
#include "lisp.h"
extern std::string ast2str(cell * L) ;
extern std::string AST2STR(cell * L) ;
static inline std::ostream & operator<<(std::ostream& out, cell * L){return out << AST2STR(L);}
#endif /* AST2STR_H */
