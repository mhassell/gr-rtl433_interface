find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_RTL433_INTERFACE gnuradio-rtl433_interface)

FIND_PATH(
    GR_RTL433_INTERFACE_INCLUDE_DIRS
    NAMES gnuradio/rtl433_interface/api.h
    HINTS $ENV{RTL433_INTERFACE_DIR}/include
        ${PC_RTL433_INTERFACE_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_RTL433_INTERFACE_LIBRARIES
    NAMES gnuradio-rtl433_interface
    HINTS $ENV{RTL433_INTERFACE_DIR}/lib
        ${PC_RTL433_INTERFACE_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-rtl433_interfaceTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_RTL433_INTERFACE DEFAULT_MSG GR_RTL433_INTERFACE_LIBRARIES GR_RTL433_INTERFACE_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_RTL433_INTERFACE_LIBRARIES GR_RTL433_INTERFACE_INCLUDE_DIRS)
