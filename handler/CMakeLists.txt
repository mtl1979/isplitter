cmake_minimum_required(VERSION 3.16)

project(SplitterShellExtContextMenuHandler VERSION 1.0.0 LANGUAGES CXX)

add_definitions(-DUNICODE)

add_library(SplitterShellExtContextMenuHandler SHARED
  ClassFactory.cpp
  ClassFactory.h
  dllmain.cpp
  FileContextMenuExt.cpp
  FileContextMenuExt.h
  GlobalExportFunctions.def
  Reg.cpp
  Reg.h
  resource.h
  ShellExtContextMenuHandler.rc
)

set_target_properties(SplitterShellExtContextMenuHandler PROPERTIES VERSION ${PROJECT_VERSION})

install(TARGETS SplitterShellExtContextMenuHandler LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
