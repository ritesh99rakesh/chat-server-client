# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(chat-serverTests LANGUAGES CXX)

# ---- Tests ----

add_executable(chat-server_test source/chat-server_test.cpp)
target_link_libraries(chat-server_test PRIVATE chat-server_lib)
target_compile_features(chat-server_test PRIVATE cxx_std_20)

add_test(NAME chat-server_test COMMAND chat-server_test)

# ---- End-of-file commands ----

add_folders(Test)
