QT       += core gui
QT       += network
QT       += widgets
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    autocleartextedit.cpp \
    changepassword.cpp \
    choicedialog.cpp \
    cutavator.cpp \
    dialog.cpp \
    findpassword.cpp \
    friendmessage.cpp \
    login.cpp \
    main.cpp \
    mainwindow-else.cpp \
    mainwindow.cpp \
    registerwindow.cpp

HEADERS += \
    autocleartextedit.h \
    changepassword.h \
    choicedialog.h \
    cutavator.h \
    dialog.h \
    findpassword.h \
    friendmessage.h \
    login.h \
    mainwindow-else.h \
    mainwindow.h \
    registerwindow.h

FORMS += \
    changepassword.ui \
    choicedialog.ui \
    cutavator.ui \
    dialog.ui \
    findpassword.ui \
    friendmessage.ui \
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
