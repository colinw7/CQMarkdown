TEMPLATE = app

TARGET = CQMarkdownMain

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
main.cpp \
CQMarkdownMain.cpp \

HEADERS += \
CQMarkdownMain.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../include \
../../CQUtil/include \
../../CCommand/include \
../../CUtil/include \

unix:LIBS += \
-L../lib \
-L../../CQUtil/lib \
-L../../CCommand/lib \
-L../../CReadLine/lib \
-L../../CStrUtil/lib \
-L../../CFile/lib \
-L../../COS/lib \
-lCQMarkdown -lCQUtil -lCCommand -lCReadLine -lCFile -lCStrUtil -lCOS \
-lreadline -lcurses
