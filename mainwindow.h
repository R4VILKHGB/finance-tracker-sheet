#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QMenuBar>
#include <QAction>
#include <QMap>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addCompany();
    void editCompanyName();
    void deleteCompany();
    void openMonthlyDialog(int row);
    void importCsv();
    void exportCsv();
    void addRowToCurrentTable();
    void addColumnToCurrentTable();
    void showColumnContextMenu(QTableWidget *table, const QPoint &pos);
    void showRowContextMenu(QTableWidget *table, const QPoint &pos);

private:
    QTabWidget *tabWidget; // Tabs for companies
    QMap<QString, QTableWidget*> companyTables; // Map company names to their tables
    QMap<int, QVector<int>> monthlyValuesMap; // Map row index to monthly values

    void createTable(const QString &companyName);
};

#endif // MAINWINDOW_H




