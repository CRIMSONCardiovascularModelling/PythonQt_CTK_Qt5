# It looks like only a Release build is really supported

TARGET   = PyGettingStarted
TEMPLATE = app

CONFIG += console

# should be something like C:/_FILES/home/PythonQt_CTK/src
INCLUDEPATH += ../../src

# You'll need to edit this to wherever your build folder is for PythonQt
win32::LIBS += C:/_FILES/home/PythonQt_CTK_Build/Release/PythonQt.lib

# You may need to edit this if your Python installation is somewhere else
win32:INCLUDEPATH += C:/Python27/include
win32:LIBS += C:/Python27/libs/python27.lib

contains(QT_MAJOR_VERSION, 5) {
  QT += widgets
}

SOURCES +=                    \
  main.cpp
