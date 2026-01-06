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
target_compile_definitions(
        nuklearpower PRIVATE
        NK_INCLUDE_VERTEX_BUFFER_OUTPUT
        NK_INCLUDE_FONT_BAKING
        NK_INCLUDE_COMMAND_USERDATA
        NK_INCLUDE_DEFAULT_ALLOCATOR
)

file(GLOB nk_sources ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

target_sources(
    nuklearpower

    PUBLIC

    FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_LIST_DIR}/src
      ${CMAKE_CURRENT_LIST_DIR}/include

    FILES
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/nuklear.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/memory.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/string.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/drawing.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/tree.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/gui.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/image.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/style.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/properties.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/widgets.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/group.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/layout.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/window.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/nk/input.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/internal/nuklear_internal.hpp

        PRIVATE

    ${nk_sources}
)
