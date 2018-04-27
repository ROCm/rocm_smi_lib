cmake_minimum_required(VERSION 2.8.0)

# Default installation path
set(CMAKE_INSTALL_PREFIX "/opt/rocm" CACHE PATH "")

set(ROCMINFO_EXE "rocminfo")
set(PROJECT_NAME ${ROCMINFO_EXE})
project (${PROJECT_NAME})

find_package(ROCM PATHS /opt/rocm)
include(ROCMSetupVersion)
include(ROCMCreatePackage)

rocm_setup_version(VERSION 1.0.0)

find_path(ROCR_INC_DIR hsa/hsa.h PATH_SUFFIXES include PATHS /opt/rocm)
if(NOT ROCR_INC_DIR)
    message(FATAL_ERROR "Can't find hsa.h.")
endif()

find_path(ROCR_LIB_DIR libhsa-runtime64.so PATH_SUFFIXES lib PATHS /opt/rocm)
if(NOT ROCR_LIB_DIR)
    message(FATAL_ERROR "Can't find libhsa-runtime64.so.")
endif()

#
# Flag to enable / disable verbose output.
#
SET( CMAKE_VERBOSE_MAKEFILE on )

#
# Compiler pre-processor definitions.
#
add_definitions(-D__linux__)
add_definitions(-DLITTLEENDIAN_CPU=1)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()

#
# Linux Compiler options
#
set(CMAKE_CXX_FLAGS "-std=c++11 ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmerge-all-constants")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-extensions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0")
endif ()

#
# Linux Linker options
##
# Specify the directory containing various libraries of ROCR
# to be linked against for building ROC Perf applications
#
link_directories(${ROCR_LIB_DIR})

#
# Extend the list of libraries to be used for linking ROC Perf Apps
#
set(ROCR_LIBS ${ROCR_LIBS} hsa-runtime64)

include_directories(${ROCR_INC_DIR})

###########################
# rocm_agent_enumerator
###########################
configure_file(rocm_agent_enumerator rocm_agent_enumerator COPYONLY)


###########################
# RocR Info
###########################
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ROCMINFO_SOURCES)
add_executable(${ROCMINFO_EXE} ${ROCMINFO_SOURCES})
target_link_libraries(${ROCMINFO_EXE} ${ROCR_LIBS} c stdc++ dl pthread rt)

###########################
# Install directives
###########################
install (
   FILES ${CMAKE_CURRENT_BINARY_DIR}/${ROCMINFO_EXE}
   PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
   DESTINATION bin )
install (
   FILES ${CMAKE_CURRENT_BINARY_DIR}/rocm_agent_enumerator
   PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
   DESTINATION bin )

###########################
# Packaging directives
###########################
set(CPACK_DEBIAN_PACKAGE_DEPENDS "hsa-rocr-dev, python")
set(CPACK_RPM_PACKAGE_REQUIRES "hsa-rocr-dev, python")

###########################
# Debian package specific variables
###########################
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_DEBIAN_PACKAGE_HOMEPAGE} CACHE STRING "https://github.com/RadeonOpenCompute/ROCm")

###########################
# RPM package specific variables
###########################
if (DEFINED CPACK_PACKAGING_INSTALL_PREFIX)
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX} ${CPACK_PACKAGING_INSTALL_PREFIX}/bin")
endif( )

rocm_create_package(
    NAME rocminfo
    DESCRIPTION "Radeon Open Compute (ROCm) Runtime rocminfo tool"
    MAINTAINER "Advanced Micro Devices Inc."
)
