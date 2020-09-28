cmake_minimum_required(VERSION 2.8)

project(aimNode)

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(PROJECT_NAME aimNode)

set(SOURCE_FILES
    aimNode.h
    aimNode.cpp
    mainPlugin.cpp

)

set(LIBRARIES
      OpenMaya
      Foundation

)

build_plugin()
