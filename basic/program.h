#ifndef PROGRAM_H
#define PROGRAM_H
#include <QMap>
#include "evalstate.h"
#include "parser.h"
#include "expression.h"
class program
{
public:
    program();
    ~program();
    QString giveAllCode() const;

    QString generateTree();
    QString runProgram();
    QString runSingleCommand(QString str);
    QList<int> getErrorLines() const;
    bool stringProcess(QString &);
    void clear();
private:
    QMap<int,QStringList> map;
    evalstate symbol;
    parser parser;
    QList<int> errlines;
    QMap<int,QStringList>::ConstIterator jmpN(int n);
};

#endif // PROGRAM_H
