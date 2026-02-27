#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registrationdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    RegistrationDialog *dialog = new RegistrationDialog(this);
    dialog->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
