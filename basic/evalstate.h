#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QMap>
class evalstate
{
    friend class MainWindow;
public:
    evalstate();
    ~evalstate();
    bool contains(QString );
    int getvalue(QString);
    void addVar(QString,int);
    void clear();
private:
    QMap<QString,int> var;


};

#endif // EVALSTATE_H
