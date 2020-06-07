INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SANDIA_UTILS sandia_utils)

FIND_PATH(
    SANDIA_UTILS_INCLUDE_DIRS
    NAMES sandia_utils/api.h
    HINTS $ENV{SANDIA_UTILS_DIR}/include
        ${PC_SANDIA_UTILS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SANDIA_UTILS_LIBRARIES
    NAMES gnuradio-sandia_utils
    HINTS $ENV{SANDIA_UTILS_DIR}/lib
        ${PC_SANDIA_UTILS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/sandia_utilsTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SANDIA_UTILS DEFAULT_MSG SANDIA_UTILS_LIBRARIES SANDIA_UTILS_INCLUDE_DIRS)
MARK_AS_ADVANCED(SANDIA_UTILS_LIBRARIES SANDIA_UTILS_INCLUDE_DIRS)
