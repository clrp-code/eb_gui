######################################################################
# Automatically generated by qmake (3.1) Thu Nov 18 12:06:56 2021
######################################################################

QT += widgets
QT += charts
LIBS += -lz
TEMPLATE = app
TARGET = ../eb_gui
INCLUDEPATH += .

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += data.h \
           interface.h \
           data/DICOM.h \
           data/dose.h \
           data/egsphant.h \
           data/input.h \
           GUI/appInterface.h \
           GUI/doseInterface.h \
           GUI/ebInterface.h \
           GUI/phantInterface.h \
           GUI/sourceInterface.h \
           libraries/gzstream.h
SOURCES += data.cpp \
           interface.cpp \
           main.cpp \
           data/database.cpp \
           data/DICOM.cpp \
           data/dose.cpp \
           data/egsphant.cpp \
           data/input.cpp \
           GUI/appInterface.cpp \
           GUI/doseInterface.cpp \
           GUI/ebInterface.cpp \
           GUI/phantInterface.cpp \
           GUI/sourceInterface.cpp \
           libraries/gzstream.cpp
