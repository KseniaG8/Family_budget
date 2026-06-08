#ifndef GROUPSDIALOG_H
#define GROUPSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QJsonArray>

namespace Ui {
class GroupsDialog;
}

class GroupsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GroupsDialog(QWidget *parent = nullptr, int userId = -1, const QString &baseUrl = "");
    ~GroupsDialog();

private slots:
    void onCreateGroupClicked();
    void onAddMemberClicked();
    void onGroupSelected();
    void onReplyFinished(QNetworkReply *reply);
    void onAddTransactionClicked();

private:
    Ui::GroupsDialog *ui;
    QNetworkAccessManager *networkManager;
    int currentUserId;
    int currentGroupId = -1;
    QString baseUrl;
    QLineEdit *userLoginEdit;

    void sendGetRequest(const QString &endpoint);
    void sendPostRequest(const QString &endpoint, const QJsonObject &data);
    void loadGroups();
    void fillGroupsList(const QJsonArray &groups);
    void loadGroupTransactions(int groupId);
    void fillTransactionsTable(const QJsonArray &transactions);
    void loadGroupBalance(int groupId);
    void updateBalance(double balance);
};

#endif // GROUPSDIALOG_H
