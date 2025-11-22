CXX = /usr/bin/clang++
CXXFLAGS = -std=c++17 -stdlib=libc++ -g -Wall -Wextra -Wunused -Wunused-function -Wunused-variable -Wunused-parameter -fsanitize=address -fno-omit-frame-pointer
LDFLAGS = $(CXXFLAGS)
INCLUDES = -I. -I./estructuras -I./gestorBloques

TARGET = backend

SRCS := $(shell find . -name '*.cpp')
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean clean_o show

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "enlazando $(TARGET)"
	$(CXX) $(LDFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	@echo "compilando $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

clean_o:
	rm -f $(OBJS)

show:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"