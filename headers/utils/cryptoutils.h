#ifndef CRYPTOUTILS_H
#define CRYPTOUTILS_H

#include <QByteArray>

class CryptoUtils {
public:
    static QByteArray keyFromPassword(const QString &password);

    static QByteArray keyFromFile(const QString &filePath, bool &ok);

    static QByteArray generateIV();
};

#endif // CRYPTOUTILS_H
