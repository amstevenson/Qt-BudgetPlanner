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
    signupwindow.cpp \
    accountsetupwindow.cpp \
    accountbudgetwindow.cpp \
    tableoperations.cpp

HEADERS  += mainwindow.h \
    jsonparser.h \
    signupwindow.h \
    accountsetupwindow.h \
    accountbudgetwindow.h \
    tableoperations.h

FORMS    += mainwindow.ui \
    accountsetupwindow.ui \
    signupwindow.ui \
    accountbudgetwindow.ui

RESOURCES += \
    Extras.qrc

OTHER_FILES +=
