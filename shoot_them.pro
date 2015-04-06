#TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += irrlicht-1.5.2/include
INCLUDEPATH += irrKlang-64bit-1.5.0/include

DEPENDPATH += irrlicht-1.5.2
DEPENDPATH += irrKlang-64bit-1.5.0

LIBS += -L../../irrlicht-1.5.2/lib/Linux -L../../irrKlang-64bit-1.5.0/lib/linux-gcc-64 -lIrrlicht -lIrrKlang -lGL -lGLU -lX11 -lXrandr -lXext -lXxf86vm

#include(deployment.pri)
#qtcAddDeployment()

