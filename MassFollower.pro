QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 openssl

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    datatool.cpp \
    stringtools.cpp \
    snowconf.cpp \
    usarchive.cpp \
    loadmodal.cpp \
    wasteofengine.cpp \
    wasteofmasswindow.cpp \
    main.cpp \

HEADERS += \
    datatool.h \
    loadmodal.h \
    snowconf.h \
    stringtools.h \
    usarchive.h \
    wasteofengine.h \
    wasteofmasswindow.h

FORMS += \
    wasteofmasswindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    main.qrc

#unix|win32: LIBS += -L'/Program Files/OpenSSL-Win64/lib/VC/x64/MT/' -llibcrypto_static
#LIBS += -L'/Program Files/OpenSSL-Win64/lib' -lcrypto #$$PWD/
LIBS += -L'/Program Files/OpenSSL-Win64/lib/VC/x64/MT/' -lcrypto

INCLUDEPATH += '/Program Files/OpenSSL-Win64/include'
DEPENDPATH += '/Program Files/OpenSSL-Win64/include'

#win32:!win32-g++: PRE_TARGETDEPS += '/Program Files/OpenSSL-Win64/lib/VC/x64/MT/libcrypto_static.lib'
# else:unix|win32-g++: PRE_TARGETDEPS += '/Program Files/OpenSSL-Win64/lib/VC/x64/MT/liblibcrypto_static.a'
