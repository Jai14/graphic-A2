QT       += core gui opengl

TARGET = A2
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    glwidget.cpp \
    objects.cpp \
    point.cpp \
    vector.cpp


HEADERS  += mainwindow.h \
    glwidget.h \
    objects.h \
    point.h \
    vector.h

FORMS    += mainwindow.ui

RESOURCES += resources.qrc

win32: LIBS += -lopengl32
