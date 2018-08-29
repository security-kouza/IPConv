%option yylineno 

%{
#define YYSTYPE char*
extern YYSTYPE yylval;
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "syntactic.h"
#include "octet_string.h"
#include "symbol_stack.h"

// extern char * strdup(const char * s) ;

extern int yylineno ;
#define line ((intptr_t)yylineno)

// static int eof = 0 ;

static octet_string * literal = NULL ;

#ifdef LEXICAL_DEBUG
static int lexical_debug = 1 ;
#else
static int lexical_debug = 0 ;
#endif /* LEXICAL_DEBUG */

%}

alpha [A-Za-z_$]
alnum [A-Za-z0-9_$]
digit [0-9]
space [ \t\r\n]

%start STR COM1 COM2

%%

<INITIAL>\"		{
			  BEGIN STR;
			  if(!literal)literal = new_octet_string(1024);
			}
<STR>\\\"		{ append_octet_string(literal,'"') ; }
<STR>\"			{
			  BEGIN INITIAL;
                          append_octet_string(literal,0) ;
			  yylval = strdup(literal->s) ;
			  clear_octet_string(literal) ;
			  return(STRING) ;
			}
<STR>\n			{ append_octet_string(literal,'\n') ; }
<STR>.			{ append_octet_string(literal,yytext[0]) ; }
<INITIAL>"/*"           BEGIN COM1;
<COM1>"*/"              BEGIN INITIAL;
<COM1>\n
<COM1>.
<INITIAL>"//"           BEGIN COM2;
<COM2>\n                BEGIN INITIAL;
<COM2>.
<INITIAL>"->"		return ARROW ;
<INITIAL>"--"		return MM ;
<INITIAL>"++"		return PP ;
<INITIAL>"function"	return FUNCTION ;
<INITIAL>"case"		return CASE ;
<INITIAL>"default"	return DEFAULT ;
<INITIAL>"if"		return IF ;
<INITIAL>"else"		return ELSE ;
<INITIAL>"switch"	return SWITCH ;
<INITIAL>"while"	return WHILE ;
<INITIAL>"do"		return DO ;
<INITIAL>"for"		return FOR ;
<INITIAL>"goto"		return GOTO ;
<INITIAL>"continue"	return CONTINUE ;
<INITIAL>"break"	return BREAK ;
<INITIAL>"return"	return RETURN ;
<INITIAL>"sizeof"	return SIZEOF ;
<INITIAL>"inline"	return INLINE ;
<INITIAL>"<<"		return SHL ;
<INITIAL>">>"		return SHR ;
<INITIAL>"<="		return LE ;
<INITIAL>">="		return GE ;
<INITIAL>"=="		return EQ ;
<INITIAL>"!="		return NE ;
<INITIAL>"&&"		return LOGICALAND ;
<INITIAL>"||"		return LOGICALOR ;
<INITIAL>"+="		return ADDASSIGN ;
<INITIAL>"-="		return SUBASSIGN ;
<INITIAL>"*="		return MULASSIGN ;
<INITIAL>"/="		return DIVASSIGN ;
<INITIAL>"%="		return MODASSIGN ;
<INITIAL>"<<="		return SHLASSIGN ;
<INITIAL>">>="		return SHRASSIGN ;
<INITIAL>"&="		return ANDASSIGN ;
<INITIAL>"|="		return ORASSIGN ;
<INITIAL>"^="		return POWASSIGN ;
<INITIAL>"(+)="		return XORASSIGN ;
<INITIAL>"(+)"		return BITXOR ;
<INITIAL>"..."		return ELLIPSIS ;
<INITIAL>{digit}+	yylval=strdup(yytext); return NUMBER ;
<INITIAL>{alpha}{alnum}* {
			  yylval=strdup(yytext);
			  return symbol_to_symbol_type(yylval) ;
			}
<INITIAL>{space}+
<INITIAL>.		return yytext[0] ;

%%

int yywrap()
{
  return 1;
}

#ifdef LEXICAL_DEBUG
#include "operation.h"
#include "type_define.txt"

char * yylval ;

#define CASE1(X) \
case X: printf("%02ld " # X ":\n",line); break;

#define CASE2(X) \
case X: printf("%02ld " # X ":'%s'\n",line,yylval); break;


int main()
{
  int t ;

#include "symbol_define.txt"

  while((t = yylex()) != 0){
    switch(t){

      CASE1(ARROW) ;
      CASE1(MM) ;
      CASE1(PP) ;
      CASE1(FUNCTION) ;
      CASE1(CASE) ;
      CASE1(DEFAULT) ;
      CASE1(IF) ;
      CASE1(ELSE) ;
      CASE1(SWITCH) ;
      CASE1(WHILE) ;
      CASE1(DO) ;
      CASE1(FOR) ;
      CASE1(GOTO) ;
      CASE1(CONTINUE) ;
      CASE1(BREAK) ;
      CASE1(RETURN) ;
      CASE1(SIZEOF) ;
      CASE1(INLINE) ;
      CASE1(SHL) ;
      CASE1(SHR) ;
      CASE1(LE) ;
      CASE1(GE) ;
      CASE1(EQ) ;
      CASE1(NE) ;
      CASE1(LOGICALAND) ;
      CASE1(LOGICALOR) ;
      CASE1(ADDASSIGN) ;
      CASE1(SUBASSIGN) ;
      CASE1(MULASSIGN) ;
      CASE1(DIVASSIGN) ;
      CASE1(MODASSIGN) ;
      CASE1(SHLASSIGN) ;
      CASE1(SHRASSIGN) ;
      CASE1(ANDASSIGN) ;
      CASE1(ORASSIGN) ;
      CASE1(POWASSIGN) ;
      CASE1(XORASSIGN) ;
      CASE1(BITXOR) ;

      CASE2(STRING) ;
      CASE2(NUMBER) ;
      CASE2(IDENTIFIER) ;
      CASE2(TYPE) ;

      default        : printf("%02ld token :'%c'\n",line,t     ); break;
    }
  }
}
#endif /* LEXICAL_DEBUG */
