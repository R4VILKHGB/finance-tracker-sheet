#include "MainWindow.h"
#include "MonthlyDialog.h"
#include <QApplication>
#include <QHeaderView>
#include <QInputDialog>
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), tabWidget(new QTabWidget(this)) {

    // Menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("ファイル");
    QAction *importAction = fileMenu->addAction("CSVインポート");
    QAction *exportAction = fileMenu->addAction("CSVエクスポート");
    QAction *exitAction = fileMenu->addAction("終了");

    QMenu *companyMenu = menuBar->addMenu("会社");
    QAction *addCompanyAction = companyMenu->addAction("会社追加");
    QAction *editCompanyNameAction = companyMenu->addAction("会社名編集");
    QAction *deleteCompanyAction = companyMenu->addAction("会社削除");

    QMenu *insertMenu = menuBar->addMenu("挿入");
    QAction *insertRowAction = insertMenu->addAction("行挿入");
    QAction *insertColumnAction = insertMenu->addAction("列挿入");

    // Central layout
    // QWidget *centralWidget = new QWidget(this);
    // QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Add tab widget
    // mainLayout->addWidget(tabWidget);

    // Connect menu actions
    connect(insertRowAction, &QAction::triggered, this, &MainWindow::addRowToCurrentTable);
    connect(insertColumnAction, &QAction::triggered, this, &MainWindow::addColumnToCurrentTable);
    connect(importAction, &QAction::triggered, this, &MainWindow::importCsv);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportCsv);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    connect(addCompanyAction, &QAction::triggered, this, &MainWindow::addCompany);
    connect(editCompanyNameAction, &QAction::triggered, this, &MainWindow::editCompanyName);
    connect(deleteCompanyAction, &QAction::triggered, this, &MainWindow::deleteCompany);

    // Set the central widget
    setCentralWidget(tabWidget);

    // Add default company
    createTable("デフォルト会社");

    setWindowTitle("財務トラッカー");
    resize(1000, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::createTable(const QString &companyName) {
    QTableWidget *table = new QTableWidget(18, 6, this); // Start with 18 rows, 6 columns
    QStringList headers = {"年", "支払約定", "サイト（当社締日起算）", "売り上げ金額", "支払い約定月", "月次内訳"};
    table->setHorizontalHeaderLabels(headers);

    // Set column and row resize behavior
    // Stretch the initial six columns to fit the table width
    for (int i = 0; i < 6; ++i) {
        table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        table->horizontalHeader()->setMinimumSectionSize(100); // Prevent columns from shrinking too small
    }

    // Add "View/Edit" buttons to the Monthly Breakdown column (column index: 5)
    for (int row = 0; row < table->rowCount(); ++row) {
        QPushButton *monthlyButton = new QPushButton("表示/編集"); // "View/Edit"
        table->setCellWidget(row, 5, monthlyButton);

        // Connect the button to open the monthly dialog
        connect(monthlyButton, &QPushButton::clicked, [this, row]() {
            openMonthlyDialog(row);
        });
    }

    // Enable horizontal scrolling for future/ new columns
    table->horizontalHeader()->setStretchLastSection(false); // Prevent stretching all columns to fit
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel); // Smooth horizontal scrolling

    // Configure row resizing and appearance
    table->verticalHeader()->setDefaultSectionSize(30);
    table->setAlternatingRowColors(true);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); // Smooth vertical scrolling

    // Enable context menus for rows and columns
    table->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    table->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(table->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, [this, table](const QPoint &pos) {
        showColumnContextMenu(table, pos);
    });
    connect(table->verticalHeader(), &QHeaderView::customContextMenuRequested, this, [this, table](const QPoint &pos) {
        showRowContextMenu(table, pos);
    });

    companyTables[companyName] = table;
    tabWidget->addTab(table, companyName);
}

