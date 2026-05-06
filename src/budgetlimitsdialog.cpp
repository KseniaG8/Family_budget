#include "../inc/budgetlimitsdialog.h"
#include "ui_budgetlimitsdialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

BudgetLimitsDialog::BudgetLimitsDialog(QWidget *parent, int userId, const QString &baseUrl)
    : QDialog(parent)
    , ui(new Ui::BudgetLimitsDialog)
    , currentUserId(userId)
    , baseUrl(baseUrl)
{
    ui->setupUi(this);
    setWindowTitle("Управление лимитами");

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &BudgetLimitsDialog::onReplyFinished);

    connect(ui->saveButton, &QPushButton::clicked, this, &BudgetLimitsDialog::onSaveClicked);
    connect(ui->checkButton, &QPushButton::clicked, this, &BudgetLimitsDialog::onCheckClicked);
    connect(ui->categoryCombo, &QComboBox::currentTextChanged, this, &BudgetLimitsDialog::loadCurrentLimit);
}

BudgetLimitsDialog::~BudgetLimitsDialog()
{
    delete ui;
}

void BudgetLimitsDialog::sendPostRequest(const QString &endpoint, const QJsonObject &data)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    networkManager->post(request, doc.toJson());
}

void BudgetLimitsDialog::sendGetRequest(const QString &endpoint)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->get(request);
}

void BudgetLimitsDialog::loadCurrentLimit(const QString &category)
{
    sendGetRequest(QString("/limits/check?user_id=%1&category=%2").arg(currentUserId).arg(category));
}

void BudgetLimitsDialog::onSaveClicked()
{
    QString category = ui->categoryCombo->currentText();
    double limit = ui->limitSpin->value();

    QJsonObject request;
    request["user_id"] = currentUserId;
    request["category"] = category;
    request["limit"] = limit;

    sendPostRequest("/limits", request);
}

void BudgetLimitsDialog::onCheckClicked()
{
    QString category = ui->categoryCombo->currentText();
    sendGetRequest(QString("/limits/check?user_id=%1&category=%2").arg(currentUserId).arg(category));
}

void BudgetLimitsDialog::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        ui->statusLabel->setText("Ошибка: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QString url = reply->url().toString();

    if (url.contains("/limits") && reply->operation() == QNetworkAccessManager::PostOperation) {
        if (obj.contains("error")) {
            QMessageBox::warning(this, "Ошибка", obj["error"].toString());
        } else {
            QMessageBox::information(this, "Успех", "Лимит сохранён!");
            loadCurrentLimit(ui->categoryCombo->currentText());
        }
    }
    else if (url.contains("/limits/check")) {
        if (obj.contains("error")) {
            ui->statusLabel->setText("Ошибка: " + obj["error"].toString());
        } else {
            double limit = obj["limit"].toDouble();
            double spent = obj["spent"].toDouble();
            double remaining = obj["remaining"].toDouble();

            ui->statusLabel->setText(
                QString("Лимит: %1\nПотрачено: %2\nОсталось: %3")
                    .arg(limit).arg(spent).arg(remaining)
                );

            ui->limitSpin->setValue(limit);
        }
    }

    reply->deleteLater();
}
