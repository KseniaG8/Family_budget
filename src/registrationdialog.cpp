#include "../inc/registrationdialog.h"
#include "ui_registrationdialog.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

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

    // По умолчанию SIGN IN
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
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    networkManager->post(request, doc.toJson());
}

void RegistrationDialog::setActiveTab(const QString &tab)
{
    if (tab == "signin") {
        // SIGN IN активен
        ui->signInLabel->setStyleSheet("color: #2c3e50; font: bold 12pt; background-color: transparent;");
        ui->signUpLabel->setStyleSheet("color: #95a5a6; font: bold 12pt; background-color: transparent;");

        // Передвигаем линию под SIGN IN
        ui->underline->move(110, ui->underline->y());

        // Скрываем поле подтверждения пароля
        ui->confirmLabel->hide();
        ui->confirmEdit->hide();
    }
    else if (tab == "signup") {
        // SIGN UP активен
        ui->signInLabel->setStyleSheet("color: #95a5a6; font: bold 12pt; background-color: transparent;");
        ui->signUpLabel->setStyleSheet("color: #2c3e50; font: bold 12pt; background-color: transparent;");

        // Передвигаем линию под SIGN UP
        ui->underline->move(340, ui->underline->y());

        // Показываем поле подтверждения пароля
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
        request["login"] = login;
        request["password"] = password;

        sendPostRequest("/register", request);
    } else {
        QJsonObject request;
        request["login"] = login;
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
        if (obj.contains("error")) {
            QMessageBox::warning(this, "Ошибка регистрации", obj["error"].toString());
        } else {
            QMessageBox::information(this, "Успех", "Регистрация успешна! Теперь войдите.");
        }
    }
    else if (url.contains("/login")) {
        if (obj.contains("error")) {
            QMessageBox::warning(this, "Ошибка входа", obj["error"].toString());
        } else {
            int userId = obj["user_id"].toInt();
            emit loginSuccess(userId);
            accept();  // закрываем диалог
        }
    }

    reply->deleteLater();
}

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}


