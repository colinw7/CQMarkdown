TEMPLATE = app

QT += widgets

TARGET = CQMarkdown

DEPENDPATH += .

INCLUDEPATH += . ../include

QMAKE_CXXFLAGS += -std=c++11 -DGNUPLOT_EXPR

CONFIG += debug

# Input
SOURCES += \
CMarkdown.cpp \
CQMarkdown.cpp \
CQMarkdownEdit.cpp \
CQMarkdownMain.cpp \
CQMarkdownPreview.cpp \
main.cpp \

HEADERS += \
CMarkdown.h \
CQMarkdownEdit.h \
CQMarkdown.h \
CQMarkdownMain.h \
CQMarkdownPreview.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../../CCommand/include \
../../CUtil/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CCommand/lib \
-L../../CReadLine/lib \
-L../../CStrUtil/lib \
-L../../CFile/lib \
-L../../COS/lib \
-lCCommand -lCReadLine -lCFile -lCStrUtil -lCOS \
-lreadline -lcurses
