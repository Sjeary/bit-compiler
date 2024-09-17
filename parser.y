%define parse.error verbose
%locations
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ast.h"

ASTNode *root;

extern FILE *yyin;
extern int line_cnt;
extern int yylineno;
extern char *yytext;
extern int yylex();
extern int yyparse();
void yyerror(const char* fmt, ...);
int syntax_error = 0;
char filename[100];
%}

%union {
    int int_val;
    float float_val;
    char *str_val;
    struct ASTNode *node_val;
}

%type <node_val> CompUnit VarDecl FuncDef VarDef InitVal FuncFParam FuncFParams Block Decl Cond 
%type <node_val> Root BlockItem Stmt LVal PrimaryExp UnaryExp FuncRParams MulExp Exp RelExp EqExp LAndExp 
%type <node_val> AddExp LOrExp BitAndExp BitOrExp BitXorExp
//ForList

%token <str_val> ID
%token <int_val> INT_LIT

%token <int_val> INT FLOAT VOID CONST RETURN IF ELSE WHILE BREAK CONTINUE LP RP LB RB LC RC COMMA SEMICOLON
%token <int_val> MINUS NOT ASSIGN PLUS MUL DIV MOD AND OR EQ NE LT LE GT GE LEX_ERR BITWISE_NOT BITWISE_AND BITWISE_OR BITWISE_XOR

%nonassoc NOELSE

%%
Root
    : CompUnit {root = new_node(Root,NULL,NULL,$1,0,0,NULL,NonType);}
    ;
CompUnit
    : Decl CompUnit { $$ = new_node(CompUnit, $2, NULL, $1, 0, 0, NULL, NonType); }
    | FuncDef CompUnit { $$ = new_node(CompUnit, $2, NULL, $1, 0, 0, NULL, NonType);
        
     }
    | Decl { $$ = new_node(CompUnit, NULL, NULL, $1, 0, 0, NULL, NonType); }
    | FuncDef {
         $$ = new_node(CompUnit, NULL, NULL, $1, 0, 0, NULL, NonType);
         
          }
    ;

Decl
    : VarDecl { $$ = new_node(Decl, NULL, NULL, $1, 0, 0, NULL, NonType); }
    ;

VarDecl
    : INT VarDef SEMICOLON { $$ = new_node(VarDecl, NULL, NULL, $2, 0, 0, NULL, Int); }
    ;

VarDef 
    : ID ASSIGN InitVal {$$ = new_node(VarDef,NULL,NULL,$3,0,0,$1,NonType);}
    | ID {$$ = new_node(VarDef,NULL,NULL,NULL,0,0,$1,NonType);}
    | ID ASSIGN InitVal COMMA VarDef {$$ = new_node(VarDef,$5,NULL,$3,0,0,$1,NonType);}
    | ID COMMA VarDef {$$ = new_node(VarDef,$3,NULL,NULL,0,0,$1,NonType);}
    ;

InitVal
    : Exp { $$ = new_node(InitVal, NULL, NULL, $1, Exp, 0, NULL, NonType); }
    ;

FuncDef
    : VOID ID LP FuncFParams RP Block { $$ = new_node(FuncDef, $6, NULL, $4, 0, 0, $2, Void); } 
    | VOID ID LP RP Block { $$ = new_node(FuncDef, $5, NULL, NULL, 0, 0, $2, Void); }
    | INT ID LP FuncFParams RP Block { $$ = new_node(FuncDef, $6, NULL, $4, 0, 0, $2, Int); }
    | INT ID LP RP Block { $$ = new_node(FuncDef, $5, NULL, NULL, 0, 0, $2, Int); }
    ;

FuncFParams
    : FuncFParam COMMA FuncFParams {$$ = new_node(FuncFParams,$1,NULL,$3,0,0,NULL,NonType);}
    | FuncFParam { $$ = new_node(FuncFParams,$1,NULL,NULL,0,0,NULL,NonType);}
    ;

FuncFParam
    : INT ID {$$ = new_node(FuncFParam,NULL,NULL,NULL,0,0,$2,Int);}
    ;

Block
    : LC BlockItem RC {$$ = new_node(Block,NULL,NULL,$2,0,0,NULL,NonType);}
    ;

