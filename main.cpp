#include "MainWindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set the application locale to Japanese
    QLocale::setDefault(QLocale(QLocale::Japanese, QLocale::Japan));

    // Load Japanese translations for standard Qt dialogs and widgets
    QTranslator translator;
    if (translator.load("qt_ja", QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&translator);
    }

    // Set a font that supports Japanese characters (optional)
    QFont font("MS Gothic", 10);
    app.setFont(font);

    // Create and show the main window
    MainWindow window;
    window.setWindowTitle("財務トラッカー"); // "Finance Tracker"
    window.resize(800, 600);
    window.show();
    return app.exec();
}
