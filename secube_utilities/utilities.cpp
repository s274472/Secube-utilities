#include "utilities.h"
#include "ui_utilities.h"
#include <QFileDialog>
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
        ui -> lineEdit -> setText(target_file);
    }

}


Utilities::~Utilities()
{
    delete ui;
}

void Utilities::set_target_file(QString filename)
{
    target_file = filename;
    ui -> lineEdit -> setText(target_file);
}

//If button "Browse" is clicked...
void Utilities::on_browseButton_clicked()
{
    //Open a dialog to choose the file, save the path in the target_file variable
    target_file = QFileDialog::getOpenFileName();
    //And update the content of the "File" form.
    ui -> lineEdit -> setText(target_file);
}


void Utilities::on_exitButton_clicked()
{
    QCoreApplication::quit();
}


void Utilities::on_deviceListButton_clicked()
{
    //This launches a cmd.exe line command window, modifies the PATH for the current window,
    //adding the secube installation path, and then launches the backend
    system("cmd /K  \"set PATH=%PATH%;%ProgramFiles%\\secube&&secube_cmd.exe -dl\" ");
}



