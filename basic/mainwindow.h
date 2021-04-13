#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "program.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
enum state_e{S_NORM,S_LOAD,S_RUN,S_CLEAR,S_HELP,S_QUIT};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static state_e state;
private:
    Ui::MainWindow *ui;
    program pro;
    QMap<int,QString> map;
private slots:
        void codeUpdate();
        void resultUpdate();
        void clearWindow();
        void loadProgram();
};
#endif // MAINWINDOW_H
