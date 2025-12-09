#include "headers/utils/cryptoutils.h"

#include <QFile>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QtEndian>

QByteArray CryptoUtils::keyFromPassword(const QString &password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
}

QByteArray CryptoUtils::keyFromFile(const QString &filePath, bool &ok) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ok = false;
        return QByteArray();
    }
    QByteArray data = file.readAll();
    file.close();
    ok = true;
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256);
}

QByteArray CryptoUtils::generateIV() {
    quint32 randomPart = QRandomGenerator::global()->generate();
    QByteArray iv;
    iv.resize(4);
    qToBigEndian(randomPart, reinterpret_cast<quint8*>(iv.data()));
    return iv;
}
