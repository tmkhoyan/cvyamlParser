cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(cvyamlParser)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")
message(${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

find_package(Matlab REQUIRED)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    message("GCC.")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fwrapv -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    message("Clang.")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fwrapv -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DPLB_DEBUG -O0")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    message("MSVC.")
    add_definitions(/D_USE_MATH_DEFINES)
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /Ot /GS- /GL /DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /DDEBUG")
endif()

if(WIN32)
    matlab_add_mex(NAME readcvYaml SRC src/readcvYaml_win.cpp LINK_TO opencv_core)
else()
    matlab_add_mex(NAME readcvYaml SRC src/readcvYaml.cpp LINK_TO opencv_core)
endif()

add_executable(genyamlData generate/generate_random/genyamlData.cpp)
target_link_libraries(genyamlData opencv_core)

add_executable(generate_image generate/generate_image/write_test_img.cpp)
target_link_libraries(generate_image opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc)
