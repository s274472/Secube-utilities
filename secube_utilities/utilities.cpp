#include "utilities.h"
#include "ui_utilities.h"
#include <QFileDialog>
#include <QMessageBox>

#ifdef __linux__
    #include "linux_backend_interface.h"
#elif _WIN32
    #include "backend_interface.h"
#endif

#include <iostream>
#include "cereal/archives/binary.hpp"

QString target_file;

Utilities::Utilities(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Utilities)
{
    ui->setupUi(this);
    //If there is an argument passed to the application,
    //it will be used as filename by default
    if (QCoreApplication::arguments().length() > 1) {
        QString prova = QCoreApplication::arguments().at(1);
        target_file = prova;
        ui -> file_line -> setText(target_file);
    }

}


Utilities::~Utilities()
{
    delete ui;
}

void Utilities::set_target_file(QString filename)
{
    target_file = filename;
    ui -> file_line -> setText(target_file);
}

//If button "Browse" is clicked...
void Utilities::on_browseButton_clicked()
{
    //Open a dialog to choose the file, save the path in the target_file variable
    target_file = QFileDialog::getOpenFileName();
    //And update the content of the "File" form.
    ui -> file_line -> setText(target_file);
}

void Utilities::on_group_line_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->user_line ->setEnabled(true);
        ui ->key_line ->setEnabled(true);
    } else {
         ui ->user_line ->setEnabled(false);
         ui ->key_line ->setEnabled(false);
    }
}

void Utilities::on_user_line_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->group_line ->setEnabled(true);
        ui ->key_line ->setEnabled(true);
    } else {
         ui ->group_line ->setEnabled(false);
         ui ->key_line ->setEnabled(false);
    }
}


void Utilities::on_key_line_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->user_line ->setEnabled(true);
        ui ->group_line ->setEnabled(true);
    } else {
         ui ->user_line ->setEnabled(false);
         ui ->group_line ->setEnabled(false);
    }
}


void Utilities::on_browseButton_2_clicked()
{
    //Open a dialog to choose the file, save the path in the target_file variable
    target_file = QFileDialog::getOpenFileName();
    //And update the content of the "File" form.
    ui -> file_line_Decryption -> setText(target_file);
}


void Utilities::on_browseButton_3_clicked()
{
    //Open a dialog to choose the file, save the path in the target_file variable
    target_file = QFileDialog::getOpenFileName();
    //And update the content of the "File" form.
    ui -> file_line_Digest -> setText(target_file);
}


void Utilities::on_group_line_Digest_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->group_line_Digest ->setModified(false);
        ui ->user_line_Digest ->setEnabled(true);
        ui ->key_line_Digest ->setEnabled(true);
    } else {
         ui ->user_line_Digest ->setEnabled(false);
         ui ->key_line_Digest ->setEnabled(false);
    }
}


void Utilities::on_user_line_Digest_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->user_line_Digest ->setModified(false);
        ui ->group_line_Digest ->setEnabled(true);
        ui ->key_line_Digest ->setEnabled(true);
    } else {
         ui ->group_line_Digest ->setEnabled(false);
         ui ->key_line_Digest ->setEnabled(false);
    }
}


void Utilities::on_key_line_Digest_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->key_line_Digest ->setModified(false);
        ui ->group_line_Digest ->setEnabled(true);
        ui ->user_line_Digest ->setEnabled(true);
    } else {
         ui ->group_line_Digest ->setEnabled(false);
         ui ->user_line_Digest ->setEnabled(false);
    }
}


void Utilities::on_comboBox_2_activated(int index)
{
    if(index == 1)
    {
        ui ->group_line_Digest ->setEnabled(false);
        ui ->user_line_Digest ->setEnabled(false);
        ui ->key_line_Digest ->setEnabled(false);
    } else if(ui ->group_line_Digest ->isModified()||ui ->user_line_Digest ->isModified()||ui ->key_line_Digest ->isModified()) {
        if(ui ->group_line_Digest ->isModified())
        ui ->group_line_Digest ->setEnabled(true);
        if(ui ->user_line_Digest ->isModified())
        ui ->user_line_Digest ->setEnabled(true);
        if(ui ->key_line_Digest ->isModified())
        ui ->key_line_Digest ->setEnabled(true);
    } else {
        ui ->group_line_Digest ->setEnabled(true);
        ui ->user_line_Digest ->setEnabled(true);
        ui ->key_line_Digest ->setEnabled(true);
    }
}


void Utilities::on_browseButton_4_clicked()
{
    //Open a dialog to choose the path, save the path in the target_file variable
    target_file = QFileDialog::getExistingDirectory();
    //And update the content of the "Path" form.
    ui -> path_line_UpdatePath -> setText(target_file);
}

