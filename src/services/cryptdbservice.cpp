#include "headers/services/cryptdbservice.h"
#include "headers/utils/cryptoutils.h"
#include "headers/utils/exceptions.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

CryptDBService::CryptDBService(QObject *parent)
    : QObject{parent}
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(TEMP_DB_FILE);
    _fileHandler = new EncryptedFileHandler(this);
}

CryptDBService::~CryptDBService()
{
    if (_db.isOpen()) {
        _db.close();
    }
}

InitDBStatus CryptDBService::initAndLoad(const QString& passphrase)
{
    QFile tempFile(TEMP_DB_FILE);
    bool isFirstRun = !QFile::exists(ENCRYPTED_DB_FILE);

    QByteArray decryptedData;

    try {
        decryptedData = _fileHandler->decrypt(ENCRYPTED_DB_FILE, passphrase, _key);

    } catch (const PassphraseIncorrectException&) {
        return InitDBStatus::PassphraseIncorrect;
    } catch (const DecryptionException&) {
        return InitDBStatus::DecryptionException;
    } catch (const DBFileCorruptedException&) {
        return InitDBStatus::DBFileCorrupted;
    } catch (const FileOpenException&) {
        return InitDBStatus::FileOpenError;
    } catch (const std::exception& e) {
        qCritical() << "Unexpected exception during decryption:" << e.what();
        return InitDBStatus::SystemError;
    }

    if (!decryptedData.isEmpty()) {
        if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qCritical() << "Cannot open temporary file for writing:" << tempFile.errorString();
            return InitDBStatus::FileOpenError;
        }
        tempFile.write(decryptedData);
        tempFile.close();
    }

    if (!_db.open()) {
        qCritical() << "Cannot open temporary SQLite database:" << _db.lastError().text();
        QFile::remove(TEMP_DB_FILE);
        return InitDBStatus::SystemError;
    }

    InitDBStatus schemaStatus = setupDatabaseSchema(isFirstRun);

    if (schemaStatus != InitDBStatus::Success && schemaStatus != InitDBStatus::FirstRunSuccess) {
        _db.close();
        QFile::remove(TEMP_DB_FILE);
        return schemaStatus;
    }

    qDebug() << "DB Service initialized successfully";
    return isFirstRun ? InitDBStatus::FirstRunSuccess : InitDBStatus::Success;
}

bool CryptDBService::saveAndCleanup()
{
    QFile tempFile(TEMP_DB_FILE);

    if (!_db.isOpen()) {
        qDebug() << "DB is not open. Nothing to save";
        return true;
    }

    _db.close();

    if (!tempFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open temporary file for reading before encryption: " << tempFile.errorString();
        return false;
    }
    QByteArray plainData = tempFile.readAll();
    tempFile.close();

    bool success = _fileHandler->encrypt(ENCRYPTED_DB_FILE, plainData, _key);

    if (success && !tempFile.remove()) {
        qWarning() << "Could not remove temporary DB file: " << tempFile.errorString();
    }

    return success;
}

InitDBStatus CryptDBService::setupDatabaseSchema(bool isFirstRun)
{
    if (!_db.isOpen()) {
        qCritical() << "Database connection is not open for schema setup";
        return InitDBStatus::SystemError;
    }

    QSqlQuery query(_db);
    QString createTable = QString("CREATE TABLE IF NOT EXISTS accounts ("
                                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "username TEXT NOT NULL UNIQUE, "
                                  "password_hash TEXT NOT NULL, "
                                  "is_blocked INTEGER NOT NULL, "
                                  "restrictions_enabled INTEGER NOT NULL, "
                                  "min_password_length INTEGER NOT NULL, "
                                  "password_expiration_months INTEGER NOT NULL, "
                                  "is_first_login INTEGER NOT NULL)");

    if (!query.exec(createTable)) {
        qCritical() << "Failed to create accounts table: " << query.lastError().text();
        return InitDBStatus::SystemError;
    }

    bool adminExists = checkAdminRecord();

    if (!adminExists) {
        if (isFirstRun) {
            qDebug() << "Creating initial ADMIN account";

            UserAccount admin;
            admin.setUsername(UserAccount::ADMIN_USERNAME);
            admin.setPasswordHash(CryptoUtils::hashMD5(""));

            if (!createUser(admin)) {
                qCritical() << "Failed to create initial ADMIN account";
                return InitDBStatus::SystemError;
            }
            return InitDBStatus::FirstRunSuccess;
        } else {
            qCritical() << "Database corruption: Encrypted file exists, but ADMIN record is missing";
            return InitDBStatus::DBFileCorrupted;
        }
    }
    return InitDBStatus::Success;
}

