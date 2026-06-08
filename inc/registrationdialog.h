#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class RegistrationDialog; }
QT_END_NAMESPACE

class RegistrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationDialog(QWidget *parent = nullptr);
    ~RegistrationDialog();

signals:
<<<<<<< HEAD
    void loginSuccess(int userId, const QString &login);
=======
    void loginSuccess(int userId, QString login);
>>>>>>> budget-client

private slots:
    void onSignInClicked();
    void onSignUpClicked();
    void onReplyFinished(QNetworkReply *reply);
    void onRegisterButtonClicked();

private:
    void setActiveTab(const QString &tab);
    Ui::RegistrationDialog *ui;
    QNetworkAccessManager *networkManager;
    QString baseUrl = "http://localhost:8080";

    void sendPostRequest(const QString &endpoint, const QJsonObject &data);


};

#endif // REGISTRATIONDIALOG_H