void Utilities::on_deviceListButton_clicked()
{
    // Clear the list:
    ui->devices_treeWidget_Encryption->clear();

    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>("secube_cmd.exe -dl -gui_server");

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            cout << resp.serials[i] << endl;

            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_Encryption);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_Encryption->addTopLevelItem(treeItem);
        }
    }

}

void Utilities::on_deviceListButton_Decryption_clicked()
{
    // Clear the list:
    ui->devices_treeWidget_Decryption->clear();


    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>("secube_cmd.exe -dl -gui_server");

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {

        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            cout << resp.serials[i] << endl;

            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_Decryption);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_Decryption->addTopLevelItem(treeItem);
        }
    }

}

void Utilities::on_deviceListButton_Digest_clicked()
{

    // Clear the list:
    ui->devices_treeWidget_Digest->clear();

    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>("secube_cmd.exe -dl -gui_server");

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            cout << resp.serials[i] << endl;

            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_Digest);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_Digest->addTopLevelItem(treeItem);
        }
    }

}

void Utilities::on_deviceListButton_UpdatePath_clicked()
{

    // Clear the list:
    ui->devices_treeWidget_UpdatePath->clear();

    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>("secube_cmd.exe -dl -gui_server");

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            cout << resp.serials[i] << endl;

            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_UpdatePath);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_UpdatePath->addTopLevelItem(treeItem);
        }
    }


}

void Utilities::on_listkeys_button_clicked()
{
    // Clear the list:
    ui->keys_treeWidget_Encryption->clear();

    // Prepare command:
    QString command = "secube_cmd.exe -gui_server -kl ";

    if( ui->pin_line->text().size()>0 ) {
        command += "-p";
        command += " " + ui->pin_line->text() + " ";
    }

    if( ui->device_line->text().size()>0 ) {
        command += "-dev";
        command += " " + ui->device_line->text() + " ";
    }

    cout << command.toUtf8().constData() << endl; // For Debug

    // Send request and wait for response:
    Response_LIST_KEYS resp;
    resp = sendRequestToBackend<Response_LIST_KEYS>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {

        int i = 0;
        for(i=0; i<resp.num_keys;i++) {
            cout << resp.key_ids[i] << endl;

            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->keys_treeWidget_Encryption);
            treeItem->setText(0, QString::number(resp.key_ids[i]) );
            treeItem->setText(1, QString::number(resp.key_sizes[i]) );
            ui->keys_treeWidget_Encryption->addTopLevelItem(treeItem);
        }
    }

}

void Utilities::on_listkeys_button_Digest_clicked()
{

    // Clear the list:
    ui->keys_treeWidget_Digest->clear();

    // Prepare command:
    QString command = "secube_cmd.exe -gui_server -kl ";

    if( ui->pin_line_Digest->text().size()>0 ) {
        command += "-p";
        command += " " + ui->pin_line_Digest->text() + " ";
    }

    if( ui->device_line_Digest->text().size()>0 ) {
        command += "-dev";
        command += " " + ui->device_line_Digest->text() + " ";
    }

    cout << command.toUtf8().constData() << endl; // For Debug

    // Send request and wait for response:
    Response_LIST_KEYS resp;
    resp = sendRequestToBackend<Response_LIST_KEYS>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_keys;i++) {
            cout << resp.key_ids[i] << endl;

            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->keys_treeWidget_Digest);
            treeItem->setText(0, QString::number(resp.key_ids[i]) );
            treeItem->setText(1, QString::number(resp.key_sizes[i]) );
            ui->keys_treeWidget_Digest->addTopLevelItem(treeItem);
        }
    }

}


void Utilities::on_decrypt_button_clicked()
{
    // Prepare command:
    QString command = "secube_cmd.exe -gui_server -d ";

    if( ui->pin_line_Decryption->text().size()>0 ) {
        command += "-p";
        command += " " + ui->pin_line_Decryption->text() + " ";
    }

    if( ui->device_line_Decryption->text().size()>0 ) {
        command += "-dev";
        command += " " + ui->device_line_Decryption->text() + " ";
    }

    if( ui->file_line_Decryption->text().size()>0 ) {
        command += "-f";
        command += " " + ui->file_line_Decryption->text() + " ";
    }

    cout << command.toUtf8().constData() << endl; // For Debug

    // Send request and wait for response:
    Response_GENERIC resp;
    resp = sendRequestToBackend<Response_GENERIC>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        cout << resp.err_msg << endl;
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

}

