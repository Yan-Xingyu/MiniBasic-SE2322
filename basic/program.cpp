#include "program.h"
#include "evalstate.h"
#include "mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>

#define LEGAL_LINENUM(n)  ((n>0)&&(n<=1000000))
#define IS_QUOTE(s) (s=='\''||s=='\"')
program::program()
{
    debugInfo.debugline = nullptr;
    debugInfo.errflag = false;
    debugInfo.ondebug = false;
    debugInfo.lineorder = -1;
}

program::~program()
{
    if(debugInfo.debugline != nullptr)
        delete debugInfo.debugline;

}
//获取所有命令（带行号）
QString program::getAllCode() const
{
    QString code="";
    for(auto k = map.cbegin();k!=map.cend();k++)
        code+=QString::number((k.key()))+" "+k.value().join(' ')+"\n";
    return code;
}
//根据代码分情况构建语法树
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
            Expression* tree = parser.buildTree(tokens.join(" "));
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
            codeTree+=QString::number(k.key())+" PRINTF "+first+"\n"+"\t"+params.join(" ")+delim;
        }
        else if(firsttoken == QString("IF"))
        {
            tokens.pop_front();
            tokens=tokens.join("").replace('=',"==").split("THEN",Qt::SkipEmptyParts);
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
//获取行号为n的迭代器，lineorder为第几行（可选）。
QMap<int,QStringList>::ConstIterator program::jmpN(int n,int* lineorder)
{
    int i=0;
    auto tmp = map.cbegin();
    for(;tmp!=map.cend();tmp++,i++)
    {
        if(tmp.key()== n)
        {
            tmp--;
            if(lineorder!=nullptr)
                *lineorder=i;
            return tmp;
        }
    }
    return tmp;
}
//获取一个有着第几行为错误行信息的数组
QList<int> program::getErrorLines() const
{
    return errlines;
}
//获取所有变量（整型和字符串）
QString program::getVariables() const
{
    return symbol.showAllVariables();
}
//打印错误信息或在debug模式下改变错误标识
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
//实现PRINTF语句的功能 返回打印的最终内容
QString program::printfHandler(QStringList tokens)
{
    tokens.pop_front();

    QStringList params=tokens.join(" ").split(',',Qt::SkipEmptyParts);
    QString output = params.value(0);
    //检查前后引号是否匹配 且是否有串中引号
    if(!IS_QUOTE(output[0])||!IS_QUOTE(output[output.length()-1])||
            output[0]!=output[output.length()-1]||output.count(output[0])!=2)
        throw QString("invalid format");
    output = output.left(output.length()-1);
    output = output.right(output.length()-1);
    //如果只有一个参数 则直接返回首参数
    params.pop_front();
    if(params.isEmpty())
    {
        return output;
    }
    int len=params.length();
    //处理所有参数，若字符串常量则去掉引号，如为变量则取出值变为字符串
    for(int i=0;i<len;i++)
    {
        params[i] = params[i].simplified();
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
           throw QString("invalid parameter"+params[i]);
    }
    //括号数量不同
    if(output.count('{')!=output.count('}'))
        throw QString("Invalid format!");
    QStringList tmp = output.split('}');
    for(int i=0;i<tmp.length();i++)
    {
        //错误信息 括号不匹配
        if(tmp[i].count('{')!=1&&tmp[i]!="")
            throw QString("Invalid format!");
        if(tmp[i]!="")
            tmp[i]=tmp[i].replace(QString("{"),params.value(i));
    }
    return tmp.join("");
}
//处理IF ... THEN ...语句 tokens为命令参数 lineorder为跳转到的命令为第几行
QMap<int,QStringList>::ConstIterator program::ifThenHandler(QStringList tokens,int* lineorder)
{
    tokens.pop_front();
    tokens=tokens.join("").replace('=',QString("==")).split("THEN",Qt::SkipEmptyParts);
    Expression* condtree = parser.buildTree(tokens.value(0));
    if(condtree != nullptr)
    {
        int cond;
        QVariant tmp= condtree->getValue(symbol);//计算条件的值
        if(!strcmp(tmp.typeName(),"QString"))
            throw QString("invalid int number");
        cond = tmp.toInt();
        int n = tokens.value(1).toInt();
        if(!LEGAL_LINENUM(n))
            throw " Invalid line number "+tokens.value(1);
        if(cond) {
            /*跳转到行号为n的行*/
            auto tmp =jmpN(n,lineorder);
            if(tmp==map.cend())
                throw": Can't find line number "+tokens.value(1);
            else return tmp;
        }
    }
    parser.clear();
    return map.cend();
}
//一次执行完所有命令 对于于正常情况下的运行按钮
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
            Expression* tree = parser.buildTree(tokens.join(" "));
            if(tree == nullptr)
            {
                err_print(QString::number(k.key())+" Invalid Expression in line "+QString::number(k.key()));
                return result;
            }
            else
            {
                try {
                    tree->getValue(symbol);
                }  catch (QString e) {
                    err_print(QString::number(k.key())+" "+e);
                    return result;
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
                err_print(QString::number(k.key())+" Invalid Expression in line "+QString::number(k.key()));
                return result;
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
                    return result;
                }
            }

            parser.clear();
        }
        else if(firsttoken == QString("PRINTF"))
        {
            try {
                result+=printfHandler(tokens)+"\n";
            }
            catch(QString e )
            {
                err_print(QString::number(k.key())+" "+e);
                return result;
            }

        }
        else if(firsttoken == QString("IF"))
        {
            QMap<int,QStringList>::ConstIterator tmp = map.cend();
            try {
                tmp=ifThenHandler(tokens);
            }  catch (QString e) {
                err_print(QString::number(k.key())+" "+e);
            }
            if(tmp != map.cend())
                k=tmp;
            continue;
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
                return result;
                //err
            }
            else k=tmp;
        }
        else if(firsttoken == QString("END"))
        {
            break;
        }
        else if(firsttoken == QString("INPUT")){
            tokens.pop_front();
            bool ok;
            int var = QInputDialog::getInt(NULL,"INPUT","Input "+tokens.value(0),0,-2147483647,2147483647,1,&ok);
            if(ok)
                symbol.addVar(tokens.value(0),var);
            else
            {
                err_print(QString::number(k.key())+" No Valid Input.");
                return result;
            }
        }
        else if(firsttoken == QString("INPUTS"))
        {
            tokens.pop_front();
            bool ok;
            QString var = QInputDialog::getText(NULL,"INPUT","Input "+tokens.value(0),QLineEdit::Normal,
                                                NULL,&ok);
            if(ok&&!var.contains('\'')&&!var.contains('\"'))
                symbol.addVar(tokens.value(0),var);
            else
            {
                err_print(QString::number(k.key())+" No Valid Input.");
                return result;
            }
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
            err_print(QString::number(k.key())+" Invalid command :"+firsttoken);
            return result;
        }
    }
    return result;
}

