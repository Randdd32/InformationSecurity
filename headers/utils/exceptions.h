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
        : BaseException("Critical database error: " + sqlError) {}
};

class UserNotFoundException : public BaseException
{
public:
    UserNotFoundException(const QString& username)
        : BaseException("User not found: " + username) {}
};

class PassphraseIncorrectException : public BaseException
{
public:
    PassphraseIncorrectException(const QString& message)
        : BaseException("Passphrase is incorrect: " + message) {}
};

class DecryptionException : public BaseException
{
public:
    DecryptionException(const QString& message)
        : BaseException("Decryption exception: " + message) {}
};

class FileOpenException : public BaseException
{
public:
    FileOpenException(const QString& message)
        : BaseException("File opening error: " + message) {}
};

class DBFileCorruptedException : public BaseException
{
public:
    DBFileCorruptedException(const QString& message)
        : BaseException("Database file is corrupted: " + message) {}
};

#endif // EXCEPTIONS_H
