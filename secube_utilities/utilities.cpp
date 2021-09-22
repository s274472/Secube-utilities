/**
 * All the UI events handling are contained here.
 *
 * For connecting the signal to the handling function the QT Autoconnection(connectSlotsByName) is used,
 * for this reason all the signal handling functions must be in the form: on_widgetName_signalName().
 */

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

Utilities::Utilities(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Utilities)
{
    ui->setupUi(this);
    //If there is an argument passed to the application, it will be used as path/filename by default:
    if (QCoreApplication::arguments().length() > 1) {
        QString path = QCoreApplication::arguments().at(1);

        // Update all the file_lines and the path_line:
        ui -> file_line -> setText(path);
        ui -> file_line_Decryption -> setText(path);
        ui -> file_line_Digest -> setText(path);
        ui -> path_line_UpdatePath -> setText(path);
    }

}

Utilities::~Utilities()
{
    delete ui;
}

void Utilities::on_group_line_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->user_line ->setEnabled(true);
        ui ->key_line ->setEnabled(true);
        ui ->user_line ->setPlaceholderText("");
        ui ->key_line ->setPlaceholderText("");


    } else {
         ui ->user_line ->setPlaceholderText("N/A");
         ui ->key_line ->setPlaceholderText("N/A");
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
        ui ->key_line ->setPlaceholderText("");
        ui ->group_line ->setPlaceholderText("");
    } else {
         ui ->group_line ->setEnabled(false);
         ui ->key_line ->setEnabled(false);
         ui ->group_line ->setPlaceholderText("N/A");
         ui ->key_line ->setPlaceholderText("N/A");
    }
}


void Utilities::on_key_line_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->user_line ->setEnabled(true);
        ui ->group_line ->setEnabled(true);
        ui ->group_line ->setPlaceholderText("");
        ui ->user_line ->setPlaceholderText("");
    } else {
         ui ->user_line ->setEnabled(false);
         ui ->group_line ->setEnabled(false);
         ui ->user_line ->setPlaceholderText("N/A");
         ui ->group_line ->setPlaceholderText("N/A");
    }
}

void Utilities::on_group_line_Digest_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->user_line_Digest ->setEnabled(true);
        ui ->key_line_Digest ->setEnabled(true);
        ui ->user_line_Digest ->setPlaceholderText("");
        ui ->key_line_Digest ->setPlaceholderText("");
    } else {
         ui ->user_line_Digest ->setEnabled(false);
         ui ->key_line_Digest ->setEnabled(false);
         ui ->user_line_Digest ->setPlaceholderText("N/A");
         ui ->key_line_Digest ->setPlaceholderText("N/A");
    }
}


void Utilities::on_user_line_Digest_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->group_line_Digest ->setEnabled(true);
        ui ->key_line_Digest ->setEnabled(true);
        ui ->group_line_Digest ->setPlaceholderText("");
        ui ->key_line_Digest ->setPlaceholderText("");
    } else {
         ui ->group_line_Digest ->setEnabled(false);
         ui ->key_line_Digest ->setEnabled(false);
         ui ->group_line_Digest ->setPlaceholderText("N/A");
         ui ->key_line_Digest ->setPlaceholderText("N/A");
    }
}


void Utilities::on_key_line_Digest_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->group_line_Digest ->setEnabled(true);
        ui ->user_line_Digest ->setEnabled(true);
        ui ->user_line_Digest ->setPlaceholderText("");
        ui ->group_line_Digest ->setPlaceholderText("");
    } else {
         ui ->group_line_Digest ->setEnabled(false);
         ui ->user_line_Digest ->setEnabled(false);
         ui ->user_line_Digest ->setPlaceholderText("N/A");
         ui ->group_line_Digest ->setPlaceholderText("N/A");
    }
}


void Utilities::on_algorithm_comboBox_Digest_activated(int index)
{
    if(index == 1)
    {
        ui ->group_line_Digest ->setEnabled(false);
        ui ->user_line_Digest ->setEnabled(false);
        ui ->key_line_Digest ->setEnabled(false);
        ui ->nonce_line_Digest ->setEnabled(false);
        ui ->user_line_Digest ->setText("N/A");
        ui ->group_line_Digest ->setText("N/A");
        ui ->key_line_Digest ->setText("N/A");
        ui ->nonce_line_Digest ->setText("N/A");
    } else {
        ui ->group_line_Digest ->setEnabled(true);
        ui ->user_line_Digest ->setEnabled(true);
        ui ->key_line_Digest ->setEnabled(true);
        ui ->nonce_line_Digest ->setEnabled(true);
        ui ->user_line_Digest ->clear();
        ui ->group_line_Digest ->clear();
        ui ->key_line_Digest ->clear();
        ui ->nonce_line_Digest ->clear();
    }
}

