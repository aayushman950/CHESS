QT += widgets
#requires(qtConfig(filedialog))

HEADERS     = mainwindow.h \
              boardwidget.h
RESOURCES   = \
    resources.qrc
SOURCES     = main.cpp \
              boardwidget.cpp \
              mainwindow.cpp

#QMAKE_PROJECT_NAME = dndpuzzle


#INSTALLS += target
