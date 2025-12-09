#ifndef MAGMACIPHER_H
#define MAGMACIPHER_H

#include <QByteArray>

class MagmaCipher
{
public:
    MagmaCipher();

    bool setKey(const QByteArray &key);

    QByteArray processCTR(const QByteArray &data, const QByteArray &iv);

private:
    static const quint8 Pi[8][16];

    quint32 roundKeys[32];

    quint32 g(quint32 k, quint32 a);

    void encryptBlock(quint32 &n1, quint32 &n2);

    void expandKey(const QByteArray &key);

    void incrementCounter(QByteArray &counter);
};

#endif // MAGMACIPHER_H
