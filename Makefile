# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer -Wno-format-security
ASAN_LIBS  = -static-libasan
SANITIZE_FLAGS = -fsanitize=address,undefined

# Directories
SRC_DIR = .
LEXER_DIR = $(SRC_DIR)/Lexer
PARSER_DIR = $(SRC_DIR)/Parser

# Source files
LEXER_SRCS = $(LEXER_DIR)/Helpers.cpp $(LEXER_DIR)/Lexer.cpp $(LEXER_DIR)/Token.cpp
PARSER_SRCS = $(PARSER_DIR)/Parser.cpp 
MAIN_SRC = $(SRC_DIR)/main.cpp

# Object files
LEXER_OBJS = $(LEXER_SRCS:.cpp=.o)
PARSER_OBJS = $(PARSER_SRCS:.cpp=.o)
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

# Headers
LEXER_HDRS = $(LEXER_DIR)/Helpers.hpp $(LEXER_DIR)/Lexer.hpp $(LEXER_DIR)/Token.hpp $(LEXER_DIR)/Types.hpp
PARSER_HDRS = $(PARSER_DIR)/Node.hpp $(PARSER_DIR)/Parser.hpp $(PARSER_DIR)/StackAllocator.hpp

# Target executable
TARGET = main
SANITIZE_TARGET = a_sanitize.out

# Default rule
all: $(TARGET)
	./main

# Rule to build the target executable
$(TARGET): $(LEXER_OBJS) $(PARSER_OBJS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(LEXER_OBJS) $(PARSER_OBJS) $(MAIN_OBJ)

# Rules to build object files
$(LEXER_DIR)/%.o: $(LEXER_DIR)/%.cpp $(LEXER_HDRS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(PARSER_DIR)/%.o: $(PARSER_DIR)/%.cpp $(PARSER_HDRS) $(LEXER_HDRS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(LEXER_HDRS) $(PARSER_HDRS) 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Sanitize rule
sanitize: $(LEXER_OBJS) $(PARSER_OBJS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) -o $(SANITIZE_TARGET) $(LEXER_OBJS)
	$(PARSER_OBJS) $(MAIN_OBJ) $(SANITIZE_TARGET)

# Clean rule
clean:
	rm -f $(LEXER_OBJS) $(PARSER_OBJS) $(MAIN_OBJ) $(TARGET) $(SANITIZE_TARGET)

# Phony targets
.PHONY: all clean
