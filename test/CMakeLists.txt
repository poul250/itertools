cmake_minimum_required(VERSION 3.0.0)

add_executable(itertools_test test.cpp)
target_link_libraries(itertools_test PRIVATE itertools ${CONAN_LIBS})
set_target_properties(itertools_test PROPERTIES CXX_STANDARD 20)
