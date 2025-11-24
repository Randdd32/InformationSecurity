#include "headers/services/authservice.h"
#include "headers/utils/cryptoutils.h"
#include "headers/utils/passwordvalidator.h"
#include "headers/utils/exceptions.h"

#include <QDebug>
#include <QFile>

AuthService::AuthService(QSharedPointer<CryptDBService> dbService, QObject *parent)
    : QObject{parent}, _dbService(dbService)
{}

InitDBStatus AuthService::initDB(const QString& passphrase)
{
    InitDBStatus status = _dbService->initAndLoad(passphrase);

    if (status != InitDBStatus::Success && status != InitDBStatus::FirstRunSuccess) {
        qCritical() << "Initialization failed. Status: " << (int)status;
        QFile::remove(_dbService->TEMP_DB_FILE);
    }
    return status;
}

AuthStatus AuthService::authenticate(const QString& username, const QString& password, UserAccount& accountOut)
{
    UserAccount account;
    try {
        account = _dbService->getUser(username);
    } catch (const UserNotFoundException& e) {
        return AuthStatus::UserNotFound;
    } catch (const DBException& e) {
        qCritical() << "Authentication aborted due to DB error:" << e.what();
        return AuthStatus::DatabaseError;
    } catch (const std::exception& e) {
        qCritical() << "Unexpected error during authentication:" << e.what();
        return AuthStatus::SystemError;
    }

    if (account.isBlocked()) {
        qWarning() << "Authentication failed: User is blocked";
        return AuthStatus::UserBlocked;
    }

    QString hash = CryptoUtils::hashMD5(password);
    if (hash != account.passwordHash()) {
        incrementLoginAttemptCount();
        qWarning() << "Authentication failed: Incorrect password. Attempt: " << _loginAttemptCount;
        return AuthStatus::IncorrectPassword;
    }

    resetLoginAttemptCount();
    accountOut = account;
    return AuthStatus::Success;
}

bool AuthService::changePassword(const QString& username, const QString& oldPassword, const QString& newPassword, QString& errorMessageOut)
{
    UserAccount account;
    try {
        account = _dbService->getUser(username);
    } catch (const UserNotFoundException& e) {
        errorMessageOut = "Ошибка: Пользователь с таким именем не найден";
        return false;
    } catch (const DBException& e) {
        errorMessageOut = QString("Критическая ошибка БД при загрузке данных пользователя. Смена пароля невозможна. (%1)").arg(e.what());
        return false;
    } catch (const std::exception& e) {
        errorMessageOut = QString("Ошибка при работе системы: (%1)").arg(e.what());
        return false;
    }

    if (CryptoUtils::hashMD5(oldPassword) != account.passwordHash()) {
        errorMessageOut = "Неверный старый пароль";
        return false;
    }

    if (!validateNewPassword(username, newPassword, errorMessageOut)) {
        return false;
    }

    account.setPasswordHash(CryptoUtils::hashMD5(newPassword));
    if (account.isFirstLogin()) {
        account.setIsFirstLogin(false);
    }
    if (!_dbService->updateUser(account)) {
        errorMessageOut = "Ошибка при сохранении нового пароля в базе данных";
        return false;
    }

    return true;
}

bool AuthService::saveAndCleanup()
{
    return _dbService->saveAndCleanup();
}

int AuthService::loginAttemptCount() const
{
    return _loginAttemptCount;
}

void AuthService::resetLoginAttemptCount()
{
    _loginAttemptCount = 0;
}

void AuthService::incrementLoginAttemptCount()
{
    _loginAttemptCount++;
}

bool AuthService::validateNewPassword(const QString& username, const QString& newPassword, QString& errorMessageOut)
{
    UserAccount account;
    try {
        account = _dbService->getUser(username);
    } catch (const UserNotFoundException& e) {
        errorMessageOut = "Ошибка: Пользователь с таким именем не найден";
        return false;
    } catch (const DBException& e) {
        errorMessageOut = QString("Критическая ошибка БД при загрузке данных пользователя. Смена пароля невозможна. (%1)").arg(e.what());
        return false;
    } catch (const std::exception& e) {
        errorMessageOut = QString("Ошибка при работе системы: (%1)").arg(e.what());
        return false;
    }

    if (!PasswordValidator::validatePassword(
            newPassword,
            account.passwordRestrictions(),
            account.minPasswordLength()))
    {
        errorMessageOut = QString("Пароль не соответствует требованиям учетной записи:\n"
                                  "- Минимальная длина: %1\n"
                                  "- Ограничения на символы: %2")
                              .arg(account.minPasswordLength())
                              .arg(account.passwordRestrictions() ? "ВКЛ" : "ВЫКЛ");

        if (account.passwordRestrictions()) {
            errorMessageOut += "\n\nТекущее ограничение состоит в том, что требуется наличие латинских букв, символов кириллицы и цифр";
        }

        return false;
    }

    return true;
}
