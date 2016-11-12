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

file(GLOB SRC *.cpp *.h ${PROJECT_SOURCE_DIR}/src/application.cpp ${PROJECT_SOURCE_DIR}/src/application.h)
add_executable(util-gabor-test ${SRC})

include_directories("${PROJECT_SOURCE_DIR}/src/libs/common" "${PROJECT_SOURCE_DIR}/src/libs/feature-extraction")
target_link_libraries(util-gabor-test Qt5::Core lib-common lib-feature-extraction)

set_target_properties(util-gabor-test PROPERTIES OUTPUT_NAME fgtest)
set_target_properties(util-gabor-test PROPERTIES OUTPUT_NAME_DEBUG fgtestd)

add_definitions(-DCONSOLE)