#include <string>
#include <iostream>

#include <TMCompiler/utils/text_editor/text_editor.hpp>
#include <TMCompiler/utils/unittesting/unittests.hpp>

TEST_CASE(test_move_right) {
	std::string text = "abc\ndef\nghi";
	//abc\n
	//def\n
	//ghi
	TextEditor editor {text};
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 0);

	editor.right();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 1);

	editor.right();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 2);

	editor.right();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 3);

	editor.right();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 0);

	editor.right();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 1);

	editor.right();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 2);

	editor.right();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 3);

	editor.right();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 0);

	editor.right();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 1);

	editor.right();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 2);

	editor.right();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 3);

	// reached end already
	editor.right();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 3);
}

TEST_CASE(test_move_left) {
	std::string text = "abc\ndef\nghi";
	//abc\n
	//def\n
	//ghi
	TextEditor editor {text};
	editor.absolutePosition(text.size());
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 3);

	editor.left();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 2);

	editor.left();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 1);

	editor.left();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 0);

	editor.left();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 3);

	editor.left();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 2);

	editor.left();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 1);

	editor.left();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 0);

	editor.left();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 3);

	editor.left();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 2);

	editor.left();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 1);

	editor.left();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 0);

	// reached end already
	editor.left();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 0);
}

TEST_CASE(test_move_up_and_down) {
	std::string text = "abc\ndef\nghi";
	//abc\n
	//def\n
	//ghi
	TextEditor editor {text};
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 0);

	editor.down();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 0);

	editor.down();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 0);

	editor.down();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 0);

	editor.right();
	ASSERT(editor.lineNum() == 2);
	ASSERT(editor.colNum() == 1);

	editor.up();
	ASSERT(editor.lineNum() == 1);
	ASSERT(editor.colNum() == 1);

	editor.up();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 1);

	editor.up();
	ASSERT(editor.lineNum() == 0);
	ASSERT(editor.colNum() == 1);
}

// TEST_CASE(test_general1) {
	// std::string text = "void my_function{int x}";

	// // change to "void myFunction(int x);"
	
	// TextEditor editor {text};
	// editor.find("_").visual().deleteText();
	// editor.visual().toggleCase();
	// editor.find(")").setMark().opposite().changeChar("{").toMark().changeChar("}");
	// editor.endOfLine().append(";");

	// std::string changedText = editor.getText();
	// std::cout << "changed text = " << changedText << std::endl;
	// ASSERT(changedText == "void myFunction(int x);");
// }
