#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class RegistrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationDialog(QWidget *parent = nullptr);
    ~RegistrationDialog();
;

private slots:
    void onSignInClicked();
    void onSignUpClicked();

private:
    Ui::Dialog *ui;
    void setActiveTab(const QString &tab);

};

#endif // REGISTRATIONDIALOG_H
