#-------------------------------------------------
#
# Project created by QtCreator 2019-09-22T09:34:39
#
#-------------------------------------------------

QT      += core gui widgets

TARGET   = Gomer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    boardrectobject.cpp \
    gamecontrol.cpp \
    libertycountdisplay.cpp \
    main.cpp \
    mainwindow.cpp \
    board.cpp \
    stonegroup.cpp \
    stones.cpp \
    stone.cpp \
    boardscene.cpp \
    boardrect.cpp \
    territory.cpp \
    territorygroup.cpp \
    territorysquare.cpp

HEADERS += \
    boardrectobject.h \
    gamecontrol.h \
    libertycountdisplay.h \
    mainwindow.h \
    board.h \
    stonegroup.h \
    stones.h \
    stone.h \
    boardscene.h \
    boardrect.h \
    territory.h \
    territorygroup.h \
    territorysquare.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
