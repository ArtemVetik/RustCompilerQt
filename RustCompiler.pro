#-------------------------------------------------
#
# Project created by QtCreator 2020-06-14T21:21:56
#
#-------------------------------------------------

QT       += core gui
QMAKE_LFLAGS += -static -static-libgcc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RustCompiler
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

CONFIG += c++14

SOURCES += \
        aboutdialog.cpp \
        codeeditor.cpp \
        filehandler.cpp \
        highlighter.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        aboutdialog.h \
        codeeditor.h \
        filehandler.h \
        highlighter.h \
        mainwindow.h

FORMS += \
        aboutdialog.ui \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32: RC_ICONS = $$PWD/Images/RustCompilerIcon.ico

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/CompilerCore/ -llibRustCompilerDLL.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/CompilerCore/ -llibRustCompilerDLL.dll
else:unix: LIBS += -L$$PWD/CompilerCore/ -llibRustCompilerDLL.dll

INCLUDEPATH += $$PWD/CompilerCore
DEPENDPATH += $$PWD/CompilerCore
