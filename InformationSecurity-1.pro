QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

OPENSSL_ROOT = "C:/OpenSSL-Win64"
INCLUDEPATH += $$OPENSSL_ROOT/include
LIBS += -L$$OPENSSL_ROOT/lib
LIBS += -lcrypto -lssl

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    headers/utils/cryptoutils.cpp \
    headers/views/useraccount.cpp \
    main.cpp \
    mainwindow.cpp \
    src/main.cpp \
    src/models/useraccount.cpp \
    src/utils/cryptoutils.cpp \
    src/views/mainwindow.cpp

HEADERS += \
    headers/models/useraccount.h \
    headers/utils/cryptoutils.h \
    headers/views/mainwindow.h \
    headers/views/useraccount.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore \
    README.md
