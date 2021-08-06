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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Utilities
{
public:
    QScrollArea *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab_1;
    QPushButton *listkeys_button;
    QLabel *label_3;
    QPushButton *browseButton;
    QPushButton *deviceListButton;
    QLineEdit *user_line;
    QLabel *label_5;
    QLineEdit *device_line;
    QLineEdit *file_line;
    QLabel *label_6;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QTextEdit *textEdit;
    QLineEdit *pin_line;
    QLabel *label;
    QLineEdit *key_line;
    QPushButton *encrypt_button;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QLabel *label_4;
    QLineEdit *group_line;
    QLabel *label_2;
    QLabel *label_7;
    QComboBox *comboBox;
    QProgressBar *progressBar_3;
    QWidget *tab_2;
    QLabel *label_9;
    QLineEdit *lineEdit;
    QPushButton *browseButton_2;
    QLabel *label_10;
    QLineEdit *lineEdit_2;
    QLabel *label_11;
    QLineEdit *lineEdit_3;
    QScrollArea *scrollArea_3;
    QWidget *scrollAreaWidgetContents_3;
    QPushButton *deviceListButton_2;
    QCheckBox *checkBox;
    QPushButton *decrypt_button;
    QProgressBar *progressBar_2;
    QWidget *tab_3;
    QLabel *label_12;
    QLineEdit *lineEdit_4;
    QPushButton *browseButton_3;
    QLabel *label_13;
    QLineEdit *lineEdit_5;
    QLabel *label_14;
    QLineEdit *lineEdit_6;
    QScrollArea *scrollArea_4;
    QWidget *scrollAreaWidgetContents_4;
    QPushButton *deviceListButton_3;
    QLabel *label_15;
    QLineEdit *lineEdit_7;
    QLabel *label_16;
    QLineEdit *lineEdit_8;
    QLabel *label_17;
    QLineEdit *lineEdit_9;
    QLabel *label_18;
    QComboBox *comboBox_2;
    QScrollArea *scrollArea_5;
    QWidget *scrollAreaWidgetContents_5;
    QPushButton *listkeys_button_2;
    QPushButton *digest_button;
    QProgressBar *progressBar;
    QWidget *tab_4;
    QLabel *label_8;
    QLineEdit *lineEdit_10;
    QPushButton *browseButton_4;
    QLabel *label_19;
    QLineEdit *lineEdit_11;
    QLabel *label_20;
    QLineEdit *lineEdit_12;
    QScrollArea *scrollArea_6;
    QWidget *scrollAreaWidgetContents_6;
    QPushButton *deviceListButton_4;
    QProgressBar *progressBar_4;
    QPushButton *updatePath_button;

    void setupUi(QMainWindow *Utilities)
    {
        if (Utilities->objectName().isEmpty())
            Utilities->setObjectName(QString::fromUtf8("Utilities"));
        Utilities->resize(731, 507);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Utilities->sizePolicy().hasHeightForWidth());
        Utilities->setSizePolicy(sizePolicy);
        Utilities->setMaximumSize(QSize(731, 507));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Logo/logo (1)/logo_small_icon_only.png"), QSize(), QIcon::Normal, QIcon::Off);
        Utilities->setWindowIcon(icon);
        Utilities->setAutoFillBackground(false);
        Utilities->setDocumentMode(false);
        Utilities->setTabShape(QTabWidget::Rounded);
        centralwidget = new QScrollArea(Utilities);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setMaximumSize(QSize(731, 507));
        tabWidget = new QTabWidget();
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 721, 500));
        tabWidget->setAutoFillBackground(true);
        tab_1 = new QWidget();
        tab_1->setObjectName(QString::fromUtf8("tab_1"));
        listkeys_button = new QPushButton(tab_1);
        listkeys_button->setObjectName(QString::fromUtf8("listkeys_button"));
        listkeys_button->setGeometry(QRect(620, 410, 80, 25));
        label_3 = new QLabel(tab_1);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 80, 61, 16));
        browseButton = new QPushButton(tab_1);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setGeometry(QRect(630, 40, 75, 23));
        deviceListButton = new QPushButton(tab_1);
        deviceListButton->setObjectName(QString::fromUtf8("deviceListButton"));
        deviceListButton->setGeometry(QRect(600, 210, 101, 23));
        user_line = new QLineEdit(tab_1);
        user_line->setObjectName(QString::fromUtf8("user_line"));
        user_line->setGeometry(QRect(60, 217, 201, 24));
        label_5 = new QLabel(tab_1);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 170, 55, 16));
        device_line = new QLineEdit(tab_1);
        device_line->setObjectName(QString::fromUtf8("device_line"));
        device_line->setGeometry(QRect(70, 80, 171, 21));
        file_line = new QLineEdit(tab_1);
        file_line->setObjectName(QString::fromUtf8("file_line"));
        file_line->setGeometry(QRect(40, 10, 671, 20));
        label_6 = new QLabel(tab_1);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 220, 55, 16));
        scrollArea = new QScrollArea(tab_1);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(270, 90, 431, 111));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 429, 109));
        textEdit = new QTextEdit(scrollAreaWidgetContents);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(0, 0, 431, 111));
        textEdit->setReadOnly(true);
        scrollArea->setWidget(scrollAreaWidgetContents);
        pin_line = new QLineEdit(tab_1);
        pin_line->setObjectName(QString::fromUtf8("pin_line"));
        pin_line->setGeometry(QRect(40, 120, 181, 21));
        pin_line->setEchoMode(QLineEdit::Password);
        label = new QLabel(tab_1);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 47, 21));
        key_line = new QLineEdit(tab_1);
        key_line->setObjectName(QString::fromUtf8("key_line"));
        key_line->setGeometry(QRect(40, 277, 151, 24));
        encrypt_button = new QPushButton(tab_1);
        encrypt_button->setObjectName(QString::fromUtf8("encrypt_button"));
        encrypt_button->setGeometry(QRect(0, 440, 711, 23));
        scrollArea_2 = new QScrollArea(tab_1);
        scrollArea_2->setObjectName(QString::fromUtf8("scrollArea_2"));
        scrollArea_2->setGeometry(QRect(270, 270, 431, 131));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 429, 129));
        scrollArea_2->setWidget(scrollAreaWidgetContents_2);
        label_4 = new QLabel(tab_1);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 280, 55, 16));
        group_line = new QLineEdit(tab_1);
        group_line->setObjectName(QString::fromUtf8("group_line"));
        group_line->setGeometry(QRect(57, 167, 113, 24));
        label_2 = new QLabel(tab_1);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 120, 47, 16));
        label_7 = new QLabel(tab_1);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 330, 61, 16));
        comboBox = new QComboBox(tab_1);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(80, 326, 171, 24));
        progressBar_3 = new QProgressBar(tab_1);
        progressBar_3->setObjectName(QString::fromUtf8("progressBar_3"));
        progressBar_3->setGeometry(QRect(10, 410, 118, 23));
        progressBar_3->setValue(0);
        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        label_9 = new QLabel(tab_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 10, 47, 21));
        lineEdit = new QLineEdit(tab_2);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(40, 10, 671, 20));
        browseButton_2 = new QPushButton(tab_2);
        browseButton_2->setObjectName(QString::fromUtf8("browseButton_2"));
        browseButton_2->setGeometry(QRect(630, 40, 75, 23));
        label_10 = new QLabel(tab_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(10, 80, 61, 16));
        lineEdit_2 = new QLineEdit(tab_2);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(70, 80, 171, 21));
        label_11 = new QLabel(tab_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 120, 47, 16));
        lineEdit_3 = new QLineEdit(tab_2);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(40, 120, 181, 21));
        lineEdit_3->setEchoMode(QLineEdit::Password);
        scrollArea_3 = new QScrollArea(tab_2);
        scrollArea_3->setObjectName(QString::fromUtf8("scrollArea_3"));
        scrollArea_3->setGeometry(QRect(270, 90, 431, 111));
        scrollArea_3->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_3"));
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 429, 109));
        scrollArea_3->setWidget(scrollAreaWidgetContents_3);
        deviceListButton_2 = new QPushButton(tab_2);
        deviceListButton_2->setObjectName(QString::fromUtf8("deviceListButton_2"));
        deviceListButton_2->setGeometry(QRect(600, 210, 101, 23));
        checkBox = new QCheckBox(tab_2);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(620, 400, 91, 22));
        checkBox->setMouseTracking(false);
        checkBox->setTristate(false);
        decrypt_button = new QPushButton(tab_2);
        decrypt_button->setObjectName(QString::fromUtf8("decrypt_button"));
        decrypt_button->setGeometry(QRect(0, 440, 711, 23));
        progressBar_2 = new QProgressBar(tab_2);
        progressBar_2->setObjectName(QString::fromUtf8("progressBar_2"));
        progressBar_2->setGeometry(QRect(10, 410, 118, 23));
        progressBar_2->setValue(0);
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        label_12 = new QLabel(tab_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(10, 10, 47, 21));
        lineEdit_4 = new QLineEdit(tab_3);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(40, 10, 671, 20));
        browseButton_3 = new QPushButton(tab_3);
        browseButton_3->setObjectName(QString::fromUtf8("browseButton_3"));
        browseButton_3->setGeometry(QRect(630, 40, 75, 23));
        label_13 = new QLabel(tab_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(10, 80, 61, 16));
        lineEdit_5 = new QLineEdit(tab_3);
        lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));
        lineEdit_5->setGeometry(QRect(70, 80, 171, 21));
        label_14 = new QLabel(tab_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(10, 120, 47, 16));
        lineEdit_6 = new QLineEdit(tab_3);
        lineEdit_6->setObjectName(QString::fromUtf8("lineEdit_6"));
        lineEdit_6->setGeometry(QRect(40, 120, 181, 21));
        lineEdit_6->setEchoMode(QLineEdit::Password);
        scrollArea_4 = new QScrollArea(tab_3);
        scrollArea_4->setObjectName(QString::fromUtf8("scrollArea_4"));
        scrollArea_4->setGeometry(QRect(270, 90, 431, 111));
        scrollArea_4->setWidgetResizable(true);
        scrollAreaWidgetContents_4 = new QWidget();
        scrollAreaWidgetContents_4->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_4"));
        scrollAreaWidgetContents_4->setGeometry(QRect(0, 0, 429, 109));
        scrollArea_4->setWidget(scrollAreaWidgetContents_4);
        deviceListButton_3 = new QPushButton(tab_3);
        deviceListButton_3->setObjectName(QString::fromUtf8("deviceListButton_3"));
        deviceListButton_3->setGeometry(QRect(600, 210, 101, 23));
        label_15 = new QLabel(tab_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(10, 170, 55, 16));
        lineEdit_7 = new QLineEdit(tab_3);
        lineEdit_7->setObjectName(QString::fromUtf8("lineEdit_7"));
        lineEdit_7->setGeometry(QRect(57, 167, 113, 24));
        label_16 = new QLabel(tab_3);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(10, 220, 55, 16));
        lineEdit_8 = new QLineEdit(tab_3);
        lineEdit_8->setObjectName(QString::fromUtf8("lineEdit_8"));
        lineEdit_8->setGeometry(QRect(60, 217, 201, 24));
        label_17 = new QLabel(tab_3);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(10, 280, 55, 16));
        lineEdit_9 = new QLineEdit(tab_3);
        lineEdit_9->setObjectName(QString::fromUtf8("lineEdit_9"));
        lineEdit_9->setGeometry(QRect(40, 277, 151, 24));
        label_18 = new QLabel(tab_3);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(10, 330, 61, 16));
        comboBox_2 = new QComboBox(tab_3);
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setGeometry(QRect(80, 326, 171, 24));
        scrollArea_5 = new QScrollArea(tab_3);
        scrollArea_5->setObjectName(QString::fromUtf8("scrollArea_5"));
        scrollArea_5->setGeometry(QRect(270, 270, 431, 131));
        scrollArea_5->setWidgetResizable(true);
        scrollAreaWidgetContents_5 = new QWidget();
        scrollAreaWidgetContents_5->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_5"));
        scrollAreaWidgetContents_5->setGeometry(QRect(0, 0, 429, 129));
        scrollArea_5->setWidget(scrollAreaWidgetContents_5);
        listkeys_button_2 = new QPushButton(tab_3);
        listkeys_button_2->setObjectName(QString::fromUtf8("listkeys_button_2"));
        listkeys_button_2->setGeometry(QRect(620, 410, 80, 25));
        digest_button = new QPushButton(tab_3);
        digest_button->setObjectName(QString::fromUtf8("digest_button"));
        digest_button->setGeometry(QRect(0, 440, 711, 23));
        progressBar = new QProgressBar(tab_3);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(10, 410, 118, 23));
        progressBar->setValue(0);
        progressBar->setTextVisible(true);
        progressBar->setOrientation(Qt::Horizontal);
        progressBar->setInvertedAppearance(false);
        progressBar->setTextDirection(QProgressBar::TopToBottom);
        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        label_8 = new QLabel(tab_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 10, 47, 21));
        lineEdit_10 = new QLineEdit(tab_4);
        lineEdit_10->setObjectName(QString::fromUtf8("lineEdit_10"));
        lineEdit_10->setGeometry(QRect(40, 10, 671, 20));
        browseButton_4 = new QPushButton(tab_4);
        browseButton_4->setObjectName(QString::fromUtf8("browseButton_4"));
        browseButton_4->setGeometry(QRect(630, 40, 75, 23));
        label_19 = new QLabel(tab_4);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(10, 80, 61, 16));
        lineEdit_11 = new QLineEdit(tab_4);
        lineEdit_11->setObjectName(QString::fromUtf8("lineEdit_11"));
        lineEdit_11->setGeometry(QRect(70, 80, 171, 21));
        label_20 = new QLabel(tab_4);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setGeometry(QRect(10, 120, 47, 16));
        lineEdit_12 = new QLineEdit(tab_4);
        lineEdit_12->setObjectName(QString::fromUtf8("lineEdit_12"));
        lineEdit_12->setGeometry(QRect(40, 120, 181, 21));
        lineEdit_12->setEchoMode(QLineEdit::Password);
        scrollArea_6 = new QScrollArea(tab_4);
        scrollArea_6->setObjectName(QString::fromUtf8("scrollArea_6"));
        scrollArea_6->setGeometry(QRect(270, 90, 431, 111));
        scrollArea_6->setWidgetResizable(true);
        scrollAreaWidgetContents_6 = new QWidget();
        scrollAreaWidgetContents_6->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_6"));
        scrollAreaWidgetContents_6->setGeometry(QRect(0, 0, 429, 109));
        scrollArea_6->setWidget(scrollAreaWidgetContents_6);
        deviceListButton_4 = new QPushButton(tab_4);
        deviceListButton_4->setObjectName(QString::fromUtf8("deviceListButton_4"));
        deviceListButton_4->setGeometry(QRect(600, 210, 101, 23));
        progressBar_4 = new QProgressBar(tab_4);
        progressBar_4->setObjectName(QString::fromUtf8("progressBar_4"));
        progressBar_4->setGeometry(QRect(10, 410, 118, 23));
        progressBar_4->setValue(0);
        updatePath_button = new QPushButton(tab_4);
        updatePath_button->setObjectName(QString::fromUtf8("updatePath_button"));
        updatePath_button->setGeometry(QRect(0, 440, 711, 23));
        tabWidget->addTab(tab_4, QString());
        centralwidget->setWidget(tabWidget);
        Utilities->setCentralWidget(centralwidget);

        retranslateUi(Utilities);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Utilities);
    } // setupUi

    void retranslateUi(QMainWindow *Utilities)
    {
        Utilities->setWindowTitle(QCoreApplication::translate("Utilities", "SECube Utilities", nullptr));
        listkeys_button->setText(QCoreApplication::translate("Utilities", "List keys...", nullptr));
        label_3->setText(QCoreApplication::translate("Utilities", "Device ID:", nullptr));
        browseButton->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        deviceListButton->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
#if QT_CONFIG(tooltip)
        user_line->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as U+the number of the user (e.g., U10). Put list of users between &quot; &quot;, each one separated by space.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("Utilities", "Group:", nullptr));
        label_6->setText(QCoreApplication::translate("Utilities", "User(s):", nullptr));
        label->setText(QCoreApplication::translate("Utilities", "File:", nullptr));
        encrypt_button->setText(QCoreApplication::translate("Utilities", "Encrypt", nullptr));
        label_4->setText(QCoreApplication::translate("Utilities", "Key:", nullptr));
