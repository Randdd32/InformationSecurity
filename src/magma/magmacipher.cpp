#include "headers/magma/magmacipher.h"

#include <QtEndian>

const quint8 MagmaCipher::Pi[8][16] = {
    {1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2},
    {8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7},
    {5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0},
    {7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12},
    {12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11},
    {11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0},
    {6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15},
    {12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1}
};

MagmaCipher::MagmaCipher()
{
    for(int i = 0; i < 32; i++) {
        roundKeys[i] = 0;
    }
}

void MagmaCipher::setKey(const QByteArray &key)
{
    if (key.size() != 32) {

        return;
    }
    expandKey(key);
}

void MagmaCipher::expandKey(const QByteArray &key)
{
    quint32 masterKeys[8];
    const quint8* kData = reinterpret_cast<const quint8*>(key.constData());

    for (int i = 0; i < 8; ++i) {
        masterKeys[i] = qFromBigEndian<quint32>(kData + i * 4);
    }

    for (int i = 0; i < 32; ++i) {
        if (i < 24) {
            roundKeys[i] = masterKeys[i % 8];
        } else {
            roundKeys[i] = masterKeys[7 - (i % 8)];
        }
    }
}

quint32 MagmaCipher::g(quint32 k, quint32 a)
{
    quint32 internal = a + k;

    quint32 out = 0;
    for (int i = 0; i < 8; ++i) {
        quint8 part = (internal >> (4 * i)) & 0x0F;
        out |= (static_cast<quint32>(Pi[i][part]) << (4 * i));
    }

    return (out << 11) | (out >> 21);
}


void MagmaCipher::encryptBlock(quint32 &n1, quint32 &n2)
{
    for (int i = 0; i < 32; i++) {
        quint32 internal = g(roundKeys[i], n1);
        internal = internal ^ n2;

        if (i < 31) {
            n2 = n1;
            n1 = internal;
        } else {
            n2 = internal;
        }
    }
}

QByteArray MagmaCipher::processCTR(const QByteArray &data, const QByteArray &iv)
{
    QByteArray result;
    result.reserve(data.size());

    QByteArray counter = iv;
    if (counter.size() < 8) {
        counter.resize(8, 0x00);
    }

    QByteArray gammaBlock(8, 0);
    int numBlocks = data.size() / 8;
    int remaining = data.size() % 8;

    const quint8* inputPtr = reinterpret_cast<const quint8*>(data.constData());

    for (int i = 0; i < numBlocks + (remaining > 0 ? 1 : 0); ++i) {
        quint32 n1 = qFromBigEndian<quint32>(counter.constData());
        quint32 n2 = qFromBigEndian<quint32>(counter.constData() + 4);

        encryptBlock(n1, n2);

        qToBigEndian(n1, reinterpret_cast<quint8*>(gammaBlock.data()));
        qToBigEndian(n2, reinterpret_cast<quint8*>(gammaBlock.data() + 4));

        int currentBlockSize = (i == numBlocks) ? remaining : 8;

        for (int j = 0; j < currentBlockSize; ++j) {
            result.append(inputPtr[i * 8 + j] ^ gammaBlock[j]);
        }

        incrementCounter(counter);
    }

    return result;
}

void MagmaCipher::incrementCounter(QByteArray &ctr)
{
    quint8* ptr = reinterpret_cast<quint8*>(ctr.data());
    quint16 carry = 1;
    quint16 temp;

    for (int i = 7; i >= 0; i--) {
        if (carry == 0)
            break;

        temp = ptr[i] + carry;
        ptr[i] = temp & 0xFF;
        carry = temp >> 8;
    }
}
