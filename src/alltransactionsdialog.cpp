#include "../inc/alltransactionsdialog.h"
#include "ui_alltransactionsdialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QDebug>

AllTransactionsDialog::AllTransactionsDialog(QWidget *parent, int userId, const QString &baseUrl)
    : QDialog(parent)
    , ui(new Ui::AllTransactionsDialog)
    , currentUserId(userId)
    , baseUrl(baseUrl)
{
    ui->setupUi(this);

    setWindowTitle("Все операции");
    setModal(false);  // не блокирует главное окно

    // Настраиваем таблицу
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Дата" << "Категория" << "Сумма");
    ui->tableWidget->verticalHeader()->setVisible(false);

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &AllTransactionsDialog::onReplyFinished);

    // Подключаем кнопки
    connect(ui->refreshButton, &QPushButton::clicked, this, &AllTransactionsDialog::onRefreshClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &AllTransactionsDialog::onBackClicked);

    // Загружаем данные
    loadTransactions();
}

AllTransactionsDialog::~AllTransactionsDialog()
{
    delete ui;
}

void AllTransactionsDialog::onBackClicked()
{
    close();  // или accept() / reject()
}

void AllTransactionsDialog::onRefreshClicked()
{
    loadTransactions();
}

void AllTransactionsDialog::loadTransactions()
{
    QUrl url(QString("%1/transactions?user_id=%2").arg(baseUrl).arg(currentUserId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->get(request);

    qDebug() << "GET:" << url.toString();
}

void AllTransactionsDialog::fillTable(const QJsonArray &transactions)
{
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(30);

    ui->tableWidget->setFixedHeight(10);

    ui->tableWidget->setRowCount(transactions.size());

    for (int i = 0; i < transactions.size(); ++i) {
        QJsonObject obj = transactions[i].toObject();
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(obj["date"].toString()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(obj["category"].toString()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(obj["amount"].toDouble())));
    }

    ui->tableWidget->resizeColumnsToContents();
}

void AllTransactionsDialog::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("transactions")) {
            fillTable(obj["transactions"].toArray());
        } else if (obj.contains("error")) {
            QMessageBox::warning(this, "Ошибка", obj["error"].toString());
        }
    }

    reply->deleteLater();
}
