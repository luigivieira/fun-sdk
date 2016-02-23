# Copyright (C) 2015-2016 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
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

# Minimum CMake version required 
cmake_minimum_required(VERSION 3.5)

# Project information
project (fun-sdk)

# Version data and header file
set (FSDK_VERSION_MAJOR 1)
set (FSDK_VERSION_MINOR 0)
set (FSDK_VERSION_PATCH 0)
set (FSDK_VERSION "${FSDK_VERSION_MAJOR}.${FSDK_VERSION_MINOR}.${FSDK_VERSION_PATCH}")
configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)
include_directories("${PROJECT_BINARY_DIR}")

# Qt configuration
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed
find_package(Qt5Core)
find_package(Qt5Widgets)

# GUI application
add_subdirectory(gui)