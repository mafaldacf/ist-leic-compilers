%option c++ prefix="fir_scanner_" outfile="fir_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "fir_parser.tab.h"

static std::vector<int> deferred;
static std::ostringstream strlit;
static std::stringstream str;

// don't change this
#define yyerror LexerError
%}

%x X_COMMENT
%x X_STRING X_TILDE X_STRING_IGNORE

%%

 /* ====================================================================== */
 /* ====[                  3.8.1 - Inteiros                          ]==== */
 /* ====[                  3.8.2 - Reais em vírgula flutuante        ]==== */
 /* ====[                  3.8.3 - Cadeias de caracteres             ]==== */
 /* ====[                  3.8.4 - Ponteiros                         ]==== */
 /* ====================================================================== */
 
"int"                 return tTYPE_INT;
"float"               return tTYPE_FLOAT;
"string"              return tTYPE_STRING;
"void"                return tTYPE_VOID;
"null"                return tNULL;

 /* ====================================================================== */
 /* ====[                  5 - Funções                               ]==== */
 /* ====================================================================== */

"->"                 return tDEFAULT_RETURN;
">>"                 return tEPILOGUE;

 /* ====================================================================== */
 /* ====[                  6.2 - Instrução condicional               ]==== */
 /* ====[                  6.3 - Instrução de iteração               ]==== */
 /* ====[                  6.4 - Instrução de terminação             ]==== */
 /* ====[                  6.5 - Instrução de reinício               ]==== */
 /* ====[                  6.6 - Instrução de retorno                ]==== */
 /* ====[                  6.8 - Instrução de impressão              ]==== */
 /* ====================================================================== */

"if"                   return tIF; 
"then"                 return tTHEN;
"else"                 return tELSE;
"while"                return tWHILE;
"do"                   return tDO;
"finally"              return tFINALLY;
"leave"                return tLEAVE;
"restart"              return tRESTART;
"return"               return tRETURN;
"write"                return tWRITE;
"writeln"              return tWRITELN;

 /* ====================================================================== */
 /* ====[                  7 - Operadores                            ]==== */
 /* ====[                  7.1.2 - Expressão de leitura              ]==== */
 /* ====[                  7.2.5 - Expressão de dimensão             ]==== */
 /* ====================================================================== */

">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"&&"                   return tAND;
"||"                   return tOR;
"sizeof"               return tSIZEOF;

 /* ====================================================================== */
 /* ====[                  3.2 - Comentários                         ]==== */
 /* ====================================================================== */

"!!".*$             ; /* comment corresponds to the whole line */

"(*"                yy_push_state(X_COMMENT);
<X_COMMENT>.|\n     ; /* ignores every character inside comments */
<X_COMMENT>"*)"     yy_pop_state();

 /* ====================================================================== */
 /* ====[                  3.7 - Identificadores                     ]==== */
 /* ====================================================================== */

[A-Za-z][A-Za-z0-9_]*  { yylval.s = new std::string(yytext); return tIDENTIFIER; }

 /* ====================================================================== */
 /* ====[                  3.8.1 - Inteiros                          ]==== */
 /* ====================================================================== */

0|[1-9][0-9]*      { try { yylval.i = strtol(yytext, nullptr, 10); return tINTEGER; }
                            catch (std::out_of_range& e) { yyerror("Integer out of range."); }
                            catch (std::invalid_argument& e) { yyerror("Invalid number, conversion could not be performed."); }
                    }



0[0-7]+             { try{ yylval.i = strtol(yytext, nullptr, 8); return tINTEGER; }
                            catch (std::out_of_range& e) { yyerror("Base 8 integer out of range."); }
                            catch (std::invalid_argument& e) { yyerror("Invalid number, conversion could not be performed."); }
                    }

0[0-7]*[8|9]+[0-7]* yyerror("Invalid base 8 literal number.");

 /* ====================================================================== */
 /* ====[                  3.8.2 - Reais em vírgula flutuante (float)]==== */
 /* ====================================================================== */

([0-9]*"."[0-9]+|[0-9]+"."[0-9]*)([eE][-+]?[0-9]+)?     { try { yylval.d = strtod(yytext, NULL); return tFLOAT; }
                                                                catch (std::out_of_range& e) { yyerror("Float literal out of range."); }
                                                                catch (std::invalid_argument& e) { yyerror("Invalid number, conversion could not be performed."); }
                                                        }                                                       

([0-9]+)([eE][+-]?[0-9]+)                               { try { yylval.d = strtod(yytext, NULL); return tFLOAT; }
                                                                catch (std::out_of_range& e) { yyerror("Float literal out of range."); }
                                                                catch (std::invalid_argument& e) { yyerror("Invalid number: conversion could not be performed."); }
                                                        }

 /* ====================================================================== */
 /* ====[                  3.8.3 - Cadeias de caracteres             ]==== */
 /* ====================================================================== */

\'                          yy_push_state(X_STRING);
<X_STRING>~                 yy_push_state(X_TILDE);
<X_STRING>\'                {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                return tSTRING;
                            }
<X_STRING>\n                yyerror("newline in string");
<X_STRING>.                 strlit << *yytext;

<X_TILDE>n                  strlit << '\n'; yy_pop_state();
<X_TILDE>r                  strlit << '\r'; yy_pop_state();
<X_TILDE>t                  strlit << '\t'; yy_pop_state();
<X_TILDE>\'                 strlit << "'"; yy_pop_state();
<X_TILDE>~                  strlit << '~'; yy_pop_state();
<X_TILDE>[[:xdigit:]]{1,2}  { strlit << (char)(unsigned char)std::stoul(yytext, NULL, 16); yy_pop_state(); }
<X_TILDE>0[^:xdigit:]      { strlit << "\0"; yy_push_state(X_STRING_IGNORE);}
<X_TILDE>.                  { strlit << *yytext; yy_pop_state(); }

<X_STRING_IGNORE>\'         {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                yy_pop_state();
                                yy_pop_state();
                                return tSTRING;
                            }
<X_STRING_IGNORE>\n         yyerror("newline in string");
<X_STRING_IGNORE>.          ; /* ignore every character */

 /* ====================================================================== */
 /* ====[                        Other Characters                    ]==== */
 /* ====================================================================== */

[-(){}\[\]<>=+*/%;.,~@?]  return *yytext;

[ \t\n\r]+                ; /* ignore whitespace */

.                         yyerror("Unknown character");

%%
