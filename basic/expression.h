#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "evalstate.h"
enum expType_e{CONST,ID,COMP};
class Expression
{
public:
    Expression();
    virtual ~Expression();
    virtual expType_e type()const=0 ;
    virtual int getValue(evalstate&)=0;
    virtual QString getName()=0;
    virtual QString tranverse(int)=0;
};

class ConstantExp : public Expression
{
public:
    ConstantExp(int val);
    ~ConstantExp();
    expType_e type() const;
    int getValue(evalstate&);
    virtual QString getName();
    QString tranverse(int);
private:
    int value;
};

class IdentifierExp : public Expression
{
public:
    IdentifierExp(QString name);
    ~IdentifierExp();
    expType_e type() const;
    int getValue(evalstate &);
    QString getName();
    QString tranverse(int);
private:
    QString name;
};

class CompoundExp : public Expression
{
public:
    CompoundExp(QString op,Expression* lhs,Expression* rhs);
    ~CompoundExp();
    expType_e type() const;
    int getValue(evalstate&);
    virtual QString getName();
    QString tranverse(int);
    Expression *getLHS();
    Expression *getRHS();
private:
    QString op;
    Expression* lhs;
    Expression* rhs;
};

#endif // EXPRESSION_H
