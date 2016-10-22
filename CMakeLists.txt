# Copyright (C) 2016-2017 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
#
# This file is part of Fun SDK (FSDK).
#
# FSDK is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# FSDK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.1.0)
project (fun-sdk)

#############################################
# Configuration
#############################################

set (FSDK_VERSION_MAJOR 1)
set (FSDK_VERSION_MINOR 0)
set (FSDK_VERSION_PATCH 0)
set (FSDK_VERSION "${FSDK_VERSION_MAJOR}.${FSDK_VERSION_MINOR}.${FSDK_VERSION_PATCH}")
configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Qt5
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 REQUIRED Core Gui Widgets Multimedia MultimediaWidgets Xml)
list(APPEND Qt5_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml)

# OpenCV
find_package(OpenCV REQUIRED core highgui imgproc)

# CSIRO Face Analysis SDK
find_package(CSIRO REQUIRED clmTracker)

#############################################
# Sub-projects
#############################################
include_directories("${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src")

add_subdirectory(src/libs/common)
add_subdirectory(src/libs/face-tracking)
add_subdirectory(src/libs/feature-extraction)

add_subdirectory(src/utils/landmarks-extractor)

add_subdirectory(src/gui/fun-inspector)