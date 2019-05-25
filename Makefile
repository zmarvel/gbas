
CXXFLAGS += -g -Wextra -Wall -std=c++11
LDFLAGS += -g -Wextra -Wall

EXE_SRC = src/main.cpp
EXE = gbas
TEST_EXE = gbas_test
SRCS = src/tokenizer.cpp \
       src/parser.cpp \
       src/assembler.cpp \

OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)
COVS = $(SRCS:.cpp=.gcno)
EXE_OBJS = $(OBJS) $(EXE_SRC:.cpp=.o)
INC = -Iinclude

TEST_SRCS = test/tokenizer_test.cpp \
	    test/parser_test.cpp \
	    test/assembler_test.cpp \

TEST_OBJS = $(patsubst %.cpp,%.o,$(TEST_SRCS))

TEST_DEPS = $(patsubst %.cpp,%.d,$(TEST_SRCS))

TEST_COVS = $(patsubst %.cpp,%.gcno,$(TEST_SRCS)) \
	    $(patsubst %.cpp,%.gcda,$(TEST_SRCS))


$(EXE): $(EXE_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: test
test: $(TEST_EXE)


$(TEST_EXE): $(OBJS) $(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ -lboost_unit_test_framework

.PHONY: check
check: $(TEST_EXE)
	./$< $(CHECK_OPTIONS)

%.o: %.cpp Makefile
	$(CXX) -MD -c $(CXXFLAGS) $(INC) -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(EXE) $(TEST_OBJS) $(TEST_DEPS) $(TEST_EXE)
	rm -rf docs
	rm -rf coverage/ coverage.info $(COVS) $(TEST_COVS)

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
