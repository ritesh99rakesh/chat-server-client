install(
    TARGETS chat-server_exe
    RUNTIME COMPONENT chat-server_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