// Browse buttons:
void Utilities::on_browseButton_Encryption_clicked()
{
    // Open a dialog to choose the file:
    QString file_path = QFileDialog::getOpenFileName();

    // And update the content of the "File" form for all the file_lines:
    ui -> file_line -> setText(file_path);
    ui -> file_line_Decryption -> setText(file_path);
    ui -> file_line_Digest -> setText(file_path);
}

void Utilities::on_browseButton_Decryption_clicked()
{
    // Open a dialog to choose the file:
    QString file_path = QFileDialog::getOpenFileName();

    // And update the content of the "File" form for all the file_lines:
    ui -> file_line -> setText(file_path);
    ui -> file_line_Decryption -> setText(file_path);
    ui -> file_line_Digest -> setText(file_path);
}


void Utilities::on_browseButton_Digest_clicked()
{
    // Open a dialog to choose the file:
    QString file_path = QFileDialog::getOpenFileName();

    // And update the content of the "File" form for all the file_lines:
    ui -> file_line -> setText(file_path);
    ui -> file_line_Decryption -> setText(file_path);
    ui -> file_line_Digest -> setText(file_path);
}

void Utilities::on_browseButton_UpdatePath_clicked()
{
    // Open a dialog to choose the path:
    QString path = QFileDialog::getExistingDirectory();

    //A nd update the content of the "Path" form.
    ui -> path_line_UpdatePath -> setText(path);
}

// Device list butttons:
void Utilities::on_deviceListButton_clicked()
{

    ui->deviceListButton->setText("Searching...");
    ui->deviceListButton->repaint(); // Forces update

    // Clear the list:
    ui->devices_treeWidget_Encryption->clear();

    // Prepare command:
    QString command = "secube_cmd.exe -dl -gui_server";

    cout << command.toUtf8().constData() << endl;

    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>(command.toUtf8().constData());

    // Update UI:

    if(resp.err_code<0) {
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_Encryption);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_Encryption->addTopLevelItem(treeItem);
        }
    }

    ui->deviceListButton->setText("List devices...");
}

void Utilities::on_deviceListButton_Decryption_clicked()
{

    ui->deviceListButton_Decryption->setText("Searching...");
    ui->deviceListButton_Decryption->repaint(); // Forces update

    // Clear the list:
    ui->devices_treeWidget_Decryption->clear();

    // Prepare command:
    QString command = "secube_cmd.exe -dl -gui_server";

    cout << command.toUtf8().constData() << endl;

    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_Decryption);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_Decryption->addTopLevelItem(treeItem);
        }
    }

    ui->deviceListButton_Decryption->setText("List devices...");
}

void Utilities::on_deviceListButton_Digest_clicked()
{

    ui->deviceListButton_Digest->setText("Searching...");
    ui->deviceListButton_Digest->repaint(); // Forces update

    // Clear the list:
    ui->devices_treeWidget_Digest->clear();

    // Prepare command:
    QString command = "secube_cmd.exe -dl -gui_server";

    cout << command.toUtf8().constData() << endl;

    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_Digest);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_Digest->addTopLevelItem(treeItem);
        }
    }

    ui->deviceListButton_Digest->setText("List devices...");
}

void Utilities::on_deviceListButton_UpdatePath_clicked()
{

    ui->deviceListButton_UpdatePath->setText("Searching...");
    ui->deviceListButton_UpdatePath->repaint(); // Forces update

    // Clear the list:
    ui->devices_treeWidget_UpdatePath->clear();

    // Prepare command:
    QString command = "secube_cmd.exe -dl -gui_server";

    cout << command.toUtf8().constData() << endl;

    // Send request and wait for response:
    Response_DEV_LIST resp;
    resp = sendRequestToBackend<Response_DEV_LIST>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_devices;i++) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->devices_treeWidget_UpdatePath);
            treeItem->setText(0, QString::number(i) );
            treeItem->setText(1, resp.paths[i] );
            treeItem->setText(2, resp.serials[i] );
            ui->devices_treeWidget_UpdatePath->addTopLevelItem(treeItem);
        }
    }

    ui->deviceListButton_UpdatePath->setText("List devices...");
}

