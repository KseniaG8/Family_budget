#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddButtonClicked();
    void onReplyFinished(QNetworkReply *reply);
    void refreshData();
    void onAllTransactionsClicked();
    void onLimitsClicked();
    void checkBudgetLimit(const QString &category);

private:
    QNetworkAccessManager *networkManager;
    int currentUserId = -1;
    QString baseUrl = "http://localhost:8080";
    QString pendingBudgetCategory;

    void sendGetRequest(const QString &endpoint);
    void sendPostRequest(const QString &endpoint, const QJsonObject &data);
    void sendPutRequest(const QString &endpoint, const QJsonObject &data);
    void sendDeleteRequest(const QString &endpoint);

    void handleRegisterResponse(const QJsonObject &response);
    void handleLoginResponse(const QJsonObject &response);

    void loadBalance();
    void loadTransactions();
    void fillTable(const QJsonArray &transactions);
    void updateBalance(double balance);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
