#include "../inc/mainwindow.h"
#include "../inc/registrationdialog.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(
        "QWidget { color: black; }" 
        "QLineEdit { background-color: white; color: black; border: 1px solid gray; }" 
        "QPushButton { color: black; }" 
    );
    
    RegistrationDialog dialog;
    int loggedInUserId = -1;
    QString loggedInLogin = "";

    QObject::connect(&dialog, &RegistrationDialog::loginSuccess, [&](int userId, const QString &login) {
        loggedInUserId = userId;
        loggedInLogin = login;
    });

    if (dialog.exec() == QDialog::Accepted) {
        MainWindow w;
        w.setCurrentUser(loggedInUserId, loggedInLogin); 
        w.show();
        return a.exec();
    }

    return 0;
}
