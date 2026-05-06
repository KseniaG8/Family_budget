#include "../inc/mainwindow.h"
#include "ui_mainwindow.h"
#include "../inc/alltransactionsdialog.h"
#include "../inc/registrationdialog.h"
#include "../inc/budgetlimitsdialog.h"

#include <QMessageBox>
#include <QDebug>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QUrlQuery>
#include <QDialogButtonBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Дата" << "Категория" << "Сумма");
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->update();

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplyFinished);

    RegistrationDialog *dialog = new RegistrationDialog(this);
    connect(dialog, &RegistrationDialog::loginSuccess, this, [this](int userId) {
        currentUserId = userId;
        refreshData();
    });
    dialog->exec();

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onAllTransactionsClicked);
    connect(ui->limitsButton, &QPushButton::clicked, this, &MainWindow::onLimitsClicked);
}

void MainWindow::sendGetRequest(const QString &endpoint)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->get(request);

    qDebug() << "GET:" << url.toString();
}

void MainWindow::sendPostRequest(const QString &endpoint, const QJsonObject &data)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    QByteArray postData = doc.toJson();

    networkManager->post(request, postData);

    qDebug() << "POST:" << url.toString() << postData;
}

void MainWindow::sendPutRequest(const QString &endpoint, const QJsonObject &data)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    QByteArray putData = doc.toJson();

    networkManager->put(request, putData);

    qDebug() << "PUT:" << url.toString() << putData;
}

void MainWindow::sendDeleteRequest(const QString &endpoint)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    networkManager->deleteResource(request);

    qDebug() << "DELETE:" << url.toString();
}

void MainWindow::refreshData()
{
    if (currentUserId == -1) return;
    loadBalance();
    loadTransactions();
}

void MainWindow::loadBalance()
{
    sendGetRequest(QString("/balance?user_id=%1").arg(currentUserId));
}

void MainWindow::loadTransactions()
{
    sendGetRequest(QString("/transactions?user_id=%1").arg(currentUserId));
}

void MainWindow::updateBalance(double balance)
{
    ui->balanceLabel->setText(QString::number(balance, 'f', 2));
}

void MainWindow::fillTable(const QJsonArray &transactions)
{
    ui->tableWidget->setRowCount(transactions.size());

    for (int i = 0; i < transactions.size(); ++i) {
        QJsonObject obj = transactions[i].toObject();

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(obj["date"].toString()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(obj["category"].toString()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(obj["amount"].toDouble())));
    }

    ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::onAddButtonClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить операцию");

    QFormLayout layout(&dialog);

    QComboBox *typeBox = new QComboBox;
    typeBox->addItems({"Расход", "Доход"});
    layout.addRow("Тип:", typeBox);

    QComboBox *categoryBox = new QComboBox;
    categoryBox->addItems({"Еда", "Транспорт", "Развлечения", "Зарплата", "Другое"});
    layout.addRow("Категория:", categoryBox);

    QDoubleSpinBox *amountSpin = new QDoubleSpinBox;
    amountSpin->setRange(0, 1000000);
    layout.addRow("Сумма:", amountSpin);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString type = typeBox->currentText() == "Доход" ? "income" : "expense";

        QJsonObject request;
        request["user_id"] = currentUserId;
        request["type"] = type;
        request["amount"] = amountSpin->value();
        request["category"] = categoryBox->currentText();

        sendPostRequest("/transactions", request);

        refreshData();

        if (type == "expense") {
            QString category = categoryBox->currentText();
            checkBudgetLimit(category);
        }
    }
}

void MainWindow::onAllTransactionsClicked()
{
    AllTransactionsDialog *dialog = new AllTransactionsDialog(this, currentUserId, baseUrl);
    dialog->setAttribute(Qt::WA_DeleteOnClose);  // удалится при закрытии
    dialog->show();  // не блокирует главное окно (не modal)
}

void MainWindow::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "HTTP Error:" << reply->errorString();
        QMessageBox::warning(this, "Ошибка", "Ошибка запроса:\n" + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QString url = reply->url().toString();
    qDebug() << "Response from" << url << ":" << responseData;

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON";
        reply->deleteLater();
        return;
    }

    QJsonObject obj = doc.object();

    // Проверяем статус ошибки
    if (obj.contains("error")) {
        QMessageBox::warning(this, "Ошибка", obj["error"].toString());
        reply->deleteLater();
        return;
    }

    // Обрабатываем в зависимости от URL
    if (url.contains("/balance")) {
        double balance = obj["balance"].toDouble();
        updateBalance(balance);
    }
    else if (url.contains("/transactions") && !url.contains("/transaction")) {
        QJsonArray transactions = obj["transactions"].toArray();
        fillTable(transactions);
    }
    else if (url.contains("/limits/check")) {
        if (obj.contains("error")) return;

        double limit = obj["limit"].toDouble();
        double spent = obj["spent"].toDouble();
        double remaining = obj["remaining"].toDouble();
        QString category = pendingBudgetCategory;

        if (spent > limit) {
            QMessageBox::warning(this, "Превышение лимита",
                                 QString("Вы превысили лимит по категории '%1'!\n"
                                         "Лимит: %2\nПотрачено: %3\nОсталось: %4")
                                     .arg(category).arg(limit).arg(spent).arg(remaining));
        } else if (spent > limit * 0.8) {
            QMessageBox::information(this, "Внимание",
                                     QString("По категории '%1' осталось менее 20%% лимита.\n"
                                             "Осталось: %2")
                                         .arg(category).arg(remaining));
        }
    }

    reply->deleteLater();
}

void MainWindow::onLimitsClicked()
{
    BudgetLimitsDialog *dialog = new BudgetLimitsDialog(this, currentUserId, baseUrl);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

void MainWindow::checkBudgetLimit(const QString &category)
{
    pendingBudgetCategory = category;

    sendGetRequest(QString("/limits/check?user_id=%1&category=%2").arg(currentUserId).arg(category));
}

MainWindow::~MainWindow()
{
    delete ui;
}
