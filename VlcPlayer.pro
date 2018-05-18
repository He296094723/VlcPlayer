QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VlcPlayer
TEMPLATE = app

INCLUDEPATH += $$PWD/../vlc-3.0.2/sdk/include
LIBS += $$PWD/../vlc-3.0.2/sdk/lib/libvlc.lib \
        $$PWD/../vlc-3.0.2/sdk/lib/libvlccore.lib

HEADERS += \
        vlc_player.h \
    main_window.h

SOURCES += \
        main.cpp \
        vlc_player.cpp \
    main_window.cpp