BlockItem
    : {$$ = NULL;}
    | Decl BlockItem {$$ = new_node(BlockItem,$2,NULL,$1,0,0,NULL,NonType);}
    | Stmt BlockItem {$$ = new_node(BlockItem,$2,NULL,$1,0,0,NULL,NonType);}
    ;
Cond
    : Exp {$$ = new_node(Cond, NULL, NULL, $1, Exp, 0, NULL, NonType); }
    ;
Stmt
    : LVal ASSIGN Exp SEMICOLON 
    {
        $$ =new_node(Stmt,$1,NULL,$3,AssignStmt,0,NULL,NonType);
    }
    | Exp SEMICOLON
    {
        $$ = new_node(Stmt,NULL,NULL,$1,ExpStmt,0,NULL,NonType);
        
    }
    | Block
    {
        $$ = new_node(Stmt,NULL,NULL,$1,Block,0,NULL,NonType);
    }
    | RETURN Exp SEMICOLON
    {
        $$ = new_node(Stmt,NULL,NULL,$2,ReturnStmt,0,NULL,NonType);
    }
    | RETURN SEMICOLON
    {
        $$ = new_node(Stmt,NULL,NULL,NULL,BlankReturnStmt,0,NULL,NonType);
    }
    | IF LP Cond RP Stmt ELSE Stmt
    {
        $$ = new_node(Stmt,$3,$5,$7,IfElseStmt,0,NULL,NonType);
    }
    | IF LP Cond RP Stmt %prec NOELSE
    {
        $$ = new_node(Stmt,$3,NULL,$5,IfStmt,0,NULL,NonType);
    }
    | WHILE LP Cond RP Stmt
    {
        $$ = new_node(Stmt,$3,NULL,$5,WhileStmt,0,NULL,NonType);
    }
    | BREAK SEMICOLON
    {
        $$ = new_node(Stmt,NULL,NULL,NULL,BreakStmt,0,NULL,NonType);
    }
    | CONTINUE SEMICOLON
    {
        $$ = new_node(Stmt,NULL,NULL,NULL,ContinueStmt,0,NULL,NonType);
    }
    ;

Exp
    : LOrExp {$$ = new_node(Exp,NULL,NULL,$1,0,0,NULL,NonType); }
    ;

LVal
    : ID {$$ = new_node(LVal,NULL,NULL,NULL,0,0,$1,NonType);}
    ;
//从上往下优先级依次降低
PrimaryExp
    : LP Exp RP {$$ = new_node(PrimaryExp,NULL,NULL,$2,Exp,0,NULL,NonType);}
    | LVal {$$ = new_node(PrimaryExp,NULL,NULL,$1,LVal,0,NULL,NonType);}
    | INT_LIT {$$ = new_node(PrimaryExp,NULL,NULL,NULL,$1,0,NULL,Int);}
    ;

UnaryExp
    : PrimaryExp {$$ = new_node(UnaryExp,NULL,NULL,$1,PrimaryExp,0,NULL,NonType);}
    | ID LP FuncRParams RP {$$ = new_node(UnaryExp,NULL,NULL,$3,FuncRParams,0,$1,NonType);}
    | PLUS UnaryExp {$$ = new_node(UnaryExp,NULL,NULL,$2,NT_PLUS,0,NULL,NonType);}
    | MINUS UnaryExp {$$ = new_node(UnaryExp,NULL,NULL,$2,NT_MINUS,0,NULL,NonType);}
    | BITWISE_NOT UnaryExp {$$ = new_node(UnaryExp,NULL,NULL,$2,NT_BITWISE_NOT,0,NULL,NonType);}
    | NOT UnaryExp {$$ = new_node(UnaryExp,NULL,NULL,$2,NT_NOT,0,NULL,NonType);}
    ;



FuncRParams
    :  {$$ = NULL;}
    | Exp { $$ = new_node(FuncRParams, NULL, NULL, $1, 0, 0, NULL, NonType); }
    | Exp COMMA FuncRParams{ $$ = new_node(FuncRParams, $3, NULL, $1, 0, 0, NULL, NonType); };
    ;

