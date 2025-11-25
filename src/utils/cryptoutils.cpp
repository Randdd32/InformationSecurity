#include "headers/utils/cryptoutils.h"

#include <QDebug>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QFile>

#include <openssl/evp.h>
#include <openssl/err.h>

QString CryptoUtils::hashMD5(const QString &data)
{
    QByteArray hash = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5);
    return hash.toHex();
}

QByteArray CryptoUtils::deriveKey(const QString &passphrase)
{
    QByteArray hash = QCryptographicHash::hash(passphrase.toUtf8(), QCryptographicHash::Md5);
    return hash.left(DES_KEY_SIZE);
}

QByteArray CryptoUtils::generateRandomIV()
{
    QByteArray iv(DES_IV_SIZE, 0);
    QRandomGenerator *rand = QRandomGenerator::global();

    for (int i = 0; i < DES_IV_SIZE; ++i) {
        iv[i] = (char)rand->generate();
    }

    return iv;
}

QByteArray CryptoUtils::encryptDES_CBC(const QByteArray &plainData, const QByteArray &key, const QByteArray &iv)
{
    if (key.size() != DES_KEY_SIZE || iv.size() != DES_IV_SIZE) {
        qCritical() << "DES Key or IV size is incorrect";
        return QByteArray();
    }

    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    QByteArray ciphertext;

    ciphertext.resize(plainData.size() + DES_KEY_SIZE);

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        qCritical() << "EVP_CIPHER_CTX_new failed";
        return QByteArray();
    }
    EVP_CIPHER_CTX_init(ctx);

    if (1 != EVP_EncryptInit_ex(ctx, EVP_des_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
        qCritical() << "EVP_EncryptInit_ex failed";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    if (1 != EVP_EncryptUpdate(ctx, (unsigned char*)ciphertext.data(), &len, (const unsigned char*)plainData.constData(), plainData.size())) {
        qCritical() << "EVP_EncryptUpdate failed";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char*)ciphertext.data() + len, &len)) {
        qCritical() << "EVP_EncryptFinal_ex failed";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

QByteArray CryptoUtils::decryptDES_CBC(const QByteArray &encryptedData, const QByteArray &key, const QByteArray &iv)
{
    if (key.size() != DES_KEY_SIZE || iv.size() != DES_IV_SIZE) {
        qCritical() << "DES Key or IV size is incorrect";
        return QByteArray();
    }

    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    QByteArray plaintext;

    plaintext.resize(encryptedData.size());

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        qCritical() << "EVP_CIPHER_CTX_new failed";
        return QByteArray();
    }
    EVP_CIPHER_CTX_init(ctx);

    if (1 != EVP_DecryptInit_ex(ctx, EVP_des_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
        qCritical() << "EVP_DecryptInit_ex failed";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    if (1 != EVP_DecryptUpdate(ctx, (unsigned char*)plaintext.data(), &len, (const unsigned char*)encryptedData.constData(), encryptedData.size())) {
        qCritical() << "EVP_DecryptUpdate failed";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char*)plaintext.data() + len, &len)) {
        qCritical() << "EVP_DecryptFinal_ex failed";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    plaintext.resize(plaintext_len);
    return plaintext;
}