#if QT_CONFIG(tooltip)
        group_line->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as G+the number of the group (e.g., G10).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("Utilities", "PIN:", nullptr));
        label_7->setText(QCoreApplication::translate("Utilities", "Algorithm:", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("Utilities", "AES_HMACSHA256", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("Utilities", "AES (not available)", nullptr));

        tabWidget->setTabText(tabWidget->indexOf(tab_1), QCoreApplication::translate("Utilities", "Encryption", nullptr));
        label_9->setText(QCoreApplication::translate("Utilities", "File:", nullptr));
        browseButton_2->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        label_10->setText(QCoreApplication::translate("Utilities", "Device ID:", nullptr));
        label_11->setText(QCoreApplication::translate("Utilities", "PIN:", nullptr));
        deviceListButton_2->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
#if QT_CONFIG(tooltip)
        checkBox->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>Checking it, SEKey will be used in order to retrieve the proper key. Otherwise, the infos in the file header will be used.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox->setText(QCoreApplication::translate("Utilities", "Use SEKey", nullptr));
        decrypt_button->setText(QCoreApplication::translate("Utilities", "Decrypt", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("Utilities", "Decryption", nullptr));
        label_12->setText(QCoreApplication::translate("Utilities", "File:", nullptr));
        browseButton_3->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        label_13->setText(QCoreApplication::translate("Utilities", "Device ID:", nullptr));
        label_14->setText(QCoreApplication::translate("Utilities", "PIN:", nullptr));
        deviceListButton_3->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
        label_15->setText(QCoreApplication::translate("Utilities", "Group:", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_7->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as G+the number of the group (e.g., G10).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_16->setText(QCoreApplication::translate("Utilities", "User(s):", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_8->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as U+the number of the user (e.g., U10). Put list of users between &quot; &quot;, each one separated by space.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_17->setText(QCoreApplication::translate("Utilities", "Key:", nullptr));
        label_18->setText(QCoreApplication::translate("Utilities", "Algorithm:", nullptr));
        comboBox_2->setItemText(0, QCoreApplication::translate("Utilities", "HMAC-SHA-256", nullptr));
        comboBox_2->setItemText(1, QCoreApplication::translate("Utilities", "SHA-256", nullptr));

        listkeys_button_2->setText(QCoreApplication::translate("Utilities", "List keys...", nullptr));
        digest_button->setText(QCoreApplication::translate("Utilities", "Compute Digest", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("Utilities", "Digest", nullptr));
        label_8->setText(QCoreApplication::translate("Utilities", "Path:", nullptr));
        browseButton_4->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        label_19->setText(QCoreApplication::translate("Utilities", "Device ID:", nullptr));
        label_20->setText(QCoreApplication::translate("Utilities", "PIN:", nullptr));
        deviceListButton_4->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
        updatePath_button->setText(QCoreApplication::translate("Utilities", "Update SEKey path", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("Utilities", "Update SEKey path", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Utilities: public Ui_Utilities {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UTILITIES_H
