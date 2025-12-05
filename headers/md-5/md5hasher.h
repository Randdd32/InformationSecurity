#ifndef MD5HASHER_H
#define MD5HASHER_H

#include <QByteArray>
#include <QString>

class MD5Hasher
{
public:
    MD5Hasher();

    void update(const QByteArray &data);

    void update(const QString &str);

    QString toString();

    void reset();

private:
    void transform(const quint8 block[64]);

    inline quint32 F(quint32 x, quint32 y, quint32 z) { return (x & y) | ((~x) & z); }
    inline quint32 G(quint32 x, quint32 y, quint32 z) { return (x & z) | (y & (~z)); }
    inline quint32 H(quint32 x, quint32 y, quint32 z) { return x ^ y ^ z; }
    inline quint32 I(quint32 x, quint32 y, quint32 z) { return y ^ (x | (~z)); }

    inline quint32 rotateLeft(quint32 x, int n) { return (x << n) | (x >> (32 - n)); }

    void FF(quint32 &a, quint32 b, quint32 c, quint32 d, quint32 x, quint32 s, quint32 ac);
    void GG(quint32 &a, quint32 b, quint32 c, quint32 d, quint32 x, quint32 s, quint32 ac);
    void HH(quint32 &a, quint32 b, quint32 c, quint32 d, quint32 x, quint32 s, quint32 ac);
    void II(quint32 &a, quint32 b, quint32 c, quint32 d, quint32 x, quint32 s, quint32 ac);

    //A B C D
    quint32 state[4];

    quint64 count;

    QByteArray buffer;
};

#endif // MD5HASHER_H
