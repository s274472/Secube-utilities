#include "utilities.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Utilities w;

    a.setWindowIcon(QIcon(":/Logo/logo (1)/logo_small_icon_only.png")); // In this way the app icon is shown also in the QMessageBoxes
    w.show();

    return a.exec();
}
