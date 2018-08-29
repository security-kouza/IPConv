#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cell_s {
  char          * car ;
  struct cell_s * cdr ;
} cell ;

extern cell * _ast        (int dummy, ... ) ;
extern cell * _ast_append (int dummy, ... ) ;

#define ast(...) ast_i(__VA_ARGS__)
#define ast_i(...) _ast(0,##__VA_ARGS__,ENDOFVA)

#define ast_append(L,...) ast_append_j(L,ast(__VA_ARGS__))
#define ast_append_j(...) ast_append_i(__VA_ARGS__)
#define ast_append_i(...) _ast_append(0,##__VA_ARGS__,ENDOFVA)
#define ENDOFVA ((void*)-1)

#ifdef __cplusplus
}
#endif

#endif /* AST_H */
