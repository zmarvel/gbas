
add_executable(gbas_test
    char_utils_test.cpp
    tokenizer_test.cpp
    parser_test.cpp
    assembler_test.cpp
    elf_test.cpp
)

target_link_libraries(gbas_test libgbas Boost::unit_test_framework)


# if (UNIX)
#     target_compile_options(gbas_test PRIVATE
#         -ggdb -Wextra -Wall -Og
#         -fsanitize=address -fsanitize=undefined
#     )
#     target_link_options(gbas_test PRIVATE
#         -fsanitize=address -fsanitize=undefined
#     )
# else (UNIX)
#     message(FATAL_ERROR "Unsupported platform")
# endif (UNIX)
