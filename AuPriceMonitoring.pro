QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:/OpenSSL-Win32/include

SOURCES += \
    aupricemonitor.cpp \
    include/mainmonitorwin.cpp \
    include/monitorthread.cpp \
    include/traymanagement.cpp \
    main.cpp

HEADERS += \
    aupricemonitor.h \
    include/configmanagement.h \
    include/default.h \
    include/mainmonitorwin.h \
    include/monitorthread.h \
    include/traymanagement.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
