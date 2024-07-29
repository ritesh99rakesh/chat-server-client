cmake_minimum_required(VERSION 3.30)

include(cmake/prelude.cmake)

project(
    chat-server
    VERSION 0.1.0
    DESCRIPTION ""
    HOMEPAGE_URL ""
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----
# ---- Server ----
add_library(
    chat-server_lib OBJECT
    source/lib.cpp
)

target_include_directories(
    chat-server_lib ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(chat-server_lib PUBLIC cxx_std_23)

# ---- Server ----
add_library(
    chat-client_lib OBJECT
    source/lib.cpp
)

target_include_directories(
    chat-client_lib ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(chat-client_lib PUBLIC cxx_std_23)

# ---- Declare executable ----

# ---- Server ----
add_executable(chat-server_exe source/server.cpp)
add_executable(chat-server::exe ALIAS chat-server_exe)

set_property(TARGET chat-server_exe PROPERTY OUTPUT_NAME chat-server)

target_compile_features(chat-server_exe PRIVATE cxx_std_23)

target_link_libraries(chat-server_exe PRIVATE chat-server_lib)

# ---- Client ----
add_executable(chat-client_exe source/client.cpp)
add_executable(chat-client::exe ALIAS chat-client_exe)

set_property(TARGET chat-client_exe PROPERTY OUTPUT_NAME chat-client)

target_compile_features(chat-client_exe PRIVATE cxx_std_23)

target_link_libraries(chat-client_exe PRIVATE chat-client_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT chat-server_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of chat-server"
  )
endif()

include(cmake/dev-mode.cmake)
