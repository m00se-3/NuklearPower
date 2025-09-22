# Enable compiler flags for certain build configurations
if(NOT ${CMAKE_BUILD_TYPE} MATCHES Debug)
  list(
    APPEND CompilerFlags
    "/O2"
  )
endif()

if(${NP_EXCEPTIONS} MATCHES ON)
  list(APPEND CompilerFlags "/EHsc")
else()
  list(APPEND CompilerFlags "/EHs-" "/EHc-")
endif()

# Enable compiler flags for all builds.
list(
  APPEND CompilerFlags

  "/Wall"
  "/W4"
  "/permissive-"
  "/sdl"
  "/GS"
  "/analyze"
  "/we4165"
  "/we4191"
  "/we4242"
  "/we4254"
  "/we4263"
  "/we4265"
  "/we4289"
  "/we4388"
  "/we4800"
  "/we4826"
  "/we5204"
  "/we5219"
  "/we5263"
)

list(
  APPEND LinkerFlags

  "/guard:cf"
  "/guard:ehcont"
  "/DYNAMICBASE"
  "/HIGHENTROPYVA"
)
