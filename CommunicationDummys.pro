QT       += core gui statemachine mqtt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dummywindowgw2.cpp \
    dummywindowgw4.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    datatypes.h \
    dummywindowgw2.h \
    dummywindowgw4.h \
    mainwindow.h

FORMS += \
    dummywindowgw2.ui \
    dummywindowgw4.ui \
    mainwindow.ui

RC_ICONS = haw.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
