#ifndef MONTHLYDIALOG_H
#define MONTHLYDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>

class MonthlyDialog : public QDialog {
    Q_OBJECT

public:
    explicit MonthlyDialog(QWidget *parent, const QVector<int> &values);
    QVector<int> getMonthlyValues() const;

private:
    QLineEdit *monthlyInputs[12]; // Change to QLineEdit*
};

#endif // MONTHLYDIALOG_H

