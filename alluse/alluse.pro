QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calc.cpp \
    main.cpp \
    mainwindow.cpp \
    myForce.cpp \
    myPoint.cpp \
    myTriangle.cpp \
    mydrawwidget.cpp \
    mymaterial.cpp

HEADERS += \
    calc.h \
    mainwindow.h \
    myForce.h \
    myPoint.h \
    myTriangle.h \
    mydrawwidget.h \
    mymaterial.h

FORMS += \
    mainwindow.ui


DISTFILES += \
    ../../../Users/47131/gmsh-git-Windows64-sdk/gmsh-git-Windows64-sdk/lib/gmsh-4.12.dll \
    icons/broom.png \
    icons/calculator.png \
    icons/delete.png \
    icons/displacement.png \
    icons/dry-clean.png \
    icons/error-404.png \
    icons/formula.png \
    icons/frame.png \
    icons/keys.png \
    icons/mesh.png \
    icons/paint-brush.png \
    icons/rec.png \
    icons/rectangle.png \
    icons/steel.png



win32: LIBS += -L$$PWD/../../../Users/47131/gmsh-git-Windows64-sdk/gmsh-git-Windows64-sdk/lib/ -lgmsh

INCLUDEPATH += $$PWD/../../../Users/47131/gmsh-git-Windows64-sdk/gmsh-git-Windows64-sdk/include
DEPENDPATH += $$PWD/../../../Users/47131/gmsh-git-Windows64-sdk/gmsh-git-Windows64-sdk/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../Users/47131/gmsh-git-Windows64-sdk/gmsh-git-Windows64-sdk/lib/gmsh.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../Users/47131/gmsh-git-Windows64-sdk/gmsh-git-Windows64-sdk/lib/libgmsh.a


RC_ICONS = mesh2.ico
