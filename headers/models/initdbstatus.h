#ifndef INITDBSTATUS_H
#define INITDBSTATUS_H

enum class InitDBStatus {
    Success,
    FirstRunSuccess,
    DBFileCorrupted,
    PassphraseIncorrect,
    DecryptionException,
    SystemError,
    FileOpenError
};

#endif // INITDBSTATUS_H
