#include "../inc/Verify2FADialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

Verify2FADialog::Verify2FADialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Проверка 2FA");
    setFixedSize(300, 150);
    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("Введите 6-значный код:", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    codeEdit = new QLineEdit(this);
    codeEdit->setPlaceholderText("000000");
    codeEdit->setMaxLength(6);
    codeEdit->setAlignment(Qt::AlignCenter);
    layout->addWidget(codeEdit);

    auto *verifyBtn = new QPushButton("Подтвердить", this);
    verifyBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 5px;");
    layout->addWidget(verifyBtn);

    connect(verifyBtn, &QPushButton::clicked, this, &QDialog::accept);
}

QString Verify2FADialog::getCode() const { 
    return codeEdit->text(); 
}
