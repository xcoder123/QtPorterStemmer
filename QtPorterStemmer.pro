#-------------------------------------------------
#
# Project created by QtCreator 2016-11-05T14:10:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtPorterStemmer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    enporterstemmer.cpp \
    lvporterstemmer.cpp

HEADERS  += mainwindow.h \
    enporterstemmer.h \
    lvporterstemmer.h

FORMS    += mainwindow.ui
