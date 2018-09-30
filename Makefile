
CXXFLAGS = -g -Wall -std=c++11
LDFLAGS = -g -Wall

EXE_SRC = src/main.cpp
EXE = gbas
TEST_EXE = gbas_test
SRCS = src/tokenizer.cpp
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
EXE_OBJS = $(OBJS) $(patsubst %.cpp,%.o,$(EXE_SRC))
INC = -Iinclude

TEST_SRCS = test/TokenizerTest.cpp
TEST_OBJS = $(OBJS) $(patsubst %.cpp,%.o,$(TEST_SRCS))


$(EXE): $(EXE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_EXE): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_unit_test_framework

%.o: %.cpp Makefile
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

clean:
	rm -f $(OBJS) $(EXE) $(TEST_OBJS) $(TEST_EXE)
