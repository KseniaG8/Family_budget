#ifndef GOALSDIALOG_H
#define GOALSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QJsonArray>

namespace Ui {
class GoalsDialog;
}

class GoalsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoalsDialog(QWidget *parent = nullptr, int userId = -1, const QString &baseUrl = "");
    ~GoalsDialog();

private slots:
    void onAddGoalClicked();
    void onContributeClicked();
    void onRefreshClicked();
    void onReplyFinished(QNetworkReply *reply);

private:
    Ui::GoalsDialog *ui;
    QNetworkAccessManager *networkManager;
    int currentUserId;
    QString baseUrl;

    void sendGetRequest(const QString &endpoint);
    void sendPostRequest(const QString &endpoint, const QJsonObject &data);
    void sendPutRequest(const QString &endpoint, const QJsonObject &data);
    void loadGoals();
    void fillTable(const QJsonArray &goals);
};

#endif // GOALSDIALOG_H
