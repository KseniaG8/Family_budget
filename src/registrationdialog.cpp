#include "../inc/registrationdialog.h"
#include "ui_registrationdialog.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "../inc/setup2FAdialog.h"
#include "../inc/Verify2FADialog.h"

const QString SERVER_URL = "http://localhost:8080";

RegistrationDialog::RegistrationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistrationDialog)
{
    ui->setupUi(this);

    this->setStyleSheet("Qlabel:focus { outline: none; border: none; background-color: transparent; }");

    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &RegistrationDialog::onReplyFinished);

    connect(ui->signInLabel, &QLabel::linkActivated, this, &RegistrationDialog::onSignInClicked);
    connect(ui->signUpLabel, &QLabel::linkActivated, this, &RegistrationDialog::onSignUpClicked);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &RegistrationDialog::onRegisterButtonClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &RegistrationDialog::reject);

    setActiveTab("signin");

    ui->signInLabel->setText("<a href=\"#\" style=\"color: black; text-decoration: none; background-color: transparent;\">SIGN IN</a>");
    ui->signUpLabel->setText("<a href=\"#\" style=\"color: black; text-decoration: none; background-color: transparent;\">SIGN UP</a>");
    ui->signInLabel->setOpenExternalLinks(false);
    ui->signUpLabel->setOpenExternalLinks(false);
}

void RegistrationDialog::onSignInClicked()
{
    setActiveTab("signin");
}

void RegistrationDialog::onSignUpClicked()
{
    setActiveTab("signup");
}

void RegistrationDialog::sendPostRequest(const QString &endpoint, const QJsonObject &data)
{
    QUrl url(SERVER_URL + endpoint); 
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    networkManager->post(request, doc.toJson());
}

void RegistrationDialog::setActiveTab(const QString &tab)
{
    if (tab == "signin") {
        ui->signInLabel->setStyleSheet("color: #2c3e50; font: bold 12pt; background-color: transparent;");
        ui->signUpLabel->setStyleSheet("color: #95a5a6; font: bold 12pt; background-color: transparent;");
        ui->underline->move(110, ui->underline->y());
        ui->confirmLabel->hide();
        ui->confirmEdit->hide();
    }
    else if (tab == "signup") {
        ui->signInLabel->setStyleSheet("color: #95a5a6; font: bold 12pt; background-color: transparent;");
        ui->signUpLabel->setStyleSheet("color: #2c3e50; font: bold 12pt; background-color: transparent;");
        ui->underline->move(340, ui->underline->y());
        ui->confirmLabel->show();
        ui->confirmEdit->show();
    }
}

void RegistrationDialog::onRegisterButtonClicked()
{
    QString username = ui->lineEdit_2->text();
    QString password = ui->lineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    if (ui->confirmLabel->isVisible()) {
        QString confirmPassword = ui->confirmEdit->text();

        if (password != confirmPassword) {
            QMessageBox::warning(this, "Ошибка", "Пароли не совпадают!");
            return;
        }

        QJsonObject request;
        request["login"] = username; 
        request["password"] = password;

        sendPostRequest("/register", request);
    } else {
        QJsonObject request;
        request["login"] = username; 
        request["password"] = password;

        sendPostRequest("/login", request);
    }
}

void RegistrationDialog::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Ошибка", "Ошибка сети: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject obj = doc.object();

    QString url = reply->url().toString();

    if (url.contains("/register")) {
        if (obj.contains("error") || obj["status"].toString() == "error") {
            QString errMsg = obj.contains("message") ? obj["message"].toString() : obj["error"].toString();
            QMessageBox::warning(this, "Ошибка регистрации", errMsg);
        } else {
            QMessageBox::information(this, "Успех", "Регистрация успешна! Теперь войдите.");
            setActiveTab("signin"); 
        }
    }
    else if (url.contains("/login") && !url.contains("/verify")) {
        if (obj.contains("error") || obj["status"].toString() == "error") {
            QString errMsg = obj.contains("message") ? obj["message"].toString() : obj["error"].toString();
            QMessageBox::warning(this, "Ошибка входа", errMsg);
        } else {
            bool is2faEnabled = obj["is_2fa_enabled"].toBool();
            
            if (is2faEnabled) {
                Verify2FADialog dialog(this);
                if (dialog.exec() == QDialog::Accepted) {
                    QString code = dialog.getCode();
                    
                    QJsonObject request;
                    request["login"] = ui->lineEdit_2->text();
                    request["code"] = code;
                    
                    sendPostRequest("/2fa/verify", request);
                }
            } else {
                int userId = obj["user_id"].toInt();
                emit loginSuccess(userId);
                accept();  
            }
        }
    }
    else if (url.contains("/2fa/verify")) {
        if (obj.contains("error") || obj["status"].toString() == "error") {
            QString errMsg = obj.contains("message") ? obj["message"].toString() : "Неверный код!";
            QMessageBox::warning(this, "Ошибка 2FA", errMsg);
        } else {
            int userId = obj["user_id"].toInt();
            emit loginSuccess(userId);
            accept();
        }
    }
    
    reply->deleteLater();
}

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}
