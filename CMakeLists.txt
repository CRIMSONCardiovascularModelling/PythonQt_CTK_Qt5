cmake_minimum_required(VERSION 2.8)

#-----------------------------------------------------------------------------
project(PythonQt)
#-----------------------------------------------------------------------------

# NOTE: Only Release builds are supported. Debug builds don't seem to compile properly.

#----------------------------------------------------------------------------
# Qt version

# We want Qt5 for this, so I'm going to forcibly set the required version to 5
# without this line, it will expect us to predefine Qt5_DIR (e.g., from command line)
# and you won't be able to browse for Qt5_DIR through cmakegui
#
# there is probably still more Qt4 stuff that can be cleaned up, though.

# Note that for me, Qt5_DIR is C:/Qt/Qt5.7.0/5.7/msvc2013_64/lib/cmake/Qt5
unset(QT_QMAKE_EXECUTABLE CACHE)

set(PythonQt_QT_VERSION "5")

# Sanity checks
if(DEFINED Qt5_DIR AND DEFINED QT_QMAKE_EXECUTABLE)
  message(FATAL_ERROR
    "${PROJECT_NAME} shoult NOT be configured setting both Qt5_DIR and QT_QMAKE_EXECUTABLE options.
To build with Qt4, specify QT_QMAKE_EXECUTABLE. To build with Qt5, specify  Qt5_DIR.")
endif()

# Requirements
set(minimum_required_qt5_version "5.3.0")

set(minimum_required_qt_version ${minimum_required_qt${PythonQt_QT_VERSION}_version})

# Qt components
set(qt5libs Core Widgets Network OpenGL Sql Svg UiTools Xml XmlPatterns)
set(qtlibs ${qt${PythonQt_QT_VERSION}libs})
  
# Webkit need to be removed Qt >= 5.6
if(PythonQt_QT_VERSION MATCHES 5)
  find_package(Qt5 ${minimum_required_qt5_version} COMPONENTS Core REQUIRED)
endif()
#-----------------------------------------------------------------------------
# Python libraries

find_package(PythonLibs REQUIRED)
include_directories("${PYTHON_INCLUDE_DIR}")
add_definitions(
  -DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK
  -DPYTHONQT_SUPPORT_NAME_PROPERTY
  )

#-----------------------------------------------------------------------------
# Build options

if(NOT DEFINED PythonQt_INSTALL_RUNTIME_DIR)
  set(PythonQt_INSTALL_RUNTIME_DIR bin)
endif()

if(NOT DEFINED PythonQt_INSTALL_LIBRARY_DIR)
  set(PythonQt_INSTALL_LIBRARY_DIR lib${LIB_SUFFIX})
endif()

if(NOT DEFINED PythonQt_INSTALL_ARCHIVE_DIR)
  set(PythonQt_INSTALL_ARCHIVE_DIR lib${LIB_SUFFIX})
endif()

if(NOT DEFINED PythonQt_INSTALL_INCLUDE_DIR)
  set(PythonQt_INSTALL_INCLUDE_DIR include/PythonQt)
endif()

# Hard code wrapping to a known good configuration
set(PythonQt_Wrap_QtAll           ON CACHE BOOL "Make all of QtAll available in python"   FORCE)
set(PythonQt_Wrap_Qtcore          ON CACHE BOOL "Make all of Qtcore available in python"  FORCE)
set(PythonQt_Wrap_Qtgui           ON CACHE BOOL "Make all of Qtgui available in python"  FORCE)
set(PythonQt_Wrap_Qtnetwork       ON CACHE BOOL "Make all of Qtnetwork available in python"  FORCE)
set(PythonQt_Wrap_Qtopengl        ON CACHE BOOL "Make all of Qtopengl available in python"  FORCE)
set(PythonQt_Wrap_Qtsql           ON CACHE BOOL "Make all of Qtsql available in python"  FORCE)
set(PythonQt_Wrap_Qtsvg           ON CACHE BOOL "Make all of Qtsvg available in python"  FORCE)
set(PythonQt_Wrap_Qtuitools       ON CACHE BOOL "Make all of Qttuitools available in python"  FORCE)
set(PythonQt_Wrap_Qtxml           ON CACHE BOOL "Make all of Qtxml available in python"  FORCE)

set(PythonQt_Wrap_Qtxmlpatterns   OFF CACHE BOOL "Don't turn this on, this doesn't even compile"  FORCE)

option(PythonQt_DEBUG "Enable/Disable PythonQt debug output -- Don't turn this on, it doesn't compile!" OFF)
if(PythonQt_DEBUG)
  add_definitions(-DPYTHONQT_DEBUG)
else()
  remove_definitions(-DPYTHONQT_DEBUG)
endif()

#-----------------------------------------------------------------------------
# Setup Qt


# Required components
set(qt_required_components Core Widgets)
foreach(qtlib ${qtlibs})
  set(qt_wrapped_lib ${qtlib_to_wraplib_${qtlib}})
  if(${PythonQt_Wrap_Qt${qt_wrapped_lib}})
    list(APPEND qt_required_components ${qtlib} ${qt${PythonQt_QT_VERSION}_wrapped_lib_depends_${qt_wrapped_lib}})
  endif()
endforeach()
if(BUILD_TESTING)
  list(APPEND qt_required_components Test)
endif()
list(REMOVE_DUPLICATES qt_required_components)

message(STATUS "${PROJECT_NAME}: Required Qt components [${qt_required_components}]")
message("Attempting to find Qt5")
find_package(Qt5 ${minimum_required_qt_version} COMPONENTS ${qt_required_components} REQUIRED)

set(QT_LIBRARIES )
foreach(qtlib ${qt_required_components})
  include_directories(${Qt5${qtlib}_INCLUDE_DIRS})
  add_definitions(${Qt5${qtlib}_DEFINITIONS})
  list(APPEND QT_LIBRARIES ${Qt5${qtlib}_LIBRARIES})
endforeach()

set(QT_VERSION_MAJOR ${Qt5Core_VERSION_MAJOR})
set(QT_VERSION_MINOR ${Qt5Core_VERSION_MINOR})

macro(pythonqt_wrap_cpp)
  qt5_wrap_cpp(${ARGV})
endmacro()


if(UNIX)
  find_package(OpenGL)
  if(OPENGL_FOUND)
    list(APPEND QT_LIBRARIES ${OPENGL_LIBRARIES})
  endif()
endif()
#-----------------------------------------------------------------------------
# The variable "generated_cpp_suffix" allows to conditionnally compile the generated wrappers
# associated with the Qt version being used.

set(generated_cpp_suffix_46 _47)
set(generated_cpp_suffix_52 _50)
set(generated_cpp_suffix_51 _50)

set(generated_cpp_suffix "_${QT_VERSION_MAJOR}${QT_VERSION_MINOR}")
if(DEFINED generated_cpp_suffix_${QT_VERSION_MAJOR}${QT_VERSION_MINOR})
  set(generated_cpp_suffix "${generated_cpp_suffix_${QT_VERSION_MAJOR}${QT_VERSION_MINOR}}")
elseif(${QT_VERSION_MAJOR}.${QT_VERSION_MINOR} VERSION_GREATER "5.4")
  set(generated_cpp_suffix "_54")
endif()

#-----------------------------------------------------------------------------
# Sources

set(sources
    src/PythonQtBoolResult.cpp
    src/PythonQtClassInfo.cpp
    src/PythonQtClassWrapper.cpp
    src/PythonQtConversion.cpp
    src/PythonQt.cpp
    src/PythonQtImporter.cpp
    src/PythonQtInstanceWrapper.cpp
    src/PythonQtMethodInfo.cpp
    src/PythonQtMisc.cpp
    src/PythonQtObjectPtr.cpp
    src/PythonQtQFileImporter.cpp
    src/PythonQtSignalReceiver.cpp
    src/PythonQtSlot.cpp
    src/PythonQtSignal.cpp
    src/PythonQtStdDecorators.cpp
    src/PythonQtStdIn.cpp
    src/PythonQtStdOut.cpp
    src/gui/PythonQtScriptingConsole.cpp

    generated_cpp${generated_cpp_suffix}/PythonQt_QtBindings.cpp

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin_init.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin_init.cpp
)

#-----------------------------------------------------------------------------
# List headers.  This is list is used for the install command.

set(headers
    src/PythonQtBoolResult.h
    src/PythonQtClassInfo.h
    src/PythonQtClassWrapper.h
    src/PythonQtConversion.h
    src/PythonQtCppWrapperFactory.h
    src/PythonQtDoc.h
    src/PythonQt.h
    src/PythonQtImporter.h
    src/PythonQtImportFileInterface.h
    src/PythonQtInstanceWrapper.h
    src/PythonQtMethodInfo.h
    src/PythonQtMisc.h
    src/PythonQtObjectPtr.h
    src/PythonQtQFileImporter.h
    src/PythonQtSignalReceiver.h
    src/PythonQtSlot.h
    src/PythonQtSignal.h
    src/PythonQtStdDecorators.h
    src/PythonQtStdIn.h
    src/PythonQtStdOut.h
    src/PythonQtSystem.h
    src/PythonQtUtils.h
    src/PythonQtVariants.h
    src/PythonQtPythonInclude.h
    generated_cpp${generated_cpp_suffix}/PythonQt_QtBindings.h
)

#-----------------------------------------------------------------------------
# Headers that should run through moc

set(moc_sources
    src/PythonQt.h
    src/PythonQtSignalReceiver.h
    src/PythonQtStdDecorators.h
    src/gui/PythonQtScriptingConsole.h

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.h
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.h
)

#-----------------------------------------------------------------------------
# Add extra sources
foreach(qtlib ${qt_wrapped_libs})

  if (${PythonQt_Wrap_Qt${qtlib}})

    ADD_DEFINITIONS(-DPYTHONQT_WRAP_Qt${qtlib})

    set(file_prefix generated_cpp${generated_cpp_suffix}/com_trolltech_qt_${qtlib}/com_trolltech_qt_${qtlib})

    foreach(index RANGE 0 12)

      # Source files
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file_prefix}${index}.cpp)
        list(APPEND sources ${file_prefix}${index}.cpp)
      endif()

      # Headers that should run through moc
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file_prefix}${index}.h)
        list(APPEND moc_sources ${file_prefix}${index}.h)
      endif()

    endforeach()

    list(APPEND sources ${file_prefix}_init.cpp)

  endif()
