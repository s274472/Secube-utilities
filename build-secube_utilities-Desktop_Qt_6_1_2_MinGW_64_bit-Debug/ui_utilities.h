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
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
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
    QLineEdit *pin_line;
    QLabel *label;
    QLineEdit *key_line;
    QPushButton *encrypt_button;
    QLabel *label_4;
    QLineEdit *group_line;
    QLabel *label_2;
    QLabel *label_7;
    QComboBox *comboBox;
    QLabel *label_21;
    QTreeWidget *keys_treeWidget_Encryption;
    QTreeWidget *devices_treeWidget_Encryption;
    QWidget *tab_2;
    QLabel *label_9;
    QLineEdit *file_line_Decryption;
    QPushButton *browseButton_2;
    QLabel *label_10;
    QLineEdit *device_line_Decryption;
    QLabel *label_11;
    QLineEdit *pin_line_Decryption;
    QPushButton *deviceListButton_Decryption;
    QCheckBox *checkBox;
    QPushButton *decrypt_button;
    QTreeWidget *devices_treeWidget_Decryption;
    QWidget *tab_3;
    QLabel *label_12;
    QLineEdit *file_line_Digest;
    QPushButton *browseButton_3;
    QLabel *label_13;
    QLineEdit *device_line_Digest;
    QLabel *label_14;
    QLineEdit *pin_line_Digest;
    QPushButton *deviceListButton_Digest;
    QLabel *label_15;
    QLineEdit *group_line_Digest;
    QLabel *label_16;
    QLineEdit *user_line_Digest;
    QLabel *label_17;
    QLineEdit *key_line_Digest;
    QLabel *label_18;
    QComboBox *algorithm_comboBox_Digest;
    QPushButton *listkeys_button_Digest;
    QPushButton *digest_button;
    QTreeWidget *devices_treeWidget_Digest;
    QTreeWidget *keys_treeWidget_Digest;
    QLabel *label_22;
    QWidget *tab_4;
    QLabel *label_8;
    QLineEdit *path_line_UpdatePath;
    QPushButton *browseButton_4;
    QLabel *label_19;
    QLineEdit *device_line_UpdatePath;
    QLabel *label_20;
    QLineEdit *pin_line_UpdatePath;
    QPushButton *deviceListButton_UpdatePath;
    QPushButton *updatePath_button;
    QTreeWidget *devices_treeWidget_UpdatePath;

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
        listkeys_button->setGeometry(QRect(620, 400, 80, 25));
        label_3 = new QLabel(tab_1);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 120, 71, 21));
        browseButton = new QPushButton(tab_1);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setGeometry(QRect(630, 40, 75, 23));
        deviceListButton = new QPushButton(tab_1);
        deviceListButton->setObjectName(QString::fromUtf8("deviceListButton"));
        deviceListButton->setGeometry(QRect(600, 210, 101, 23));
        user_line = new QLineEdit(tab_1);
        user_line->setObjectName(QString::fromUtf8("user_line"));
        user_line->setGeometry(QRect(80, 310, 171, 24));
        label_5 = new QLabel(tab_1);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(0, 270, 71, 21));
        device_line = new QLineEdit(tab_1);
        device_line->setObjectName(QString::fromUtf8("device_line"));
        device_line->setGeometry(QRect(80, 120, 171, 21));
        file_line = new QLineEdit(tab_1);
        file_line->setObjectName(QString::fromUtf8("file_line"));
        file_line->setGeometry(QRect(40, 10, 671, 20));
        label_6 = new QLabel(tab_1);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(0, 310, 71, 21));
        pin_line = new QLineEdit(tab_1);
        pin_line->setObjectName(QString::fromUtf8("pin_line"));
        pin_line->setGeometry(QRect(80, 160, 171, 21));
        pin_line->setEchoMode(QLineEdit::Password);
        label = new QLabel(tab_1);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 47, 21));
        key_line = new QLineEdit(tab_1);
        key_line->setObjectName(QString::fromUtf8("key_line"));
        key_line->setGeometry(QRect(80, 350, 171, 24));
        encrypt_button = new QPushButton(tab_1);
        encrypt_button->setObjectName(QString::fromUtf8("encrypt_button"));
        encrypt_button->setGeometry(QRect(0, 440, 711, 23));
        label_4 = new QLabel(tab_1);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(0, 350, 71, 21));
        group_line = new QLineEdit(tab_1);
        group_line->setObjectName(QString::fromUtf8("group_line"));
        group_line->setGeometry(QRect(80, 270, 171, 24));
        label_2 = new QLabel(tab_1);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 160, 71, 21));
        label_7 = new QLabel(tab_1);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(0, 400, 71, 21));
        comboBox = new QComboBox(tab_1);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(80, 396, 171, 24));
        label_21 = new QLabel(tab_1);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setGeometry(QRect(290, 400, 321, 31));
        keys_treeWidget_Encryption = new QTreeWidget(tab_1);
        keys_treeWidget_Encryption->setObjectName(QString::fromUtf8("keys_treeWidget_Encryption"));
        keys_treeWidget_Encryption->setGeometry(QRect(270, 260, 431, 131));
        devices_treeWidget_Encryption = new QTreeWidget(tab_1);
        devices_treeWidget_Encryption->setObjectName(QString::fromUtf8("devices_treeWidget_Encryption"));
        devices_treeWidget_Encryption->setGeometry(QRect(270, 90, 431, 111));
        tabWidget->addTab(tab_1, QString());
        listkeys_button->raise();
        browseButton->raise();
        deviceListButton->raise();
        label->raise();
        encrypt_button->raise();
        label_21->raise();
        keys_treeWidget_Encryption->raise();
        devices_treeWidget_Encryption->raise();
        file_line->raise();
        device_line->raise();
        pin_line->raise();
        group_line->raise();
        user_line->raise();
        key_line->raise();
        comboBox->raise();
        label_3->raise();
        label_2->raise();
        label_5->raise();
        label_6->raise();
        label_4->raise();
        label_7->raise();
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        label_9 = new QLabel(tab_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 10, 47, 21));
        file_line_Decryption = new QLineEdit(tab_2);
        file_line_Decryption->setObjectName(QString::fromUtf8("file_line_Decryption"));
        file_line_Decryption->setGeometry(QRect(40, 10, 671, 20));
        browseButton_2 = new QPushButton(tab_2);
        browseButton_2->setObjectName(QString::fromUtf8("browseButton_2"));
        browseButton_2->setGeometry(QRect(630, 40, 75, 23));
        label_10 = new QLabel(tab_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(0, 120, 71, 21));
        device_line_Decryption = new QLineEdit(tab_2);
        device_line_Decryption->setObjectName(QString::fromUtf8("device_line_Decryption"));
        device_line_Decryption->setGeometry(QRect(80, 120, 171, 21));
        label_11 = new QLabel(tab_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(0, 160, 71, 21));
        pin_line_Decryption = new QLineEdit(tab_2);
        pin_line_Decryption->setObjectName(QString::fromUtf8("pin_line_Decryption"));
        pin_line_Decryption->setGeometry(QRect(80, 160, 171, 21));
        pin_line_Decryption->setEchoMode(QLineEdit::Password);
        deviceListButton_Decryption = new QPushButton(tab_2);
        deviceListButton_Decryption->setObjectName(QString::fromUtf8("deviceListButton_Decryption"));
        deviceListButton_Decryption->setGeometry(QRect(600, 210, 101, 23));
        checkBox = new QCheckBox(tab_2);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(620, 400, 91, 22));
        checkBox->setMouseTracking(false);
        checkBox->setTristate(false);
        decrypt_button = new QPushButton(tab_2);
        decrypt_button->setObjectName(QString::fromUtf8("decrypt_button"));
        decrypt_button->setGeometry(QRect(0, 440, 711, 23));
        devices_treeWidget_Decryption = new QTreeWidget(tab_2);
        devices_treeWidget_Decryption->setObjectName(QString::fromUtf8("devices_treeWidget_Decryption"));
        devices_treeWidget_Decryption->setGeometry(QRect(270, 90, 431, 111));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        label_12 = new QLabel(tab_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(10, 10, 47, 21));
        file_line_Digest = new QLineEdit(tab_3);
        file_line_Digest->setObjectName(QString::fromUtf8("file_line_Digest"));
        file_line_Digest->setGeometry(QRect(40, 10, 671, 20));
        browseButton_3 = new QPushButton(tab_3);
        browseButton_3->setObjectName(QString::fromUtf8("browseButton_3"));
        browseButton_3->setGeometry(QRect(630, 40, 75, 23));
        label_13 = new QLabel(tab_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(0, 120, 71, 21));
        device_line_Digest = new QLineEdit(tab_3);
        device_line_Digest->setObjectName(QString::fromUtf8("device_line_Digest"));
        device_line_Digest->setGeometry(QRect(80, 120, 171, 21));
        label_14 = new QLabel(tab_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(0, 160, 71, 21));
        pin_line_Digest = new QLineEdit(tab_3);
        pin_line_Digest->setObjectName(QString::fromUtf8("pin_line_Digest"));
        pin_line_Digest->setGeometry(QRect(80, 160, 171, 21));
        pin_line_Digest->setEchoMode(QLineEdit::Password);
        deviceListButton_Digest = new QPushButton(tab_3);
        deviceListButton_Digest->setObjectName(QString::fromUtf8("deviceListButton_Digest"));
        deviceListButton_Digest->setGeometry(QRect(600, 210, 101, 23));
        label_15 = new QLabel(tab_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(10, 270, 61, 21));
        group_line_Digest = new QLineEdit(tab_3);
        group_line_Digest->setObjectName(QString::fromUtf8("group_line_Digest"));
        group_line_Digest->setGeometry(QRect(80, 270, 171, 24));
        label_16 = new QLabel(tab_3);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(0, 310, 71, 21));
        user_line_Digest = new QLineEdit(tab_3);
        user_line_Digest->setObjectName(QString::fromUtf8("user_line_Digest"));
        user_line_Digest->setGeometry(QRect(80, 310, 171, 24));
        label_17 = new QLabel(tab_3);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(0, 350, 71, 21));
        key_line_Digest = new QLineEdit(tab_3);
        key_line_Digest->setObjectName(QString::fromUtf8("key_line_Digest"));
        key_line_Digest->setGeometry(QRect(80, 350, 171, 24));
        label_18 = new QLabel(tab_3);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(0, 400, 71, 21));
        algorithm_comboBox_Digest = new QComboBox(tab_3);
        algorithm_comboBox_Digest->addItem(QString());
        algorithm_comboBox_Digest->addItem(QString());
        algorithm_comboBox_Digest->setObjectName(QString::fromUtf8("algorithm_comboBox_Digest"));
        algorithm_comboBox_Digest->setGeometry(QRect(80, 396, 171, 24));
        listkeys_button_Digest = new QPushButton(tab_3);
        listkeys_button_Digest->setObjectName(QString::fromUtf8("listkeys_button_Digest"));
        listkeys_button_Digest->setGeometry(QRect(620, 400, 80, 25));
        digest_button = new QPushButton(tab_3);
        digest_button->setObjectName(QString::fromUtf8("digest_button"));
        digest_button->setGeometry(QRect(0, 440, 711, 23));
        devices_treeWidget_Digest = new QTreeWidget(tab_3);
        devices_treeWidget_Digest->setObjectName(QString::fromUtf8("devices_treeWidget_Digest"));
        devices_treeWidget_Digest->setGeometry(QRect(270, 90, 431, 111));
        keys_treeWidget_Digest = new QTreeWidget(tab_3);
        keys_treeWidget_Digest->setObjectName(QString::fromUtf8("keys_treeWidget_Digest"));
        keys_treeWidget_Digest->setGeometry(QRect(270, 260, 431, 131));
        label_22 = new QLabel(tab_3);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setGeometry(QRect(290, 400, 321, 31));
        tabWidget->addTab(tab_3, QString());
        label_12->raise();
        file_line_Digest->raise();
        browseButton_3->raise();
        label_13->raise();
        device_line_Digest->raise();
        label_14->raise();
        pin_line_Digest->raise();
        deviceListButton_Digest->raise();
        label_15->raise();
        group_line_Digest->raise();
        label_16->raise();
        user_line_Digest->raise();
        label_17->raise();
        key_line_Digest->raise();
        label_18->raise();
        listkeys_button_Digest->raise();
        digest_button->raise();
        devices_treeWidget_Digest->raise();
        keys_treeWidget_Digest->raise();
        algorithm_comboBox_Digest->raise();
        label_22->raise();
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        label_8 = new QLabel(tab_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 10, 151, 21));
        path_line_UpdatePath = new QLineEdit(tab_4);
        path_line_UpdatePath->setObjectName(QString::fromUtf8("path_line_UpdatePath"));
        path_line_UpdatePath->setGeometry(QRect(50, 10, 661, 20));
        browseButton_4 = new QPushButton(tab_4);
        browseButton_4->setObjectName(QString::fromUtf8("browseButton_4"));
        browseButton_4->setGeometry(QRect(630, 40, 75, 23));
        label_19 = new QLabel(tab_4);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(0, 120, 71, 21));
        device_line_UpdatePath = new QLineEdit(tab_4);
        device_line_UpdatePath->setObjectName(QString::fromUtf8("device_line_UpdatePath"));
        device_line_UpdatePath->setGeometry(QRect(80, 120, 171, 21));
        label_20 = new QLabel(tab_4);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setGeometry(QRect(0, 160, 71, 21));
        pin_line_UpdatePath = new QLineEdit(tab_4);
        pin_line_UpdatePath->setObjectName(QString::fromUtf8("pin_line_UpdatePath"));
        pin_line_UpdatePath->setGeometry(QRect(80, 160, 171, 21));
        pin_line_UpdatePath->setEchoMode(QLineEdit::Password);
        deviceListButton_UpdatePath = new QPushButton(tab_4);
        deviceListButton_UpdatePath->setObjectName(QString::fromUtf8("deviceListButton_UpdatePath"));
        deviceListButton_UpdatePath->setGeometry(QRect(600, 210, 101, 23));
        updatePath_button = new QPushButton(tab_4);
        updatePath_button->setObjectName(QString::fromUtf8("updatePath_button"));
        updatePath_button->setGeometry(QRect(0, 440, 711, 23));
        devices_treeWidget_UpdatePath = new QTreeWidget(tab_4);
        devices_treeWidget_UpdatePath->setObjectName(QString::fromUtf8("devices_treeWidget_UpdatePath"));
        devices_treeWidget_UpdatePath->setGeometry(QRect(270, 90, 431, 111));
        tabWidget->addTab(tab_4, QString());
        label_8->raise();
        browseButton_4->raise();
        label_19->raise();
        device_line_UpdatePath->raise();
        label_20->raise();
        pin_line_UpdatePath->raise();
        deviceListButton_UpdatePath->raise();
        updatePath_button->raise();
        devices_treeWidget_UpdatePath->raise();
        path_line_UpdatePath->raise();
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
        label_3->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Device ID:</p></body></html>", nullptr));
        browseButton->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        deviceListButton->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
