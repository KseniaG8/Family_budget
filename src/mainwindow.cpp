#include "../inc/mainwindow.h"
#include "ui_mainwindow.h"
#include "../inc/alltransactionsdialog.h"
#include "../inc/registrationdialog.h"
#include "../inc/budgetlimitsdialog.h"
#include "../inc/goalsdialog.h"
#include "../inc/setup2FAdialog.h"
#include "../inc/Verify2FADialog.h"

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

    QPushButton *setup2faBtn = new QPushButton("Включить 2FA", this);
    setup2faBtn->setGeometry(10, 10, 120, 30); 
    connect(setup2faBtn, &QPushButton::clicked, this, [this]() {
        QJsonObject request;
        request["user_id"] = currentUserId; 
        request["login"] = "User_" + QString::number(currentUserId); 
        sendPostRequest("/2fa/setup", request);
    });

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Дата" << "Категория" << "Сумма");
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->update();

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplyFinished);

    // ВНИМАНИЕ: Я закомментировал этот блок, так как окно авторизации 
    // уже вызывается и успешно работает в client_main.cpp!
    /*
    RegistrationDialog *dialog = new RegistrationDialog(this);
    connect(dialog, &RegistrationDialog::loginSuccess, this, [this](int userId) {
        currentUserId = userId;
        refreshData();
    });
    dialog->exec();
    */

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onAllTransactionsClicked);
    connect(ui->limitsButton, &QPushButton::clicked, this, &MainWindow::onLimitsClicked);
    connect(ui->goalsButton, &QPushButton::clicked, this, &MainWindow::onGoalsButtonClicked);
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