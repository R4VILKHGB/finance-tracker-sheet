#include "MonthlyDialog.h"
#include <QIntValidator>
#include <QLineEdit>
#include <QGroupBox>

MonthlyDialog::MonthlyDialog(QWidget *parent, const QVector<int> &values)
    : QDialog(parent) {
    setWindowTitle("月次内訳"); // "Monthly Breakdown"

    // QVBoxLayout *layout = new QVBoxLayout(this);
    // QGridLayout *grid = new QGridLayout();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // GroupBox for month inputs
    QGroupBox *monthGroupBox = new QGroupBox("月別詳細入力"); // "Monthly Details Input"
    QGridLayout *grid = new QGridLayout();

    const QStringList monthNames = {"1月", "2月", "3月", "4月", "5月", "6月",
                                    "7月", "8月", "9月", "10月", "11月", "12月"};

    for (int i = 0; i < 12; ++i) {
        QLabel *label = new QLabel(monthNames[i]);
        label->setStyleSheet("font-weight: bold; color: #333;"); // Bold labels with subtle color

        monthlyInputs[i] = new QLineEdit();
        monthlyInputs[i]->setText(QString::number(values[i])); // Load existing value
        monthlyInputs[i]->setValidator(new QIntValidator(0, 1000000, this)); // Ensure only integers
        monthlyInputs[i]->setStyleSheet("padding: 5px; border: 1px solid #ccc; border-radius: 4px;");

        // Add label and input field to the grid
        grid->addWidget(label, i / 3, (i % 3) * 2); // Row, Column for label
        grid->addWidget(monthlyInputs[i], i / 3, (i % 3) * 2 + 1); // Row, Column for input
    }

    monthGroupBox->setLayout(grid);
    mainLayout->addWidget(monthGroupBox);

    // OK and Cancel buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("キャンセル");

    // OK Button Stylesheet
    okButton->setStyleSheet(
        "QPushButton {"
        "    padding: 8px 20px;"
        "    background-color: #4CAF50;"    // Green background
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45A049;"    // Slightly lighter green on hover
        "}"
        "QPushButton:pressed {"
        "    background-color: #388E3C;"    // Dark green when clicked
        "}"
        "QPushButton:focus {"
        "    outline: 2px solid #76FF03;"   // Highlight outline when focused
        "}"
        );

    // Cancel Button Stylesheet
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    padding: 8px 20px;"
        "    background-color: #f44336;"    // Red background
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #E57373;"    // Lighter red on hover
        "}"
        "QPushButton:pressed {"
        "    background-color: #D32F2F;"    // Dark red when clicked
        "}"
        "QPushButton:focus {"
        "    outline: 2px solid #FF5252;"   // Highlight outline when focused
        "}"
        );

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
}

QVector<int> MonthlyDialog::getMonthlyValues() const {
    QVector<int> values(12);
    for (int i = 0; i < 12; ++i) {
        values[i] = monthlyInputs[i]->text().toInt();
    }
    return values;
}
