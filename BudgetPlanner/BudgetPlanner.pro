#-------------------------------------------------
#
# Project created by QtCreator 2014-10-22T17:46:32
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BudgetPlanner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    jsonparser.cpp \
    signupwindow.cpp

HEADERS  += mainwindow.h \
    jsonparser.h \
    signupwindow.h

FORMS    += mainwindow.ui \
    signupwindow.ui

RESOURCES += \
    Extras.qrc
