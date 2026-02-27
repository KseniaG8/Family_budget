#include "registrationdialog.h"
#include "ui_registrationdialog.h"
#include <QPushButton>

RegistrationDialog::RegistrationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->setStyleSheet("Qlabel:focus { outline: none; border: none; background-color: transparent; }");

    // Подключаем клики по надписям
    connect(ui->signInLabel, &QLabel::linkActivated, this, &RegistrationDialog::onSignInClicked);
    connect(ui->signUpLabel, &QLabel::linkActivated, this, &RegistrationDialog::onSignUpClicked);

    // По умолчанию показываем SIGN IN
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

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}
