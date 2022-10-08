#include <string>
#include <iostream>

#include "TMCompiler/src/text_editor/text_editor.hpp"

bool test_line1() {
	std::string text = "abc\ndef\nghi";
	TextEditor editor {text};
	return 
}

bool test_general1() {
	std::string text = "void my_function{int x}";

	// change to "void myFunction(int x);"
	
	TextEditor editor {text};
	editor.find("_").visual().deleteText();
	editor.visual().toggleCase();
	editor.find(")").setMark().opposite().changeChar("{").toMark().changeChar("}");
	editor.endOfLine().append(";");

	std::string changedText = editor.getText();
	return changedText == "void myFunction(int x);";
}

int main() {
	std::cout << "changedText = " << changedText << std::endl;
}

