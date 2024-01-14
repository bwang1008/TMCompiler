# Steps to build with CMake from root of project:
# mkdir build
# cd build
# cmake -S .. -B .
# make

cmake_minimum_required(VERSION 3.12)

project(TMCompiler
	VERSION 2.0
	DESCRIPTION "Compile C-like programs into Turing Machines"
	LANGUAGES CXX
)

#############################
### Third-party libraries ###
#############################

include(FetchContent)

# tomlplusplus: library to parse TOML files
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

# Catch2: unittesting library
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(tomlplusplus Catch2)

####################
### Main targets ###
####################

# source files to generate tmc executable
add_executable(tmc
	main.cpp
	TMCompiler/compiler/compiler.cpp
	TMCompiler/compiler/lexer/lexer.cpp
	TMCompiler/compiler/models/grammar.cpp
	TMCompiler/compiler/parser/earley_parser.cpp
	TMCompiler/compiler/utils/bnf_parser.cpp
	TMCompiler/utils/logger/logger.cpp
	# Configuration files
	TMCompiler/config/regex_lexical_grammar.bnf
	TMCompiler/config/language_grammar.bnf
)

configure_file(
	"${CMAKE_SOURCE_DIR}/sample_program.cpp"
	"${CMAKE_BINARY_DIR}/sample_program.cpp"
	COPYONLY
)

add_executable(tests
	TMCompiler/tests/test_bnf_parser.cpp
	TMCompiler/tests/test_compiler.cpp
	TMCompiler/tests/test_lexer.cpp
)

##############################
### Compile and Link Flags ###
##############################

# warning flags
set(WARNINGS
	-Wall 					# enable many other warnings
	-Wextra 				# more warnings
	-Wpedantic  			# strict ISO C++
	-pedantic-errors    	# depends on c++ standard used
	-Wmissing-braces 		# initalizers have proper braces
	-Wmissing-include-dirs 	# if include directory not valid
	-Wswitch-default 		# case statements should have default case
	-Wunused 				# all -Wunused-* warnings
	-Wuninitialized 		# use without initialization
	-Wunknown-pragmas 		# do not use unknown #pragma
	-Wmissing-noreturn 		# if we should have attribute [[noreturn]]
	-Wsuggest-attribute=pure 	# warn if some functions should have these attributes
	-Wsuggest-attribute=const
	-Wsuggest-attribute=noreturn
	-Wsuggest-attribute=malloc
	-Wbool-compare 			# prevent comparing boolean with int
	-Wduplicated-branches  	# prevent branch statements that provide same value
	-Wduplicated-cond 		# prevent same condition in if and else if
	-Wdouble-promotion 		# warn if float implicitly casts to double
	-Wfloat-equal 			# do not use equality on floating-points
	-Wshadow 				# prevent shadowing of local variables
	-Wunsafe-loop-optimizations 	# warn if compiler cannot optimize loop
	-Wunused-macros 		# warn about macros that are not expanded anywhere
	-Wcast-qual 			# warn if casting removes type-qualifiers
	-Wconversion 			# warn about conversions that may change the value
	-Wdate-time 			# do not use __TIME__, __DATE__, or __TIMESTAMP__ to get reproducible builds
	-Wsign-conversion 		# warn about conversions between signed and unsigned integers
	-Wlogical-op 			# warn about suspicious usages of logical operations ||, &&, ^
	-Wredundant-decls 		# warn when multiple declarations
	-fno-operator-names 	# do not use "and", "or", and "xor" (and others) as keywords
	-fno-common 			# prevent tentative definitions
)

# additional compile flags
# see https://cliutils.gitlab.io/modern-cmake/chapters/features/cpp11.html
set_target_properties(tmc PROPERTIES
	CXX_STANDARD 17			# use -std=c++17
    CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF		# do not use -std=gnu++17
)

target_compile_options(tmc
	PRIVATE "${WARNINGS}" "--optimize=3"
)

# add -I. from root of project directory
target_include_directories(tmc
	PUBLIC "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests
	PUBLIC "${CMAKE_SOURCE_DIR}"
)
set_target_properties(tests PROPERTIES
	CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF		# do not use -std=gnu++17
)
