#include "evalstate.h"

evalstate::evalstate()
{

}
evalstate::~evalstate()
{

}
void evalstate::addVar(QString name, int val)
{
    var.insert(name,val);
}
void evalstate::addVar(QString name, QString val)
{
    var.insert(name,val);
}
void evalstate::addVar(QString name, QVariant val)
{
    var.insert(name,val);
}
bool evalstate::contains(QString str)
{
    return (var.find(str)!=var.end());
}
QVariant evalstate::getvalue(QString str)
{
    return var[str];
}
QString evalstate::showAllVariables() const
{
    QString vars="";
    for(QMap<QString,QVariant>::ConstIterator variable= var.cbegin();variable!=var.cend();variable++)
    {
        vars+=variable.key();
        vars+=":";
        if(!strcmp(variable.value().typeName(),"int"))
            vars=vars+"INT = "+QString::number(variable.value().toInt());
        else if(!strcmp(variable.value().typeName(),"QString"))
            vars+="STR = "+variable.value().toString();
        vars+="\n";
    }
    return vars;
}
void evalstate::clear()
{
    var.clear();
}
