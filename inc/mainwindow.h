#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
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
    void onConnected();
    void onReadyRead();
    void refreshData();

private:
    QTcpSocket *socket;
    int currentUserId = -1;
    QString serverAddress = "127.0.0.1";
    quint16 serverPort = 8080;

    void connectToServer();
    void sendRequest(const QJsonObject &request);
    void handleResponse(const QJsonObject &response);
    void loadBalance();
    void loadTransactions();
    void fillTable(const QJsonArray &transactions);
    void updateBalance(double balance);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
