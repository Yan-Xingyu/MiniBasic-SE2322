#include "expression.h"

Expression::Expression()
{

}
Expression::~Expression()
{

}

ConstantExp::ConstantExp(int num)
{
    this->value =num;
}
ConstantExp::~ConstantExp()
{

}
expType_e ConstantExp::type() const
{
    return CONST;
}
int ConstantExp::getValue(evalstate &)
{
    return this->value;
}
QString ConstantExp::getName()
{
    return QString::number(value);
}
QString ConstantExp::tranverse(int)
{
    return getName();
}
IdentifierExp::IdentifierExp(QString name)
{
    this->name = name;
}
IdentifierExp::~IdentifierExp()
{

}
expType_e IdentifierExp::type() const
{
    return ID;
}
int IdentifierExp::getValue(evalstate &var)
{
    if(!var.contains(name))
        throw "Invalid Variable:"+name+" !";
    return var.getvalue(name);
}
QString IdentifierExp::getName()
{
    return name;
}
QString IdentifierExp::tranverse(int)
{
    return getName();
}
CompoundExp::CompoundExp(QString op,Expression* lhs,Expression* rhs)
{
    this->op = op;
    this->lhs =lhs;
    this->rhs =rhs;
}
CompoundExp::~CompoundExp()
{
    if(rhs!=nullptr)
        delete rhs;
    if(lhs!=nullptr)
        delete lhs;
}
expType_e CompoundExp::type() const
{
    return COMP;
}
//calculate a**b
int pow(int a,int b)
{
    int ans=1,base=a;
        while(b)
        {
            if(b & 1)
            {
            ans=ans*base;
            }
            base=base*base;
            b = b >> 1;
        }
        return ans;
}
int CompoundExp::getValue(evalstate & val)
{
    int left,right;
    if(rhs == nullptr)
    {
        //err
        throw "Invalid Expression";
    }
    right = rhs->getValue(val);
    if(op == "=")
    {
        //handle the situation like LET a=2+1
        val.addVar(lhs->getName(),right);
        return right;
    }
    if(lhs == nullptr)
    {
        throw "Invalid Expression";
        //err
    }
    left = lhs->getValue(val);
    if(op == "+")return left+right;
    if(op == "-")return left-right;
    if(op == "*")return left*right;
    if(op == "/")
    {
        if(right == 0)
        {
            throw "Can't divide by zero";
            //err
        }
        return left/right;
    }
    if(op == "**")return pow(left,right);
    if(op == "<")return int(left<right);
    if(op == ">")return int(left>right);
    if(op == "==")return int(left==right);
    return 0;
}
QString CompoundExp::getName()
{
    return op;
}
#define IS_BOOLOPS(s) ((s)==">"||(s)== "<"||(s)=="<="||(s)==">="||(s)=="==")
//return the Syntax Tree
QString CompoundExp::tranverse(int dep=0)
{
    QString res="";
    if(IS_BOOLOPS(op))
    {
        for(int i=-1;i<dep;i++)
            res+="    ";
        res+=lhs->tranverse(dep+1)+"\n";
        for(int i=-1;i<dep;i++)
            res+="    ";
        res+=op+"\n";
        for(int i=-1;i<dep;i++)
            res+="    ";
        res+=rhs->tranverse(dep+1);
    }
    else
    {
        res+=op+"\n";
        for(int i=-1;i<dep;i++)
            res+="    ";
        res+=lhs->tranverse(dep+1)+"\n";
        for(int i=-1;i<dep;i++)
            res+="    ";
        res+=rhs->tranverse(dep+1);
    }
    return res;
}
Expression* CompoundExp::getLHS()
{
    return lhs;
}
Expression* CompoundExp::getRHS()
{
    return rhs;
}
