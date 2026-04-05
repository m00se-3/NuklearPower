cpmaddpackage("gh:glfw/glfw#3.4")

find_library(glew glew32s.lib PATHS ${CMAKE_CURRENT_LIST_DIR}/glew/lib/Release/x64)

function(add_example name source_file)
  add_executable(${name} ${source_file})

  target_compile_features(${name} PRIVATE cxx_std_23)
  target_compile_options(${name} PRIVATE ${CompilerFlags})
  target_link_options(${name} PRIVATE ${LinkerFlags})

  if(WIN32)
    target_link_options(${name} PRIVATE "-lGLU32" "-lopengl32")
  else()
    target_link_options(${name} PRIVATE "-lGLU" "-lGL")
  endif()

  target_compile_definitions(
    ${name} PRIVATE
    NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    NK_INCLUDE_FONT_BAKING
    NK_INCLUDE_COMMAND_USERDATA
    NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_INCLUDE_DEFAULT_FONT
    NK_INCLUDE_STANDARD_IO
    GLEW_STATIC
  )

  target_sources(
    ${name}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}

    FILES
    utils.hpp
  )

  target_link_libraries(${name} PRIVATE nuklearpower glfw ${glew})

endfunction()

add_example(file_browser file_browser.cpp)
add_example(canvas canvas.cpp)
add_example(extended extended.cpp)
add_example(skinning skinning.cpp)
