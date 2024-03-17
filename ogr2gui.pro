# ogr2gui 0.6
QT += core gui widgets
CONFIG += c++14
TEMPLATE = app
TARGET = ogr2gui
DEPENDPATH += . inc src
# INCLUDEPATH += . inc src /usr/local/include

# Input
HEADERS += config.h \
	   inc/App.h \
           inc/Dta.h \
           inc/Ogr.h \
	   inc/Inf.h \
           src/App.cpp \
           src/Ogr.cpp

SOURCES += src/App.cpp \
	   src/Ogr.cpp \
	   src/Inf.cpp

# LIBS += -L/usr/local/lib -lgdal -lproj -lpq

CONFIG += static

QT += sql

win32:CONFIG(release, debug|release): LIBS += -LD:/gdal3.7.2/lib/ -lgdal_i -lproj -llibpq
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/gdal3.7.2/lib/ -lgdal_i -lproj -llibpq

INCLUDEPATH += D:/gdal3.7.2/include
DEPENDPATH += D:/gdal3.7.2/include

TRANSLATIONS    +=  ogr2gui_zh.ts

