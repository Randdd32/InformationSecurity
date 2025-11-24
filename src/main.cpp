#include "headers/views/mainwindow.h"
#include "headers/views/loginwindow.h"
#include "headers/views/initpassphrasewindow.h"
#include "headers/services/cryptdbservice.h"
#include "headers/services/authservice.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/mw-icon.png"));

    auto dbService = QSharedPointer<CryptDBService>::create();
    auto authService = QSharedPointer<AuthService>::create(dbService);

    InitPassphraseWindow initWindow(authService, dbService->ENCRYPTED_DB_FILE);
    if (initWindow.exec() != QDialog::Accepted) {
        return 0;
    }

    LoginWindow loginWindow(authService);
    MainWindow mainWindow;

    QObject::connect(&loginWindow, &LoginWindow::loginSuccess,
                     &mainWindow,
                     [&mainWindow](const UserAccount& user) { //не забыть передать в главное окно
                         mainWindow.show();
                     });

    loginWindow.show();

    return a.exec();
}
