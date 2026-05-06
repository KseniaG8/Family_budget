#ifndef ALLTRANSDIALOG_H
#define ALLTRANSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class AllTransactionsDialog; }
QT_END_NAMESPACE

class AllTransactionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllTransactionsDialog(QWidget *parent = nullptr, int userId = -1, const QString &baseUrl = "");
    ~AllTransactionsDialog();

private slots:
    void onRefreshClicked();
    void onBackClicked();
    void onReplyFinished(QNetworkReply *reply);

private:
    Ui::AllTransactionsDialog *ui;
    QNetworkAccessManager *networkManager;
    int currentUserId;
    QString baseUrl;

    void loadTransactions();
    void fillTable(const QJsonArray &transactions);
};

#endif
