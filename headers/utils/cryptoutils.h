#ifndef CRYPTOUTILS_H
#define CRYPTOUTILS_H

#include <QString>
#include <QByteArray>

class CryptoUtils
{
public:
    static const int DES_KEY_SIZE = 8;
    static const int DES_IV_SIZE = 8;

    static QString hashMD5(const QString &data);

    static QByteArray deriveKeyAndIV(const QString &passphrase);

    static QByteArray encryptDES_CBC(const QByteArray &plainData, const QByteArray &key, const QByteArray &iv);

    static QByteArray decryptDES_CBC(const QByteArray &encryptedData, const QByteArray &key, const QByteArray &iv);
};

#endif // CRYPTOUTILS_H
