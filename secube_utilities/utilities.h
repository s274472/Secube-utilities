#ifndef UTILITIES_H
#define UTILITIES_H

#include <QMainWindow>

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

    void on_browseButton_clicked();

    void on_exitButton_clicked();

    void on_deviceListButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Utilities *ui;
};
#endif // UTILITIES_H
