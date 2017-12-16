TEMPLATE = lib

TARGET = CQMarkdown

QT += widgets

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

CONFIG += staticlib

SOURCES += \
CMarkdown.cpp \
CQMarkdown.cpp \
CQMarkdownEdit.cpp \
CQMarkdownPreview.cpp \

HEADERS += \
../include/CMarkdown.h \
../include/CQMarkdownEdit.h \
../include/CQMarkdown.h \
../include/CQMarkdownPreview.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CQUtil/include \
../../CCommand/include \
../../CUtil/include \
