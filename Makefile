# To run tests, run "make run_test"
# References: https://codingnest.com/basic-makefiles/

### USER DEFINED VARIABLES
# SOURCES = TMCompiler/tests/test_bnf_parser.cpp TMCompiler/compiler/models/bnf_parser.cpp TMCompiler/utils/logger/logger.cpp
# SOURCES = main.cpp
# SOURCES += TMCompiler/compiler/front_end/earley_parser.cpp
# SOURCES += TMCompiler/compiler/models/bnf_parser.cpp
SOURCES += TMCompiler/utils/logger/logger.cpp

SOURCES += TMCompiler/utils/unittesting/unittests.cpp
SOURCES += TMCompiler/tests/test_unittesting.cpp

OBJECTS = $(SOURCES:.cpp=.o)

# see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# WARNINGS += -Werror 				# warnings become errors
WARNINGS += -Wpedantic  			# strict ISO C++
WARNINGS += -pedantic-errors    	# depends on c++ standard used
WARNINGS += -Wall 					# enable many other warnings
WARNINGS += -Wextra 				# more warnings
WARNINGS += -Wfloat-equal 			# do not use equality on floating-points
WARNINGS += -Wmissing-braces 		# initalizers have proper braces
WARNINGS += -Wmissing-include-dirs 	# if include directory not valid
WARNINGS += -Wuninitialized 		# use without initialization
WARNINGS += -fno-common 			# prevent tentative definitions

# reformat whitespace in WARNINGS list
WARNINGS := $(shell echo $(WARNINGS) | sed -e "s/\s+/ /g")

### C++ SPECIFIC MAKE VARIABLES
# g++ 9.4.0
CXX = g++
CPPFLAGS = -I.
CXXFLAGS = -std=c++14 $(WARNINGS) -MMD -MP

### MAKE RECIPES
.PHONY: clean

run_test: test.out
	./test.out

# link object files to create executable
test.out: $(OBJECTS)
	$(CXX) $(OBJECTS) -o test.out

# remove executables, object files, and .d files generated from compilation
clean:
	rm -f test.out $(OBJECTS) $(OBJECTS:.o=.d)

# generate implicit C++ recipes that specify what files are needed to generate each .o file
-include $(OBJECTS:.o=.d)
