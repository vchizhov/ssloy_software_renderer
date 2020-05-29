cmake_minimum_required(VERSION 3.16)

#----------------------------------------------------------------------------#
#                                                                            #
#                          SOFTWARE RENDERER CMAKE                           #
#                                                                            #
#                 (Copyright by Vassillen Chizhov, 5/2020)                   #
#                                                                            #
#----------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.16)


set(PROJECT_MAIN "software_renderer")
project(${PROJECT_MAIN} VERSION 1.0
                  	DESCRIPTION "A basic software renderer."
                  	LANGUAGES CXX)
					
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(PROJECT_MAIN_DIR ${CMAKE_CURRENT_LIST_DIR})

if(MSVC)
  add_compile_options("/std:c++latest")
  add_compile_options(/W4)
  add_definitions (/D _CRT_SECURE_NO_WARNINGS)
endif()

#OPTION(PBRT_FLOAT_AS_DOUBLE "Use 64-bit floats" OFF)

#IF (PBRT_FLOAT_AS_DOUBLE)
#  ADD_DEFINITIONS ( -D PBRT_FLOAT_AS_DOUBLE )
#ENDIF()


set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add source and header files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp)

# Exclude the build directory
#list(FILTER ${SOURCES} EXCLUDE REGEX "build/")
# Create the VS filters based on the directory tree layout
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_MAIN})