void MainWindow::addRowToCurrentTable() {
    QTableWidget *currentTable = qobject_cast<QTableWidget*>(tabWidget->currentWidget());
    if (!currentTable) {
        QMessageBox::warning(this, "エラー", "会社が選択されていません"); // "Error"- "No company selected!"
        return;
    }

    int rowCount = currentTable->rowCount();
    currentTable->insertRow(rowCount);

    for (int col = 0; col < currentTable->columnCount(); ++col) {
        currentTable->setItem(rowCount, col, new QTableWidgetItem(""));
    }

    // Add the "View/Edit" button to the Monthly Breakdown column
    if (currentTable->columnCount() >= 6) {
        QPushButton *monthlyButton = new QPushButton("表示/編集"); // "View/Edit"
        currentTable->setCellWidget(rowCount, currentTable->columnCount() - 1, monthlyButton);

        connect(monthlyButton, &QPushButton::clicked, [this, rowCount]() { openMonthlyDialog(rowCount); });
    }
}

void MainWindow::addColumnToCurrentTable() {
    QTableWidget *currentTable = qobject_cast<QTableWidget*>(tabWidget->currentWidget());
    if (!currentTable) {
        QMessageBox::warning(this, "エラー", "会社が選択されていません！"); // "Error"- "No company selected!"
        return;
    }

    bool ok;
    // "Add column"- "Enter column title:"
    QString columnName = QInputDialog::getText(this, "列追加", "列のタイトルを入力してください:", QLineEdit::Normal, "", &ok);

    if (ok && !columnName.isEmpty()) {
        int columnIndex = currentTable->columnCount();
        currentTable->insertColumn(columnIndex);
        currentTable->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(columnName));

        // Set new columns to interactive resizing
        currentTable->horizontalHeader()->setSectionResizeMode(columnIndex, QHeaderView::Interactive);

        // Ensure scrolling for additional columns
        currentTable->horizontalHeader()->setStretchLastSection(false);
    }
}

void MainWindow::addCompany() {
    bool ok;
    //"Add company"- "Enter company name:"
    QString companyName = QInputDialog::getText(this, "会社追加", "会社名を入力してください:", QLineEdit::Normal, "", &ok);
    if (ok && !companyName.isEmpty()) {
        if (companyTables.contains(companyName)) {
            QMessageBox::warning(this, "エラー", "会社は既に存在しています！"); // "Error"- "Company already exists!"
            return;
        }
        createTable(companyName);
    }
}

void MainWindow::editCompanyName() {
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex == -1) {
        QMessageBox::warning(this, "エラー", "会社が選択されていません！"); // "Error"- "No company selected!"
        return;
    }

    bool ok;
    // "Edit Company Name"- "Enter new company name:"
    QString currentName = tabWidget->tabText(currentIndex);
    QString newName = QInputDialog::getText(this, "会社名編集", "新しい会社名を入力してください:", QLineEdit::Normal, currentName, &ok);

    if (ok && !newName.isEmpty()) {
        if (companyTables.contains(newName)) {
            QMessageBox::warning(this, "エラー", "この名前の会社は既に存在しています！"); // "Error"- "A company with this name already exists!"
            return;
        }

        companyTables[newName] = companyTables.take(currentName);
        tabWidget->setTabText(currentIndex, newName);
    }
}

void MainWindow::deleteCompany() {
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex == -1) {
        QMessageBox::warning(this, "エラー", "会社が選択されていません！"); // "Error"- "No company selected!"
        return;
    }

    QString companyName = tabWidget->tabText(currentIndex);
    companyTables.remove(companyName);
    tabWidget->removeTab(currentIndex);
}

