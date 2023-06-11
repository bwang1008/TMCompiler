#include "compiler.hpp"

#include <TMCompiler/compiler/models/grammar.hpp>

class Compiler {

	public:
		Compiler();
		void compile() const;

	private:
		Grammar lexical_grammar;
		Grammar syntactical_grammar;
};
