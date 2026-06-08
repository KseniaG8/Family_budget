#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QChartView>
#include <QPieSeries>
#include <QBarSeries>

QT_BEGIN_NAMESPACE
namespace Ui { class StatisticsDialog; }
QT_END_NAMESPACE

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget *parent = nullptr, int userId = -1, const QString &baseUrl = "");
    ~StatisticsDialog();

private slots:
    void onRefreshClicked();
    void onChartTypeChanged();
    void onReplyFinished(QNetworkReply *reply);

private:
    Ui::StatisticsDialog *ui;
    QNetworkAccessManager *networkManager;
    int currentUserId;
    QString baseUrl;

    void sendGetRequest(const QString &endpoint);
    void loadCategoryStats();
    void parseAndDisplay(const QJsonArray &data);

    void showPieChart(const QJsonArray &data);
    void showBarChart(const QJsonArray &data);
    void fillTable(const QJsonArray &data);
    void updateTotalLabel(const QJsonArray &data);
};

#endif // STATISTICSDIALOG_H