//运行单个命令 不带行号或debug模式下使用
QString program::runSingleCommand(QString str)
{
    if(str.isEmpty())return "";
    QStringList tokens=str.split(' ',Qt::SkipEmptyParts);
    QString firsttoken = tokens.front();
    if(firsttoken==QString("LET"))
    {
        tokens.pop_front();
        Expression* tree = parser.buildTree(tokens.join(" "));
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
    }
    else if(firsttoken == QString("INPUT")){
        tokens.pop_front();
        bool ok;
        int var = QInputDialog::getInt(NULL,"INPUT","Input "+tokens.value(0),0,-2147483647,2147483647,1,&ok);
        if(ok)
            symbol.addVar(tokens.value(0),var);
        else
            err_print("No Valid Input.");
    }
    else if(firsttoken == QString("INPUTS"))
    {
        tokens.pop_front();
        bool ok;
        QString var = QInputDialog::getText(NULL,"INPUT","Input "+tokens.value(0),QLineEdit::Normal,
                                            NULL,&ok);
        //确有输入且不含引号
        if(ok&&!var.contains('\'')&&!var.contains('\"'))
            symbol.addVar(tokens.value(0),var);
        else
            err_print("No Valid Input.");
    }
    else if(debugInfo.ondebug)
    {
        if(firsttoken == QString("REM")||firsttoken == QString("LIST")||firsttoken == QString("END"))
            return "";
        else if(firsttoken == QString("IF"))
        {
            QMap<int,QStringList>::ConstIterator tmp = map.cend();
            int lineorder=0;
            try {
                tmp = ifThenHandler(tokens,&lineorder);
            }  catch (QString e) {
                err_print(e);
            }
            if(tmp!=map.cend())
            {
                //更新下一行调试代码对应的迭代器
                debugInfo.lineorder=lineorder-1;
                delete debugInfo.debugline;
                debugInfo.debugline = new QMap<int,QStringList>::ConstIterator(tmp);
            }
        }
        else if(firsttoken == QString("GOTO"))
        {
            tokens.pop_front();
            int linenum = tokens.join("").simplified().toInt();
            int lineorder=0; //第几行
            /*获取要跳转行的迭代器*/
            auto tmp =jmpN(linenum,&lineorder);

            if(tmp==map.cend())
            {
                err_print("Can't find line number "+tokens.value(1));
                return "";
                //err
            }
            else
            {
                //更新下一行调试代码对应的迭代器
                debugInfo.lineorder=lineorder-1;
                delete debugInfo.debugline;
                debugInfo.debugline = new QMap<int,QStringList>::ConstIterator(tmp);
            }
        }
        else err_print("Invalid command :"+firsttoken);
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

//分割并存储命令 判断
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
