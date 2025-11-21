#include "headers/models/useraccount.h"

const QString UserAccount::ADMIN_USERNAME = "ADMIN";

UserAccount::UserAccount() {}

QString UserAccount::username() const
{
    return _username;
}

void UserAccount::setUsername(const QString &newUsername)
{
    _username = newUsername;
}

QString UserAccount::passwordHash() const
{
    return _passwordHash;
}

void UserAccount::setPasswordHash(const QString &newPasswordHash)
{
    _passwordHash = newPasswordHash;
}

bool UserAccount::isBlocked() const
{
    return _isBlocked;
}

void UserAccount::setIsBlocked(bool newIsBlocked)
{
    _isBlocked = newIsBlocked;
}

bool UserAccount::passwordRestrictions() const
{
    return _passwordRestrictions;
}

void UserAccount::setPasswordRestrictions(bool newPasswordRestrictions)
{
    _passwordRestrictions = newPasswordRestrictions;
}

int UserAccount::minPasswordLength() const
{
    return _minPasswordLength;
}

void UserAccount::setMinPasswordLength(int newMinPasswordLength)
{
    _minPasswordLength = newMinPasswordLength;
}

int UserAccount::passwordExpirationMonths() const
{
    return _passwordExpirationMonths;
}

void UserAccount::setPasswordExpirationMonths(int newPasswordExpirationMonths)
{
    _passwordExpirationMonths = newPasswordExpirationMonths;
}

bool UserAccount::isAdmin() const
{
    return _username == ADMIN_USERNAME;
}

QVariantMap UserAccount::toVariantMap() const
{
    QVariantMap map;
    map["username"] = _username;
    map["password_hash"] = _passwordHash;
    map["is_blocked"] = _isBlocked;
    map["restrictions_enabled"] = _passwordRestrictions;
    map["min_password_length"] = _minPasswordLength;
    map["password_expiration_months"] = _passwordExpirationMonths;
    return map;
}

UserAccount UserAccount::fromVariantMap(const QVariantMap& map)
{
    UserAccount acc;
    acc.setUsername(map.value("username").toString());
    acc.setPasswordHash(map.value("password_hash").toString());
    acc.setIsBlocked(map.value("is_blocked").toBool());
    acc.setPasswordRestrictions(map.value("restrictions_enabled").toBool());
    acc.setMinPasswordLength(map.value("min_password_length").toInt());
    acc.setPasswordExpirationMonths(map.value("password_expiration_months").toInt());
    return acc;
}
