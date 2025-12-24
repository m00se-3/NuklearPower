# Enable compiler flags for certain build configurations
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  list(
    APPEND CompilerFlags
    "-g"
    "-D_GLIBCXX_ASSERTIONS"
  )
elseif(${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
  list(
    APPEND CompilerFlags
    "-g"
    "-O2"
    "-U_FORTIFY_SOURCE"
    "-D_FORTIFY_SOURCE=3"
    "-D_GLIBCXX_ASSERTIONS"
  )
else()
  list(
    APPEND CompilerFlags
    "-O3"
    "-U_FORTIFY_SOURCE"
    "-D_FORTIFY_SOURCE=2"
  )
endif()

if(${NP_EXCEPTIONS} MATCHES ON)
  list(APPEND CompilerFlags "-fexceptions")
else()
  list(APPEND CompilerFlags "-fno-exceptions")
endif()

# Enable special flags when using GCC
if(${CMAKE_CXX_COMPILIER_ID} MATCHES "GNU")
  list(
    APPEND CompilerFlags

    "-fhardened"
    "-Wbidi-chars=any"
    "-Wtrampolines"
    "-Whardened"
  )
endif()

#Exclude unsupported flags on Windows
if(NOT ${CMAKE_HOST_WIN32})
  list(
    APPEND CompilerFlags

    "-mbranch-protection=standard"
  )

  list(
    APPEND LinkerFlags

    "-fPIC"
    "-fPIE"
  )
endif()


# Enable flags for all builds
list(
  APPEND CompilerFlags

  "-Wall"
  "-Wextra"
  "-Wformat"
  "-Wformat=2"
  "-Wimplicit-fallthrough"
  "-Werror=format-security"
  "-Werror=conversion"
  "-Werror=sign-conversion"
  "-Werror=div-by-zero"
  "-Werror=alloca"
  "-Wreturn-local-addr"
  "-Werror=return-local-addr"
  "-Wpedantic"
  "-Wdouble-promotion"
  "-Wconversion"
  "-Wsign-conversion"
  "-Wsuggest-override"

  "-fvisibility=hidden"
  "-fstrict-flex-arrays=3"
  "-fno-strict-overflow"
  "-fno-strict-aliasing"
  "-ftrivial-auto-var-init=zero"
)

if(NOT DEFINED EMSCRIPTEN)
  list(APPEND CompilerFlags "-fcf-protection=full")
endif()

list(
  APPEND LinkerOptions

  "-Wl,-z,nodlopen"
  "-Wl,-z,noexecstack"
  "-Wl,-z,relro"
  "-Wl,--as-needed"
  "-Wl,--no-copy-dt-needed-entries"
  "-pie"
)
