#-------------------------------------------------
#
# Project created by QtCreator 2014-04-24T01:45:24
#
#-------------------------------------------------

QT       += multimedia core multimediawidgets gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11

TARGET = GUI_DESIGN_TRIAL2
TEMPLATE = app


SOURCES += main.cpp\
        videowidget.cpp\
        gen-cpp/shoggoth_constants.cpp\
        gen-cpp/Tracking.cpp\
        gen-cpp/shoggoth_types.cpp

HEADERS  += videowidget.h\
         gen-cpp/shoggoth_constants.h\
         gen-cpp/Tracking.h\
         gen-cpp/shoggoth_types.h


target.path = $$[QT_INSTALL_EXAMPLES]/multimediawidgets/videowidget
INSTALLS += target


INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib/ -lthrift -lvlc