void Utilities::on_encrypt_button_clicked()
{
    // Prepare command:
    QString command = "secube_cmd.exe -gui_server -e ";

    if( ui->pin_line->text().size()>0 ) {
        command += "-p";
        command += " " + ui->pin_line->text() + " ";
    }

    if( ui->device_line->text().size()>0 ) {
        command += "-dev";
        command += " " + ui->device_line->text() + " ";
    }

    if( ui->file_line->text().size()>0 ) {
        command += "-f";
        command += " " + ui->file_line->text() + " ";
    }

    if( ui->key_line->isEnabled() && ui->key_line->text().size()>0 ) {
        command += "-k";
        command += " " + ui->key_line->text() + " ";
    }

    if( ui->group_line->isEnabled() && ui->group_line->text().size()>0 ){
        command += "-g";
        command += " " + ui->group_line->text() + " ";
    }

    if( ui->user_line->isEnabled() && ui->user_line->text().size()>0 ){
        command += "-u";
        command += " " + ui->user_line->text() + " ";
    }

    if(ui->comboBox->currentIndex()==0) {
        command += "-aes_hmac ";
    } else {
        command += "-aes ";
    }

    cout << command.toUtf8().constData() << endl; // For Debug

    // Send request and wait for response:
    Response_GENERIC resp;
    resp = sendRequestToBackend<Response_GENERIC>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        cout << resp.err_msg << endl;
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

}

void Utilities::on_digest_button_clicked()
{

    // Prepare command:
    QString command = "secube_cmd.exe -gui_server -di ";

    if( ui->pin_line_Digest->text().size()>0 ) {
        command += "-p";
        command += " " + ui->pin_line_Digest->text() + " ";
    }

    if( ui->device_line_Digest->text().size()>0 ) {
        command += "-dev";
        command += " " + ui->device_line_Digest->text() + " ";
    }

    if( ui->file_line_Digest->text().size()>0 ) {
        command += "-f";
        command += " " + ui->file_line_Digest->text() + " ";
    }

    if( ui->key_line_Digest->isEnabled() && ui->key_line_Digest->text().size()>0 ) {
        command += "-k";
        command += " " + ui->key_line_Digest->text() + " ";
    }

    if( ui->group_line_Digest->isEnabled() && ui->group_line_Digest->text().size()>0 ){
        command += "-g";
        command += " " + ui->group_line_Digest->text() + " ";
    }

    if( ui->user_line_Digest->isEnabled() && ui->user_line_Digest->text().size()>0 ){
        command += "-u";
        command += " " + ui->user_line_Digest->text() + " ";
    }

    if(ui->algorithm_comboBox_Digest->currentIndex()==0) {
        command += "-hmac ";
    } else {
        command += "-sha ";
    }

    cout << command.toUtf8().constData() << endl; // For Debug

    // Send request and wait for response:
    Response_GENERIC resp;
    resp = sendRequestToBackend<Response_GENERIC>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        cout << resp.err_msg << endl;
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

}

void Utilities::on_updatePath_button_clicked()
{

    // Prepare command:
    QString command = "secube_cmd.exe -gui_server -update_path ";

    if( ui->path_line_UpdatePath->text().size()>0 ) {
        command += "-f";
        command += " " + ui->path_line_UpdatePath->text() + " ";
    }

    if( ui->pin_line_UpdatePath->text().size()>0 ) {
        command += "-p";
        command += " " + ui->pin_line_UpdatePath->text() + " ";
    }

    if( ui->device_line_UpdatePath->text().size()>0 ) {
        command += "-dev";
        command += " " + ui->device_line_UpdatePath->text() + " ";
    }

    cout << command.toUtf8().constData() << endl; // For Debug

    // Send request and wait for response:
    Response_GENERIC resp;
    resp = sendRequestToBackend<Response_GENERIC>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        cout << resp.err_msg << endl;
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        cout << resp.err_msg << endl;
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

}

void Utilities::on_devices_treeWidget_Encryption_itemClicked(QTreeWidgetItem *item){

    // Update the device line with the selected ID:
    ui->device_line->setText(item->data(0, Qt::DisplayRole).toString());
}

void Utilities::on_devices_treeWidget_Decryption_itemClicked(QTreeWidgetItem *item){

    // Update the device line with the selected ID:
    ui->device_line_Decryption->setText(item->data(0, Qt::DisplayRole).toString());
}


void Utilities::on_devices_treeWidget_Digest_itemClicked(QTreeWidgetItem *item){

    // Update the device line with the selected ID:
    ui->device_line_Digest->setText(item->data(0, Qt::DisplayRole).toString());
}

void Utilities::on_devices_treeWidget_UpdatePath_itemClicked(QTreeWidgetItem *item){

    // Update the device line with the selected ID:
    ui->device_line_UpdatePath->setText(item->data(0, Qt::DisplayRole).toString());
}

void Utilities::on_keys_treeWidget_Encryption_itemClicked(QTreeWidgetItem *item){

    // Update the key line with the selected ID:
    ui->key_line->setText( item->data(0, Qt::DisplayRole).toString() ) ;
}

void Utilities::on_keys_treeWidget_Digest_itemClicked(QTreeWidgetItem *item){

    // Update the key line with the selected ID:
    ui->key_line_Digest->setText( item->data(0, Qt::DisplayRole).toString() ) ;
}
