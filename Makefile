
CXXFLAGS += -ggdb -Wextra -Wall -std=c++17
LDFLAGS += -ggdb -Wextra -Wall -std=c++17

EXE_SRC = src/main.cpp
EXE = gbas
TEST_EXE = gbas_test
SRCS = src/tokenizer.cpp \
       src/parser.cpp \
       src/assembler.cpp \
       src/elf.cpp \
       src/elf_writer.cpp \
       src/elf_reader.cpp \

OBJS = $(patsubst %.cpp,build/%.o,$(notdir $(SRCS)))
DEPS = $(OBJS:.o=.d)
COVS = $(OBJS:.o=.gcda) $(OBJS:.o=.gcno)
EXE_OBJS = $(OBJS) $(patsubst %.cpp,build/%.o,$(notdir $(EXE_SRC)))
INC = -Iinclude -Ilib/expected/include

TEST_SRCS = test/char_utils_test.cpp \
	    test/tokenizer_test.cpp \
	    test/parser_test.cpp \
	    test/assembler_test.cpp \
	    test/elf_test.cpp \

TEST_OBJS = $(patsubst %.cpp,build/%.o,$(notdir $(TEST_SRCS)))

TEST_DEPS = $(TEST_OBJS:.o=.d)

TEST_COVS = $(TEST_OBJS:.o=.gcno) \
    $(TEST_OBJS:.o=.gcda)


$(EXE): $(EXE_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: test
test: $(TEST_EXE)
test: CXXFLAGS += -Itest


$(TEST_EXE): $(OBJS) $(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ -lboost_unit_test_framework

.PHONY: check
check: CXXFLAGS += -Itest
check: $(TEST_EXE)
ifdef CHECK_LOG
	-./$(TEST_EXE) $(CHECK_OPTIONS) > $(CHECK_LOG)
else
	-./$(TEST_EXE) $(CHECK_OPTIONS)
endif

build/%.o: src/%.cpp Makefile
	mkdir -p build
	$(CXX) -MD -c $(CXXFLAGS) $(INC) -o $@ $<

build/%.o: test/%.cpp Makefile
	mkdir -p build
	$(CXX) -MD -c $(CXXFLAGS) $(INC) -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(EXE) $(TEST_OBJS) $(TEST_DEPS) $(TEST_EXE)
	rm -rf coverage/ coverage.info $(COVS) $(TEST_COVS)

.PHONY: distclean
distclean: clean
	rm -rf docs

.PHONY: ctags
ctags:
	ctags -R src include test

.PHONY: doxygen
doxygen:
	doxygen Doxyfile

.PHONY: coverage
coverage: CXXFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: check
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage/

-include $(DEPS) $(TEST_DEPS)
