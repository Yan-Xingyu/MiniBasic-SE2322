#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QMap>
#include <QVariant>
class evalstate
{
public:
    evalstate();
    ~evalstate();
    bool contains(QString);
    QVariant getvalue(QString);
    void addVar(QString,int);
    void addVar(QString,QString);
    void addVar(QString,QVariant);
    QString showAllVariables() const;
    void clear();
private:
    QMap<QString,QVariant> var;
};

#endif // EVALSTATE_H
