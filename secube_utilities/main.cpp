#include "utilities.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Utilities w;

    // Set dark theme:
    QFile f(":qdarkstyle/dark/style.qss");

    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }

    a.setWindowIcon(QIcon(":/Logo/logo (1)/logo_small_icon_only.png")); // In this way the app icon is shown also in the QMessageBoxes
    w.show();

    return a.exec();
}