#if QT_CONFIG(tooltip)
        user_line->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as U+the number of the user (e.g., U10). Put list of users between &quot; &quot;, each one separated by space.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Group:</p></body></html>", nullptr));
        label_6->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">User(s):</p></body></html>", nullptr));
        label->setText(QCoreApplication::translate("Utilities", "File:", nullptr));
        encrypt_button->setText(QCoreApplication::translate("Utilities", "Encrypt", nullptr));
        label_4->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Key:</p></body></html>", nullptr));
#if QT_CONFIG(tooltip)
        group_line->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as G+the number of the group (e.g., G10).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">PIN:</p></body></html>", nullptr));
        label_7->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Algorithm:</p></body></html>", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("Utilities", "AES_HMACSHA256", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("Utilities", "AES (not available)", nullptr));

        label_21->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Before listing keys, please insert your pin!</p></body></html>", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = keys_treeWidget_Encryption->headerItem();
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("Utilities", "Key Size", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("Utilities", "Key ID", nullptr));
        QTreeWidgetItem *___qtreewidgetitem1 = devices_treeWidget_Encryption->headerItem();
        ___qtreewidgetitem1->setText(2, QCoreApplication::translate("Utilities", "Device Serial", nullptr));
        ___qtreewidgetitem1->setText(1, QCoreApplication::translate("Utilities", "Device Path", nullptr));
        ___qtreewidgetitem1->setText(0, QCoreApplication::translate("Utilities", "Device ID", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QCoreApplication::translate("Utilities", "Encryption", nullptr));
        label_9->setText(QCoreApplication::translate("Utilities", "File:", nullptr));
        browseButton_2->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        label_10->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Device ID:</p></body></html>", nullptr));
        label_11->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">PIN:</p></body></html>", nullptr));
        deviceListButton_Decryption->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
