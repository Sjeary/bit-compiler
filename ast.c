#include "ast.h"
#include "parser.h"

ASTNode *new_node(
    node_type type, 
    ASTNode *left, 
    ASTNode *mid, 
    ASTNode *right, 
    int int_val, 
    float float_val, 
    char *str_val, 
    node_type Next_Node_type
    )
    {
        ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
        n->type = type;
        n->left = left;
        n->mid = mid;
        n->right = right;
        n->int_val = int_val;
        n->float_val = float_val;
        n->str_val = str_val;
        n->Next_Node_type= Next_Node_type;
        return n;
    }




#include <stdio.h>
#include <string.h>
char* func_name;
int INDEX = 0;
int INDEX2 = 0;
int para_level_FParams = 0;
int para_level_RParams = 0;
int label_cnt=0;
const char* node_type_to_string(node_type nt) ;
typedef struct para_in_func{
    char func_name[10000];
    char para_name[10000];
    int offset;
} para_in_func;
typedef struct func_para_num{
    char func_name[10000];
    int func_para_num;
}func_para_num;
para_in_func Para_In_Func[20000];
func_para_num Func_Para_Num[1000];
int label_stack[500][2];
int label_stack_head=0;
int root_flag = 0;
ASTNode* ROOT ;
void debug(char* s,const char* ss)
{
    printf(s,ss);
}
void process_tree(ASTNode* T,int lev)
{
    if(root_flag == 0)
    {
        ROOT = T;
        root_flag = 1;
    }
    
    //printf("%3d",lev);
    for(int i = 0; i < lev ;i++)
    {
        //printf(" ");
    }
    //debug("%s\n",node_type_to_string(T->type));
    switch (T->type)
    {
    case Root:
        printf(".intel_syntax noprefix\n");
        ASTNode *t = ROOT->right;
        while(1)
        {
            if(t->right->type == FuncDef)
            {
                printf(".global %s\n",t->right->str_val);
            }
            if(t->left != NULL)t = t->left; 
            else if (t->left == NULL)break;
        }
        printf(".data\n");
        printf("format_str:\n");
        printf(".asciz \"%%d\\n\"\n");
        printf(".extern printf\n");
        printf(".text\n");

        process_tree(T->right,lev+1);

        break;
    case CompUnit:
        func_name = NULL;
        para_level_RParams = 0;
        para_level_FParams = 0;
        process_tree(T->right,lev+1); 
        if(T->left)
        {
            process_tree(T->left,lev+1);
        }
        

        break;
    case Decl:
        
        process_tree(T->right,lev+1);

        break;
    case VarDecl:
        process_tree(T->right,lev+1);
        break;
    case VarDef:
        //缺个ID等会记得补上
        int exist_flag = 0;
        for(int i = 0;i<INDEX;i++)
        {
            //printf("INDEX=%d\n",INDEX);
            //printf("i=%d\n",i);
            if
            (
                strcmp(Para_In_Func[i].func_name,func_name)!=0
                ||
                strcmp(Para_In_Func[i].para_name,T->str_val)!=0
            )
            continue;
            else 
            {
                exist_flag = 1;
                break;
            }
        }
        if(exist_flag == 0)
        {
            //memset(Para_In_Func[INDEX].para_name, 0, sizeof(Para_In_Func[INDEX].para_name));
            //memset(Para_In_Func[INDEX].func_name, 0, sizeof(Para_In_Func[INDEX].func_name));
            strcpy(Para_In_Func[INDEX].func_name, func_name);
            strcpy(Para_In_Func[INDEX].para_name, T->str_val);
            Para_In_Func[INDEX].offset = (para_level_RParams-1)*4;
            INDEX++;
            //printf("Para_In_Func[INDEX].func_name:%s\n",Para_In_Func[INDEX-1].func_name);
            //printf("Para_In_Func[INDEX].para_name:%s\n",Para_In_Func[INDEX-1].para_name);
            //printf("%d\n",Para_In_Func[INDEX-1].offset);
            para_level_RParams--;
            //printf("debug1\n");
            
        }

        if(T->right==NULL)
        {
            printf("mov DWORD PTR[ebp%d],0\n",Para_In_Func[INDEX-1].offset);
        }
        if(T->right)
        {
            process_tree(T->right,lev+1);
            for(int i = 0;i<INDEX;i++)
            {
                if(
                strcmp(Para_In_Func[i].func_name,func_name)==0
                &&strcmp(Para_In_Func[i].para_name,T->str_val)==0
                )
                {
                    if(Para_In_Func[i].offset<0)
                    {
                        printf("pop eax\n");
                        printf("mov DWORD PTR[ebp%d],eax\n",Para_In_Func[i].offset);
                    }
                    else if (Para_In_Func[i].offset>0)
                    {
                        printf("pop eax\n");
                        printf("mov DWORD PTR[ebp+%d],eax\n",Para_In_Func[i].offset);
                    }
                    break;
                }
            }
            
        }
        if(T->left)
        {
            process_tree(T->left,lev+1);
        }
        break;
    case InitVal:
        process_tree(T->right,lev+1);
        break;
    case FuncDef:
        //初始化函数，汇编语言中的开头部分
        if(func_name == NULL) func_name = (char* )malloc(1000);
        strcpy(func_name,T->str_val);
        strcpy(Func_Para_Num[INDEX2].func_name,func_name);
        printf("%s:\n",func_name);
        printf("push ebp\n");
        printf("mov ebp, esp\n");
        printf("sub esp, 200\n");
        if(T->right == NULL)
        {
            Func_Para_Num[INDEX2].func_para_num = 0;
        }
        if(T->right)//有参数列表，ebp+8之前有参数
        {
            process_tree(T->right,lev+1);
        }
        INDEX2++;
        process_tree(T->left,lev+1);//block区域
        if(T->Next_Node_type == Int)
        {
            //return操作
        }
        else if (T->Next_Node_type == Void)
        {
            printf("leave\n");
            printf("ret\n");
            //return操作
        }
        break;
    case FuncFParams:
        para_level_FParams++;
        process_tree(T->left,lev+1);//FuncFParam
        Func_Para_Num[INDEX2].func_para_num++;
        if(T->right)
        {
            process_tree(T->right,lev+1);//FuncFParams
            para_level_FParams--;
        }
        break;
    case FuncFParam:
        //变量的定义，建议放入符号表中
        //
        strcpy(Para_In_Func[INDEX].func_name, func_name);
        strcpy(Para_In_Func[INDEX].para_name, T->str_val);
        Para_In_Func[INDEX].offset = (para_level_FParams+1)*4;
        
        INDEX++;
        break;
    case Block:
        if(T->right)
        {
            process_tree(T->right,lev+1);//goto BlockItem
        }
        
        break;
    case BlockItem:
        process_tree(T->right,lev+1);
        if(T->left)
        {
            process_tree(T->left,lev+1);
        }
        
        break;
    case Stmt:
        switch(T->int_val)
        {
            case AssignStmt:
                process_tree(T->right,lev+1);
                //LVal是变量名字，和ebp-+的
                //地方对应。这个在最后算完之后才赋值的。
                //所以先计算Exp，最后弄就行。
                printf("pop eax\n");
                for(int i = 0;i<INDEX;i++)
                {
                    if(
                    strcmp(Para_In_Func[i].func_name,func_name)==0
                    &&strcmp(Para_In_Func[i].para_name,T->left->str_val)==0
                    )
                    {
                        if(Para_In_Func[i].offset<0)
                        {
                            printf("mov DWORD PTR[ebp%d],eax\n",Para_In_Func[i].offset);
                        }
                        else if (Para_In_Func[i].offset>0)
                        {
                            printf("mov DWORD PTR[ebp+%d],eax\n",Para_In_Func[i].offset);
                        }
                        break;
                    }
                }
                char *name = T->left->str_val;
                break;
            case ExpStmt:
                process_tree(T->right,lev+1);
                break;
            case Block:
                process_tree(T->right,lev+1);
                break;
            case ReturnStmt:
                process_tree(T->right,lev+1);
                //得到exp的结果存储再eax之后，再返回
                //此处写有返回值的返回
                printf("pop eax\n");
                printf("leave\n");
                printf("ret\n");
                break;
            case BlankReturnStmt:
                //写没有返回的blank语句

                break;
            case IfElseStmt:
                process_tree(T->left,lev+1);
                printf("pop eax\n");
                printf("cmp eax,0\n");
                int _label1 = label_cnt;
                label_cnt++;
                int _label_end_if = label_cnt;
                label_cnt++;
                printf("je _%s_%d\n",func_name,_label1);
                process_tree(T->mid,lev+1);
                printf("jmp _%s_%d\n", func_name, _label_end_if);
                printf("_%s_%d:\n",func_name,_label1);
                process_tree(T->right,lev+1);
                printf("_%s_%d:\n",func_name,_label_end_if);
                break;
            case IfStmt:
                process_tree(T->left,lev+1);
                printf("pop eax\n");
                printf("cmp eax,0\n");
                int _label2 = label_cnt;
                label_cnt++;
                printf("je _%s_%d\n",func_name,_label2);
                process_tree(T->right,lev+1);
                printf("_%s_%d:\n",func_name,_label2);
                break;
            case WhileStmt:
                
                int _label_begin = label_cnt;
                label_cnt++;
                int _label_end = label_cnt;
                label_cnt++;
                label_stack[label_stack_head][0] = _label_begin;
                label_stack[label_stack_head][1] = _label_end;
                label_stack_head++;
                printf("_%s_%d:\n",func_name,_label_begin);
                process_tree(T->left,lev+1);
                printf("pop eax\n");
                printf("cmp eax,0\n");

                printf("je _%s_%d\n",func_name,_label_end);
                
                process_tree(T->right,lev+1);
                printf("jmp _%s_%d\n",func_name,_label_begin);
                printf("_%s_%d:\n",func_name,_label_end);
                label_stack_head--;
                break;
            case ContinueStmt:
                printf("jmp _%s_%d\n",func_name,label_stack[label_stack_head-1][0]);
                break;
            case BreakStmt:
                printf("jmp _%s_%d\n",func_name,label_stack[label_stack_head-1][1]);
                
                break;

        }
        break;
    case Cond:
        process_tree(T->right,lev+1);
        break;
    case Exp:
        process_tree(T->right,lev+1);
        break;
    case LVal:
        //变量名字，似乎不用写
        //printf("LVal : %s\n",T->str_val);
        break;
    case PrimaryExp:
        if(T->Next_Node_type == NonType&&T->int_val == Exp)
        {//Exp
            process_tree(T->right,lev+1);
        }
        if(T->Next_Node_type == NonType&&T->int_val == LVal)
        {//LVal
            process_tree(T->right,lev+1);

            //printf("push eax\n");
            for(int i = 0;i < INDEX ;i++)
            {

                if(
                strcmp(Para_In_Func[i].func_name,func_name)==0
                &&strcmp(Para_In_Func[i].para_name,T->right->str_val)==0
                )
                {
                    if(Para_In_Func[i].offset<0)
                    {
                        printf("mov eax ,DWORD PTR[ebp%d]\n",Para_In_Func[i].offset);
                    }
                    else if (Para_In_Func[i].offset>0)
                    {
                        printf("mov eax, DWORD PTR[ebp+%d]\n",Para_In_Func[i].offset);
                    }
                    break;
                }
            }
            printf("push eax\n");
            //printf("pop eax\n");
        }
        if(T->Next_Node_type == Int)
        {//INT_LIT
            //这个节点唯一有用的值就是int_val，存放整数，生成汇编代码时才用得到
            //printf("Debug:Int 类型的:%d\n",T->int_val);
            printf("push %d\n",T->int_val);
        }
        break;
    case UnaryExp:
        switch (T->int_val)
        {
            case PrimaryExp:
                process_tree(T->right,lev+1);
                break;
            case FuncRParams://函数调用，这个即将压入实参
                if(T->right)
                {
                    process_tree(T->right,lev+1);
                }
                //处理完参数后再call T->str_val这个函数
                if(strcmp(T->str_val,"println_int")==0)
                {
                    printf("push offset format_str\n");
                    printf("call printf\n");
                    printf("add esp, 8\n");
                }
                else 
                {
                    printf("call %s\n",T->str_val);
                    //printf("add esp,200\n");
                    for(int i = 0 ; i < INDEX2; i++ )
                    {
                        if(strcmp(T->str_val,Func_Para_Num[i].func_name)==0)
                        {
                            printf("add esp, %d\n",Func_Para_Num[i].func_para_num*4);
                            break;
                        }
                    }
                    printf("push eax\n");
                }

                break;
            case NT_PLUS:
                
                //遇到了加号，处理，最简单的方式就是，
                //等到后面的式子处理好变成一个数存放在栈的时候，
                //把栈内的弹出来，然后什么都不干
                //所以要等到后面这个运算完毕之后再做操作
                process_tree(T->right,lev+1);
                //操作
                printf("pop eax\n");
                printf("push eax\n");

                break;
            case NT_MINUS:
                
                //遇到了负号，处理，最简单的方式就是，
                //等到后面的式子处理好变成一个数存放在栈的时候，
                //把栈内的弹出来，然后取负数
                //所以要等到后面这个运算完毕之后再做操作
                process_tree(T->right,lev+1);
                printf("pop eax\n");
                printf("push ebx\n");
                printf("push ecx\n");
                printf("push edx\n");

                printf("neg eax\n");

                printf("pop edx\n");
                printf("pop ecx\n");
                printf("pop ebx\n");
                printf("push eax\n");
                //操作

                break;
            case NT_BITWISE_NOT:
                
                //遇到了~，处理，最简单的方式就是，
                //等到后面的式子处理好变成一个数存放在栈的时候，
                //把栈内的弹出来，然后按位取反
                //所以要等到后面这个运算完毕之后再做操作
                process_tree(T->right,lev+1);
                //操作
                printf("pop eax\n");
                printf("push ebx\n");
                printf("push ecx\n");
                printf("push edx\n");

                printf("not eax\n");

                printf("pop edx\n");
                printf("pop ecx\n");
                printf("pop ebx\n");
                printf("push eax\n");

                break;
            case NT_NOT:
                
                //遇到了加号，处理，最简单的方式就是，
                //等到后面的式子处理好变成一个数存放在栈的时候，
                //把栈内的弹出来，然后加感叹号操作
                //所以要等到后面这个运算完毕之后再做操作
                process_tree(T->right,lev+1);
                //操作
                printf("pop eax\n");
                printf("push ebx\n");
                printf("push ecx\n");
                printf("push edx\n");

                printf("test eax, eax\n");
                printf("setz al\n");      
                printf("movzx eax, al\n");

                printf("pop edx\n");
                printf("pop ecx\n");
                printf("pop ebx\n");
                printf("push eax\n");
                break;
        } 
        break;
    case FuncRParams:
        //后面的先压栈
        if(T->left)
        {
            process_tree(T->left,lev+1);
        }
        //先操作，然后等Exp算完之后，再把这个存放再eax中的值压栈
        process_tree(T->right,lev+1);//计算Exp

        //压栈操作

        break;
    case MulExp:
        switch(T->int_val)
        {
            case UnaryExp:
                process_tree(T->right,lev+1);
                break;
            case NT_MUL:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                printf("pop ecx\n");     
                printf("pop eax\n");     
                printf("imul eax, ecx\n"); 
                printf("push eax\n");    
                //pop , pop  然后计算
                break;
            case NT_DIV:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop , pop  然后计算
                printf("pop ecx\n");    
                printf("pop eax\n");     
                printf("cdq\n");          
                printf("idiv ecx\n");    
                printf("push eax\n");    

                break;
            case NT_MOD:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop , pop  然后计算
                printf("pop ecx\n");    
                printf("pop eax\n");     
                printf("cdq\n");          
                printf("idiv ecx\n");    
                printf("push edx\n");   
                break;

        }
        break;
    case AddExp:
        switch (T->int_val)
        {
        case MulExp:
            process_tree(T->right,lev+1);
            break;
        case NT_PLUS:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");
                printf("pop eax\n");
                printf("add eax, ebx\n");
                printf("push eax\n");
            break;      
        case NT_MINUS:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");
                printf("pop eax\n");
                printf("sub eax, ebx\n");
                printf("push eax\n");
            break;      
        default:
            break;
        }  
        break;
    case RelExp:
        switch(T->int_val)
        {
            case AddExp:
                process_tree(T->right,lev+1);
                break;
            case NT_LT:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");       
                printf("pop eax\n");       
                printf("cmp eax, ebx\n");   
                printf("setl al\n");         
                printf("movzx eax, al\n");   
                printf("push eax\n");        
                break;
            case NT_GT:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");       
                printf("pop eax\n");       
                printf("cmp eax, ebx\n");   
                printf("setg al\n");         
                printf("movzx eax, al\n");   
                printf("push eax\n");      
                break;
            case NT_LE:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");       
                printf("pop eax\n");       
                printf("cmp eax, ebx\n");   
                printf("setle al\n");         
                printf("movzx eax, al\n");   
                printf("push eax\n");   
                break;
            case NT_GE:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");       
                printf("pop eax\n");       
                printf("cmp eax, ebx\n");   
                printf("setge al\n");         
                printf("movzx eax, al\n");   
                printf("push eax\n");   
                break;
        }
        break;
    case EqExp:
        switch(T->int_val)
        {
            case RelExp:
                process_tree(T->right,lev+1);
                break;
            case NT_EQ:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");       
                printf("pop eax\n");       
                printf("cmp eax, ebx\n");   
                printf("sete al\n");         
                printf("movzx eax, al\n");   
                printf("push eax\n");   
                break;
            case NT_NE:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");       
                printf("pop eax\n");       
                printf("cmp eax, ebx\n");   
                printf("setne al\n");         
                printf("movzx eax, al\n");   
                printf("push eax\n");   
                break;

        }
        break;
    case BitAndExp:
        switch(T->int_val)
        {
            case EqExp:
                process_tree(T->right,lev+1);
                break;
            case NT_BITWISE_AND:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");
                printf("pop eax\n");
                printf("and eax, ebx\n");
                printf("push eax\n");
                break;

        }
        break;
    case BitXorExp:
        switch(T->int_val)
        {
            case BitAndExp:
                process_tree(T->right,lev+1);
                break;
            case NT_BITWISE_XOR:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                printf("pop ebx\n");
                printf("pop eax\n");
                printf("xor eax, ebx\n");
                printf("push eax\n");
                //pop eax, pop ebx 然后计算
                break;

        }
        break;
    case BitOrExp:
        switch(T->int_val)
        {
            case BitXorExp:
                process_tree(T->right,lev+1);
                break;
            case NT_BITWISE_OR:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                printf("pop ebx\n");
                printf("pop eax\n");
                printf("or eax, ebx\n");
                printf("push eax\n");
                //pop eax, pop ebx 然后计算
                break;

        }
        break;
    case LAndExp:
        switch(T->int_val)
        {
            case BitOrExp:
                process_tree(T->right,lev+1);
                break;
            case NT_AND:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");          
                printf("pop eax\n");         
                printf("test eax, eax\n");    
                printf("setnz al\n");        
                printf("test ebx, ebx\n");   
                printf("setnz bl\n");        
                printf("and al, bl\n");       
                printf("movzx eax, al\n");   
                printf("push eax\n");     
                break;

        }
        break;
    case LOrExp:
        switch(T->int_val)
        {
            case LAndExp:
                process_tree(T->right,lev+1);
                break;
            case NT_OR:
                //处理好左右，都压栈，然后再计算
                process_tree(T->right,lev+1);
                process_tree(T->left,lev+1);
                //pop eax, pop ebx 然后计算
                printf("pop ebx\n");          
                printf("pop eax\n");         
                printf("test eax, eax\n");    
                printf("setnz al\n");        
                printf("test ebx, ebx\n");   
                printf("setnz bl\n");        
                printf("or al, bl\n");       
                printf("movzx eax, al\n");   
                printf("push eax\n");  
                break;

        }
        break;

    default:
        break;
    }
}















