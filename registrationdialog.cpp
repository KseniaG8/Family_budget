#include "registrationdialog.h"
#include "ui_registrationdialog.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

RegistrationDialog::RegistrationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->setStyleSheet("Qlabel:focus { outline: none; border: none; background-color: transparent; }");

    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &RegistrationDialog::onNetworkReply);

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

void RegistrationDialog::sendRegistrationRequest(const QString &username, const QString &password)
{
    QJsonObject json;
    json["action"] = "register";
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    qDebug() << "Запрос на регистрацию отправлен" << data;

    QMessageBox::information(this, "Регистрация", "Данные готовы к отправке");
    accept();

    /*
    QNetworkRequest request(QUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(request, data);
    */
}

void RegistrationDialog::sendLoginRequest(const QString &username, const QString &password)
{
    QJsonObject json;
    json["action"] = "login";
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    qDebug() << "Запрос на вход отправлен" << data;

    QMessageBox::information(this, "Регистрация", "Данные готовы к отправке");
    accept();

    /*
    QNetworkRequest request(QUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(request, data);
    */
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

        sendRegistrationRequest(username, password);
    } else {
        sendLoginRequest(username, password);
    }
}

void RegistrationDialog::onNetworkReply(QNetworkReply *reply) {
    if(reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        qDebug() << "Ответ сервера:" << response;

        QMessageBox::information(this, "Успех", "Операция выполнена успешно!");
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка:" + reply->errorString());
    }

    reply->deleteLater();
}

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}


