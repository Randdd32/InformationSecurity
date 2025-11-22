#ifndef CRYPTDBSERVICE_H
#define CRYPTDBSERVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>

#include "headers/models/useraccount.h"
#include "headers/services/encryptedfilehandler.h"

class CryptDBService : public QObject
{
    Q_OBJECT
public:
    explicit CryptDBService(QObject *parent = nullptr);
    ~CryptDBService();

    const QString ENCRYPTED_DB_FILE = "accounts.enc";
    const QString TEMP_DB_FILE = "accounts.db.tmp";

    bool initAndLoad(const QString& passphrase);
    bool saveAndCleanup();

    bool userExists(const QString& username);
    bool createUser(const UserAccount& account);
    UserAccount getUser(const QString& username);
    bool updateUser(const UserAccount& account);
    QList<UserAccount> getAllUsers();

private:
    QByteArray _key;
    QSqlDatabase _db;

    EncryptedFileHandler* _fileHandler;

    bool setupDatabaseSchema();
    bool checkAdminRecord();
};

#endif // CRYPTDBSERVICE_H