endforeach()

#-----------------------------------------------------------------------------
# Do wrapping
pythonqt_wrap_cpp(gen_moc_sources ${moc_sources})

#-----------------------------------------------------------------------------
# Build the library

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

add_library(PythonQt SHARED
            ${sources}
            ${gen_moc_sources}
  )
set_target_properties(PythonQt PROPERTIES DEFINE_SYMBOL PYTHONQT_EXPORTS)

#
# That should solve linkage error on Mac when the project is used in a superbuild setup
# See http://blog.onesadcookie.com/2008/01/installname-magic.html
#
set_target_properties(PythonQt  PROPERTIES
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  )

target_link_libraries(PythonQt
              ${PYTHON_LIBRARY}
              ${QT_LIBRARIES}
  )

#-----------------------------------------------------------------------------
# Install library (on windows, put the dll in 'bin' and the archive in 'lib')

install(TARGETS PythonQt
        RUNTIME DESTINATION ${PythonQt_INSTALL_RUNTIME_DIR}
        LIBRARY DESTINATION ${PythonQt_INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION ${PythonQt_INSTALL_ARCHIVE_DIR})
install(FILES ${headers} DESTINATION ${PythonQt_INSTALL_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Testing

option(BUILD_TESTING "Build the testing tree." OFF)
include(CTest)

if(BUILD_TESTING)
  create_test_sourcelist(test_sources PythonQtCppTests.cpp
    tests/PythonQtTestMain.cpp
    )

  set_property(SOURCE tests/PythonQtTestMain.cpp PROPERTY COMPILE_DEFINITIONS "main=tests_PythonQtTestMain")

  list(APPEND test_sources
    tests/PythonQtTests.cpp
    tests/PythonQtTests.h
    )

  pythonqt_wrap_cpp(test_sources
    tests/PythonQtTests.h
    )

  if(PythonQt_Wrap_Qtcore)
    include_directories(generated_cpp${generated_cpp_suffix})

    list(APPEND test_sources
      tests/PythonQtTestCleanup.cpp
      tests/PythonQtTestCleanup.h
      )
    pythonqt_wrap_cpp(test_sources
      tests/PythonQtTestCleanup.h
      )

    set_property(SOURCE tests/PythonQtTestMain.cpp APPEND PROPERTY COMPILE_DEFINITIONS "PythonQt_Wrap_Qtcore")
  endif()

  add_executable(PythonQtCppTests ${test_sources})
  target_link_libraries(PythonQtCppTests PythonQt)

  add_test(
    NAME tests_PythonQtTestMain
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:PythonQtCppTests> tests/PythonQtTestMain
    )
endif()

