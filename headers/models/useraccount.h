#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QString>
#include <QVariantMap>

class UserAccount
{
public:
    static const QString ADMIN_USERNAME;

    UserAccount();
    ~UserAccount() = default;

    QString username() const;
    void setUsername(const QString &newUsername);

    QString passwordHash() const;
    void setPasswordHash(const QString &newPasswordHash);

    bool isBlocked() const;
    void setIsBlocked(bool newIsBlocked);

    bool passwordRestrictions() const;
    void setPasswordRestrictions(bool newPasswordRestrictions);

    int minPasswordLength() const;
    void setMinPasswordLength(int newMinPasswordLength);

    int passwordExpirationMonths() const;
    void setPasswordExpirationMonths(int newPasswordExpirationMonths);

    bool isAdmin() const;

    QVariantMap toVariantMap() const;
    static UserAccount fromVariantMap(const QVariantMap& map);

private:
    QString _username = "";
    QString _passwordHash = "";
    bool _isBlocked = false;
    bool _passwordRestrictions = false;
    int _minPasswordLength = 0;
    int _passwordExpirationMonths = 0;
};

#endif // USERACCOUNT_H
