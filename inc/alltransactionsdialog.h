#ifndef ALLTRANSDIALOG_H
#define ALLTRANSDIALOG_H

#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class AllTransactionsDialog; }
QT_END_NAMESPACE

class AllTransactionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllTransactionsDialog(QWidget *parent = nullptr, int userId = -1, QTcpSocket *existingSocket = nullptr);
    ~AllTransactionsDialog();

private slots:
    void onRefreshClicked();
    void onBackClicked();

private:
    Ui::AllTransactionsDialog *ui;
    QTcpSocket *socket;
    int currentUserId;

    void loadTransactions();
    void fillTable(const QJsonArray &transactions);
};

#endif
