#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>

class Setup2FADialog : public QDialog {
    Q_OBJECT
public:
    explicit Setup2FADialog(const QString& otpUri, const QString& secretText, QWidget *parent = nullptr) 
        : QDialog(parent), networkManager(new QNetworkAccessManager(this)) {
        
        setWindowTitle("Настройка 2FA");
        setFixedSize(350, 400);
        auto *layout = new QVBoxLayout(this);

        layout->addWidget(new QLabel("1. Скачайте Google Authenticator на телефон.", this));
        layout->addWidget(new QLabel("2. Отсканируйте этот QR-код:", this));

        qrLabel = new QLabel(this);
        qrLabel->setAlignment(Qt::AlignCenter);
        qrLabel->setText("Загрузка QR-кода...");
        layout->addWidget(qrLabel);

        QString qrApiUrl = "https://api.qrserver.com/v1/create-qr-code/?size=200x200&data=" + QUrl::toPercentEncoding(otpUri);
        QNetworkRequest request((QUrl(qrApiUrl)));
        
        connect(networkManager, &QNetworkAccessManager::finished, this, &Setup2FADialog::onQrDownloaded);
        networkManager->get(request);

        auto *secretLabel = new QLabel("Или введите этот ключ вручную:\n<b>" + secretText + "</b>", this);
        secretLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(secretLabel);

        auto *closeBtn = new QPushButton("Готово, я отсканировал", this);
        layout->addWidget(closeBtn);
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }

private slots:
    void onQrDownloaded(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray imageData = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(imageData);
            qrLabel->setPixmap(pixmap);
        } else {
            qrLabel->setText("Ошибка загрузки QR-кода.\nИспользуйте текстовый ключ.");
        }
        reply->deleteLater();
    }

private:
    QLabel *qrLabel;
    QNetworkAccessManager *networkManager;
};
