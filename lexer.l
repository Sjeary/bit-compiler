%option noyywrap
%option yylineno

%{
#include<stdio.h>
#include<string.h>
#include "ast.h"


%}

%%
"int" {return INT;}
"void" {return VOID;}
"return" { return RETURN; }
"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
"break" {return BREAK;}
"continue" {return CONTINUE;}
"(" { return LP; }
")" { return RP; }
"{" { return LC; }
"}" { return RC; }
"," { return COMMA; }
";" { return SEMICOLON; }
"+" { return PLUS; }
"-" { return MINUS; }
"!" { return NOT; }
"=" { return ASSIGN; }
"*" { return MUL; }
"/" { return DIV; }
"%" { return MOD; }
"&&" { return AND; }
"||" { return OR; }
"==" { return EQ; }
"!=" { return NE; }
"<" { return LT; }
"<=" { return LE; }
">" { return GT; }
">=" { return GE; }
"~"  {return BITWISE_NOT;}
"&"  { return BITWISE_AND; }
"|"  { return BITWISE_OR; }
"^"  { return BITWISE_XOR; }
[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str_val = (char *)malloc(strlen(yytext) + 1);
    strcpy(yylval.str_val, yytext);
    yylval.str_val[strlen(yytext)] = '\0';
    return ID;
}
[1-9][0-9]*|0 { 
    yylval.int_val = atoi(yytext);
    return INT_LIT;
    }
. {}
%%