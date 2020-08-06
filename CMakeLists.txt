cmake_minimum_required(VERSION 3.13)
project(gbas CXX)

find_package(Boost COMPONENTS unit_test_framework)

set(GBAS_SRCS
    src/tokenizer.cpp
    src/parser.cpp
    src/assembler.cpp
    src/elf.cpp
    src/elf_writer.cpp
    src/elf_reader.cpp
)

set(TEST_SRCS
    test/char_utils_test.cpp
    test/tokenizer_test.cpp
    test/parser_test.cpp
    test/assembler_test.cpp
    test/elf_test.cpp
)

add_executable(gbas
    ${GBAS_SRCS}
    src/main.cpp
)

add_executable(gbas_test
    ${GBAS_SRCS}
    ${TEST_SRCS}
)


set_target_properties(gbas
    PROPERTIES
        CXX_STANDARD 17
)

set_target_properties(gbas_test
    PROPERTIES
        CXX_STANDARD 17
)


target_include_directories(gbas PRIVATE include)
target_include_directories(gbas_test PRIVATE include test)

add_subdirectory(lib/expected)

target_link_libraries(gbas expected)
target_link_libraries(gbas_test expected)
target_link_libraries(gbas_test Boost::unit_test_framework)

if (UNIX)
    target_compile_options(gbas PRIVATE -ggdb -Wextra -Wall -O3)
    target_compile_options(gbas_test PRIVATE
        -ggdb -Wextra -Wall -Og
        -fsanitize=address -fsanitize=undefined
    )
    target_link_options(gbas_test PRIVATE
        -fsanitize=address -fsanitize=undefined
    )
else (UNIX)
    message(FATAL_ERROR "Unsupported platform")
endif (UNIX)
