#include "parser.h"

parser::parser()
{

}
parser::~parser()
{

}
#define IS_DIGIT(s) (((s)>='0' && (s)<='9'))
#define IS_NUM(s) (((s)>='0' && (s)<='9')||(s)=='-')
#define IS_LETTER(s) (((s)>='a' && (s)<='z') || ((s)>='A' && (s)<='Z'))
#define IS_OPERATOR(s) ((s)=='+' || (s)=='-' || (s)=='*' || (s)=='/' \
||(s)=='='||(s)=='>'||(s)=='<')
#define IS_QUOTE(s) ((s)=='\'' || (s)=='\"')
#define IS_BRACK(s) ((s)=='('||(s)==')')
#define IS_LEGAL_LETTER(s) (IS_DIGIT(s)||IS_LETTER(s)||IS_OPERATOR(s)||IS_QUOTE(s)||IS_BRACK(s))
Expression* parser::buildTree(QString raw)
{
    QString id="";
    QString num="";
    QString op="";
    Type_e lastState = P_DEFAULT;
    for(int i=0;i<raw.length();i++)
    {
        if(raw[i]==' ')
            continue;
        if(!IS_LEGAL_LETTER(raw[i]))
            return nullptr;
        //process the constant number
        while(IS_NUM(raw[i]))
        {
            if(raw[i]=='-')
            {
                if(lastState == P_NUM || lastState == P_VAR)
                {
                    break;
                }
                else if(lastState == P_OPS&&raw[i-1]!='=')
                {
                    return nullptr;
                    //err
                }
            }
            else if(lastState == P_NUM || lastState == P_VAR)
            {
                return nullptr;
            }
            num+=raw[i];
            if(i+1<raw.length())
                i++;
            else
                break;
        }
        if(num!="")
        {
            ConstantExp* con = new ConstantExp(num.toInt());
            operands.push(con);
            lastState = P_NUM;
            num="";
        }
        //process the variables
        if(IS_LETTER(raw[i]))
        {
            if(lastState == P_NUM)
                return nullptr;
            while(IS_LETTER(raw[i])||IS_DIGIT(raw[i]))
            {
                id+=raw[i];
                if(i+1<raw.length())
                    i++;
                else break;
            }
            if(id!="")
            {
                IdentifierExp* con = new IdentifierExp(id);
                operands.push(con);
                lastState = P_VAR;
                id="";
                if(!IS_LETTER(raw[i])&&!IS_DIGIT(raw[i]))
                    i--;
                continue;
            }
        }
        if(raw[i]=='(')
        {
            lastState = P_LBR;
            operators.push("(");
            continue;
        }
        //pop until '('
        if(raw[i] == ')')
        {
            lastState = P_RBR;
            while(operators.top()!="(")
            {
                if(operators.empty())
                    return nullptr;
                lastState = P_RBR;
                try {
                    popOps();
                }  catch (const char*) {
                    return nullptr;
                }
            }
            operators.pop();
            continue;
        }
        //process the operators
        if(IS_OPERATOR(raw[i]))
        {
            if(lastState == P_OPS)
            {
                //err
                return nullptr;
            }
            op+=raw[i];
            //read**,>=,<=,== operators
            if(i+1<raw.length())
                if((raw[i]=='*' && raw[i+1] =='*')||(raw[i]=='>' && raw[i+1] =='=')||
                    (raw[i]=='<' && raw[i+1] =='=')||(raw[i]=='=' && raw[i+1] =='='))
                    {
                        op+=raw[++i];
                    }
            //push in if the stack is empty
            if(operators.empty())
            {
                operators.push(op);
                lastState = P_OPS;
                op = "";
                continue;
            }
            //compare the precedence between read operators and stack-top operators
            //pop until temporary operators greater than stack-top operators
            int curp = getPrecedence(op);
            int topp = getPrecedence(operators.top());
            if((curp>topp)||(op=="**"&&operators.top()=="**"))//right associativity of **
            {
                operators.push(op);
                lastState = P_OPS;
                op= "";
                continue;
            }
            else
            {
                while(curp<=topp)
                {
                    try {
                        popOps();
                    }  catch (const char*) {
                        return nullptr;
                    }

                    if(operators.empty())
                        break;
                    topp = getPrecedence(operators.top());
                }
                operators.push(op);
                lastState = P_OPS;
                op = "";
            }
        }
        if(IS_QUOTE(raw[i]))
        {
            int j = i+1,len=raw.length();
            QString str="";
            while(j<len)
            {
                if(IS_QUOTE(raw[j])&&j+1!=len)
                    return nullptr;
                str+=raw[j++];
            }
            if(raw[i]!=raw[j-1])
                return nullptr;
            str= str.left(str.length()-1);
            ConstantExp* con=new ConstantExp(str);
            operands.push(con);
            break;
        }
    }
    //pop until stack is empty
    while(!operators.empty())
    {
        try {
            popOps();
        }  catch (const char*) {
            return nullptr;
        }
    }
    return operands.top();
}
//make a compound expression
void parser::popOps()
{
    QString top = operators.pop();
    if(top == "(")
    {
        throw "error";
    }
    Expression* left;
    Expression* right;
    right = (!operands.empty())?operands.pop():nullptr;
    left = (!operands.empty())?operands.pop():nullptr;
    if(right == nullptr || left == nullptr)
        throw "error";
    CompoundExp* com = new CompoundExp(top,left,right);
    operands.push(com);
}
int parser::getPrecedence(QString ops)
{
    if(!QString::compare(ops,"**"))
        return 4;
    if(!QString::compare(ops,"*") || !QString::compare(ops,"/"))
        return 3;
    if(!QString::compare(ops,"+") || !QString::compare(ops,"-"))
        return 2;
    if(!QString::compare(ops,"="))
        return 1;
    return -1;
}
void parser::clear()
{
    operators.clear();
    operands.top() = nullptr;
}
