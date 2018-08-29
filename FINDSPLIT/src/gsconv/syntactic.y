%{
#include <stdio.h>
#include <stdint.h>

#define YYSTYPE cell*

#include "lisp.h"
#include "ast.h"
#include "syntactic.h"

extern char * TYPE_ELLIPSIS ;

extern int yylineno;
#define line ((intptr_t)yylineno)
// int yyget_column(yyscan_t);

extern int yylex(void) ;
extern int yyparse(void) ;
extern char *yytext;
int yyerror(const char *s)
{
  fprintf (stderr, "line %ld %s at '%s'\n", line, s, yytext);
  return 0;
}

#ifdef SYNTACTIC_DEBUG

#include "symbol_stack.h"

cell * print_node(cell * L, int n, int recursive){
  cell * M ;
  int i ;

  for(i=0;i<n;i++){putchar(' ');}
  printf("%p:",L);
  if(!L){
    printf("NULL\n");
  }else{
    cell * lineno    = L->cdr ;
    cell * arg0      = lineno->cdr ;
    printf("%s line %ld ", L->car, (intptr_t)lineno->car) ;
    if(
      (L->car == AST_TYPE)    ||
      (L->car == AST_NUMBER)  ||
      (L->car == AST_STRING)  ||
      (L->car == AST_IDENTIFIER)
    ){
      printf("[%s]\n", arg0->car) ;
    }else{
      printf("\n") ;
      if(recursive){
        for(;arg0;arg0=arg0->cdr)
          print_node((cell*)arg0->car,n+1,recursive);
      }
    }
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

#define list(...)      print_node_A(LIST(__VA_ARGS__))
#define integrate(...) print_node_B(INTEGRATE(__VA_ARGS__))

#include "operation.h"
#include "type_define.txt"

int main() {
#include "symbol_define.txt"
  return yyparse(); 
}

#else

YYSTYPE AST ;
void accept(YYSTYPE L){ AST = L ; }
#define list(...)      LIST(__VA_ARGS__)
#define integrate(...) INTEGRATE(__VA_ARGS__)

#endif

#ifdef SYNTACTIC_DEBUG
#else
#endif

char * literal(cell * L){
  cell * lineno = L->cdr ;
  cell * opname = lineno->cdr ;
  cell * arg    = opname->cdr ;
  return arg->car ;
}

%}

%nonassoc EMPTY            /* level 24 ;                                 */
%left     SEMICOLON ';'    /* level 23 ;                                 */
%nonassoc STATEMENT        /* level 22                                   */
                           /* level 21                                   */
%nonassoc FUNCTION CASE DEFAULT IF SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN ELSE INLINE
%right    DECL             /* level 20                                   */
%right    INIT             /* level 19                                   */
%left     COMMA ','        /* level 18 ,                                 */
%nonassoc ELLIPSIS         /* level 17 ...                               */
                           /* level 16 = += -= *= /= %= <<= >>= &= ^= |= */
%right    ASSIGN '=' ADDASSIGN SUBASSIGN MULASSIGN DIVASSIGN MODASSIGN SHLASSIGN SHRASSIGN ANDASSIGN XORASSIGN ORASSIGN POWASSIGN
%right    TERNARY '?' ':'  /* level 15 ? :                               */
%left     LOGICALOR        /* level 14 ||                                */
%left     LOGICALAND       /* level 13 &&                                */
%left     BITOR  '|'       /* level 12 |                                 */
%left     BITXOR           /* level 11 ^                                 */
%left     BITAND '&'       /* level 10 &                                 */
%left     EQ NE            /* level  9 ==  !=                            */
%left     CMP LE GE '<' '>'/* level  8 < <= >= >                         */
%left     SHIFT SHL SHR    /* level  7 << >>                             */
%left     ADD '+' '-'      /* level  6 + -                               */
%left     MUL '*' '/' '%'  /* level  5 * / %                             */
%right    CAST             /* level  4 CAST                              */
                           /* level  3 + - ! ~ & * sizeof ++x --x        */
%right    UNARY SIZEOF '!' '~'
%right    POW '^'          /* level  2 '^'                               */
                           /* level  1 x->y x.y x++ x-- x(y) x[y]        */
%left     BIND PP MM ARROW '.'
                           /* level  0                                   */
%nonassoc PRIME '(' ')' '[' ']' '{' '}' IDENTIFIER STRING NUMBER TYPE

%%

accept	: program
		{ accept($1); }
	;

program : /* empty */
		{ $$ = list(AST_PROG,line) ; }
	| program func_def_statement
		{ $$ = integrate($1,$2) ; }
	| program declaration_statement
		{ $$ = integrate($1,$2) ; }
	| program inline_statement
		{ $$ = integrate($1,$2) ; }
	;

func_def_statement
	: identifier '(' arg_list_opt ')' compound_statement %prec STATEMENT
                { $$ = list(AST_FUNC_DEF,line,$1,$3,$5) ; }
	| identifier '[' arg_list_opt ']' compound_statement %prec STATEMENT
                { $$ = list(AST_FUNC_DEF,line,$1,$3,$5) ; }
	| type identifier '(' arg_list_opt ')' ';' %prec STATEMENT
                { $$ = list(AST_FUNC_DECL,line,$1,$2,$4) ; }
	| type identifier '[' arg_list_opt ']' ';' %prec STATEMENT
                { $$ = list(AST_FUNC_DECL,line,$1,$2,$4) ; }
	;

statement_list
	:                                                   %prec EMPTY
		{ $$ = list(AST_STAT_LIST,line) ; }
	| statement_list statement
		{ $$ = integrate($1,$2) ; }
	;

statement
	: declaration_statement
		{ $$ = $1 ; }
	| expression_statement
		{ $$ = $1 ; }
	| compound_statement
		{ $$ = $1 ; }
	| selection_statement
		{ $$ = $1 ; }
	| iteration_statement
		{ $$ = $1 ; }
	| jump_statement
		{ $$ = $1 ; }
	| inline_statement
		{ $$ = $1 ; }
	;

arg_list_opt
	:
                { $$ = list(AST_ARG_LIST,line) ; }
	| arg_list_ell
		{ $$ = $1 ; }
	;

arg_list_ell
	: arg_list
		{ $$ = $1 ; }
	| ELLIPSIS
                { $$ = list(AST_ARG_LIST,line,
		         list(AST_TYPE,line,"$ellipsis"),
                         list(AST_IDENTIFIER,line,"...")) ; }
	| arg_list ',' ELLIPSIS
		{ $$ = integrate($1,
		         list(AST_TYPE,line,"$ellipsis"),
                         list(AST_IDENTIFIER,line,"...")) ; }
	;

arg_list
	: type identifier
                { $$ = list(AST_ARG_LIST,line,$1,$2) ; }
	| arg_list ',' type identifier
		{ $$ = integrate($1,$3,$4) ; }
	;

declaration_statement
	: init_list ';'
                { $$ = $1 ; }
	;

init_list
	: type init
                { $$ = list(AST_INIT_LIST,line,$1,$2) ; }
	| init_list ',' init
		{ $$ = integrate($1,$3) ; }
	;

init	: identifier
                { $$ = list(AST_DECL,line,$1) ; }
	| identifier '=' expression
                { $$ = list(AST_INIT,line,$1,$3) ; }
	;

expression_statement
	: expression_opt ';'                                %prec STATEMENT
                { $$ = $1 ; }
	;

compound_statement
	: '{' statement_list '}'
		{ $$ = $2 ; }
	;

selection_statement
	: IF parentheses_expression statement               %prec STATEMENT
                { $$ = list(AST_IF,line,$2,$3) ; }
	| IF parentheses_expression statement
          ELSE                      statement               %prec ELSE
                { $$ = list(AST_IF_ELSE,line,$2,$3,$5) ; }
        ;

iteration_statement
	: WHILE parentheses_expression statement            %prec STATEMENT
                { $$ = list(AST_WHILE,line,$2,$3) ; }
	| DO statement WHILE parentheses_expression ';'     %prec STATEMENT
                { $$ = list(AST_DO,line,$2,$4) ; }
	| FOR '(' expression_opt ';' expression_opt ';' expression_opt ')'
	  statement                                         %prec STATEMENT
                { $$ = list(AST_FOR,line,$3,$5,$7,$9) ; }
	| FOR '[' expression_opt ';' expression_opt ';' expression_opt ']'
	  statement                                         %prec STATEMENT
                { $$ = list(AST_FOR,line,$3,$5,$7,$9) ; }
        ;

jump_statement
	: CONTINUE ';'                                      %prec STATEMENT
                { $$ = list(AST_CONTINUE,line); }
	| BREAK ';'                                         %prec STATEMENT
                { $$ = list(AST_BREAK,line); }
	| RETURN expression_opt ';'                         %prec STATEMENT
                { $$ = list(AST_RETURN,line,$2) ; }
	;

inline_statement
	: INLINE string ';'                                 %prec STATEMENT
                { $$ = list(AST_INLINE,line,$2) ; }
	;

/* ===================================================================== */

expression_opt
	:                                                   %prec EMPTY
		{ $$ = list(AST_EXP_LIST,line) ; }
	| expression_list                                   %prec COMMA
		{ $$ = $1 ; }
	;

expression_list
	: expression                                        %prec TERNARY
		{ $$ = list(AST_EXP_LIST,line,$1) ; }
	| expression_list ',' expression                    %prec COMMA
		{ $$ = integrate($1,$3) ; }
	;

expression
        : expression '?' expression ':' expression          %prec TERNARY
                { $$ = list(AST_TERNARY,line,$1,$3,$5) ; }
        | expression POWASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_POWASSIGN,line,$1,$3) ; }
        | expression ORASSIGN expression                    %prec ASSIGN
                { $$ = list(AST_ORASSIGN,line,$1,$3) ; }
        | expression XORASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_XORASSIGN,line,$1,$3) ; }
        | expression ANDASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_ANDASSIGN,line,$1,$3) ; }
        | expression SHRASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_SHRASSIGN,line,$1,$3) ; }
        | expression SHLASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_SHLASSIGN,line,$1,$3) ; }
        | expression MODASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_MODASSIGN,line,$1,$3) ; }
        | expression DIVASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_DIVASSIGN,line,$1,$3) ; }
        | expression MULASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_MULASSIGN,line,$1,$3) ; }
        | expression SUBASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_SUBASSIGN,line,$1,$3) ; }
        | expression ADDASSIGN expression                   %prec ASSIGN
                { $$ = list(AST_ADDASSIGN,line,$1,$3) ; }
        | expression '=' expression                         %prec ASSIGN
                { $$ = list(AST_ASSIGN,line,$1,$3) ; }
        | expression LOGICALOR expression                   %prec LOGICALOR
                { $$ = list(AST_LOGICALOR,line,$1,$3) ; }
        | expression LOGICALAND expression                  %prec LOGICALAND
                { $$ = list(AST_LOGICALAND,line,$1,$3) ; }
        | expression '|' expression                         %prec BITOR
                { $$ = list(AST_BITOR,line,$1,$3) ; }
        | expression BITXOR expression                      %prec BITXOR
                { $$ = list(AST_BITXOR,line,$1,$3) ; }
        | expression '&' expression                         %prec BITAND
                { $$ = list(AST_BITAND,line,$1,$3) ; }
        | expression NE expression                          %prec EQ
                { $$ = list(AST_NE,line,$1,$3) ; }
        | expression EQ expression                          %prec EQ
                { $$ = list(AST_EQ,line,$1,$3) ; }
        | expression GE expression                          %prec CMP
                { $$ = list(AST_GE,line,$1,$3) ; }
        | expression '>' expression                         %prec CMP
                { $$ = list(AST_GT,line,$1,$3) ; }
        | expression LE expression                          %prec CMP
                { $$ = list(AST_LE,line,$1,$3) ; }
        | expression '<' expression                         %prec CMP
                { $$ = list(AST_LT,line,$1,$3) ; }
        | expression SHR expression                         %prec SHIFT
                { $$ = list(AST_SHR,line,$1,$3) ; }
        | expression SHL expression                         %prec SHIFT
                { $$ = list(AST_SHL,line,$1,$3) ; }
        | expression '-' expression                         %prec ADD
                { $$ = list(AST_SUB,line,$1,$3) ; }
        | expression '+' expression                         %prec ADD
                { $$ = list(AST_ADD,line,$1,$3) ; }
        | expression '%' expression                         %prec MUL
                { $$ = list(AST_MOD,line,$1,$3) ; }
        | expression '/' expression                         %prec MUL
                { $$ = list(AST_DIV,line,$1,$3) ; }
        | expression '*' expression                         %prec MUL
                { $$ = list(AST_MUL,line,$1,$3) ; }
        | '!' expression                                    %prec UNARY
                { $$ = list(AST_NOT,line,$2) ; }
        | '~' expression                                    %prec UNARY
                { $$ = list(AST_NEG,line,$2) ; }
        | '-' expression                                    %prec UNARY
                { $$ = list(AST_UMINUS,line,$2) ; }
        | '+' expression                                    %prec UNARY
                { $$ = list(AST_UPLUS,line,$2) ; }
        | '&' expression                                    %prec UNARY
                { $$ = list(AST_ADDRESSOF,line,$2) ; }
        | '*' expression                                    %prec UNARY
                { $$ = list(AST_DEREFERENCEOF,line,$2) ; }
        | PP expression                                     %prec UNARY
                { $$ = list(AST_PREPP,line,$2) ; }
        | MM expression                                     %prec UNARY
                { $$ = list(AST_PREMM,line,$2) ; }
        | expression '^' expression                         %prec POW
                { $$ = list(AST_POW,line,$1,$3) ; }
	| binding_expression
		{ $$ = $1 ; }
	;

