#include "headers/utils/passwordvalidator.h"

bool PasswordValidator::validatePassword(const QString &password, bool restrictionsEnabled, int minLength)
{
    if (password.length() < minLength) {
        return false;
    }

    if (restrictionsEnabled) {
        return checkRestrictions(password);
    }

    return true;
}

bool PasswordValidator::checkRestrictions(const QString &password)
{
    bool hasLatin = false;
    bool hasCyrillic = false;
    bool hasDigit = false;

    for (const QChar &c : password) {
        if (c.isDigit()) {
            hasDigit = true;
        } else if (c.isLetter()) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                hasLatin = true;
            }

            ushort unicode = c.unicode();
            if ((unicode >= 0x0410 && unicode <= 0x044F) || // А-Я, а-я
                (unicode == 0x0401 || unicode == 0x0451))    // Ё, ё
            {
                hasCyrillic = true;
            }
        }
    }

    return hasLatin && hasCyrillic && hasDigit;
}
