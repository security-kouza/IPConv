%{
#include <stdio.h>
#include <stdint.h>
#include "ast.h"
#include "ast_node_def.h"
#define YYSTYPE cell*

extern char * debug_format ;

// extern intptr_t line ;
extern int yylineno;
#define line ((intptr_t)yylineno)
extern int yylex() ;
extern int yyparse() ;
int yyerror(const char *s)
{
  fprintf (stderr, "line %ld syntactic error: %s\n", line, s);
  return 0;
}

#ifdef SYNTACTIC_DEBUG

cell * print_node(cell * L, int n, int recursive){
  cell * M ;
  int i ;

  for(i=0;i<n;i++){putchar(' ');}
  printf("%p:",L);
  if(!L){
    putchar('\n');
    return L ;
  }

        if (L->car == AST_ARROW) {
    cell * lineno      = L->cdr ;
    cell * symbol_list = lineno->cdr ;
    cell * symbol      = symbol_list->cdr ;
    cell * comment     = symbol->cdr ;
    printf("%s (%s) line %ld %%[%s]\n",
      AST_ARROW, symbol->car, (intptr_t)lineno->car, comment->car);
    if(recursive)
      print_node((cell*)symbol_list->car,n+1,1);

  }else if (L->car == AST_BLOCK) {
    cell * lineno           = L->cdr ;
    cell * symbol           = lineno->cdr ;
    printf("%s (%s) line %ld\n",
      AST_BLOCK, symbol->car, (intptr_t)lineno->car);
    if(recursive)
      for(M = symbol->cdr ; M ; M = M->cdr)
        print_node((cell*)M->car,n+1,1);

  }else if (L->car == AST_COLON) {
    cell * lineno      = L->cdr ;
    cell * symbol_list = lineno->cdr ;
    cell * number      = symbol_list->cdr ;
    cell * comment     = number->cdr ;
    printf("%s (%s) line %ld %%[%s]\n",
      AST_COLON, number->car, (intptr_t)lineno->car,comment->car);
    if(recursive)
      print_node((cell*)symbol_list->car,n+1,1) ;
  }else if (L->car == AST_COMMENT) {
    cell * lineno  = L->cdr ;
    cell * comment = lineno->cdr ;
    printf("%s line %ld %%[%s]\n",
      AST_COMMENT, (intptr_t)lineno->car, comment->car) ;

  }else if (L->car == AST_DECL) {
    cell * lineno      = L->cdr ;
    cell * symbol_list = lineno->cdr ;
    cell * comment     = symbol_list->cdr ;
    printf("%s line %ld %%[%s]\n",
      AST_DECL, (intptr_t)lineno->car, comment->car);
    if(recursive)
      print_node((cell*)symbol_list->car,n+1,1) ;

  }else if (L->car == AST_PAIRING) {
    cell * lineno      = L->cdr ;
    cell * symbol0     = lineno->cdr ;
    cell * symbol1     = symbol0->cdr ;
    cell * comment     = symbol1->cdr ;
    printf("%s line %ld (%s,%s) %%[%s]\n",
      AST_PAIRING, (intptr_t)lineno->car, 
      symbol0->car, symbol1->car, comment->car);

  }else if (L->car == AST_PROG) {
    cell * lineno = L->cdr ;
    printf("%s line %ld\n", AST_PROG, (intptr_t)lineno->car);
    if(recursive)
      for(M = lineno->cdr ; M ; M = M->cdr)
        print_node((cell*)M->car,n+1,1);

  }else if (L->car == AST_SYMLIST) {
    cell * lineno  = L->cdr ;
    printf("%s (", AST_SYMLIST);
    for(M = lineno->cdr ; M ; M = M->cdr)printf("%s,",M->car);
    printf("), line %ld\n", (intptr_t)lineno->car);

  }else {
    printf("Unkown\n");
  }

  return L ;
}

void accept(YYSTYPE L){
  printf("\naccepted !\n\n") ;
  print_node(L,0,1) ;
}

cell * print_node_A(cell * L){
  putchar(' '); print_node(L,0,0); return L;
}
cell * print_node_B(cell * L){
  putchar('+'); print_node(L,0,0); return L;
}

#define ast_node(...)        print_node_A(ast(__VA_ARGS__))
#define ast_node_append(...) print_node_B(ast_append(__VA_ARGS__))

int main() { return yyparse(); }

#else

YYSTYPE AST ;
void accept(YYSTYPE L){ AST = L ; }
#define ast_node(...)        ast(__VA_ARGS__)
#define ast_node_append(...) ast_append(__VA_ARGS__)

#endif

#ifdef SYNTACTIC_DEBUG
#else
#endif


%}

%token SYMBOL
%token NUMBER
%token ',' '[' ']' ';' ':'
%token RIGHTARROW LEFTARROW

%%

accept			: prog
				{ accept($1); }
			;

prog			: empty
				{ $$ = ast_node(AST_PROG, line) ; }
			| prog block
				{ $$ = ast_node_append($1,$2) ; }
			;

empty			:
			| empty ';'
				{ free($2) ; }
			;

block			: '[' SYMBOL ']' ';'
				{ $$ = ast_node(AST_BLOCK, line, $2,
				    ast_node(AST_COMMENT, line, $4)
				);}
			| block statement
				{ $$ = ast_node_append($1,$2) ; }
			;

statement		:             ';'
				{ $$ = ast_node(AST_COMMENT, line, $1) ; }
			| symbol_list ';'
				{ $$ = ast_node(AST_DECL, line, $1,$2) ; }
			| symbol_list RIGHTARROW SYMBOL ';'
				{ $$ = ast_node(AST_ARROW, line, $1,$3,$4);}
			| SYMBOL LEFTARROW symbol_list ';'
				{ $$ = ast_node(AST_ARROW, line, $3,$1,$4);}
			| symbol_list ':' NUMBER ';'
				{ $$ = ast_node(AST_COLON, line, $1,$3,$4);}
			| symbol_list ':' SYMBOL ';'
				{ $$ = ast_node(AST_COLONS, line, $1,$3,$4);}
			| symbol_list ':' '~' SYMBOL ';'
				{ $$ = ast_node(AST_COLONN, line, $1,$4,$5);}
			| symbol_list '~' ':' SYMBOL ';'
				{ $$ = ast_node(AST_COLONN, line, $1,$4,$5);}
			| '(' SYMBOL ',' SYMBOL ')' ';'
				{ $$ = ast_node(AST_PAIRING,line,$2,$4,$6);}
			;

symbol_list		: SYMBOL
				{ $$ = ast_node(AST_SYMLIST, line, $1) ; }
			| symbol_list ',' SYMBOL
				{ $$ = ast_node_append($1,$3) ; }
			;
%%

char * debug_format = "%s %ld\n" ;
