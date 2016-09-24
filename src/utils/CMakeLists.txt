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

file(GLOB LIB_FILES *.cpp *.h)

add_library(lib-utils SHARED ${LIB_FILES})

set_target_properties(lib-utils PROPERTIES OUTPUT_NAME fsdk-utils)
set_target_properties(lib-utils PROPERTIES OUTPUT_NAME_DEBUG fsdk-utilsd)
set_target_properties(lib-utils PROPERTIES DEFINE_SYMBOL "COMPILING_LIB")

target_link_libraries(lib-utils Qt5::Core)