# It looks like only a Release build is really supported

TARGET   = PyGettingStarted
TEMPLATE = app

CONFIG += console

# should be something like C:/_FILES/home/PythonQt_CTK/src
INCLUDEPATH += ../../src

# You'll need to edit this to wherever your build folder is for PythonQt
#
# Also if you want to run this project conveniently from QtCreator, copy the dll file from the directory below into the build directory *for this project*, e.g.,
# for me I copied PythonQt.dll from C:\_FILES\dev\PythonQt_CTK_Build\Release\ to C:\_FILES\dev\PythonQt_CTK\examples\build-MyPythonQtTestProj-Desktop_Qt_5_7_0_MSVC2013_64bit-Release\release
win32::LIBS += C:\_FILES\dev\PythonQt_CTK_Build\Release\PythonQt.lib

# You may need to edit this if your Python installation is somewhere else
win32:INCLUDEPATH += C:/Python27/include
win32:LIBS += C:/Python27/libs/python27.lib

contains(QT_MAJOR_VERSION, 5) {
  QT += widgets
}

SOURCES +=                    \
  main.cpp
