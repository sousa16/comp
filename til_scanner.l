%option c++ prefix="til_scanner_" outfile="til_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "til_parser.tab.h"

// don't change this
#define yyerror LexerError

#define CHECKED_STOI(base) { \
  try { \
    yylval.i = std::stoi(yytext, nullptr, base); \
    return tINTEGER; \
  } catch (const std::out_of_range&) { \
    yyerror("integer overflow"); \
  } \
}

#define CHECKED_STOD { \
  try { \
    yylval.d = std::stod(yytext); \
    return tDOUBLE; \
  } catch (const std::out_of_range&) { \
    yyerror("double overflow"); \
  } \
}

%}

EXPR [Ee][+-]?[0-9]+

%x X_COMMENT X_STRING X_STRING_IGN

%%
  yydebug=1; set_debug(1);

  /* ====================================================================== */
  /* 	WHITESPACE                                                       */
  /* ====================================================================== */

[ \t\n\r]+ ;

  /* ====================================================================== */
  /* 	COMMENTS                                                         */
  /* ====================================================================== */

";".*                 ; /* ignore comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>.|\n        ; /* ignore */

  /* ====================================================================== */
  /* 	OPERATORS                                                         */
  /* ====================================================================== */

">="                   return tGE;
"<="                   return tLE;
"=="                   return tEQ;
"!="                   return tNE;
"&&"             	     return tAND;
"||"                   return tOR;

  /* ====================================================================== */
  /* 	TYPES                                                            */
  /* ====================================================================== */

"int"  				   return tTYPE_INT;
"double"			   return tTYPE_DOUBLE;
"string"			   return tTYPE_STRING;
"void"				   return tTYPE_VOID;

  /* ====================================================================== */
  /* 	KEYWORDS                                                         */
  /* ====================================================================== */

"external"			       return tEXTERNAL;
"forward"			         return tFORWARD;
"public"			         return tPUBLIC;
"var"			 	           return tVAR;

"block"                return tBLOCK;
"if"                   return tIF;
"stop"                 return tSTOP;
"next"                 return tNEXT;
"return"               return tRETURN;
"print"                return tPRINT;
"println"              return tPRINTLN;

"read"                 return tREAD;
"null"                 return tNULL;
"set"                  return tSET;
"index"                return tINDEX;
"objects"              return tOBJECTS;
"sizeof"			         return tSIZEOF;
"function"			       return tFUNCTION;

"program"			         return tPROGRAM;


"loop"                 return tWHILE;
"else"                 return tELSE;
"begin"                return tBEGIN;
"end"                  return tEND;


  /* ====================================================================== */
  /* 	IDENTIFIERS                                                      */
  /* ====================================================================== */

[A-Za-z][A-Za-z0-9]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

  /* ====================================================================== */
  /* 	STRINGS                                                        */
  /* ====================================================================== */

\"                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\"           yy_pop_state(); return tSTRING;
<X_STRING>\\\"         *yylval.s += yytext + 1;
<X_STRING>\\\\         *yylval.s += yytext + 1;
<X_STRING>\\t          *yylval.s += '\t';
<X_STRING>\\n          *yylval.s += '\n';
<X_STRING>\\r          *yylval.s += '\r';
<X_STRING>\\0          yy_push_state(X_STRING_IGN);
<X_STRING>\\[0-7]{1,3} {
                          int i = std::stoi(yytext + 1, nullptr, 8);
                          if (i > 255) yyerror("octal escape sequence out of range");
                          *yylval.s += (char) i;
                       }
<X_STRING>\\.          *yylval.s += yytext + 1;
<X_STRING>\n           yyerror("newline in string");
<X_STRING>\0           yyerror("null byte in string");
<X_STRING>.            *yylval.s += yytext;

<X_STRING_IGN>\"       yy_pop_state(); yy_pop_state(); return tSTRING;
<X_STRING_IGN>\\\"     ;
<X_STRING_IGN>\\\\     ;
<X_STRING_IGN>\n       yyerror("newline in string");
<X_STRING_IGN>\0       yyerror("null byte in string");
<X_STRING_IGN>.        ;

  /* ====================================================================== */
  /* 	INTEGERS                                                       */
  /* ====================================================================== */

0x0                     yyerror("0x0 is not a valid integer literal");
0|[1-9][0-9]*          CHECKED_STOI(10);
0x[0-9a-fA-F]+		   CHECKED_STOI(16);
0x[0-9a-zA-Z]+		   yyerror("invalid base 16 integer literal");

  /* ====================================================================== */
  /* 	FLOATING POINT NUMBERS                                         */
  /* ====================================================================== */

[0-9]*\.[0-9]+{EXPR}?   CHECKED_STOD;
[0-9]+\.[0-9]*{EXPR}?   CHECKED_STOD;
[0-9]+{EXPR}            CHECKED_STOD;

  /* ====================================================================== */



[-()<>=+*/%;,@?~"!]        return *yytext;


.                      yyerror("Unknown character");

%%
