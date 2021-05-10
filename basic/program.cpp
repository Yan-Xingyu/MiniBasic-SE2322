#include "program.h"
#include "evalstate.h"
#include "mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>
program::program()
{

}

program::~program()
{

}
//return all code stored in Map
QString program::giveAllCode() const
{
    QString code="";
    for(auto k = map.cbegin();k!=map.cend();k++)
        code+=QString::number((k.key()))+" "+k.value().join(' ')+"\n";
    return code;
}
#define LEGAL_LINENUM(n)  ((n>0)&&(n<=1000000))
//build the code Tree of each command
QString program::generateTree()
{
    QString codeTree="";
    int line = 0;
    for(auto k = map.cbegin();k!=map.cend();k++,line++)
    {
        QString str = k.value().join(' ').simplified();
        QStringList tokens=str.split(' ',Qt::SkipEmptyParts);
        QString firsttoken = tokens.front();
        if(firsttoken==QString("LET"))
        {
            tokens.pop_front();
            Expression* tree = parser.buildTree(tokens.join(""));
            if(tree == nullptr)
            {
                codeTree+=QString::number(k.key())+" Error\n";
                errlines.append(line);
                continue;
            }
            codeTree+=QString::number(k.key())+" LET "+tree->tranverse(2)+"\n";
            parser.clear();
        }
        else if(firsttoken == QString("PRINT"))
        {
            tokens.pop_front();
            Expression* tree = parser.buildTree(tokens.join(""));
            if(tree == nullptr)
            {
                codeTree+=QString::number(k.key())+" Error\n";
                errlines.append(line);
                continue;
            }
            codeTree+=QString::number(k.key())+" PRINT\n        "+tree->tranverse(2)+"\n";
            parser.clear();
        }
        else if(firsttoken == QString("IF"))
        {
            tokens.pop_front();
            tokens=tokens.join("").split("THEN",Qt::SkipEmptyParts);
            Expression* condtree = parser.buildTree(tokens.value(0));
            if(condtree == nullptr)
            {
                codeTree+=QString::number(k.key())+" Error\n";
                errlines.append(line);
                continue;
            }
            codeTree+=QString::number(k.key())+" IF THEN\n"+
                    condtree->tranverse(2)+"\n          "+tokens.value(1)+"\n";
            parser.clear();
        }
        else if(firsttoken == QString("GOTO"))
        {
            tokens.pop_front();
            int linenum = tokens.join("").simplified().toInt();
            if(LEGAL_LINENUM(linenum))
                codeTree+=QString::number(k.key())+" GOTO\n        "+QString::number(linenum)+"\n";
            else
            {
                codeTree+=QString::number(k.key())+" Error\n";
                errlines.append(line);
            }
        }
        else if(firsttoken == QString("END"))
        {
            codeTree+=QString::number(k.key())+" END";
        }
        else if(firsttoken == QString("INPUT"))
        {
            tokens.pop_front();
            if(tokens.length() == 1)
                codeTree+=QString::number(k.key())+" INPUT\n        "+tokens.value(0);
            else
            {
                codeTree+=QString::number(k.key())+" Error\n";
                errlines.append(line);
            }
        }
        else if(firsttoken == QString("REM"))
        {
            tokens.pop_front();
            codeTree+= QString::number(k.key())+" REM\n        "+tokens.join(' ')+"\n";
        }
        else if(firsttoken == QString("LIST"))
        {
            continue;
        }
        else
        {
            codeTree+=QString::number(k.key())+" Error\n";
            errlines.append(line);
        }
    }
    return codeTree;
}
//move to line numberd n
QMap<int,QStringList>::ConstIterator program::jmpN(int n)
{
    auto tmp = map.cbegin();
    for(;tmp!=map.cend();tmp++)
    {
        if(tmp.key()== n)
        {
            tmp--;
            return tmp;
        }
    }
    return tmp;
}

