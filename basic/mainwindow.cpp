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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearWindow()
{
    ui->codeBrowser->clear();
    ui->resultBrowser->clear();
    ui->treeBroswer->clear();
    pro.clear();
}
void MainWindow::codeUpdate()
{
    QString str=ui->cmd->text();
    QString tmp = pro.stringProcess(str);
    if(tmp == "WITHLINE")
    {
        //add code to window if within line number
        ui->codeBrowser->clear();
        ui->codeBrowser->append(pro.giveAllCode());
    }
    else
    {
        //add result to window if without line number
        ui->resultBrowser->append(tmp);
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

void MainWindow::resultUpdate()
{
    //get result and code Tree divided by ','
    QString getS=pro.runProgram();
    QStringList result=getS.split(',');
    ui->resultBrowser->setText(result.value(0));
    ui->treeBroswer->setText(result.value(1));
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
