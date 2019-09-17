# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Invoke macro to generate various sources and headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_CC_H(sandia_utils stream_gate_XX_impl cc ff ss bb)

########################################################################
# RPC Mananger deprecation warnings
#
# In file included from /opt/gnuradio-sandia/include/gnuradio/rpcregisterhelpers.h:30:0,
#                 from /opt/gnuradio-sandia/include/gnuradio/basic_block.h:42,
#                 from /opt/gnuradio-sandia/include/gnuradio/block.h:27,
#                 from /opt/gnuradio-sandia/src/gr-sandia_utils/build/swig/sandia_utils_swigPYTHON_wrap.cxx:4366:
#/opt/gnuradio-sandia/include/gnuradio/rpcmanager.h:57:15: warning: ‘template<class> class std::auto_ptr’ is deprecated [-Wdeprecated-declarations]
#   static std::auto_ptr<rpcserver_booter_base> boot;
########################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND sandia_utils_sources
    ${generated_sources}
    file_sink_impl.cc
    file_writer_base.cc
    file_writer_raw.cc
    file_writer_raw_header.cc
    burst_power_detector_impl.cc
    tag_debug_file_impl.cc
    invert_tune_impl.cc
    block_buffer_impl.cc
    message_vector_file_sink_impl.cc
    message_vector_raster_file_sink_impl.cc
    file_source_impl.cc
    file_reader_base.cc
    interleaved_short_to_complex_impl.cc
    complex_to_interleaved_short_impl.cc
)
if(BLUEFILE_FOUND)
  list(APPEND sandia_utils_sources file_writer_bluefile.cc )
endif(BLUEFILE_FOUND)

set(sandia_utils_sources "${sandia_utils_sources}" PARENT_SCOPE)
if(NOT sandia_utils_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT sandia_utils_sources)


add_library(gnuradio-sandia_utils SHARED ${sandia_utils_sources})
target_link_libraries(gnuradio-sandia_utils ${Boost_LIBRARIES} ${LOG4CPP_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
########################################################################
# BlueFile Sources and Library
########################################################################
if (BLUEFILE_FOUND)
MESSAGE(STATUS "Adding bluefile libraries to target")
#   target_link_libraries(gnuradio-sandia_utils ${Boost_LIBRARIES}
#                         ${GNURADIO_ALL_LIBRARIES} bluefilecxx bluefile)
  target_link_libraries(gnuradio-sandia_utils bluefilecxx bluefile)
endif(BLUEFILE_FOUND)

set_target_properties(gnuradio-sandia_utils PROPERTIES DEFINE_SYMBOL "gnuradio_sandia_utils_EXPORTS")
add_dependencies(gnuradio-sandia_utils
  sandia_utils_generated_includes sandia_utils_generated_swigs
  gnuradio-runtime)

if(APPLE)
    set_target_properties(gnuradio-sandia_utils PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)



########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-sandia_utils RUNTIME_COMPONENT "sandia_utils_runtime" DEVEL_COMPONENT "sandia_utils_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_sandia_utils_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_sandia_utils.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_sandia_utils.cc
)

add_executable(test-sandia_utils ${test_sandia_utils_sources})

target_link_libraries(
  test-sandia_utils
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-sandia_utils
)

GR_ADD_TEST(test_sandia_utils test-sandia_utils)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")
