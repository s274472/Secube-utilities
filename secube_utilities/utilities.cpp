#include "utilities.h"
#include "ui_utilities.h"
#include <QFileDialog>
#include <QMessageBox>

#include "backend_interface.h"
#include <iostream>
#include <thread>
#include "cereal/archives/binary.hpp"
#include <windows.h>

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


void Utilities::on_deviceListButton_clicked()
{
    //This launches a cmd.exe line command window, modifies the PATH for the current window,
    //adding the secube installation path, and then launches the backend
    std::thread server = std::thread([]() {
       system("secube_cmd.exe -dl -gui_server");
    });
    server.detach();

    // Connect to backend server:
    int sock = connectToBackend();

    // Wait for response from backend:
    bool quit = false;
    int res = -1;
    char request[BUFLEN] = {0};
    char reply[BUFLEN] = {0};
    std::stringstream ss; // any stream can be used

    while (!quit) {
        memset(request, 0, BUFLEN);
        memset(reply, 0, BUFLEN);
        res = recv(sock, request, BUFLEN, 0);
        if (res < 0) {
            cout << "[LOG] [Client] Error reading GUI request!" << endl;
        } else { // process request depending on type
            cout << "[LOG] [Client] Received " << res << " bytes." << endl;
            Response_DEV_LIST resp;

            std::stringstream ss;
            ss.write((char*)request, res);
            cereal::BinaryInputArchive iarchive(ss);
            iarchive(resp); // Read the data from the archive

            // Update UI:
            if(resp.err_code<0) {
                cout << resp.err_msg << endl;
                QMessageBox::information(0, QString("Error!"), QString(resp.err_msg), QMessageBox::Ok);
            }
            else {
                int i = 0;
                for(i=0; i<resp.num_devices;i++) {
                    cout << resp.serials[i] << endl;
                }
            }

            quit = true;
        }
    }

    // Close the socket:
    closesocket(sock);

    // Cleanup winsock:
    WSACleanup();

    cout << "[LOG] [Client] Disconnected." << endl;
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


void Utilities::on_pushButton_clicked()
{
    //Open a dialog to choose the file, save the path in the target_file variable
    target_file = QFileDialog::getOpenFileName();
    //And update the content of the "File" form.
    ui -> lineEdit -> setText(target_file);
}


void Utilities::on_pushButton_4_clicked()
{
    //Open a dialog to choose the file, save the path in the target_file variable
    target_file = QFileDialog::getOpenFileName();
    //And update the content of the "File" form.
    ui -> lineEdit_4 -> setText(target_file);
}


void Utilities::on_lineEdit_7_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->lineEdit_7 ->setModified(false);
        ui ->lineEdit_8 ->setEnabled(true);
        ui ->lineEdit_9 ->setEnabled(true);
    } else {
         ui ->lineEdit_8 ->setEnabled(false);
         ui ->lineEdit_9 ->setEnabled(false);
    }
}


void Utilities::on_lineEdit_8_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->lineEdit_8 ->setModified(false);
        ui ->lineEdit_7 ->setEnabled(true);
        ui ->lineEdit_9 ->setEnabled(true);
    } else {
         ui ->lineEdit_7 ->setEnabled(false);
         ui ->lineEdit_9 ->setEnabled(false);
    }
}


void Utilities::on_lineEdit_9_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui ->lineEdit_9 ->setModified(false);
        ui ->lineEdit_7 ->setEnabled(true);
        ui ->lineEdit_8 ->setEnabled(true);
    } else {
         ui ->lineEdit_7 ->setEnabled(false);
         ui ->lineEdit_8 ->setEnabled(false);
    }
}


void Utilities::on_comboBox_2_activated(int index)
{
    if(index == 1)
    {
        ui ->lineEdit_7 ->setEnabled(false);
        ui ->lineEdit_8 ->setEnabled(false);
        ui ->lineEdit_9 ->setEnabled(false);
    } else if(ui ->lineEdit_7 ->isModified()||ui ->lineEdit_8 ->isModified()||ui ->lineEdit_9 ->isModified()) {
        if(ui ->lineEdit_7 ->isModified())
        ui ->lineEdit_7 ->setEnabled(true);
        if(ui ->lineEdit_8 ->isModified())
        ui ->lineEdit_8 ->setEnabled(true);
        if(ui ->lineEdit_9 ->isModified())
        ui ->lineEdit_9 ->setEnabled(true);
    } else {
        ui ->lineEdit_7 ->setEnabled(true);
        ui ->lineEdit_8 ->setEnabled(true);
        ui ->lineEdit_9 ->setEnabled(true);
    }
}


void Utilities::on_pushButton_8_clicked()
{
    //Open a dialog to choose the path, save the path in the target_file variable
    target_file = QFileDialog::getExistingDirectory();
    //And update the content of the "Path" form.
    ui -> lineEdit_10 -> setText(target_file);
}

