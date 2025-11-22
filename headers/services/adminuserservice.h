#ifndef ADMINUSERSERVICE_H
#define ADMINUSERSERVICE_H

#include <QObject>

class AdminUserService : public QObject
{
    Q_OBJECT
public:
    explicit AdminUserService(QObject *parent = nullptr);

signals:
};

#endif // ADMINUSERSERVICE_H
