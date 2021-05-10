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
    //get result and tree
    QString Tree=pro.generateTree();
    QString Result=pro.runProgram();
    //highlighter
    QList<int> errlines = pro.getErrorLines();
    QList<QPair<int,int>> highlights;
    for(int errline : errlines)
        highlights.append(QPair<int,int>(errline,GREEN));
    highLighter(highlights);
    //update the window
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
void MainWindow::on_window_textChanged()
{
    ui->codeBrowser->moveCursor(QTextCursor::End);
    ui->treeBrowser->moveCursor(QTextCursor::End);
    ui->resultBrowser->moveCursor(QTextCursor::End);
    ui->variableBrowser->moveCursor(QTextCursor::End);
}
