# To run tests, run "make run_test"
# References: https://codingnest.com/basic-makefiles/

### USER DEFINED VARIABLES
SOURCES = main.cpp
SOURCES += TMCompiler/compiler/compiler.cpp
SOURCES += TMCompiler/compiler/front_end/bnf_parser.cpp
SOURCES += TMCompiler/compiler/front_end/earley_parser.cpp
SOURCES += TMCompiler/compiler/models/grammar.cpp
SOURCES += TMCompiler/utils/logger/logger.cpp

# SOURCES += TMCompiler/utils/unittesting/unittests.cpp
# SOURCES += TMCompiler/tests/test_unittesting.cpp
# SOURCES += TMCompiler/tests/test_bnf_parser.cpp
# SOURCES += TMCompiler/compiler/front_end/bnf_parser.cpp

OBJECTS = $(SOURCES:.cpp=.o)

# see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# WARNINGS += -Werror 				# warnings become errors
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

### C++ SPECIFIC MAKE VARIABLES
# g++ 9.4.0
CXX = g++

# search current directory (root of project) for include paths
CPPFLAGS = -I.

# use c++14 standard. MMD and MP to generate .d files for use by Make
CXXFLAGS = -std=c++14 $(WARNINGS) -MMD -MP

### MAKE RECIPES
.PHONY: clean

# link object files to create executable
test.out: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@
	@echo "Generated executable $@"

# remove executables, object files, and .d files generated from compilation
clean:
	rm -f test.out $(OBJECTS) $(OBJECTS:.o=.d)

# generate implicit C++ recipes that specify what files are needed to generate each .o file
-include $(OBJECTS:.o=.d)
