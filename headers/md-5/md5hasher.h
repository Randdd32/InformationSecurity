#ifndef MD5HASHER_H
#define MD5HASHER_H

#include <QByteArray>
#include <QString>

class MD5Hasher
{
public:
    static QString hash(const QByteArray &data);

private:
    MD5Hasher();

    QString calculate(const QByteArray &input);

    void transform(const quint8 block[64]);

    inline quint32 F(quint32 x, quint32 y, quint32 z) { return (x & y) | ((~x) & z); }
    inline quint32 G(quint32 x, quint32 y, quint32 z) { return (x & z) | (y & (~z)); }
    inline quint32 H(quint32 x, quint32 y, quint32 z) { return x ^ y ^ z; }
    inline quint32 I(quint32 x, quint32 y, quint32 z) { return y ^ (x | (~z)); }

    inline quint32 rotateLeft(quint32 x, int n) { return (x << n) | (x >> (32 - n)); }

    quint32 A, B, C, D;
};

#endif // MD5HASHER_H
