#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QString>

#include "program.h"
#include "evalstate.h"
state_e MainWindow::state = S_NORM;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->cmd,SIGNAL(returnPressed()),this,SLOT(codeUpdate()));
    connect(ui->loadButton,SIGNAL(clicked()),this,SLOT(loadProgram()));
    connect(ui->runButton,SIGNAL(clicked()),this,SLOT(resultUpdate()));
    connect(ui->clearButton,SIGNAL(clicked()),this,SLOT(clearWindow()));
    connect(ui->debugButton,SIGNAL(clicked()),this,SLOT(debugHandler()));

    connect(ui->codeBrowser,SIGNAL(textChanged()),this,SLOT(allWindowFocus()));
    connect(ui->treeBrowser,SIGNAL(textChanged()),this,SLOT(allWindowFocus()));
    connect(ui->resultBrowser,SIGNAL(textChanged()),this,SLOT(allWindowFocus()));
    connect(ui->variableBrowser,SIGNAL(textChanged()),this,SLOT(allWindowFocus()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
/* 处理按下清除按钮的事件
 * 将所有窗口以及程序相关状态清除
 */
void MainWindow::clearWindow()
{
    ui->codeBrowser->clear();
    ui->resultBrowser->clear();
    ui->treeBrowser->clear();
    ui->variableBrowser->clear();
    pro.clear();
}
/* 处理输入框的提交事件
 * 将输入命令进行分析处理
 */
void MainWindow::codeUpdate()
{
    QString str=ui->cmd->text();
    if(pro.stringProcess(str))
    {
        //有行号的添加到代码框
        ui->codeBrowser->clear();
        ui->codeBrowser->append(pro.getAllCode());
    }
    else
    {
        //没有行号的直接执行并显示结果
        ui->resultBrowser->append(pro.runSingleCommand(str));
        switch(state)
        {
            case S_LOAD:loadProgram();
                break;
            case S_RUN:resultUpdate();
                break;
            case S_CLEAR:clearWindow();
                break;
            case S_HELP:
            {
                QMessageBox::information(NULL,"HELP","It's a simple Basic interpreter,\
which support INPUT,PRINT,LET,GOTO,IF THEN commands.You can see \
syntax tree of each command in the middle window,and source code in \
the left window,result in the right window.");
                break;
            }
            case S_QUIT:exit(0);
                break;
            case S_NORM:
            default:break;
        }
        state = S_NORM;
    }

    ui->cmd->clear();
}
/* 处理运行（RUN）按钮按下的事件
 * 获取运行信息并更新各个窗口
 */
void MainWindow::resultUpdate()
{
    //debug模式下 运行到结束
    while(pro.debugInfo.ondebug)
        this->debugHandler();
    //获取语法树、运行结果以及变量
    QString Tree=pro.generateTree();
    QString Result=pro.runProgram();
    QString Variables=pro.getVariables();
    //配置高亮
    QList<int> errlines = pro.getErrorLines();
    QList<QPair<int,int>> highlights;
    for(int errline : errlines)
        highlights.append(QPair<int,int>(errline,RED));
    highLighter(highlights);
    //更新窗口
    ui->variableBrowser->setText(Variables);
    ui->resultBrowser->setText(Result);
    ui->treeBrowser->setText(Tree);
}
/* 处理加载（load）按钮的点击事件
 * 读取文件内容并显示在代码窗
 */
void MainWindow::loadProgram()
{
        QString fileName = QFileDialog::getOpenFileName(
                    this, tr("打开命令文件"),
                    "", tr("Command files(*.txt);;All files (*.*)"));
        //检查是否为空
        if(fileName.isEmpty())
        {
            QMessageBox mesg;
            mesg.warning(this,"WARNING","Empty File!");
            return;
        }
        else
        {
            //读取内容
            clearWindow();
            QFile  file(fileName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return;
            QTextStream out(&file);
             QString str=out.readLine();
            while(str!="") {
                pro.stringProcess(str);
                str = out.readLine();
            }

        }
        //更新窗口
        ui->codeBrowser->setText(pro.getAllCode());
}
/* 响应debug按钮的点击事件
 * 读取存储在program对象中的debug信息结构体debugInfo来进行调试
 */
void MainWindow::debugHandler()
{
    static QList<QPair<int,int>> highlights;
    static QString tree,result;
    if(pro.map.isEmpty())
    {
        QMessageBox::warning(NULL,"ERROR!","No code here.");
        return;
    }
    if(pro.debugInfo.lineorder == -1)//每次刚进入debug模式
    {
        //清理之前的除代码外状态
        pro.symbol.clear();
        pro.errlines.clear();
        ui->resultBrowser->clear();
        ui->treeBrowser->clear();
        ui->variableBrowser->clear();
        //关闭两按钮
        ui->loadButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        //debug信息初始化
        pro.debugInfo.debugline = new QMap<int,QStringList>::ConstIterator(pro.map.cbegin());
        pro.debugInfo.ondebug = true;
        //获取所有代码树，以及错误行
        tree=pro.generateTree(',');
        QList<int> errlines = pro.getErrorLines();
        for(int errline : errlines)
            highlights.append(QPair<int,int>(errline,RED));
    }
    else//debug模式运行中
    {
        QString codetree,var;
        //获取信息
        codetree=tree.split(',').value(pro.debugInfo.lineorder);
        result+=pro.runSingleCommand(pro.debugInfo.debugline->value().join(' ').simplified());
        var=pro.getVariables();

        pro.debugInfo.lineorder++;
        //更新窗口
        ui->resultBrowser->setText(result);
        ui->treeBrowser->setText(codetree);
        ui->variableBrowser->setText(var);
    }
    //高亮
    highlights.append(QPair<int,int>(pro.debugInfo.lineorder,GREEN));
    highLighter(highlights);
    highlights.pop_back();
    //更新下一行
    if(pro.debugInfo.lineorder == -1)
        pro.debugInfo.lineorder = 0;
    else
        (*pro.debugInfo.debugline)++;
    //正常退出或出现error
    if(*pro.debugInfo.debugline == pro.map.cend()||pro.debugInfo.errflag)
    {
        delete pro.debugInfo.debugline;
        if(pro.debugInfo.errflag)
            QMessageBox::warning(NULL,"ERROR!","Occurs a wrong statement.");
        else
            QMessageBox::information(NULL,"Good!","Your program exited normally.");
        pro.debugInfo.lineorder = -1;
        tree="";
        result="";
        highlights.clear();
        highLighter(highlights);
        pro.debugInfo.ondebug = false;
        pro.debugInfo.errflag = false;
        ui->loadButton->setEnabled(true);
        ui->clearButton->setEnabled(true);
        return;
    }
}
/* 根据行号和颜色应用高亮
 */
void MainWindow::highLighter(QList<QPair<int, int>> highlights)
{
    QColor color[2]={QColor(255, 100, 100),QColor(100,255,100)};
    QTextBrowser *code = ui->codeBrowser;
    QTextCursor cursor(code->document());
    // 用于维护的所有高亮的链表
    QList<QTextEdit::ExtraSelection> extras;
    // 配置高亮，并加入到 extras 中
    for (auto &line : highlights) {
     QTextEdit::ExtraSelection h;
     h.cursor = cursor;
     for(int i=0;i<line.first;i++)
         h.cursor.movePosition(QTextCursor::Down);
     if(line.second == GREEN)
         ui->codeBrowser->setTextCursor(h.cursor);
     h.format.setProperty(QTextFormat::FullWidthSelection, true);
     h.format.setBackground(color[line.second]);
     extras.append(h);
    }
    // 应用高亮效果
    code->setExtraSelections(extras);
}
/* 处理窗口内容更改的事件
 * 将每个窗口的焦点放在内容末尾
 */
void MainWindow::allWindowFocus()
{
    ui->treeBrowser->moveCursor(QTextCursor::End);
    ui->resultBrowser->moveCursor(QTextCursor::End);
    ui->variableBrowser->moveCursor(QTextCursor::End);
}
