.PHONY: all compile run clean
.DEFAULT_GOAL = all

## Variables

CXX = g++
CXXFLAGS = -std=c++17 -Werror -Wall -pedantic -Wno-long-long -O3 -funroll-loops -MP -MD # -ggdb
EXECUTABLE = program.out
SOURCEDIR = src/
SOURCES := $(shell find $(SOURCEDIR) -name "*.cpp")

## Colors

COLOR_DEFAULT = \033[0m
COLOR_STATUS = \033[1;33m
COLOR_INFO = \033[1;36m
COLOR_DONE = \033[1;32m

## Tasks

%.o: %.cpp
	@echo "$(COLOR_STATUS)$^ -> $@$(COLOR_DEFAULT)"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

all: run

compile: $(SOURCES:.cpp=.o)
	@echo "$(COLOR_INFO)compiling with flags $(COLOR_DEFAULT)$(CXXFLAGS)"
	@$(CXX) $(CXXFLAGS) $(SOURCES:.cpp=.o) -o $(EXECUTABLE)
	@echo "$(COLOR_DONE)COMPILE DONE$(COLOR_DEFAULT)"

run: compile
	@echo "$(COLOR_INFO)starting executable$(COLOR_DEFAULT)"
	@./$(EXECUTABLE) $(file)
	@echo "$(COLOR_DONE)RUN DONE$(COLOR_DEFAULT)"

clean:
	@echo "$(COLOR_INFO)removing object files$(COLOR_DEFAULT)"
	$(shell find $(SOURCEDIR) -type f -name "*.o" -exec rm {} \;)
	@echo "$(COLOR_INFO)removing executable files$(COLOR_DEFAULT)"
	@rm -f $(EXECUTABLE)
	@echo "$(COLOR_INFO)removing link files$(COLOR_DEFAULT)"
	$(shell find $(SOURCEDIR) -type f -name "*.d" -exec rm {} \;)
	@echo "$(COLOR_DONE)CLEAN DONE$(COLOR_DEFAULT)"

test: compile
	$(shell ./bin/test.sh)

## Inlude D files

-include $(SOURCES:.cpp=.d)
