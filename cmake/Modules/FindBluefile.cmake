#
# Find the Bluefile includes and library
#
# This module defines
# BLUEFILE_INCLUDE_DIRS, where to find tiff.h, etc.
# BLUEFILE_LIBRARIES, the libraries to link against to use CppUnit.
# BLUEFILE_FOUND, If false, do not try to use Bluefile.

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_BLUEFILE bluefile)

FIND_PATH(BLUEFILE_INCLUDE_DIRS
  NAMES bluefile.hh
  HINTS ${PC_BLUEFILE_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/include/bluefile
  PATHS
  /usr/local/include
  /usr/include
  )

FIND_LIBRARY(BLUEFILE_LIBRARIES
  NAMES bluefilecxx
  HINTS ${PC_BLUEFILE_LIBDIR}
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib64
  PATHS
  ${BLUEFILE_INCLUDE_DIRS}/../lib
  /usr/local/lib
  /usr/lib
  )

#message(STATUS "Bluefile Library: ${BLUEFILE_LIBRARIES}")
LIST(APPEND BLUEFILE_LIBRARIES ${CMAKE_DL_LIBS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BLUEFILE DEFAULT_MSG BLUEFILE_LIBRARIES BLUEFILE_INCLUDE_DIRS)
MARK_AS_ADVANCED(BLUEFILE_LIBRARIES BLUEFILE_INCLUDE_DIRS)