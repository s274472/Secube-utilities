#ifndef UTILITIES_H
#define UTILITIES_H

#include <QMainWindow>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class Utilities; }
QT_END_NAMESPACE

class Utilities : public QMainWindow
{
    Q_OBJECT

public:
    Utilities(QWidget *parent = nullptr);
    ~Utilities();
    void set_target_file(QString filename);

private slots:

    void on_browseButton_Encryption_clicked();

    void on_browseButton_Decryption_clicked();

    void on_browseButton_Digest_clicked();

    void on_browseButton_UpdatePath_clicked();

    void on_deviceListButton_clicked();

    void on_deviceListButton_Digest_clicked();

    void on_deviceListButton_Decryption_clicked();

    void on_deviceListButton_UpdatePath_clicked();

    void on_listkeys_button_clicked();

    void on_listkeys_button_Digest_clicked();

    void on_group_line_textChanged(const QString &arg1);

    void on_user_line_textChanged(const QString &arg1);

    void on_key_line_textChanged(const QString &arg1);

    void on_group_line_Digest_textChanged(const QString &arg1);

    void on_user_line_Digest_textChanged(const QString &arg1);

    void on_key_line_Digest_textChanged(const QString &arg1);

    void on_updatePath_button_clicked();

    void on_digest_button_clicked();

    void on_decrypt_button_clicked();

    void on_encrypt_button_clicked();

    void on_devices_treeWidget_Decryption_itemClicked(QTreeWidgetItem *item);

    void on_devices_treeWidget_Encryption_itemClicked(QTreeWidgetItem *item);

    void on_devices_treeWidget_Digest_itemClicked(QTreeWidgetItem *item);

    void on_devices_treeWidget_UpdatePath_itemClicked(QTreeWidgetItem *item);

    void on_keys_treeWidget_Encryption_itemClicked(QTreeWidgetItem *item);

    void on_keys_treeWidget_Digest_itemClicked(QTreeWidgetItem *item);

    void on_algorithm_comboBox_Digest_activated(int index);

private:
    Ui::Utilities *ui;
};
#endif // UTILITIES_H