void MainWindow::showColumnContextMenu(QTableWidget *table, const QPoint &pos) {
    int col = table->horizontalHeader()->logicalIndexAt(pos);
    if (col < 6) return; // Prevent deletion of the first 6 columns

    QMenu contextMenu(this);
    QAction *deleteColumnAction = contextMenu.addAction("列削除");
    connect(deleteColumnAction, &QAction::triggered, [table, col]() {
        table->removeColumn(col);

        // Reset resize mode for remaining columns
        for (int i = 0; i < table->columnCount(); ++i) {
            table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    });
    contextMenu.exec(table->horizontalHeader()->mapToGlobal(pos));
}

void MainWindow::showRowContextMenu(QTableWidget *table, const QPoint &pos) {
    int row = table->verticalHeader()->logicalIndexAt(pos);

    QMenu contextMenu(this);
    QAction *deleteRowAction = contextMenu.addAction("行削除");
    connect(deleteRowAction, &QAction::triggered, [table, row]() {
        table->removeRow(row);
    });
    contextMenu.exec(table->verticalHeader()->mapToGlobal(pos));
}

void MainWindow::openMonthlyDialog(int row) {
    QTableWidget *currentTable = qobject_cast<QTableWidget *>(tabWidget->currentWidget());
    if (!currentTable) return;

    // Load existing monthly values or initialize to zero if not already saved
    QVector<int> &monthlyValues = monthlyValuesMap[row]; // Access by reference
    if (monthlyValues.isEmpty()) {
        monthlyValues = QVector<int>(12, 0); // Initialize with zeros if not already set
    }

    // Open the MonthlyDialog with the existing values
    MonthlyDialog dialog(this, monthlyValues);

    // If OK is clicked, update the values in the map
    if (dialog.exec() == QDialog::Accepted) {
        monthlyValues = dialog.getMonthlyValues();
    }
}


void MainWindow::importCsv() {
    // Get the current table
    QTableWidget *currentTable = qobject_cast<QTableWidget*>(tabWidget->currentWidget());
    if (!currentTable) {
        QMessageBox::warning(this, "エラー", "会社が選択されていません！"); // "Error"- "No company selected!"
        return;
    }

    // Open file dialog to select CSV file
    QString fileName = QFileDialog::getOpenFileName(this, "Open CSV File", "", "CSV Files (*.csv);;All Files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open file.");
        return;
    }

    // Read CSV content
    QTextStream in(&file);
    currentTable->setRowCount(0); // Clear existing rows
    int row = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        currentTable->insertRow(row);
        for (int col = 0; col < fields.size() && col < currentTable->columnCount(); ++col) {
            currentTable->setItem(row, col, new QTableWidgetItem(fields[col]));
        }

        // Add View/Edit button for Monthly Breakdown column
        QPushButton *monthlyButton = new QPushButton("表示/編集");
        currentTable->setCellWidget(row, currentTable->columnCount() - 1, monthlyButton);

        connect(monthlyButton, &QPushButton::clicked, [this, row]() { openMonthlyDialog(row); });
        ++row;
    }

    file.close();
    QMessageBox::information(this, "成功", "CSVデータのインポートに成功しました。"); // "Success"- "CSV data imported successfully."
}

void MainWindow::exportCsv() {
    // Get the current table
    QTableWidget *currentTable = qobject_cast<QTableWidget*>(tabWidget->currentWidget());
    if (!currentTable) {
        QMessageBox::warning(this, "エラー", "会社が選択されていません！"); // "Error"- "No company selected!"
        return;
    }

    // Open file dialog to save CSV file
    QString fileName = QFileDialog::getSaveFileName(this, "Save CSV File", "", "CSV Files (*.csv);;All Files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "エラー", "ファイルを保存できませんでした。"); // "Error"- "Could not save file."
        return;
    }

    QTextStream out(&file);

    // Write table data to CSV
    for (int row = 0; row < currentTable->rowCount(); ++row) {
        QStringList rowContent;
        for (int col = 0; col < currentTable->columnCount() - 1; ++col) {
            QTableWidgetItem *item = currentTable->item(row, col);
            rowContent << (item ? item->text() : "");
        }
        out << rowContent.join(',') << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "CSVデータのエクスポートに成功しました。"); // "Success"- "CSV data exported successfully."
}
