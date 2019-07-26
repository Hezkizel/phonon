# Copyright (c) 2008, Matthias Kretz <kretz@kde.org>
# Copyright (c) 2010, Mark Kretschmann <kretschmann@kde.org>
# Copyright (c) 2010-2019, Harald Sitter <sitter@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#-------------------------------------------------------------------------------

# Include Additional Magic

include(CheckCXXCompilerFlag)

get_filename_component(phonon_cmake_module_dir ${CMAKE_CURRENT_LIST_FILE} PATH)

include(${phonon_cmake_module_dir}/PhononMacros.cmake)
include(${phonon_cmake_module_dir}/MacroLogFeature.cmake)
include(${phonon_cmake_module_dir}/MacroOptionalFindPackage.cmake)
include(${phonon_cmake_module_dir}/MacroEnsureVersion.cmake)

# ECM

find_package(ECM 1.7.0 NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(ECMPoQmTools)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMGeneratePriFile)

set(QT_IMPORTS_DIR "${KDE_INSTALL_QTQUICKIMPORTSDIR}")
set(QT_MKSPECS_DIR "${ECM_MKSPECS_INSTALL_DIR}")
set(QT_PLUGINS_DIR "${KDE_INSTALL_QTPLUGINDIR}")

set(SHARE_INSTALL_PREFIX "${KDE_INSTALL_DATAROOTDIR}")
set(INCLUDE_INSTALL_DIR "${KDE_INSTALL_INCLUDEDIR}/${PHONON_LIB_SONAME}")
set(BIN_INSTALL_DIR "${KDE_INSTALL_BINDIR}")
set(LIB_INSTALL_DIR "${KDE_INSTALL_LIBDIR}")
set(PLUGIN_INSTALL_DIR "${LIB_INSTALL_DIR}/qt5")
set(BACKEND_INSTALL_DIR "${KDE_INSTALL_QTPLUGINDIR}/${PHONON_LIB_SONAME}_backend")
if(WIN32) # Imported from Phonon VLC
    set(BACKEND_INSTALL_DIR "bin/${PHONON_LIB_SONAME}_backend")
endif()
set(ICON_INSTALL_DIR "${KDE_INSTALL_ICONDIR}")
# removed service: was only used for kde4 desktop file installation
set(DBUS_INTERFACES_INSTALL_DIR "${KDE_INSTALL_DBUSINTERFACEDIR}")
set(DBUS_SERVICES_INSTALL_DIR "${KDE_INSTALL_DBUSSERVICEDIR}")

set(INSTALL_TARGETS_DEFAULT_ARGS ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

#---- essential deps

find_package(Qt5Core)
macro_log_feature(Qt5Core_FOUND "Qt5 Core (qtbase)" "" "" TRUE)

find_package(Qt5Gui)
macro_log_feature(Qt5Gui_FOUND "Qt5 Gui (qtbase)" "" "" TRUE)

find_package(Qt5Widgets)
macro_log_feature(Qt5Widgets_FOUND "Qt5 Widgets (qtbase)" "" "" TRUE)

#---- compat
# Qt 5.11 dropped this. We rely on it to enable joint Qt4+5 dep mapping.
if(NOT COMMAND qt5_use_modules)
    macro (qt5_use_modules target)
        set(_deps "")
        foreach (arg ${ARGN})
            list(APPEND _deps Qt5::${arg})
        endforeach ()
        target_link_libraries(${target} ${_deps})
    endmacro (qt5_use_modules target args)
endif()

# ---- more compat
set(QT_INCLUDES ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Widgets_INCLUDE_DIRS}
                ${Qt5DBus_INCLUDE_DIRS})

# - Automoc (using builtin introduced in 2.8.5)
# NOTE: the compatiibility macros are actively used by the backends, so they
#       cannot be dropped unless the backends get a major release removing all
#       use of them first.

# Compatiblity Macros for old automoc nonesense
macro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
    add_executable(${_target_NAME} ${ARGN})
endmacro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
macro(AUTOMOC4_ADD_LIBRARY _target_NAME _add_executable_param)
    add_library(${_target_NAME} ${_add_executable_param} ${ARGN})
endmacro(AUTOMOC4_ADD_LIBRARY)
