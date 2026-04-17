#include "../inc/mainwindow.h"
#include "ui_mainwindow.h"
#include "../inc/registrationdialog.h"

#include <QMessageBox>
#include <QDebug>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->update();

    RegistrationDialog *dialog = new RegistrationDialog(this);
    dialog->show();

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);

    connectToServer();

}

void MainWindow::connectToServer()
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к серверу:\n" + socket->errorString());
    });

    socket->connectToHost(serverAddress, serverPort);
}

void MainWindow::onConnected()
{
    qDebug() << "Подключено к серверу";
    refreshData();
}

void MainWindow::onReadyRead()
{
    QByteArray data = socket->readAll();
    QString responseStr = QString::fromUtf8(data);

    QJsonDocument doc = QJsonDocument::fromJson(responseStr.toUtf8());
    if (!doc.isNull()) {
        handleResponse(doc.object());
    }
}

void MainWindow::sendRequest(const QJsonObject &request)
{
    QJsonDocument doc(request);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void MainWindow::handleResponse(const QJsonObject &response)
{
    if (response["status"] == "error") {
        QMessageBox::warning(this, "Ошибка", response["message"].toString());
        return;
    }

    if (response.contains("balance")) {
        updateBalance(response["balance"].toDouble());
    }

    if (response.contains("transactions")) {
        fillTable(response["transactions"].toArray());
    }
}

void MainWindow::refreshData()
{
    if (currentUserId == -1) return;
    loadBalance();
    loadTransactions();
}

void MainWindow::loadBalance()
{
    QJsonObject request;
    request["action"] = "get_balance";
    request["user_id"] = currentUserId;
    sendRequest(request);
}

void MainWindow::loadTransactions()
{
    QJsonObject request;
    request["action"] = "get_transactions";
    request["user_id"] = currentUserId;
    sendRequest(request);
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
        request["action"] = "add_transaction";
        request["user_id"] = currentUserId;
        request["type"] = type;
        request["amount"] = amountSpin->value();
        request["category"] = categoryBox->currentText();

        sendRequest(request);

        refreshData();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
