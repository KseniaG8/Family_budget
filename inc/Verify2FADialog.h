#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class Verify2FADialog : public QDialog {
    Q_OBJECT
public:
    explicit Verify2FADialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Проверка 2FA");
        setFixedSize(300, 150);

        auto *layout = new QVBoxLayout(this);
        
        auto *label = new QLabel("Введите 6-значный код\nиз Google Authenticator:", this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        codeEdit = new QLineEdit(this);
        codeEdit->setPlaceholderText("000000");
        codeEdit->setMaxLength(6);
        codeEdit->setAlignment(Qt::AlignCenter);
        
        QFont font = codeEdit->font();
        font.setPointSize(16);
        font.setLetterSpacing(QFont::AbsoluteSpacing, 5);
        codeEdit->setFont(font);
        layout->addWidget(codeEdit);

        auto *verifyBtn = new QPushButton("Подтвердить", this);
        verifyBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 5px;");
        layout->addWidget(verifyBtn);

        connect(verifyBtn, &QPushButton::clicked, this, &QDialog::accept);
    }

    QString getCode() const { 
        return codeEdit->text(); 
    }

private:
    QLineEdit *codeEdit;
};
