#ifndef BUDGETLIMITSDIALOG_H
#define BUDGETLIMITSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class BudgetLimitsDialog;
}

class BudgetLimitsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BudgetLimitsDialog(QWidget *parent = nullptr, int userIs = -1, const QString &baseUrl = "");
    ~BudgetLimitsDialog();

private slots:
    void onSavedClicked();
    void onCheckClicked();
    void onReplyFinished(QNetworkReply *reply);

private:
    Ui::BudgetLimitsDialog *ui;
    QNetworkAccessManager *networkManager;
    int currentUserId;
    QString baseUrl;

    void sendPostRequest(const QString &endpoint, const QJsonObject &data);
    void sendGetRequest(const QString &endpoint);
    void loadCurrentLimit(const QString &category);
};

#endif // BUDGETLIMITSDIALOG_H