binding_expression
	: expression ARROW prime_expression                 %prec BIND
                { $$ = list(AST_ARROW,line,$1,$3) ; }
        | expression '.' prime_expression                   %prec BIND
                { $$ = list(AST_DOT,line,$1,$3) ; }
        | identifier parentheses_expression                 %prec BIND
                { $$ = list(AST_FUNC_CALL,line,$1,$2) ; }
        | expression PP                                     %prec BIND
                { $$ = list(AST_POSTPP,line,$1) ; }
        | expression MM                                     %prec BIND
                { $$ = list(AST_POSTMM,line,$1) ; }
	| prime_expression
                { $$ = $1 ; }
	;

prime_expression
	: string                                            %prec PRIME
                { $$ = $1 ; }
	| number                                            %prec PRIME
                { $$ = $1 ; }
	| identifier                                        %prec PRIME
                { $$ = $1 ; }
	| parentheses_expression                            %prec PRIME
                { $$ = $1 ; }
        ;

type  
	: TYPE                                              %prec PRIME
                { $$ = list(AST_TYPE,line,$1) ; }
	;

string
	: STRING                                            %prec PRIME
                { $$ = list(AST_STRING,line,$1) ; }
	;

number
	: NUMBER                                            %prec PRIME
                { $$ = list(AST_NUMBER,line,$1) ; }
	;

identifier
	: IDENTIFIER                                        %prec PRIME
		{ $$ = list(AST_IDENTIFIER,line,$1); }
	;

parentheses_expression
	: '[' expression_opt ']'                            %prec PRIME
                { $$ = list(AST_SQUAREBRACKETS,line,$2) ; }
        | '(' expression_opt ')'                            %prec PRIME
                { $$ = $2 ; }
/*		{ $$ = list(AST_UNARY,line,OP_PARENTHESES,$2) ; }  */
/*	| '[' compound_statement ']'                        %prec PRIME
                { $$ = list(AST_UNARY,line,OP_SQUAREBRACKETS,$2) ; }
        | '(' compound_statement ')'                        %prec PRIME
                { $$ = list(AST_UNARY,line,OP_PARENTHESES,$2) ; } */
	;

%%
