#ifndef PASSWORDVALIDATOR_H
#define PASSWORDVALIDATOR_H

#include <QString>

class PasswordValidator
{
public:
    // Ограничение: Наличие латинских букв, символов кириллицы и цифр.
    static bool validatePassword(const QString &password, bool restrictionsEnabled, int minLength);

private:
    static bool checkRestrictions(const QString &password);
};

#endif // PASSWORDVALIDATOR_H
