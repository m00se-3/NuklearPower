cmake_minimum_required(VERSION 3.28)

project(NuklearPower LANGUAGES CXX)

include(CPM.cmake)
include(cmake/Config.cmake)
include(cmake/CMakeOptions.cmake)

if(NP_BUILD_TESTS)
  include(CTest)
  CPMAddPackage("gh:catchorg/Catch2@3.10.0")
  enable_testing()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} 12)

add_library(nuklearpower STATIC)

target_compile_features(nuklearpower PRIVATE cxx_std_23)
target_compile_options(nuklearpower PRIVATE ${CompilerFlags})
target_link_options(nuklearpower PRIVATE ${LinkerFlags})

file(GLOB nk_sources ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

target_sources(
    nuklearpower

    PUBLIC

    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/src

    PRIVATE

    ${nk_sources}
)