#if QT_CONFIG(tooltip)
        checkBox->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>Checking it, SEKey will be used in order to retrieve the proper key. Otherwise, the infos in the file header will be used.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox->setText(QCoreApplication::translate("Utilities", "Use SEKey", nullptr));
        decrypt_button->setText(QCoreApplication::translate("Utilities", "Decrypt", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = devices_treeWidget_Decryption->headerItem();
        ___qtreewidgetitem2->setText(2, QCoreApplication::translate("Utilities", "Device Serial", nullptr));
        ___qtreewidgetitem2->setText(1, QCoreApplication::translate("Utilities", "Device Path", nullptr));
        ___qtreewidgetitem2->setText(0, QCoreApplication::translate("Utilities", "Device ID", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("Utilities", "Decryption", nullptr));
        label_12->setText(QCoreApplication::translate("Utilities", "File:", nullptr));
        browseButton_3->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        label_13->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Device ID:</p></body></html>", nullptr));
        label_14->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">PIN:</p></body></html>", nullptr));
        deviceListButton_Digest->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
        label_15->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Group:</p></body></html>", nullptr));
#if QT_CONFIG(tooltip)
        group_line_Digest->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as G+the number of the group (e.g., G10).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_16->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">User(s):</p></body></html>", nullptr));
#if QT_CONFIG(tooltip)
        user_line_Digest->setToolTip(QCoreApplication::translate("Utilities", "<html><head/><body><p>If specified, find keys automatically. Insert it as U+the number of the user (e.g., U10). Put list of users between &quot; &quot;, each one separated by space.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_17->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Key:</p></body></html>", nullptr));
        label_18->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Algorithm:</p></body></html>", nullptr));
        algorithm_comboBox_Digest->setItemText(0, QCoreApplication::translate("Utilities", "HMAC-SHA-256", nullptr));
        algorithm_comboBox_Digest->setItemText(1, QCoreApplication::translate("Utilities", "SHA-256", nullptr));

        listkeys_button_Digest->setText(QCoreApplication::translate("Utilities", "List keys...", nullptr));
        digest_button->setText(QCoreApplication::translate("Utilities", "Compute Digest", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = devices_treeWidget_Digest->headerItem();
        ___qtreewidgetitem3->setText(2, QCoreApplication::translate("Utilities", "Device Serial", nullptr));
        ___qtreewidgetitem3->setText(1, QCoreApplication::translate("Utilities", "Device Path", nullptr));
        ___qtreewidgetitem3->setText(0, QCoreApplication::translate("Utilities", "Device ID", nullptr));
        QTreeWidgetItem *___qtreewidgetitem4 = keys_treeWidget_Digest->headerItem();
        ___qtreewidgetitem4->setText(1, QCoreApplication::translate("Utilities", "Key Size", nullptr));
        ___qtreewidgetitem4->setText(0, QCoreApplication::translate("Utilities", "Key ID", nullptr));
        label_22->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Before listing keys, please insert your pin!</p></body></html>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("Utilities", "Digest", nullptr));
        label_8->setText(QCoreApplication::translate("Utilities", "Path:", nullptr));
        browseButton_4->setText(QCoreApplication::translate("Utilities", "Browse...", nullptr));
        label_19->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">Device ID:</p></body></html>", nullptr));
        label_20->setText(QCoreApplication::translate("Utilities", "<html><head/><body><p align=\"right\">PIN:</p></body></html>", nullptr));
        deviceListButton_UpdatePath->setText(QCoreApplication::translate("Utilities", "List devices...", nullptr));
        updatePath_button->setText(QCoreApplication::translate("Utilities", "Update SEKey path", nullptr));
        QTreeWidgetItem *___qtreewidgetitem5 = devices_treeWidget_UpdatePath->headerItem();
        ___qtreewidgetitem5->setText(2, QCoreApplication::translate("Utilities", "Device Serial", nullptr));
        ___qtreewidgetitem5->setText(1, QCoreApplication::translate("Utilities", "Device Path", nullptr));
        ___qtreewidgetitem5->setText(0, QCoreApplication::translate("Utilities", "Device ID", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("Utilities", "Update SEKey path", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Utilities: public Ui_Utilities {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UTILITIES_H
