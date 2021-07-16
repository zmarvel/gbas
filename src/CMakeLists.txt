add_library(libgbas STATIC
    tokenizer.cpp
    parser.cpp
    assembler.cpp
    elf.cpp
    elf_writer.cpp
    elf_reader.cpp
)

add_executable(gbas
    main.cpp
)

target_include_directories(libgbas PUBLIC ../include)

target_link_libraries(libgbas PUBLIC expected)

target_link_libraries(gbas PRIVATE libgbas)

target_compile_options(libgbas PRIVATE -Wextra -Wall)
target_compile_options(gbas PRIVATE -Wextra -Wall)
