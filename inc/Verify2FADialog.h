#pragma once
#include <QDialog>
#include <QLineEdit>

class Verify2FADialog : public QDialog {
    Q_OBJECT
public:
    explicit Verify2FADialog(QWidget *parent = nullptr);
    QString getCode() const;
private:
    QLineEdit *codeEdit;
};
