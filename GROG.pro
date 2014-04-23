#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T23:19:37
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = grog
TEMPLATE = app


SOURCES += \
    main.cpp\
    grogmainwindow.cpp \
    groginfobar.cpp \
    gromacstool.cpp \
    gromacsoptionsgroup.cpp \
    gromacsoption.cpp \
    gromacsoptionchoose.cpp \
    gromacsoptionenter.cpp \
    gromacsoptionfile.cpp \
    gromacstools.cpp \
    gromacstoolsdefinition.cpp \
    grogsettings.cpp

HEADERS += \
    grogmainwindow.h \
    groginfobar.h \
    gromacstool.h \
    gromacsoptionsgroup.h \
    gromacsoption.h \
    gromacsoptionchoose.h \
    gromacsoptionenter.h \
    gromacsoptionfile.h \
    gromacstools.h \
    gromacstoolsdefinition.h \
    grogsettings.h

FORMS += \
    grogmainwindow.ui \
    gromacsoptionsgroup.ui \
    grogsettings.ui

# QMAKE_CXXFLAGS += -std=c++11 -pedantic -Wall
QMAKE_CXXFLAGS += -std=c++0x -pedantic -Wall


# Add a custom target called "wrapper"
PRE_TARGETDEPS += wrapper
QMAKE_EXTRA_TARGETS += wrapper
wrapper.sources = $${_PRO_FILE_PWD_}/wrapper.c
wrapper.depends = $${wrapper.sources}
wrapper.commands = gcc -O2 -std=c99 -pedantic -Wall -Werror -o wrapper $${wrapper.sources}
