#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "headers/models/useraccount.h"
#include "headers/models/initdbstatus.h"
#include "headers/services/cryptdbservice.h"

#include <QObject>
#include <QSharedPointer>

enum class AuthStatus {
    Success,
    UserNotFound,
    IncorrectPassword,
    UserBlocked,
    DatabaseError,
    SystemError
};

class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(QSharedPointer<CryptDBService> dbService, QObject *parent = nullptr);

    InitDBStatus initDB(const QString& passphrase);
    AuthStatus authenticate(const QString& username, const QString& password, UserAccount& accountOut);
    bool changePassword(const QString& username, const QString& oldPassword, const QString& newPassword, QString& errorMessageOut);
    bool saveAndCleanup();

    int loginAttemptCount() const;
    void resetLoginAttemptCount();
    void incrementLoginAttemptCount();

    UserAccount getUser(const QString& username);
    bool checkIsFirstLogin(const QString& username, UserAccount& accountOut);

private:
    QSharedPointer<CryptDBService> _dbService;
    int _loginAttemptCount = 0;

    bool validateNewPassword(const QString& username, const QString& newPassword, QString& errorMessageOut);
};

#endif // AUTHSERVICE_H
