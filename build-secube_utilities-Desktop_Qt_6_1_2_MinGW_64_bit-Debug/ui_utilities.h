/********************************************************************************
** Form generated from reading UI file 'utilities.ui'
**
** Created by: Qt User Interface Compiler version 6.1.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UTILITIES_H
#define UI_UTILITIES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Utilities
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *browseButton;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *exitButton;
    QLineEdit *lineEdit_3;
    QLabel *label_3;
    QPushButton *deviceListButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Utilities)
    {
        if (Utilities->objectName().isEmpty())
            Utilities->setObjectName(QString::fromUtf8("Utilities"));
        Utilities->resize(800, 600);
        centralwidget = new QWidget(Utilities);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(50, 10, 341, 20));
        browseButton = new QPushButton(centralwidget);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setGeometry(QRect(320, 40, 75, 23));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 120, 47, 16));
        lineEdit_2 = new QLineEdit(centralwidget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(50, 120, 113, 20));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(240, 230, 75, 23));
        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(320, 230, 75, 23));
        exitButton = new QPushButton(centralwidget);
        exitButton->setObjectName(QString::fromUtf8("exitButton"));
        exitButton->setGeometry(QRect(20, 230, 75, 23));
        lineEdit_3 = new QLineEdit(centralwidget);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(110, 80, 113, 20));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 80, 81, 16));
        deviceListButton = new QPushButton(centralwidget);
        deviceListButton->setObjectName(QString::fromUtf8("deviceListButton"));
        deviceListButton->setGeometry(QRect(230, 80, 101, 23));
        Utilities->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Utilities);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        Utilities->setMenuBar(menubar);
        statusbar = new QStatusBar(Utilities);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        Utilities->setStatusBar(statusbar);

        retranslateUi(Utilities);

        QMetaObject::connectSlotsByName(Utilities);
    } // setupUi

    void retranslateUi(QMainWindow *Utilities)
    {
        Utilities->setWindowTitle(QCoreApplication::translate("Utilities", "Utilities", nullptr));
        label->setText(QCoreApplication::translate("Utilities", "File:", nullptr));
        browseButton->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        label_2->setText(QCoreApplication::translate("Utilities", "PIN:", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Utilities", "Encrypt...", nullptr));
        pushButton_3->setText(QCoreApplication::translate("Utilities", "Decrypt", nullptr));
        exitButton->setText(QCoreApplication::translate("Utilities", "Exit", nullptr));
        label_3->setText(QCoreApplication::translate("Utilities", "Device Number:", nullptr));
        deviceListButton->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Utilities: public Ui_Utilities {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UTILITIES_H
