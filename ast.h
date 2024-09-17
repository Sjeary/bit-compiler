#ifndef AST_H
#define AST_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "parser.h"

//我想直接按照SysY的语法写了，这个版本先简化一下，没有任何的常量和浮点数
typedef enum node_type{
    CompUnit,
    Decl,
    BType,
    VarDecl,
    VarDef,
    InitVal,
    FuncDef,
    FuncType,
    FuncFParams,
    FuncFParam,
    Block,
    BlockItem,
    Stmt,
    Exp,
    Cond,
    LVal,
    PrimaryExp,
    Number,
    UnaryExp,
    UnaryOp,
    FuncRParams,
    MulExp,
    AddExp,
    RelExp,
    EqExp,
    LAndExp,
    LOrExp,
    ConstExp,
    Root,
    NonType,
    Int,
    Void,
    BlankStmt,
    ExpStmt,
    AssignStmt,
    BlankReturnStmt,
    ReturnStmt,
    IfElseStmt,
    IfStmt,
    WhileStmt,
    BreakStmt,
    ContinueStmt,
    NT_PLUS,
    NT_MINUS,
    NT_NOT,
    NT_BITWISE_NOT,
    NT_BITWISE_AND,
    NT_BITWISE_XOR,
    NT_BITWISE_OR,
    NT_AND,
    NT_OR,
    NT_MUL,
    NT_DIV,
    NT_MOD,
    NT_GE,
    NT_LE,
    NT_GT,
    NT_LT,
    NT_EQ,
    NT_NE,
    BitOrExp,
    BitAndExp,
    BitXorExp


}node_type;

typedef struct ASTNode
{
    node_type type;
    struct ASTNode *left;
    struct ASTNode *mid;
    struct ASTNode *right;
    int int_val;
    float float_val;
    char* str_val;
    node_type Next_Node_type;

}ASTNode;
ASTNode *new_node(
    node_type type, 
    ASTNode *left, 
    ASTNode *mid, 
    ASTNode *right, 
    int int_val, 
    float float_val, 
    char *str_val, 
    node_type Next_Node_type
    );

/*


编译单元 CompUnit → [ CompUnit ] ( Decl | FuncDef )
声明 Decl → VarDecl
变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
变量定义 VarDef → Ident
             | Ident '=' InitVal
变量初值 InitVal → Exp
函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
函数类型 FuncType → 'void' | 'int'
函数形参表 FuncFParams → FuncFParam { ',' FuncFParam }
函数形参 FuncFParam → BType Ident
语句块 Block → '{' { BlockItem } '}'
语句块项 BlockItem → Decl | Stmt
语句 Stmt → LVal '=' Exp ';'
         | [Exp] ';'
         | Block
         | 'return' [Exp] ';'
表达式 Exp → AddExp
基本类型 BType → 'int'
左值表达式 LVal → Ident
基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number
数值 Number → IntConst
一元表达式 UnaryExp → PrimaryExp
             | Ident '(' [FuncRParams] ')'
             | UnaryOp UnaryExp
单目运算符 UnaryOp → '+' | '−'
函数实参表 FuncRParams → Exp { ',' Exp }
乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp
逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp
逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp




*/



#endif