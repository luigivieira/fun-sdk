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

file(GLOB SRC *.cpp *.h)
add_library(lib-feature-extraction SHARED ${SRC})

include_directories("${CSIRO_INCLUDE_DIRS}" "${PROJECT_SOURCE_DIR}/src/libs/face-tracking" "${PROJECT_SOURCE_DIR}/src/libs/common")
target_link_libraries(lib-feature-extraction Qt5::Core ${OpenCV_LIBS} lib-face-tracking lib-common)

set_target_properties(lib-feature-extraction PROPERTIES OUTPUT_NAME fextraction)
set_target_properties(lib-feature-extraction PROPERTIES OUTPUT_NAME_DEBUG fextractiond)
set_target_properties(lib-feature-extraction PROPERTIES DEFINE_SYMBOL "SHARED_LIB")