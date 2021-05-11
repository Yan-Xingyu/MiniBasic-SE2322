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

    connect(ui->codeBrowser,SIGNAL(textChanged()),this,SLOT(on_window_textChanged()));
    connect(ui->treeBrowser,SIGNAL(textChanged()),this,SLOT(on_window_textChanged()));
    connect(ui->resultBrowser,SIGNAL(textChanged()),this,SLOT(on_window_textChanged()));
    connect(ui->variableBrowser,SIGNAL(textChanged()),this,SLOT(on_window_textChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearWindow()
{
    ui->codeBrowser->clear();
    ui->resultBrowser->clear();
    ui->treeBrowser->clear();
    pro.clear();
}
void MainWindow::codeUpdate()
{
    QString str=ui->cmd->text();
    if(pro.stringProcess(str))
    {
        //add code to window if within line number
        ui->codeBrowser->clear();
        ui->codeBrowser->append(pro.giveAllCode());
    }
    else
    {
        //add result to window if without line number
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
     h.format.setProperty(QTextFormat::FullWidthSelection, true);
     h.format.setBackground(color[line.second]);
     extras.append(h);
    }
    // 应用高亮效果
    code->setExtraSelections(extras);
}
void MainWindow::resultUpdate()
{
    //get result and tree and variables
    QString Tree=pro.generateTree();
    QString Result=pro.runProgram();
    QString Variables=pro.getVariables();
    //highlighter
    QList<int> errlines = pro.getErrorLines();
    QList<QPair<int,int>> highlights;
    for(int errline : errlines)
        highlights.append(QPair<int,int>(errline,RED));
    highLighter(highlights);
    //update the window
    ui->variableBrowser->setText(Variables);
    ui->resultBrowser->setText(Result);
    ui->treeBrowser->setText(Tree);
}

void MainWindow::loadProgram()
{
        QString fileName = QFileDialog::getOpenFileName(
                    this, tr("打开命令文件"),
                    "", tr("Command files(*.txt);;All files (*.*)"));
        // check empty file
        if(fileName.isEmpty())
        {
            QMessageBox mesg;
            mesg.warning(this,"WARNING","Empty File!");
            return;
        }
        else
        {
            //read lines
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
        //update the window
        ui->codeBrowser->append(pro.giveAllCode());
}
void MainWindow::debugHandler()
{
    static int linenum = -1;
    static QList<QPair<int,int>> highlights;
    static QString tree;
    if(pro.map.isEmpty())
    {
        QMessageBox::warning(NULL,"ERROR!","No code here.");
        return;
    }
    if(linenum == -1)//每次刚进入debug模式
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
        QString result,codetree,var;
        //获取信息
        result=pro.runSingleCommand(pro.debugInfo.debugline->value().join(' ').simplified());
        codetree=tree.split(',').value(linenum);
        var=pro.getVariables();

        linenum++;
        //更新窗口
        ui->resultBrowser->append(result);
        ui->treeBrowser->setText(codetree);
        ui->variableBrowser->setText(var);
    }
    //高亮
    highlights.append(QPair<int,int>(linenum,GREEN));
    highLighter(highlights);
    highlights.pop_back();
    //正常退出或出现error
    if(*pro.debugInfo.debugline == pro.map.cend()||pro.debugInfo.errflag)
    {
        linenum = -1;
        tree="";
        highlights.clear();
        highLighter(highlights);
        delete pro.debugInfo.debugline;
        pro.debugInfo.ondebug = false;
        pro.debugInfo.errflag = false;
        if(pro.debugInfo.errflag)
            QMessageBox::warning(NULL,"ERROR!","Occurs a wrong statement.");
        else
            QMessageBox::information(NULL,"Good!","Your program exited normally.");
        ui->loadButton->setEnabled(true);
        ui->clearButton->setEnabled(true);
        return;
    }
    if(linenum == -1)
        linenum = 0;
    else
        (*pro.debugInfo.debugline)++;
}
void MainWindow::on_window_textChanged()
{
    ui->codeBrowser->moveCursor(QTextCursor::End);
    ui->treeBrowser->moveCursor(QTextCursor::End);
    ui->resultBrowser->moveCursor(QTextCursor::End);
    ui->variableBrowser->moveCursor(QTextCursor::End);
}
