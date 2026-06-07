#pragma once
#include <QDialog>
#include <QLabel>
#include <QNetworkAccessManager>

class Setup2FADialog : public QDialog {
    Q_OBJECT
public:
    explicit Setup2FADialog(const QString& otpUri, const QString& secretText, QWidget *parent = nullptr);
private slots:
    void onQrDownloaded(class QNetworkReply *reply);
private:
    QLabel *qrLabel;
    QNetworkAccessManager *networkManager;
};
