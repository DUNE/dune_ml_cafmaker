cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
	    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
	      
	      # Set the possible values for cmake-gui drop-down
	        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
	endif()


# Force the old ABI to match common experiment-built libraries (like those in UPS/Spack)

project(dune_ml_cafmaker)

# SPINE HDF5 files are read by spine-reader, which needs only the HDF5 C library.
find_package(SpineReader 0.1 REQUIRED)

find_package(duneanaobj REQUIRED)
find_package(ROOT REQUIRED)

file(GLOB SPINE_SOURCES src/*.cc)

# Helper function to ensure all includes and libraries are correctly attached to targets
function(link_dlp_dependencies target_name)
    target_include_directories(${target_name} PUBLIC
        include
        ${ROOT_INCLUDE_DIRS}
    )
    target_link_libraries(${target_name} PRIVATE
        SpineReader::spine_reader
        duneanaobj::StandardRecord
    )
endfunction()

# Libraries
add_library(dlp_simulation SHARED ${SPINE_SOURCES})
link_dlp_dependencies(dlp_simulation)
target_compile_definitions(dlp_simulation PRIVATE MC_NOT_DATA)

add_library(dlp_data SHARED ${SPINE_SOURCES})
link_dlp_dependencies(dlp_data)

# Helper for executables
function(link_executable_dependencies target_name)
    link_dlp_dependencies(${target_name})
    target_link_libraries(${target_name} PRIVATE ${ROOT_LIBRARIES})
endfunction()

# Merge executables
add_executable(merge_sources_simulation merge_sources.cc)
target_compile_definitions(merge_sources_simulation PRIVATE MC_NOT_DATA)
link_executable_dependencies(merge_sources_simulation)
target_link_libraries(merge_sources_simulation PRIVATE dlp_simulation)

add_executable(merge_sources_data merge_sources.cc)
link_executable_dependencies(merge_sources_data)
target_link_libraries(merge_sources_data PRIVATE dlp_data)

# Standalone executables
add_executable(make_standalone_simulation make_standalone.cc)
target_compile_definitions(make_standalone_simulation PRIVATE MC_NOT_DATA)
link_executable_dependencies(make_standalone_simulation)
target_link_libraries(make_standalone_simulation PRIVATE dlp_simulation)

add_executable(make_standalone_data make_standalone.cc)
link_executable_dependencies(make_standalone_data)
target_link_libraries(make_standalone_data PRIVATE dlp_data)