const char* node_type_to_string(node_type nt) {
    switch (nt) {
        case CompUnit:
            return "CompUnit";
        case Decl:
            return "Decl";
        case BType:
            return "BType";
        case VarDecl:
            return "VarDecl";
        case VarDef:
            return "VarDef";
        case InitVal:
            return "InitVal";
        case FuncDef:
            return "FuncDef";
        case FuncType:
            return "FuncType";
        case FuncFParams:
            return "FuncFParams";
        case FuncFParam:
            return "FuncFParam";
        case Block:
            return "Block";
        case BlockItem:
            return "BlockItem";
        case Stmt:
            return "Stmt";
        case Exp:
            return "Exp";
        case Cond:
            return "Cond";
        case LVal:
            return "LVal";
        case PrimaryExp:
            return "PrimaryExp";
        case Number:
            return "Number";
        case UnaryExp:
            return "UnaryExp";
        case UnaryOp:
            return "UnaryOp";
        case FuncRParams:
            return "FuncRParams";
        case MulExp:
            return "MulExp";
        case AddExp:
            return "AddExp";
        case RelExp:
            return "RelExp";
        case EqExp:
            return "EqExp";
        case LAndExp:
            return "LAndExp";
        case LOrExp:
            return "LOrExp";
        case ConstExp:
            return "ConstExp";
        case Root:
            return "Root";
        case NonType:
            return "NonType";
        case Int:
            return "Int";
        case Void:
            return "Void";
        case BlankStmt:
            return "BlankStmt";
        case ExpStmt:
            return "ExpStmt";
        case AssignStmt:
            return "AssignStmt";
        case BlankReturnStmt:
            return "BlankReturnStmt";
        case ReturnStmt:
            return "ReturnStmt";
        case IfElseStmt:
            return "IfElseStmt";
        case IfStmt:
            return "IfStmt";
        case WhileStmt:
            return "WhileStmt";
        case BreakStmt:
            return "BreakStmt";
        case ContinueStmt:
            return "ContinueStmt";
        case NT_PLUS:
            return "NT_PLUS";
        case NT_MINUS:
            return "NT_MINUS";
        case NT_NOT:
            return "NT_NOT";
        case NT_BITWISE_NOT:
            return "NT_BITWISE_NOT";
        case NT_BITWISE_AND:
            return "NT_BITWISE_AND";
        case NT_BITWISE_XOR:
            return "NT_BITWISE_XOR";
        case NT_BITWISE_OR:
            return "NT_BITWISE_OR";
        case NT_AND:
            return "NT_AND";
        case NT_OR:
            return "NT_OR";
        case NT_MUL:
            return "NT_MUL";
        case NT_DIV:
            return "NT_DIV";
        case NT_MOD:
            return "NT_MOD";
        case NT_GE:
            return "NT_GE";
        case NT_LE:
            return "NT_LE";
        case NT_GT:
            return "NT_GT";
        case NT_LT:
            return "NT_LT";
        case NT_EQ:
            return "NT_EQ";
        case NT_NE:
            return "NT_NE";
        case BitOrExp:
            return "BitOrExp";
        case BitAndExp:
            return "BitAndExp";
        case BitXorExp:
            return "BitXorExp";
        default:
            return "Unknown type";
    }
}












