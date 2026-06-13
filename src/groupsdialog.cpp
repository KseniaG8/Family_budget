#include "../inc/groupsdialog.h"
#include "ui_groupsdialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>

GroupsDialog::GroupsDialog(QWidget *parent, int userId, const QString &baseUrl)
    : QDialog(parent)
    , ui(new Ui::GroupsDialog)
    , currentUserId(userId)
    , baseUrl(baseUrl)
{
    ui->setupUi(this);
    setWindowTitle("Управление группами");

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &GroupsDialog::onReplyFinished);

    connect(ui->createButton, &QPushButton::clicked, this, &GroupsDialog::onCreateGroupClicked);
    connect(ui->addMemberButton, &QPushButton::clicked, this, &GroupsDialog::onAddMemberClicked);
    connect(ui->groupsList, &QListWidget::itemClicked, this, &GroupsDialog::onGroupSelected);
    connect(ui->addTransactionButton, &QPushButton::clicked, this, &GroupsDialog::onAddTransactionClicked);

    loadGroups();
}

GroupsDialog::~GroupsDialog()
{
    delete ui;
}

void GroupsDialog::sendGetRequest(const QString &endpoint)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->get(request);
}

void GroupsDialog::sendPostRequest(const QString &endpoint, const QJsonObject &data)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(data);
    networkManager->post(request, doc.toJson());
}

void GroupsDialog::loadGroups()
{
    sendGetRequest(QString("/groups?user_id=%1").arg(currentUserId));
}

void GroupsDialog::fillGroupsList(const QJsonArray &groups)
{
    ui->groupsList->clear();

    for (const QJsonValue &val : groups) {
        QJsonObject obj = val.toObject();
        QString name = obj["name"].toString();
        int id = obj["id"].toInt();

        QListWidgetItem *item = new QListWidgetItem(name);
        item->setData(Qt::UserRole, id);
        ui->groupsList->addItem(item);
    }
}

void GroupsDialog::onGroupSelected()
{
    QListWidgetItem *item = ui->groupsList->currentItem();
    if (!item) return;

    currentGroupId = item->data(Qt::UserRole).toInt();

    loadGroupTransactions(currentGroupId);
    loadGroupBalance(currentGroupId);
}

void GroupsDialog::loadGroupTransactions(int groupId)
{
    sendGetRequest(QString("/groups/transactions?group_id=%1").arg(groupId));
}

void GroupsDialog::loadGroupBalance(int groupId)
{
    sendGetRequest(QString("/groups/balance?group_id=%1").arg(groupId));
}

void GroupsDialog::fillTransactionsTable(const QJsonArray &transactions)
{
    ui->transactionsTable->setRowCount(transactions.size());
    ui->transactionsTable->setColumnCount(3);
    ui->transactionsTable->setHorizontalHeaderLabels(
        QStringList() << "Пользователь" << "Категория" << "Сумма"
        );

    for (int i = 0; i < transactions.size(); ++i) {
        QJsonObject obj = transactions[i].toObject();

        QString userName = obj["user_name"].toString();
        QString category = obj["category"].toString();
        double amount = obj["amount"].toDouble();

        ui->transactionsTable->setItem(i, 0, new QTableWidgetItem(userName));
        ui->transactionsTable->setItem(i, 1, new QTableWidgetItem(category));
        ui->transactionsTable->setItem(i, 2, new QTableWidgetItem(QString::number(amount)));
    }

    ui->transactionsTable->resizeColumnsToContents();
}

void GroupsDialog::updateBalance(double balance)
{
    ui->balanceLabel->setText(QString("Баланс группы: %1").arg(balance));
}

void GroupsDialog::onCreateGroupClicked()
{
    QString groupName = ui->groupNameEdit->text();
    if (groupName.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Введите название группы");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    QJsonObject request;
    request["name"] = groupName;
    request["owner_id"] = currentUserId;

    sendPostRequest("/groups", request);
}

void GroupsDialog::onAddMemberClicked()
{
    if (currentGroupId == -1) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Сначала выберите группу");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    QString userLogin = ui->userLoginEdit->text();
    if (userLogin.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Введите логин пользователя");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    QJsonObject request;
    request["group_id"] = currentGroupId;
    request["login"] = userLogin;

    sendPostRequest("/groups/members", request);
}

void GroupsDialog::onAddTransactionClicked()
{
    if (currentGroupId == -1) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Сначала выберите группу");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    QString type = ui->typeCombo->currentText() == "Доход" ? "income" : "expense";
    QString category = ui->categoryCombo->currentText();
    double amount = ui->amountSpin->value();

    if (amount <= 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Введите сумму");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    QJsonObject request;
    request["group_id"] = currentGroupId;
    request["user_id"] = currentUserId;
    request["type"] = type;
    request["amount"] = amount;
    request["category"] = category;

    sendPostRequest("/groups/transactions", request);
}

void GroupsDialog::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Ошибка сети: " + reply->errorString());
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString url = reply->url().toString();

    if (url.contains("/groups") && !url.contains("/members") && !url.contains("/transactions") && !url.contains("/balance")) {
        if (reply->operation() == QNetworkAccessManager::PostOperation) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Успех");
            msgBox.setText("Группа создана!");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
            ui->groupNameEdit->clear();
            loadGroups();
        } else {
            QJsonArray groups = doc.array();
            fillGroupsList(groups);
        }
    }
    else if (url.contains("/groups/members")) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Успех");
        msgBox.setText("Участник добавлен!");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        ui->userLoginEdit->clear();
    }
    else if (url.contains("/groups/transactions") && !url.contains("/balance")) {
        if (reply->operation() == QNetworkAccessManager::PostOperation) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Успех");
            msgBox.setText("Транзакция добавлена!");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
            ui->amountSpin->setValue(0);
            if (currentGroupId != -1) {
                loadGroupTransactions(currentGroupId);
                loadGroupBalance(currentGroupId);
            }
        } else {
            QJsonArray transactions = doc.array();
            fillTransactionsTable(transactions);
        }
    }
    else if (url.contains("/groups/balance")) {
        double balance = doc.object()["balance"].toDouble();
        updateBalance(balance);
    }

    reply->deleteLater();
}
