#include "../inc/statisticsdialog.h"
#include "ui_statisticsdialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QtCharts>

StatisticsDialog::StatisticsDialog(QWidget *parent, int userId, const QString &baseUrl)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , currentUserId(userId)
    , baseUrl(baseUrl)
{
    ui->setupUi(this);
    setWindowTitle("Статистика расходов");

    ui->chartView->setRenderHint(QPainter::Antialiasing);

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &StatisticsDialog::onReplyFinished);

    connect(ui->refreshButton, &QPushButton::clicked, this, &StatisticsDialog::onRefreshClicked);
    connect(ui->chartTypeCombo, &QComboBox::currentTextChanged, this, &StatisticsDialog::onChartTypeChanged);

    loadCategoryStats();
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::sendGetRequest(const QString &endpoint)
{
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->get(request);
}

void StatisticsDialog::loadCategoryStats()
{
    QString endpoint = QString("/analytics/categories?user_id=%1").arg(currentUserId);
    sendGetRequest(endpoint);
}

void StatisticsDialog::onRefreshClicked()
{
    loadCategoryStats();
}

void StatisticsDialog::onChartTypeChanged()
{
    onRefreshClicked();
}

void StatisticsDialog::onReplyFinished(QNetworkReply *reply)
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

    if (doc.isNull() || !doc.isArray()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Неверный формат данных");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        reply->deleteLater();
        return;
    }

    QJsonArray categories = doc.array();

    double total = 0;
    for (const QJsonValue &val : categories) {
        total += val.toObject()["amount"].toDouble();
    }
    ui->totalLabel->setText(QString("Общая сумма расходов: %1").arg(total));

    QString chartType = ui->chartTypeCombo->currentText();
    if (chartType == "Круговая") {
        showPieChart(categories);
    } else {
        showBarChart(categories);
    }

    reply->deleteLater();
}

void StatisticsDialog::showPieChart(const QJsonArray &data)
{
    QPieSeries *series = new QPieSeries();

    for (const QJsonValue &val : data) {
        QJsonObject obj = val.toObject();
        QString category = obj["category"].toString();
        double amount = obj["amount"].toDouble();

        if (amount > 0) {
            QPieSlice *slice = series->append(category, amount);
            slice->setLabelVisible(true);
            slice->setLabel(QString("%1: %2").arg(category).arg(amount));
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Расходы по категориям (всё время)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    ui->chartView->setChart(chart);
}

void StatisticsDialog::showBarChart(const QJsonArray &data)
{
    QBarSeries *series = new QBarSeries();
    QBarSet *barSet = new QBarSet("Расходы");
    QStringList categories;

    for (const QJsonValue &val : data) {
        QJsonObject obj = val.toObject();
        categories << obj["category"].toString();
        *barSet << obj["amount"].toDouble();
    }

    series->append(barSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Расходы по категориям (всё время)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Сумма");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->chartView->setChart(chart);
}
