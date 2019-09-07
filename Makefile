
CXXFLAGS += -ggdb -Wextra -Wall
LDFLAGS += -ggdb -Wextra -Wall

EXE_SRC = src/main.cpp
EXE = gbas
TEST_EXE = gbas_test
SRCS = src/tokenizer.cpp \
       src/parser.cpp \
       src/assembler.cpp \
       src/elf.cpp \

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
COVS = $(SRCS:.cpp=.gcda) \
    $(SRCS:.cpp=.gcno)
EXE_OBJS = $(OBJS) $(EXE_SRC:.cpp=.o)
INC = -Iinclude

TEST_SRCS = test/char_utils_test.cpp \
	    test/tokenizer_test.cpp \
	    test/parser_test.cpp \
	    test/assembler_test.cpp \
	    test/elf_test.cpp \

TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TEST_DEPS = $(TEST_SRCS:.cpp=.d)

TEST_COVS = $(TEST_SRCS:.cpp=.gcno) \
    $(TEST_SRCS:.cpp=.gcda)


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

%.o: %.cpp Makefile
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
