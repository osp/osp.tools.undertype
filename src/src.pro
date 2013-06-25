SOURCES += underline.cpp \
           main.cpp \
           ulglyphitem.cpp \
           ulpageitem.cpp \
           ulglyphcollection.cpp \
           ulglyphscene.cpp \
           ullineeditor.cpp \
           uleditglyph.cpp \
           ulwhiteitem.cpp \
           ulpageview.cpp \
           pagebuffer.cpp \
           ulkerningfactory.cpp \
           ulunit.cpp \
           ulextragraphic.cpp \
           ulextragraphicscene.cpp \
           ulpaintengine.cpp \
           ulpdfdevice.cpp
HEADERS += underline.h \
ulglyphitem.h \
ulpageitem.h \
ulglyphcollection.h \
ulglyphscene.h \
ullineeditor.h \
uleditglyph.h \
ulwhiteitem.h \
ulpageview.h \
pagebuffer.h \
ulkerningfactory.h \
ulunit.h \
ulextragraphic.h \
ulextragraphicscene.h \
ulpaintengine.h \
ulpdfdevice.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
QT += xml
QT += svg
TARGET = ../bin/undertype
RESOURCES = application.qrc
INCLUDEPATH += /usr/include/freetype2
INCLUDEPATH += /opt/local/include/freetype2
LIBS += $$system(freetype-config --libs)
INCLUDEPATH += math.h
LIBS += -L/usr/lib -lm 
DEFINES += QT_DEBUG_DRAW
