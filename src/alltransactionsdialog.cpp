#include "../inc/alltransactionsdialog.h"
#include "ui_alltransactionsdialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

AllTransactionsDialog::AllTransactionsDialog(QWidget *parent, int userId, QTcpSocket *existingSocket)
    : QDialog(parent)
    , ui(new Ui::AllTransactionsDialog)
    , currentUserId(userId)
    , socket(existingSocket)
{
    ui->setupUi(this);

    setWindowTitle("Все операции");
    setModal(false);  // не блокирует главное окно

    // Настраиваем таблицу
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Дата" << "Категория" << "Сумма");

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
    this->close();  // или accept() / reject()
}

void AllTransactionsDialog::onRefreshClicked()
{
    loadTransactions();
}

void AllTransactionsDialog::loadTransactions()
{
    QJsonObject request;
    request["action"] = "get_transactions";
    request["user_id"] = currentUserId;

    QJsonDocument doc(request);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void AllTransactionsDialog::fillTable(const QJsonArray &transactions)
{
    ui->tableWidget->setRowCount(transactions.size());

    for (int i = 0; i < transactions.size(); ++i) {
        QJsonObject obj = transactions[i].toObject();
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(obj["date"].toString()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(obj["category"].toString()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(obj["amount"].toDouble())));
    }
}
