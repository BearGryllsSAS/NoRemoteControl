QT       += core gui
QT       += network
QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cutavator.cpp \
    dialog.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    registerwindow.cpp

HEADERS += \
    cutavator.h \
    dialog.h \
    login.h \
    mainwindow.h \
    registerwindow.h

FORMS += \
    cutavator.ui \
    dialog.ui \
    login.ui \
    mainwindow.ui \
    registerwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Resource/Pictures/suliao.png

RESOURCES += \
    resource.qrc
