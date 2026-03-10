#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class RegistrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationDialog(QWidget *parent = nullptr);
    ~RegistrationDialog();
;

private slots:
    void onSignInClicked();
    void onSignUpClicked();
    void onRegisterButtonClicked();
    void onNetworkReply(QNetworkReply *reply);

private:
    int userId = -1;
    void sendRegistrationRequest(const QString &username, const QString &password);
    void sendLoginRequest(const QString &username, const QString &password);
    Ui::Dialog *ui;
    void setActiveTab(const QString &tab);
    QNetworkAccessManager *networkManager;

};

#endif // REGISTRATIONDIALOG_H
