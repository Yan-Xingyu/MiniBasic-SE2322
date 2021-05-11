#ifndef PROGRAM_H
#define PROGRAM_H
#include <QMap>
#include "evalstate.h"
#include "parser.h"
#include "expression.h"
struct debugInfo_t{
    QMap<int,QStringList>::ConstIterator* debugline;
    bool errflag,ondebug;
};

class program
{
    friend class MainWindow;
public:
    program();
    ~program();
    QString giveAllCode() const;

    QString generateTree(QChar delim ='\n');
    QString runProgram();
    QString runSingleCommand(QString str);
    QList<int> getErrorLines() const;
    QString getVariables()const;
    bool stringProcess(QString &);
    void clear();
private:
    QMap<int,QStringList> map;
    evalstate symbol;
    parser parser;
    QList<int> errlines;
    debugInfo_t debugInfo;
    QMap<int,QStringList>::ConstIterator jmpN(int n);
    void err_print(QString);
    QString printfHandler(QStringList);
};

#endif // PROGRAM_H
