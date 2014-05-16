QMAKE_CXXFLAGS += -std=c++11

HEADERS = mainwindow.h \
          board.h \
          minimax.h \
          boardwidget.h

SOURCES = main.cpp \
          mainwindow.cpp \
          board.cpp \
          minimax.cpp \
          boardwidget.cpp

RESOURCES = images.qrc

MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rcc