QList<int> program::getErrorLines() const
{
    return errlines;
}
//print error information
static void err_print(QString str)
{
    QMessageBox mesg;
    mesg.warning(NULL,"ERROR",str);
}
//run the program within the line number
QString program::runProgram()
{
    QString result="";
    for(auto k = map.cbegin();k!=map.cend();k++)
    {
        QString str = k.value().join(' ').simplified();
        QStringList tokens=str.split(' ',Qt::SkipEmptyParts);
        QString firsttoken = tokens.front();
        //each situation
        if(firsttoken==QString("LET"))
        {
            tokens.pop_front();
            Expression* tree = parser.buildTree(tokens.join(""));
            if(tree == nullptr)
            {
                err_print(QString::number(k.key())+" Invalid Expression in line "+QString::number(k.key()));
                return result+"\n";
            }
            else
            {
                try {
                    tree->getValue(symbol);
                }  catch (QString e) {
                    err_print(QString::number(k.key())+" "+e);
                }
            }
            parser.clear();
        }
        else if(firsttoken == QString("PRINT"))
        {
            tokens.pop_front();
            Expression* tree = parser.buildTree(tokens.join(""));
            if(tree == nullptr)
            {
                err_print(QString::number(k.key())+"Invalid Expression in line "+QString::number(k.key()));
                return result+"\n";
            }
            else
            {
                try {
                    result += QString::number(tree->getValue(symbol))+"\n";
                }  catch (QString e) {
                    err_print(QString::number(k.key())+" "+e);
                }
            }

            parser.clear();
        }
        else if(firsttoken == QString("IF"))
        {
            tokens.pop_front();
            tokens=tokens.join("").split("THEN",Qt::SkipEmptyParts);
            Expression* condtree = parser.buildTree(tokens.value(0));
            if(condtree != nullptr)
            {
                int cond;
                try {
                    cond = condtree->getValue(symbol);
                }  catch (QString e) {
                    err_print(QString::number(k.key())+" "+e);
                }
                int n = tokens.value(1).toInt();
                if(!LEGAL_LINENUM(n))
                {
                    err_print(QString::number(k.key())+": Invalid line number "+tokens.value(1));
                    return result+"\n";
                }
                if(cond) {
                    /*go to n*/
                    auto tmp =jmpN(n);
                    if(tmp==map.cend())
                    {
                        err_print(QString::number(k.key())+": Can't find line number "+tokens.value(1));
                        return result+"\n";
                        //err
                    }
                    else k=tmp;
                }
            }
            parser.clear();
        }
        else if(firsttoken == QString("GOTO"))
        {
            tokens.pop_front();
            int linenum = tokens.join("").simplified().toInt();
            /*go to n*/
            auto tmp =jmpN(linenum);
            if(tmp==map.cend())
            {
                err_print(QString::number(k.key())+": Can't find line number "+tokens.value(1));
                return result+"\n";
                //err
            }
            else k=tmp;
        }
        else if(firsttoken == QString("END"))
        {
            break;
        }
        else if(firsttoken == QString("INPUT")){
            tokens.pop_back();
            int var = QInputDialog::getInt(NULL,"INPUT","Input "+tokens.value(0),0,-2147483647,2147483647,1);
            symbol.addVar(tokens.value(0),var);
        }
        else if(firsttoken == QString("REM"))
        {
            continue;
        }
        else if(firsttoken == QString("LIST"))
        {
            continue;
        }
        // invalid command
        else
        {
            err_print(QString::number(k.key())+": Invalid command :"+firsttoken);
        }
    }
    return result;
}
//run command without line number
QString program::runSingleCommand(QString str)
{
    if(str.isEmpty())return "";
    QStringList tokens=str.split(' ',Qt::SkipEmptyParts);
    QString firsttoken = tokens.front();
    if(firsttoken==QString("LET"))
    {
        tokens.pop_front();
        Expression* tree = parser.buildTree(tokens.join(""));
        if(tree == nullptr)
        {
            err_print("Invalid Expression!");
            return "";
        }
        else
        {
            try {
                tree->getValue(symbol);
            }  catch (QString e) {
                err_print(e);
            }
        }
        parser.clear();
    }
    else if(firsttoken == QString("PRINT"))
    {
        tokens.pop_front();
        Expression* tree = parser.buildTree(tokens.join(""));
        if(tree == nullptr)
        {
            err_print("Invalid Expression!");
            return "";
        }
        else
        {
            try {
                return QString::number(tree->getValue(symbol))+"\n";
            }  catch (QString e) {
                err_print(e);
            }
        }
        parser.clear();
    }
    else if(firsttoken == QString("INPUT")){
        tokens.pop_back();
        int var = QInputDialog::getInt(NULL,"INPUT","Input "+tokens.value(0),0,-2147483647,2147483647,1);
        symbol.addVar(tokens.value(0),var);
    }
    else if(firsttoken==QString("RUN"))
    {
        MainWindow::state = S_RUN;
    }
    else if(firsttoken==QString("LOAD"))
    {
        MainWindow::state = S_LOAD;
    }
    else if(firsttoken==QString("CLEAR"))
    {
        MainWindow::state = S_CLEAR;
    }
    else if(firsttoken==QString("HELP"))
    {
        MainWindow::state = S_HELP;
    }
    else if(firsttoken==QString("QUIT"))
    {
        MainWindow::state =S_QUIT;
    }
    else
    {
        err_print("Invalid command :"+firsttoken);
        //err
    }
    return "";
}
//store the code in tokens,implement the erase by typing line number
bool program::stringProcess(QString &str)
{
    str=str.simplified();
    QStringList tokens=str.split(' ');
    QString slinenum = tokens.front();
    int linenum=slinenum.toInt();
    if(LEGAL_LINENUM(linenum))
    {
        //within line number
        tokens.pop_front();
        // whether delete the line
        if(tokens.empty())
            map.remove(linenum);
        else
            map.insert(linenum,tokens);
        return true;
    }
    else return false;//without line number
}
void program::clear()
{
    symbol.clear();
    map.clear();
}
