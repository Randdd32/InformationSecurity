#include "headers/views/mainwindow.h"
#include "headers/views/initpassphrasewindow.h"
#include "headers/services/cryptdbservice.h"
#include "headers/services/authservice.h"

#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/mw-icon.png"));

    QTranslator qtTranslator;
    if (qtTranslator.load("qtbase_ru",
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        a.installTranslator(&qtTranslator);
    } else {
        qWarning() << "Couldn't download qtbase_ru translation";
    }

    auto dbService = QSharedPointer<CryptDBService>::create();
    auto authService = QSharedPointer<AuthService>::create(dbService);
    auto adminService = QSharedPointer<AdminUserService>::create(dbService);

    InitPassphraseWindow initWindow(authService, dbService->ENCRYPTED_DB_FILE);
    if (initWindow.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow w(authService, adminService);

    QObject::connect(&a, &QApplication::aboutToQuit, [authService]() {
        bool success = authService->saveAndCleanup();

        if (!success) {
            QMessageBox::critical(nullptr, "Критическая ошибка безопасности",
                                  "Не удалось сохранить и зашифровать базу данных при выходе из системы. "
                                  "Немедленно свяжитесь с системным администратором для устранения проблемы.",
                                  QMessageBox::Ok);
        }
    });

    return a.exec();
}
