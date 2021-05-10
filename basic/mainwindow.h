#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "program.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
#define GREEN 0
#define RED 1
enum state_e{S_NORM,S_LOAD,S_RUN,S_CLEAR,S_HELP,S_DEBUG,S_QUIT};
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
    void highLighter(QList<QPair<int, int>>);
public slots:
        void codeUpdate();
        void resultUpdate();
        void clearWindow();
        void loadProgram();
        void on_window_textChanged();
};
#endif // MAINWINDOW_H
