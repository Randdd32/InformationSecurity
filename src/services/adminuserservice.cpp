#include "headers/services/adminuserservice.h"
#include "headers/utils/cryptoutils.h"
#include "headers/utils/exceptions.h"

AdminUserService::AdminUserService(QSharedPointer<CryptDBService> dbService, QObject *parent)
    : QObject{parent}, _dbService(dbService)
{}

QList<UserAccount> AdminUserService::getAllUsers(QString& errorMessageOut)
{
    QList<UserAccount> users;
    try {
        users = _dbService->getAllUsers();
    } catch (const DBException& e) {
        errorMessageOut = QString("Критическая ошибка БД при получении пользователей: %1").arg(e.what());
        return QList<UserAccount>();
    } catch (const std::exception& e) {
        errorMessageOut = QString("Ошибка при работе системы: (%1)").arg(e.what());
        return QList<UserAccount>();
    }
    return users;
}

bool AdminUserService::createUser(const QString& username, QString& errorMessageOut)
{
    try {
        if (_dbService->userExists(username)) {
            errorMessageOut = "Пользователь с таким именем уже существует";
            return false;
        }
    } catch (const DBException& e) {
        errorMessageOut = QString("Критическая ошибка БД при проверке пользователя: %1").arg(e.what());
        return false;
    } catch (const std::exception& e) {
        errorMessageOut = QString("Ошибка при работе системы: (%1)").arg(e.what());
        return false;
    }

    UserAccount newUser;
    newUser.setUsername(username);
    newUser.setPasswordHash(CryptoUtils::hashMD5(""));

    if (!_dbService->createUser(newUser)) {
        errorMessageOut = "Ошибка при создании пользователя";
        return false;
    }

    return true;
}

bool AdminUserService::changeUserBlockStatus(const QString& username, bool block, QString& errorMessageOut)
{
    if (username == UserAccount::ADMIN_USERNAME) {
        errorMessageOut = "Запрещено взаимодействовать с учетной записью администратора";
        return false;
    }

    bool success = false;
    UserAccount account = getAccount(username, errorMessageOut, success);
    if (!success) {
        return false;
    }

    account.setIsBlocked(block);
    if (!_dbService->updateUser(account)) {
        errorMessageOut = QString("Ошибка при обновлении статуса пользователя %1")
                              .arg(username);
        return false;
    }

    return true;
}

bool AdminUserService::setPasswordRestrictions(const QString& username, bool enabled, QString& errorMessageOut)
{
    bool success = false;
    UserAccount account = getAccount(username, errorMessageOut, success);
    if (!success) {
        return false;
    }

    account.setPasswordRestrictions(enabled);
    if (!_dbService->updateUser(account)) {
        errorMessageOut = QString("Ошибка при попытке изменения ограничений на пароль пользователя %1")
                              .arg(username);
        return false;
    }

    return true;
}

bool AdminUserService::setPasswordPolicy(const QString& username, int minLength, int expirationMonths, QString& errorMessageOut)
{
    if (minLength < 0 || expirationMonths < 0) {
        errorMessageOut = "Минимальная длина и срок действия пароля не могут быть отрицательными";
        return false;
    }

    bool success = false;
    UserAccount account = getAccount(username, errorMessageOut, success);
    if (!success) {
        return false;
    }

    account.setMinPasswordLength(minLength);
    account.setPasswordExpirationMonths(expirationMonths);

    if (!_dbService->updateUser(account)) {
        errorMessageOut = QString("Ошибка при попытке изменения политики пароля пользователя %1")
                              .arg(username);
        return false;
    }

    return true;
}

UserAccount AdminUserService::getAccount(const QString& username, QString& errorMessageOut, bool& success)
{
    success = false;
    try {
        UserAccount account = _dbService->getUser(username);
        success = true;
        return account;
    } catch (const UserNotFoundException& e) {
        errorMessageOut = "Пользователь с таким именем не найден";
        return UserAccount();
    } catch (const DBException& e) {
        errorMessageOut = QString("Критическая ошибка БД: %1").arg(e.what());
        return UserAccount();
    } catch (const std::exception& e) {
        errorMessageOut = QString("Ошибка при работе системы: (%1)").arg(e.what());
        return UserAccount();
    }
}