void Utilities::on_listkeys_button_clicked()
{

    ui->listkeys_button->setText("Extracting...");
    ui->listkeys_button->repaint(); // Forces update

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

    cout << command.toUtf8().constData() << endl;

    // Send request and wait for response:
    Response_LIST_KEYS resp;
    resp = sendRequestToBackend<Response_LIST_KEYS>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {

        int i = 0;
        for(i=0; i<resp.num_keys;i++) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->keys_treeWidget_Encryption);
            treeItem->setText(0, QString::number(resp.key_ids[i]) );
            treeItem->setText(1, QString::number(resp.key_sizes[i]) );
            ui->keys_treeWidget_Encryption->addTopLevelItem(treeItem);
        }
    }

    ui->listkeys_button->setText("List keys...");
}

void Utilities::on_listkeys_button_Digest_clicked()
{

    ui->listkeys_button_Digest->setText("Extracting...");
    ui->listkeys_button_Digest->repaint(); // Forces update

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
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        int i = 0;
        for(i=0; i<resp.num_keys;i++) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->keys_treeWidget_Digest);
            treeItem->setText(0, QString::number(resp.key_ids[i]) );
            treeItem->setText(1, QString::number(resp.key_sizes[i]) );
            ui->keys_treeWidget_Digest->addTopLevelItem(treeItem);
        }
    }

    ui->listkeys_button_Digest->setText("List keys...");
}


void Utilities::on_decrypt_button_clicked()
{

    ui->decrypt_button->setText("Decrypting...");
    ui->decrypt_button->repaint(); // Forces update

    // Prepare command:
    QString command = "secube_cmd.exe -gui_server ";

    if( ui->useSEKey_checkBox_Decryption->isChecked() ) {
        command += "-dk ";
    } else {
        command += "-d ";
    }

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
        ui->decrypt_button->setText("Error!");
        ui->decrypt_button->repaint(); // Forces update
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        ui->decrypt_button->setText("Done!");
        ui->decrypt_button->repaint(); // Forces update
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

    ui->decrypt_button->setText("Decrypt");
}

void Utilities::on_encrypt_button_clicked()
{
    ui->encrypt_button->setText("Encrypting...");
    ui->encrypt_button->repaint(); // Forces update

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
        ui->encrypt_button->setText("Error!");
        ui->encrypt_button->repaint(); // Forces update
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        ui->encrypt_button->setText("Done!");
        ui->encrypt_button->repaint(); // Forces update
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

    ui->encrypt_button->setText("Encrypt");
}

void Utilities::on_digest_button_clicked()
{

    ui->digest_button->setText("Computing digest...");
    ui->digest_button->repaint(); // Forces update

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

    if(ui->nonce_line_Digest->isEnabled() && ui->nonce_line_Digest->text().size()>0 ) {
        command += "-nonce \"" + ui->nonce_line_Digest->text();
        command += "\"";
    }

    cout << command.toUtf8().constData() << endl; // For Debug

    // Send request and wait for response:
    Response_GENERIC resp;
    resp = sendRequestToBackend<Response_GENERIC>(command.toUtf8().constData());

    // Update UI:
    if(resp.err_code<0) {
        ui->digest_button->setText("Error!");
        ui->digest_button->repaint();
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        ui->digest_button->setText("Done!");
        ui->digest_button->repaint();
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

    ui->digest_button->setText("Compute Digest");

}

void Utilities::on_updatePath_button_clicked()
{

    ui->updatePath_button->setText("Updating...");
    ui->updatePath_button->repaint(); // Forces update

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
        ui->updatePath_button->setText("Error!");
        ui->updatePath_button->repaint(); // Forces update
        QMessageBox::critical(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
    }
    else {
        ui->updatePath_button->setText("Done!");
        ui->updatePath_button->repaint(); // Forces update
        QMessageBox::information(0, QString("Done!"), QString(resp.err_msg), QMessageBox::Ok);
    }

    ui->updatePath_button->setText("Update SEKey path");
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
    if(ui->key_line->isEnabled() == true)
    ui->key_line->setText( item->data(0, Qt::DisplayRole).toString() ) ;
}

void Utilities::on_keys_treeWidget_Digest_itemClicked(QTreeWidgetItem *item){

    // Update the key line with the selected ID:
    if(ui->key_line_Digest->isEnabled() == true)
    ui->key_line_Digest->setText( item->data(0, Qt::DisplayRole).toString() ) ;
}
