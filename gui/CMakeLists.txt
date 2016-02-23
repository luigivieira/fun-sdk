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

# Compiler flags for the gcc compiler
if(NOT WIN32)
    set(CMAKE_C_FLAGS "-Wall -g")
endif()

# Add all source files to SRCS variable
file(GLOB SRCS *.cpp *.h)

# Add the executable name
if(WIN32)
    add_executable(fsdk-gui WIN32 ${SRCS})
else()
    add_executable(fsdk-gui ${SRCS})
endif()

# Use the Widgets module from Qt 5
target_link_libraries(fsdk-gui Qt5::Core)
target_link_libraries(fsdk-gui Qt5::Widgets)