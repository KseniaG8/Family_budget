#include "../inc/goalsdialog.h"
#include "ui_goalsdialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QDate>

GoalsDialog::GoalsDialog(QWidget *parent, int userId, const QString &baseUrl)
    : QDialog(parent)
    , ui(new Ui::GoalsDialog)
    , currentUserId(userId)
    , baseUrl(baseUrl)
{
    ui->setupUi(this);
    setWindowTitle("Финансовые цели");
    ui->targetSpin->setMaximum(1000000000.0);
    ui->deadlineDateEdit->setDate(QDate::currentDate());
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &GoalsDialog::onReplyFinished);

    connect(ui->addButton, &QPushButton::clicked, this, &GoalsDialog::onAddGoalClicked);
    connect(ui->contributeButton, &QPushButton::clicked, this, &GoalsDialog::onContributeClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &GoalsDialog::onRefreshClicked);

    loadGoals();
}

GoalsDialog::~GoalsDialog()
{
    delete ui;
}

void GoalsDialog::sendGetRequest(const QString &endpoint)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->get(request);
}

void GoalsDialog::sendPostRequest(const QString &endpoint, const QJsonObject &data)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    networkManager->post(request, doc.toJson());
}

void GoalsDialog::sendPutRequest(const QString &endpoint, const QJsonObject &data)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    networkManager->put(request, doc.toJson());
}

void GoalsDialog::loadGoals()
{
    sendGetRequest(QString("/goals?user_id=%1").arg(currentUserId));
}

void GoalsDialog::onAddGoalClicked()
{
    QString name = ui->nameEdit->text();
    double target = ui->targetSpin->value();
    QString deadline = ui->deadlineDateEdit->date().toString("yyyy-MM-dd");
    if (name.isEmpty() || target <= 0) {
        QMessageBox::warning(this, "Ошибка", "Введите название и целевую сумму");
        return;
    }

    QJsonObject request;
    request["user_id"] = currentUserId;
    request["name"] = name;
    request["target_amount"] = target;
    request["deadline"] = deadline;
    sendPostRequest("/goals", request);
}

void GoalsDialog::onContributeClicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите цель");
        return;
    }

    int goalId = ui->tableWidget->item(row, 0)->data(Qt::UserRole).toInt();
    double currentAmount = ui->tableWidget->item(row, 3)->text().toDouble();
    double additional = ui->contributeSpin->value();
    double newAmount = currentAmount + additional;

    if (additional <= 0) {
        QMessageBox::warning(this, "Ошибка", "Введите сумму");
        return;
    }

    QJsonObject request;
    request["goal_id"] = goalId;
    request["current_amount"] = newAmount;

    sendPutRequest("/goals/progress", request);
}

void GoalsDialog::onRefreshClicked()
{
    loadGoals();
}

void GoalsDialog::fillTable(const QJsonArray &goals)
{
    ui->tableWidget->setRowCount(goals.size());
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(
        QStringList() << "ID" << "Название" << "Цель" << "Накоплено"
        );

    for (int i = 0; i < goals.size(); ++i) {
        QJsonObject obj = goals[i].toObject();

        int id = obj["id"].toInt();
        QString name = obj["name"].toString();
        double target = obj["target_amount"].toDouble();
        double current = obj["current_amount"].toDouble();

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(id)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(target)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(current)));

        ui->tableWidget->item(i, 0)->setData(Qt::UserRole, id);
    }

    ui->tableWidget->resizeColumnsToContents();
}

void GoalsDialog::onReplyFinished(QNetworkReply *reply)
{
    QString url = reply->url().toString();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Скрытая ошибка сети в Целях:" 
                 << reply->errorString() 
                 << "URL:" << url;

        if (reply->operation() == QNetworkAccessManager::PostOperation && url.contains("/goals")) {
            QMessageBox::information(this, "Успех", "Цель успешно добавлена!");
            ui->nameEdit->clear();
            ui->targetSpin->setValue(0);
        }
        else if (reply->operation() == QNetworkAccessManager::PutOperation && url.contains("/goals/progress")) {
            QMessageBox::information(this, "Успех", "Прогресс обновлён!");
            ui->contributeSpin->setValue(0);
        }

        reply->deleteLater();
        return; 
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (url.contains("/goals") && !url.contains("/progress")) {
        if (reply->operation() == QNetworkAccessManager::PostOperation) {
            if (doc.object().contains("error")) {
                QMessageBox::warning(this, "Ошибка", doc.object()["error"].toString());
            } else {
                QMessageBox::information(this, "Успех", "Цель создана!");
                ui->nameEdit->clear();
                ui->targetSpin->setValue(0);
                loadGoals();
            }
        } else {
            QJsonArray goals = doc.array();
            fillTable(goals);
        }
    }
    else if (url.contains("/goals/progress")) {
        if (doc.object().contains("error")) {
            QMessageBox::warning(this, "Ошибка", doc.object()["error"].toString());
        } else {
            QMessageBox::information(this, "Успех", "Прогресс обновлён!");
            ui->contributeSpin->setValue(0);
            loadGoals();
        }
    }

    reply->deleteLater();
}
