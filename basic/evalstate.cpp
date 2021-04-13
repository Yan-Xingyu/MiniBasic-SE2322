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
bool evalstate::contains(QString str)
{
    return (var.find(str)!=var.end());
}
int evalstate::getvalue(QString str)
{
    return var[str];
}
void evalstate::clear()
{
    var.clear();
}
