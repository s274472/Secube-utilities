#include "utilities.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Utilities w;
    w.show();
    return a.exec();
}
