#include "headers/md-5/md5hasher.h"

static const quint32 K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static const int S[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

QString MD5Hasher::hash(const QByteArray &data) {
    MD5Hasher hasher;
    return hasher.calculate(data);
}

MD5Hasher::MD5Hasher() {
    A = 0x67452301;
    B = 0xefcdab89;
    C = 0x98badcfe;
    D = 0x10325476;
}

QString MD5Hasher::calculate(const QByteArray &input) {
    QByteArray data = input;
    quint64 originalLengthBits = (quint64)data.size() * 8;

    data.append((char)0x80);

    while (data.size() % 64 != 56) {
        data.append((char)0x00);
    }

    for (int i = 0; i < 8; i++) {
        data.append((char)(originalLengthBits >> (i * 8)));
    }

    for (int i = 0; i < data.size(); i += 64) {
        transform((const quint8*)data.constData() + i);
    }

    QByteArray digest;
    digest.resize(16);
    quint8 *p = (quint8 *)digest.data();

    auto appendUint32 = [](quint8* ptr, quint32 val) {
        ptr[0] = (quint8)(val);
        ptr[1] = (quint8)(val >> 8);
        ptr[2] = (quint8)(val >> 16);
        ptr[3] = (quint8)(val >> 24);
    };

    appendUint32(p, A);
    appendUint32(p + 4, B);
    appendUint32(p + 8, C);
    appendUint32(p + 12, D);

    return QString(digest.toHex());
}

void MD5Hasher::transform(const quint8 block[64]) {
    quint32 a = A, b = B, c = C, d = D;
    quint32 x[16];

    for (int i = 0, j = 0; j < 64; i++, j += 4) {
        x[i] = ((quint32)block[j]) | (((quint32)block[j+1]) << 8) |
               (((quint32)block[j+2]) << 16) | (((quint32)block[j+3]) << 24);
    }

    for (int i = 0; i < 64; ++i) {
        quint32 f, g;
        if (i < 16) {
            f = F(b, c, d);
            g = i;
        } else if (i < 32) {
            f = G(b, c, d);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = H(b, c, d);
            g = (3 * i + 5) % 16;
        } else {
            f = I(b, c, d);
            g = (7 * i) % 16;
        }

        quint32 temp = a + f + x[g] + K[i];
        a = d;
        d = c;
        c = b;
        b = b + rotateLeft(temp, S[i]);
    }

    A += a;
    B += b;
    C += c;
    D += d;
}
