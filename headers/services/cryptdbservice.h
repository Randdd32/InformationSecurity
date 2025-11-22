#ifndef CRYPTDBSERVICE_H
#define CRYPTDBSERVICE_H

#include <QObject>

class CryptDBService : public QObject
{
    Q_OBJECT
public:
    explicit CryptDBService(QObject *parent = nullptr);
    ~CryptDBService();

private:
};

#endif // CRYPTDBSERVICE_H
