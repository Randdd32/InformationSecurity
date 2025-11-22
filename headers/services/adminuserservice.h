#ifndef ADMINUSERSERVICE_H
#define ADMINUSERSERVICE_H

#include <QObject>
#include <QSharedPointer>

#include "headers/services/cryptdbservice.h"

class AdminUserService : public QObject
{
    Q_OBJECT
public:
    explicit AdminUserService(QSharedPointer<CryptDBService> dbService, QObject *parent = nullptr);

    QList<UserAccount> getAllUsers(QString& errorMessageOut);

    bool createUser(const QString& username, QString& errorMessageOut);
    bool changeUserBlockStatus(const QString& username, bool block, QString& errorMessageOut);
    bool setPasswordRestrictions(const QString& username, bool enabled, QString& errorMessageOut);
    bool setPasswordPolicy(const QString& username, int minLength, int expirationMonths, QString& errorMessageOut);

private:
    QSharedPointer<CryptDBService> _dbService;

    UserAccount getAccount(const QString& username, QString& errorMessageOut, bool& success);
};

#endif // ADMINUSERSERVICE_H
