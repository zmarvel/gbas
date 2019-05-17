
CXXFLAGS = -g -Wall -std=c++11
LDFLAGS = -g -Wall

EXE_SRC = src/main.cpp
EXE = gbas
TEST_EXE = gbas_test
SRCS = src/tokenizer.cpp \
       src/parser.cpp \

OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)
EXE_OBJS = $(OBJS) $(EXE_SRC:.cpp=.o)
INC = -Iinclude

TEST_SRCS = test/tokenizer_test.cpp \
	    test/parser_test.cpp \

TEST_OBJS = $(OBJS) $(patsubst %.cpp,%.o,$(TEST_SRCS))

TEST_DEPS = $(patsubst %.cpp,%.d,$(TEST_SRCS))


$(EXE): $(EXE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TEST_EXE)

$(TEST_EXE): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_unit_test_framework

%.o: %.cpp Makefile
	$(CXX) -MD -c $(CXXFLAGS) $(INC) -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(EXE) $(TEST_OBJS) $(TEST_DEPS) $(TEST_EXE)

.PHONY: ctags
ctags:
	ctags -R src include test

check: $(TEST_EXE)
	./$<

-include $(DEPS) $(TEST_DEPS)
