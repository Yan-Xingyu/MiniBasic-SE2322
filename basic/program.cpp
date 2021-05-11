#include "program.h"
#include "evalstate.h"
#include "mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>
program::program()
{
    debugInfo.debugline = nullptr;
    debugInfo.errflag = false;
    debugInfo.ondebug = false;
}

program::~program()
{
    if(debugInfo.debugline != nullptr)
        delete debugInfo.debugline;

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
QString program::generateTree(QChar delim)
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
                codeTree+=QString::number(k.key())+" Error"+delim;
                errlines.append(line);
                continue;
            }
            codeTree+=QString::number(k.key())+" LET "+tree->tranverse(2)+delim;
            parser.clear();
        }
        else if(firsttoken == QString("PRINT"))
        {
            tokens.pop_front();
            Expression* tree = parser.buildTree(tokens.join(""));
            if(tree == nullptr)
            {
                codeTree+=QString::number(k.key())+" Error"+delim;
                errlines.append(line);
                continue;
            }
            codeTree+=QString::number(k.key())+" PRINT\n        "+tree->tranverse(2)+delim;
            parser.clear();
        }
        else if(firsttoken == QString("PRINTF"))
        {
            tokens.pop_front();
            QStringList params =tokens.join(' ').split(',');
            QString first = params[0];
            params.pop_front();
            codeTree+=QString::number(k.key())+"PRINTF "+first+"\n"+"\t"+params.join(" ");
        }
        else if(firsttoken == QString("IF"))
        {
            tokens.pop_front();
            tokens=tokens.join("").split("THEN",Qt::SkipEmptyParts);
            Expression* condtree = parser.buildTree(tokens.value(0));
            if(condtree == nullptr)
            {
                codeTree+=QString::number(k.key())+" Error"+delim;
                errlines.append(line);
                continue;
            }
            codeTree+=QString::number(k.key())+" IF THEN\n"+
                    condtree->tranverse(2)+"\n          "+tokens.value(1)+delim;
            parser.clear();
        }
        else if(firsttoken == QString("GOTO"))
        {
            tokens.pop_front();
            int linenum = tokens.join("").simplified().toInt();
            if(LEGAL_LINENUM(linenum))
                codeTree+=QString::number(k.key())+" GOTO\n        "+QString::number(linenum)+delim;
            else
            {
                codeTree+=QString::number(k.key())+" Error"+delim;
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
                codeTree+=QString::number(k.key())+" INPUT\n        "+tokens.value(0)+delim;
            else
            {
                codeTree+=QString::number(k.key())+" Error"+delim;
                errlines.append(line);
            }
        }
        else if(firsttoken ==QString("INPUTS"))
        {
            tokens.pop_front();
            if(tokens.length() == 1)
                codeTree+=QString::number(k.key())+" INPUTS\n        "+tokens.value(0)+delim;
            else
            {
                codeTree+=QString::number(k.key())+" Error"+delim;
                errlines.append(line);
            }
        }
        else if(firsttoken == QString("REM"))
        {
            tokens.pop_front();
            codeTree+= QString::number(k.key())+" REM\n        "+tokens.join(' ')+delim;
        }
        else if(firsttoken == QString("LIST"))
        {
            continue;
        }
        else
        {
            codeTree+=QString::number(k.key())+" Error"+delim;
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

//return err lines
QList<int> program::getErrorLines() const
{
    return errlines;
}
//get all variables
QString program::getVariables() const
{
    return symbol.showAllVariables();
}
//print error information
void program::err_print(QString str)
{
    if(debugInfo.ondebug)
    {
        debugInfo.errflag = true;
    }
    else
    {
        QMessageBox mesg;
        mesg.warning(NULL,"ERROR",str);
    }
}
#define IS_QUOTE(s) (s=='\''||s=='\"')
QString program::printfHandler(QStringList tokens)
{
    tokens.pop_front();
    QStringList params=tokens.join(" ").split(',');
    QString output = params.value(0).replace(QString("\'"),QString("")).replace(QString("\""),QString(""));
    params.pop_front();
    int len=params.length();
    for(int i=0;i<len;i++)
    {
        if(IS_QUOTE(params[i][0])&&IS_QUOTE(params[i][params[i].length()-1]))
        {
            params[i]=params[i].replace('\'',"");
            params[i]=params[i].replace('\"',"");
        }

        else if(symbol.contains(params[i]))
        {
            QVariant tmp = symbol.getvalue(params[i]);
            if(strcmp(tmp.typeName(),"int"))
                params[i] = tmp.toString();
            else
                params[i] = QString::number(tmp.toInt());
        }
       else
           throw "invalid parameter"+params[i];
    }
    QStringList tmp = output.split('}');
    for(int i=0;i<tmp.length();i++)
    {
        if(tmp[i].count('{')!=1&&tmp[i]!="")
            throw "Invalid format!";
        if(tmp[i]!="")
            tmp[i]=tmp[i].replace(QString("{"),params.value(i));
    }
    return tmp.join("");
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
                    QVariant tmp= tree->getValue(symbol);
                    if(!strcmp(tmp.typeName(),"QString"))
                        result+=tmp.toString()+"\n";
                    else
                        result += QString::number(tmp.toInt())+"\n";
                }  catch (QString e) {
                    err_print(QString::number(k.key())+" "+e);
                }
            }

            parser.clear();
        }
        else if(firsttoken == QString("PRINTF"))
        {
            try {
                result+=printfHandler(tokens)+"\n";
            }  catch (QString e) {
                err_print(QString::number(k.key())+" "+e);
            }
            catch(char const* e )
            {
                err_print(QString::number(k.key())+" "+e);
            }

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
                    QVariant tmp= condtree->getValue(symbol);
                    if(!strcmp(tmp.typeName(),"QString"))
                        err_print("invalid int number");
                    cond = tmp.toInt();
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
            bool ok;
            int var = QInputDialog::getInt(NULL,"INPUT","Input "+tokens.value(0),0,-2147483647,2147483647,1,&ok);
            if(ok)
                symbol.addVar(tokens.value(0),var);
            else
                err_print(QString::number(k.key())+"No Valid Input.");
        }
        else if(firsttoken == QString("INPUTS"))
        {
            tokens.pop_back();
            bool ok;
            QString var = QInputDialog::getText(NULL,"INPUT","Input "+tokens.value(0),QLineEdit::Normal,
                                                NULL,&ok);
            if(ok)
                symbol.addVar(tokens.value(0),var);
            else
                err_print(QString::number(k.key())+"No Valid Input.");
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
                QVariant tmp= tree->getValue(symbol);
                if(!strcmp(tmp.typeName(),"QString"))
                   return tmp.toString()+"\n";
                else
                   return QString::number(tmp.toInt())+"\n";
            }  catch (QString e) {
                err_print(e);
            }
        }
        parser.clear();
    }
    else if(firsttoken == QString("PRINTF"))
    {
        try {
            return printfHandler(tokens)+"\n";
        }  catch (QString e) {
            err_print(e);
        }
        catch(char const* e )
        {
            err_print(e);
        }
    }
    else if(firsttoken == QString("INPUT")){
        tokens.pop_back();
        bool ok;
        int var = QInputDialog::getInt(NULL,"INPUT","Input "+tokens.value(0),0,-2147483647,2147483647,1,&ok);
        if(ok)
            symbol.addVar(tokens.value(0),var);
        else
            err_print("No Valid Input.");
    }
    else if(firsttoken == QString("INPUTS"))
    {
        tokens.pop_back();
        bool ok;
        QString var = QInputDialog::getText(NULL,"INPUT","Input "+tokens.value(0),QLineEdit::Normal,
                                            NULL,&ok);
        if(ok)
            symbol.addVar(tokens.value(0),var);
        else
            err_print("No Valid Input.");
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
