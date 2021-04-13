#ifndef PROGRAM_H
#define PROGRAM_H
#include <QMap>
#include "evalstate.h"
#include "parser.h"
#include "expression.h"
class program
{
    friend class MainWindow;
public:
    program();
    ~program();
    QString giveAllCode() const;
    QString runProgram();
private:
    QMap<int,QStringList> map;
    evalstate symbol;
    parser parser;
    QString generateTree();
    QMap<int,QStringList>::ConstIterator jmpN(int n);
    QString runSingleCommand(QString str);
    QString stringProcess(QString &);
    void clear();
};

#endif // PROGRAM_H
