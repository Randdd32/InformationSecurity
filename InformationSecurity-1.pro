QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

CONFIG += c++17

OPENSSL_ROOT = "C:/OpenSSL-Win64"
INCLUDEPATH += $$OPENSSL_ROOT/include
LIBS += -L$$OPENSSL_ROOT/lib/VC/x64/MD -llibcrypto -llibssl

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/models/useraccount.cpp \
    src/services/adminuserservice.cpp \
    src/services/authservice.cpp \
    src/services/cryptdbservice.cpp \
    src/services/encryptedfilehandler.cpp \
    src/utils/cryptoutils.cpp \
    src/utils/passwordvalidator.cpp \
    src/views/changepassworddialog.cpp \
    src/views/initpassphrasewindow.cpp \
    src/views/loginwindow.cpp \
    src/views/mainwindow.cpp

HEADERS += \
    headers/models/initdbstatus.h \
    headers/models/useraccount.h \
    headers/services/adminuserservice.h \
    headers/services/authservice.h \
    headers/services/cryptdbservice.h \
    headers/services/encryptedfilehandler.h \
    headers/utils/cryptoutils.h \
    headers/utils/exceptions.h \
    headers/utils/passwordvalidator.h \
    headers/views/changepassworddialog.h \
    headers/views/initpassphrasewindow.h \
    headers/views/loginwindow.h \
    headers/views/mainwindow.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore \
    README.md \
    resources/images/mw-icon.png \
    resources/styles.qss

RESOURCES += \
    resources/resources.qrc \
