#include "headers/services/encryptedfilehandler.h"
#include "headers/utils/cryptoutils.h"
#include "headers/utils/exceptions.h"

#include <QFile>
#include <QDebug>

EncryptedFileHandler::EncryptedFileHandler(QObject *parent)
    : QObject{parent}
{}

QByteArray EncryptedFileHandler::decrypt(const QString& encryptedFilePath,
                                         const QString& passphrase,
                                         QByteArray& keyOut)
{
    QFile encryptedFile(encryptedFilePath);

    QByteArray key = CryptoUtils::deriveKey(passphrase);
    if (key.isEmpty() || key.size() != CryptoUtils::DES_KEY_SIZE) {
        qCritical() << "Decryption failed: Key derivation error";
        throw PassphraseIncorrectException("Decryption failed: key derivation error");
    }
    keyOut = key;

    if (!encryptedFile.exists()) {
        qDebug() << "Encrypted DB file not found";
        return QByteArray();
    }

    if (!encryptedFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open encrypted file for reading: " << encryptedFile.errorString();
        throw FileOpenException("Decryption failed: cannot open encrypted file for reading");
    }

    QByteArray iv = encryptedFile.read(CryptoUtils::DES_IV_SIZE);
    QByteArray encryptedData = encryptedFile.readAll();
    encryptedFile.close();

    if (iv.size() != CryptoUtils::DES_IV_SIZE) {
        qCritical() << "Encrypted file corrupted: IV size incorrect or file empty";
        throw DBFileCorruptedException("Decryption failed. Encrypted file corrupted: IV size incorrect or file empty");
    }

    QByteArray decryptedData = CryptoUtils::decryptDES_CBC(encryptedData, key, iv);

    if (decryptedData.isEmpty()) {
        qCritical() << "Decryption failed. Incorrect passphrase or data corruption";
        throw DecryptionException("Decryption failed. Incorrect passphrase or data corruption");
    }

    return decryptedData;
}

bool EncryptedFileHandler::encrypt(const QString& encryptedFilePath,
                                   const QByteArray& plainData,
                                   const QByteArray& key)
{
    QFile encryptedFile(encryptedFilePath);

    if (key.isEmpty() || key.size() != CryptoUtils::DES_KEY_SIZE) {
        qCritical() << "Encryption failed: Key is invalid";
        return false;
    }

    QByteArray iv = CryptoUtils::generateRandomIV();

    QByteArray encryptedData = CryptoUtils::encryptDES_CBC(plainData, key, iv);
    if (encryptedData.isEmpty()) {
        qCritical() << "Encryption failed (CryptoUtils error)";
        return false;
    }

    if (!encryptedFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qCritical() << "Cannot open encrypted file for writing: " << encryptedFile.errorString();
        return false;
    }

    encryptedFile.write(iv);
    encryptedFile.write(encryptedData);
    encryptedFile.close();

    qDebug() << "DB file successfully encrypted";
    return true;
}
