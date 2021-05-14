#ifndef PROGRAM_H
#define PROGRAM_H
#include <QMap>
#include "evalstate.h"
#include "parser.h"
#include "expression.h"
struct debugInfo_t{
    QMap<int,QStringList>::ConstIterator* debugline;
    bool errflag,ondebug;
    int lineorder;
};
class program
{
    friend class MainWindow;
public:
    program();
    ~program();

    QString generateTree(QChar delim ='\n');
    QString runProgram();
    QString runSingleCommand(QString str);

    QString getAllCode() const;
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
    //一些工具函数
    QMap<int,QStringList>::ConstIterator jmpN(int n,int* lineorder=nullptr);
    void err_print(QString);
    QString printfHandler(QStringList);
    QMap<int,QStringList>::ConstIterator ifThenHandler(QStringList,int* lineorder=nullptr);
};

#endif // PROGRAM_H
