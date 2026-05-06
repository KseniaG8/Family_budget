#include "../inc/mainwindow.h"
#include "ui_mainwindow.h"
#include "../inc/registrationdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->update();

}

MainWindow::~MainWindow()
{
    delete ui;
}
