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

# source files to generate tmc executable
add_executable(tmc
	main.cpp
	TMCompiler/compiler/compiler.cpp
	TMCompiler/compiler/lexer/lexer.cpp
	TMCompiler/compiler/models/grammar.cpp
	TMCompiler/compiler/parser/earley_parser.cpp
	TMCompiler/compiler/utils/bnf_parser.cpp
	TMCompiler/utils/logger/logger.cpp
)

# have build tree be aware of config files
configure_file(
	"${CMAKE_SOURCE_DIR}/TMCompiler/config/regex_lexical_grammar.bnf" 
	"${CMAKE_BINARY_DIR}/TMCompiler/config/regex_lexical_grammar.bnf"
	COPYONLY
)
configure_file(
	"${CMAKE_SOURCE_DIR}/TMCompiler/config/language_grammar.bnf"
	"${CMAKE_BINARY_DIR}/TMCompiler/config/language_grammar.bnf"
	COPYONLY
)
configure_file(
	"${CMAKE_SOURCE_DIR}/sample_program.cpp"
	"${CMAKE_BINARY_DIR}/sample_program.cpp"
	COPYONLY
)

# compile warnings
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
