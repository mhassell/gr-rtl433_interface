# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT rtl433_interface_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT rtl433_interface_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND rtl433_interface_python_files
    rtl_sink_python.cc python_bindings.cc)

gr_pybind_make_oot(rtl433_interface ../../.. gr::rtl433_interface "${rtl433_interface_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET rtl433_interface_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rtl433_interface_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/rtl433_interface/)

install(
    TARGETS rtl433_interface_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/rtl433_interface
    COMPONENT pythonapi)
