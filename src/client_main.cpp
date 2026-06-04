#include "../inc/mainwindow.h"
#include "../inc/registrationdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(
        "QWidget { color: black; }" 
        "QLineEdit { background-color: white; color: black; border: 1px solid gray; }" 
        "QPushButton { color: black; }" 
    );
    RegistrationDialog dialog;

    if (dialog.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }

    return 0;
}
