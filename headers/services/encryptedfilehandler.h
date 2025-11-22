#ifndef ENCRYPTEDFILEHANDLER_H
#define ENCRYPTEDFILEHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QString>

class EncryptedFileHandler : public QObject
{
    Q_OBJECT
public:
    explicit EncryptedFileHandler(QObject *parent = nullptr);

    QByteArray decrypt(const QString& encryptedFilePath,
                       const QString& passphrase,
                       QByteArray& keyOut);

    bool encrypt(const QString& encryptedFilePath,
                 const QByteArray& plainData,
                 const QByteArray& key);
};

#endif // ENCRYPTEDFILEHANDLER_H
