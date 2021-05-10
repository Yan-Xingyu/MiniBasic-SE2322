/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.0.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *file_save;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout;
    QTextBrowser *codeBrowser;
    QTextBrowser *resultBrowser;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_5;
    QTextBrowser *treeBrowser;
    QTextBrowser *variableBrowser;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *loadButton;
    QPushButton *runButton;
    QPushButton *debugButton;
    QPushButton *clearButton;
    QLabel *label;
    QLineEdit *cmd;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 601);
        file_save = new QAction(MainWindow);
        file_save->setObjectName(QString::fromUtf8("file_save"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QFont font;
        font.setPointSize(10);
        label_3->setFont(font);

        horizontalLayout_2->addWidget(label_3);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font);

        horizontalLayout_2->addWidget(label_4);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        codeBrowser = new QTextBrowser(centralwidget);
        codeBrowser->setObjectName(QString::fromUtf8("codeBrowser"));

        horizontalLayout->addWidget(codeBrowser);

        resultBrowser = new QTextBrowser(centralwidget);
        resultBrowser->setObjectName(QString::fromUtf8("resultBrowser"));

        horizontalLayout->addWidget(resultBrowser);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        horizontalLayout_4->addWidget(label_2);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font);

        horizontalLayout_4->addWidget(label_5);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        treeBrowser = new QTextBrowser(centralwidget);
        treeBrowser->setObjectName(QString::fromUtf8("treeBrowser"));

        horizontalLayout_5->addWidget(treeBrowser);

        variableBrowser = new QTextBrowser(centralwidget);
        variableBrowser->setObjectName(QString::fromUtf8("variableBrowser"));

        horizontalLayout_5->addWidget(variableBrowser);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        loadButton = new QPushButton(centralwidget);
        loadButton->setObjectName(QString::fromUtf8("loadButton"));
        loadButton->setMinimumSize(QSize(0, 30));

        horizontalLayout_3->addWidget(loadButton);

        runButton = new QPushButton(centralwidget);
        runButton->setObjectName(QString::fromUtf8("runButton"));
        runButton->setMinimumSize(QSize(0, 30));

        horizontalLayout_3->addWidget(runButton);

        debugButton = new QPushButton(centralwidget);
        debugButton->setObjectName(QString::fromUtf8("debugButton"));
        debugButton->setMinimumSize(QSize(0, 30));

        horizontalLayout_3->addWidget(debugButton);

        clearButton = new QPushButton(centralwidget);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));
        clearButton->setMinimumSize(QSize(0, 30));

        horizontalLayout_3->addWidget(clearButton);


        verticalLayout->addLayout(horizontalLayout_3);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        cmd = new QLineEdit(centralwidget);
        cmd->setObjectName(QString::fromUtf8("cmd"));
        cmd->setMinimumSize(QSize(0, 35));

        verticalLayout->addWidget(cmd);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        file_save->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
#if QT_CONFIG(tooltip)
        file_save->setToolTip(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\226\207\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
        label_3->setText(QCoreApplication::translate("MainWindow", "\344\273\243\347\240\201", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\350\277\220\350\241\214\347\273\223\346\236\234", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\350\257\255\345\217\245\344\270\216\350\257\255\346\263\225\346\240\221", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\345\217\230\351\207\217", nullptr));
        loadButton->setText(QCoreApplication::translate("MainWindow", "\350\275\275\345\205\245\344\273\243\347\240\201", nullptr));
        runButton->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214\344\273\243\347\240\201", nullptr));
        debugButton->setText(QCoreApplication::translate("MainWindow", "\350\260\203\350\257\225\345\215\225\346\255\245", nullptr));
        clearButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\344\273\243\347\240\201", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\221\275\344\273\244\350\276\223\345\205\245\347\252\227\345\217\243", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
