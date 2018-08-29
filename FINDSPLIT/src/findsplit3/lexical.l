%option yylineno 

%{
#define YYSTYPE char*
extern YYSTYPE yylval;
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef __CYGWIN__
extern char * strdup(const char *str) ;
int fileno(FILE *stream) ;
#endif

#include "syntactic.h"

extern int yylineno ;
#define line ((intptr_t)yylineno)

static int eof_ = 0 ;

#ifdef LEXICAL_DEBUG
static int lexical_debug = 1 ;
#else
static int lexical_debug = 0 ;
#endif /* LEXICAL_DEBUG */

%}

alpha [A-Za-z_$]
alnum [A-Za-z0-9_$]
digit [0-9]
space [ \t\r]

hex      [0-9A-Fa-f]
ull      ([Uu][Ll]?[Ll]?|[Ll][Ll]?[Uu]?)

exponent [eE][-+]?{digit}+
mantissa ({digit}*"."{digit}+|{digit}+".")
fl       [FLfl]

%%
\;{space}*\%.*		|
\%.*			{
			  char * s = yytext ;
			  while(*s!='%')s++;
			  s++;
			  yylval=strdup(s);
			  return ';' ;
			}
<<EOF>>			{
			  if(!eof_){
			    yylval=strdup(lexical_debug?"<<EOF>>":"");
			    eof_=1;
			    return ';' ;
			  }else{
			    yyterminate() ;
			    return ';' ;
			  }
			}
\n			{
			  yylval=strdup(lexical_debug?"<<EOL>>":"");
			  return ';' ;
			}
\;			{
			  yylval=strdup(lexical_debug?"<<SEMICOLON>>":"");
			  return ';' ;
			}
{alpha}{alnum}*		yylval=strdup(yytext); return SYMBOL ;

{mantissa}{exponent}?{fl}?      yylval=strdup(yytext); return NUMBER ;
{digit}+{exponent}{fl}?         yylval=strdup(yytext); return NUMBER ;
0[xX]{hex}+{ull}?               yylval=strdup(yytext); return NUMBER ;
{digit}+{ull}?                  yylval=strdup(yytext); return NUMBER ;

"->"			return RIGHTARROW ;
"<-"                    return LEFTARROW ;
":="			return ':' ;
"="			return ':' ;
"!"			return '~' ;
{space}+
.			return yytext[0] ;

%%

/*
{digit}+		yylval=strdup(yytext); return NUMBER ;
*/

int yywrap()
{
  return 1;
}

#ifdef LEXICAL_DEBUG
char * yylval ;
int main()
{
  int t ;
  while((t = yylex()) != 0){
    switch(t){
      case ';'       : printf("%02ld comma :'%s'\n",line,yylval); break;
      case SYMBOL    : printf("%02ld symbol:'%s'\n",line,yylval); break;
      case NUMBER    : printf("%02ld number:'%s'\n",line,yylval); break;
      case RIGHTARROW: printf("%02ld right :'->'\n",line       ); break;
      case LEFTARROW : printf("%02ld left  :'<-'\n",line       ); break;
      default        : printf("%02ld token :'%c'\n",line,t     ); break;
    }
  }
}
#endif /* LEXICAL_DEBUG */

