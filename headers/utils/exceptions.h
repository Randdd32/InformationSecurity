#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <QString>

class BaseException : public std::runtime_error
{
public:
    BaseException(const QString& message) : std::runtime_error(message.toStdString()) {}
    virtual ~BaseException() noexcept = default;
};

class DBException : public BaseException
{
public:
    DBException(const QString& sqlError)
        : BaseException("Критическая ошибка базы данных: " + sqlError) {}
};

class UserNotFoundException : public BaseException
{
public:
    UserNotFoundException(const QString& username)
        : BaseException("Пользователь не найден: " + username) {}
};

#endif // EXCEPTIONS_H
