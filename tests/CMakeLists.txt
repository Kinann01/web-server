# tests/CMakeLists.txt

# Find the Python3 interpreter
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_test(
    NAME RunPyTests
    COMMAND ${Python3_EXECUTABLE} -m pytest -v ${TESTS_DIR}/tests.py
)
