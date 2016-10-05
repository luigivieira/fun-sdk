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

# CSIRO Face Analysis SDK
find_package(CSIRO REQUIRED clmTracker)


file(GLOB LIB_FILES lib/*.cpp lib/*.h)
file(GLOB APP_FILES app/*.cpp app/*.h)
file(GLOB GUI_FILES gui/*.cpp gui/*.h)

add_library(lib-ft SHARED ${LIB_FILES})
add_executable(app-ft ${APP_FILES})
add_executable(gui-ft ${GUI_FILES})

target_include_directories(lib-ft PUBLIC ${CSIRO_INCLUDE_DIRS} ../utils)
target_include_directories(app-ft PUBLIC lib ../utils)
target_include_directories(gui-ft PUBLIC lib ../utils)

set_target_properties(lib-ft PROPERTIES OUTPUT_NAME fsdk-ft)
set_target_properties(lib-ft PROPERTIES OUTPUT_NAME_DEBUG fsdk-ftd)
set_target_properties(lib-ft PROPERTIES DEFINE_SYMBOL "COMPILING_LIB")
set_target_properties(app-ft PROPERTIES OUTPUT_NAME fsdk-ft)
set_target_properties(app-ft PROPERTIES OUTPUT_NAME_DEBUG fsdk-ftd)
set_target_properties(gui-ft PROPERTIES OUTPUT_NAME fsdk-ft-gui)
set_target_properties(gui-ft PROPERTIES OUTPUT_NAME_DEBUG fsdk-ft-guid)

target_link_libraries(lib-ft Qt5::Core ${OpenCV_LIBS} clmTracker lib-utils)
target_link_libraries(app-ft Qt5::Core lib-utils lib-ft)
target_link_libraries(gui-ft ${Qt5_LIBS} lib-utils lib-ft)