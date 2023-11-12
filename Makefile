# Run 'make' to compile source files and generate
# 'tmc' and 'test' binaries.
#
# Once generated, run via './tmc' and `./test'.
# Run 'make clean' to clean up generated binaries and files.
# References: https://codingnest.com/basic-makefiles/

### USER DEFINED VARIABLES

# list of source files, divided by binary and general library source files
# tmc is the compiler binary
tmc_SOURCES = main.cpp

# lib are all other files that do not have a main function
lib_SOURCES = TMCompiler/compiler/compiler.cpp
lib_SOURCES += TMCompiler/compiler/lexer/lexer.cpp
lib_SOURCES += TMCompiler/compiler/models/grammar.cpp
lib_SOURCES += TMCompiler/compiler/parser/earley_parser.cpp
lib_SOURCES += TMCompiler/compiler/utils/bnf_parser.cpp
lib_SOURCES += TMCompiler/utils/logger/logger.cpp

# test runs testcases from lib
test_SOURCES = TMCompiler/tests/test_bnf_parser.cpp
test_SOURCES += TMCompiler/tests/test_compiler.cpp
test_SOURCES += TMCompiler/tests/test_lexer.cpp
test_SOURCES += TMCompiler/tests/test_unittesting.cpp
test_SOURCES += TMCompiler/utils/vendor/catch2/catch_amalgamated.cpp

# for each .cpp file, like main.cpp, define it's corresponding object file,
# such as main.o
tmc_OBJECTS = $(tmc_SOURCES:.cpp=.o)
lib_OBJECTS = $(lib_SOURCES:.cpp=.o)
test_OBJECTS = $(test_SOURCES:.cpp=.o)

# all object files and list of binaries to generate
OBJECTS = $(tmc_OBJECTS) $(lib_OBJECTS) $(test_OBJECTS)
BINARIES = tmc test

# GCC compilation warnings list
# see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
WARNINGS += -Wall 					# enable many other warnings
WARNINGS += -Wextra 				# more warnings
WARNINGS += -Wpedantic  			# strict ISO C++
WARNINGS += -pedantic-errors    	# depends on c++ standard used
# WARNINGS += -Wabi 					# check if code affected by ABI changes
WARNINGS += -Wmissing-braces 		# initalizers have proper braces
WARNINGS += -Wmissing-include-dirs 	# if include directory not valid
WARNINGS += -Wswitch-default 		# case statements should have default case
WARNINGS += -Wunused 				# all -Wunused-* warnings
WARNINGS += -Wuninitialized 		# use without initialization
WARNINGS += -Wunknown-pragmas 		# do not use unknown #pragma
WARNINGS += -Wmissing-noreturn 		# if we should have attribute [[noreturn]]
WARNINGS += -Wsuggest-attribute=pure 	# warn if some functions should have these attributes
WARNINGS += -Wsuggest-attribute=const
WARNINGS += -Wsuggest-attribute=noreturn
WARNINGS += -Wsuggest-attribute=malloc
WARNINGS += -Wbool-compare 			# prevent comparing boolean with int
WARNINGS += -Wduplicated-branches  	# prevent branch statements that provide same value
WARNINGS += -Wduplicated-cond 		# prevent same condition in if and else if
WARNINGS += -Wdouble-promotion 		# warn if float implicitly casts to double
WARNINGS += -Wfloat-equal 			# do not use equality on floating-points
WARNINGS += -Wshadow 				# prevent shadowing of local variables
WARNINGS += -Wunsafe-loop-optimizations 	# warn if compiler cannot optimize loop
WARNINGS += -Wunused-macros 		# warn about macros that are not expanded anywhere
WARNINGS += -Wcast-qual 			# warn if casting removes type-qualifiers
WARNINGS += -Wconversion 			# warn about conversions that may change the value
WARNINGS += -Wdate-time 			# do not use __TIME__, __DATE__, or __TIMESTAMP__ to get reproducible builds
WARNINGS += -Wsign-conversion 		# warn about conversions between signed and unsigned integers
WARNINGS += -Wlogical-op 			# warn about suspicious usages of logical operations ||, &&, ^
# WARNINGS += -Wmissing-declarations 	# global functions must have a prototype
WARNINGS += -Wredundant-decls 		# warn when multiple declarations
WARNINGS += -fno-operator-names 	# do not use "and", "or", and "xor" (and others) as keywords
WARNINGS += -fno-common 			# prevent tentative definitions

# reformat whitespace in WARNINGS list
WARNINGS := $(shell echo $(WARNINGS) | sed -e "s/\s+/ /g")

### C++-SPECIFIC MAKE VARIABLES
# currently using g++ version 9.4.0
CXX = g++

# search current directory (root of project) for include paths
CPPFLAGS = -I. -isystem TMCompiler/utils/vendor

# use c++14 standard. MMD and MP to generate .d files for use by Make
CXXFLAGS = -std=c++14 $(WARNINGS) -MMD -MP

### MAKE RECIPES
.PHONY: all clean

# leave as first target: running 'make' will generate both
# 'tmc' and 'test' binaries
all: $(BINARIES)

# See https://make.mad-scientist.net/secondary-expansion/
# Specifying SECONDEXPANSION lets Make know to first expand values
# in pre-requisites (i.e. from $$($$@_OBJECTS) to $($@_OBJECTS)), then
# will expand a second time (using $@ to be tmc/test) to become $(tmc_OBJECTS)
# and $(test_OBJECTS) for output files 'tmc' and 'test' respectively.
.SECONDEXPANSION:
# link object files to create executable: for instance, to create tmc, run
# 'g++ main.o TMCompiler/compiler/compiler.o ... -o tmc'
$(BINARIES): $$($$@_OBJECTS) $$(lib_OBJECTS)
	$(CXX) $^ -o $@
	@echo "\033[32mGenerated executable \033[1;33m$@\033[0m"

# run 'make clean' to remove executables, object files, and .d files generated from compilation
clean:
	rm -f $(BINARIES) $(OBJECTS) $(OBJECTS:.o=.d)

# generate implicit C++ recipes that specify what files are needed to generate each .o file
-include $(OBJECTS:.o=.d)