MulExp
    : UnaryExp { $$ = new_node(MulExp, NULL, NULL, $1, UnaryExp, 0, NULL, NonType); }
    | MulExp MUL UnaryExp { $$ = new_node(MulExp, $3, NULL, $1, NT_MUL, 0, NULL, NonType); }
    | MulExp DIV UnaryExp { $$ = new_node(MulExp, $3, NULL, $1, NT_DIV, 0, NULL, NonType); }
    | MulExp MOD UnaryExp { $$ = new_node(MulExp, $3, NULL, $1, NT_MOD, 0, NULL, NonType); }
    ;

AddExp
    : MulExp { $$ = new_node(AddExp, NULL, NULL, $1, MulExp, 0, NULL, NonType); }
    | AddExp PLUS MulExp  { $$ = new_node(AddExp, $3, NULL, $1, NT_PLUS, 0, NULL, NonType); }
    | AddExp MINUS MulExp { $$ = new_node(AddExp, $3, NULL, $1, NT_MINUS, 0, NULL, NonType); }
    ;

RelExp
    : AddExp { $$ = new_node(RelExp, NULL, NULL, $1, AddExp, 0, NULL, NonType); }
    | RelExp LT AddExp { $$ = new_node(RelExp, $3, NULL, $1, NT_LT, 0, NULL, NonType); }
    | RelExp GT AddExp { $$ = new_node(RelExp, $3, NULL, $1, NT_GT, 0, NULL, NonType); }
    | RelExp LE AddExp { $$ = new_node(RelExp, $3, NULL, $1, NT_LE, 0, NULL, NonType); }
    | RelExp GE AddExp { $$ = new_node(RelExp, $3, NULL, $1, NT_GE, 0, NULL, NonType); }
    ;

EqExp
    : RelExp { $$ = new_node(EqExp, NULL, NULL, $1, RelExp, 0, NULL, NonType); }
    | EqExp EQ RelExp { $$ = new_node(EqExp, $3, NULL, $1, NT_EQ, 0, NULL, NonType); }
    | EqExp NE RelExp { $$ = new_node(EqExp, $3, NULL, $1, NT_NE, 0, NULL, NonType); }
    ;

BitAndExp
    : EqExp { $$ = new_node(BitAndExp, NULL, NULL, $1, EqExp, 0, NULL, NonType); }
    | BitAndExp BITWISE_AND EqExp{ $$ = new_node(BitAndExp, $3, NULL, $1, NT_BITWISE_AND, 0, NULL, NonType); }
    ;

BitXorExp
    : BitAndExp{ $$ = new_node(BitXorExp, NULL, NULL, $1, BitAndExp, 0, NULL, NonType); }
    | BitXorExp BITWISE_XOR BitAndExp{ $$ = new_node(BitXorExp, $3, NULL, $1, NT_BITWISE_XOR, 0, NULL, NonType); }
    ;

BitOrExp
    : BitXorExp{ $$ = new_node(BitOrExp, NULL, NULL, $1, BitXorExp, 0, NULL, NonType); }
    | BitOrExp BITWISE_OR BitXorExp{ $$ = new_node(BitOrExp, $3, NULL, $1, NT_BITWISE_OR, 0, NULL, NonType); }
    ;


LAndExp
    : BitOrExp{ $$ = new_node(LAndExp, NULL, NULL, $1, BitOrExp, 0, NULL, NonType); }
    | LAndExp AND EqExp{ $$ = new_node(LAndExp, $3, NULL, $1, NT_AND, 0, NULL, NonType); }
    ;

LOrExp
    : LAndExp{ $$ = new_node(LOrExp, NULL, NULL, $1, LAndExp, 0, NULL, NonType); }
    | LOrExp OR LAndExp{ $$ = new_node(LOrExp, $3, NULL, $1, NT_OR, 0, NULL, NonType); }
    ;

%%


int yywrap(){
    return 1;
}
int main(int argc, char* argv[])
{
    //yydebug = 1;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(1);
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    // Redirect input to the file
    yyin = file;

    // Parse the input
    int result = yyparse();
    //printf("%d\n",(root->Next_Node_type));
    //display(root);
    process_tree(root,0);
    fclose(file);
    return result;
    
}
#include<stdarg.h>
void yyerror(const char* fmt, ...)
{
    syntax_error = 1;    
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "%s:%d ", filename, yylineno);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}	