bool CryptDBService::checkAdminRecord()
{
    QSqlQuery query(_db);
    query.prepare("SELECT username FROM accounts WHERE username = :username");
    query.bindValue(":username", UserAccount::ADMIN_USERNAME);

    if (!query.exec()) {
        qCritical() << "Failed to check ADMIN existence: " << query.lastError().text();
        return false;
    }
    return query.next(); // true, если запись найдена
}

bool CryptDBService::userExists(const QString& username)
{
    QSqlQuery query(_db);
    query.prepare("SELECT 1 FROM accounts WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qCritical() << "User existence check failed: " << query.lastError().text();
        throw DBException(query.lastError().text());
    }
    return query.next();
}

bool CryptDBService::createUser(const UserAccount& account)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO accounts (username, password_hash, is_blocked, "
                  "restrictions_enabled, min_password_length, password_expiration_months, is_first_login) "
                  "VALUES (:username, :password_hash, :is_blocked, "
                  ":restrictions_enabled, :min_password_length, :password_expiration_months, :is_first_login)");

    QVariantMap map = account.toVariantMap();
    map.remove("id");

    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }

    if (!query.exec()) {
        qCritical() << "Failed to create user: " << query.lastError().text();
        return false;
    }

    return true;
}

UserAccount CryptDBService::getUser(const QString& username)
{
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM accounts WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qCritical() << "Failed to get user: " << query.lastError().text();
        throw DBException(query.lastError().text());
    }

    if (!query.next()) {
        qWarning() << "User not found by username: " << username;
        throw UserNotFoundException(username);
    }

    QSqlRecord record = query.record();
    QVariantMap map;
    for (int i = 0; i < record.count(); ++i) {
        map.insert(record.fieldName(i), record.value(i));
    }

    return UserAccount::fromVariantMap(map);
}

bool CryptDBService::updateUser(const UserAccount& account)
{
    if (account.id() <= 0) {
        qCritical() << "Update failed: UserAccount ID is invalid or missing";
        return false;
    }

    QSqlQuery query(_db);

    query.prepare("UPDATE accounts SET "
                  "password_hash = :password_hash, "
                  "is_blocked = :is_blocked, "
                  "restrictions_enabled = :restrictions_enabled, "
                  "min_password_length = :min_password_length, "
                  "password_expiration_months = :password_expiration_months, "
                  "is_first_login = :is_first_login "
                  "WHERE id = :id");

    query.bindValue(":password_hash", account.passwordHash());
    query.bindValue(":is_blocked", account.isBlocked());
    query.bindValue(":restrictions_enabled", account.passwordRestrictions());
    query.bindValue(":min_password_length", account.minPasswordLength());
    query.bindValue(":password_expiration_months", account.passwordExpirationMonths());
    query.bindValue(":is_first_login", account.isFirstLogin());
    query.bindValue(":id", account.id());

    if (!query.exec()) {
        qCritical() << "Failed to update user: " << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<UserAccount> CryptDBService::getAllUsers(const QString& searchText)
{
    QList<UserAccount> users;
    QSqlQuery query(_db);

    QString sql = "SELECT * FROM accounts WHERE username != :admin_name";
    if (!searchText.isEmpty()) {
        sql += " AND username LIKE :search";
    }

    query.prepare(sql);
    query.bindValue(":admin_name", UserAccount::ADMIN_USERNAME);
    if (!searchText.isEmpty()) {
        query.bindValue(":search", "%" + searchText + "%");
    }

    if (!query.exec()) {
        qCritical() << "Failed to get users: " << query.lastError().text();
        throw DBException(query.lastError().text());
    }

    while (query.next()) {
        QSqlRecord record = query.record();
        QVariantMap map;
        for (int i = 0; i < record.count(); ++i) {
            map.insert(record.fieldName(i), record.value(i));
        }
        users.append(UserAccount::fromVariantMap(map));
    }
    return users;
}
