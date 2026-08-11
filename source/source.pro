######################################################################
# egs_brachy_GUI — Qt 6 project file
######################################################################

QT += widgets
QT += charts
LIBS += -lz
TEMPLATE = app
TARGET = ../eb_gui
INCLUDEPATH += .

CONFIG += c++17
# Prefer a plain executable (../eb_gui) over an .app bundle on macOS
CONFIG -= app_bundle

# Fail the build on APIs deprecated before Qt 6.0
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

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
