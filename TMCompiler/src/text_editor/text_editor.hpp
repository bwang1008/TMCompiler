#ifndef TEXT_EDITOR_HPP
#define TEXT_EDITOR_HPP

#include <cdtddef>		// std::size_t
#include <string>		// std::string
#include <map>			// std::map
#include <utility>		// std::pair
#include <vector>		// std::vector

class TextEditor {
	public:
		// constructor and convenience functions
		TextEditor(const std::string& text);
		std::string getText() const;
		std::size_t lineNum() const;
		std::size_t colNum() const;

		// cursor movement
		TextEditor down(const int num = 1);
		TextEditor left(const int num = 1);
		TextEditor right(const int num = 1);
		TextEditor up(const int num = 1);

		TextEditor line(const int lineNumber);
		TextEditor rowCol(const int row, const int col);
		TextEditor absolutePosition(const int position);

		TextEditor setMark(const std::string& label = "");

		// range
		TextEditor find(const char character);
		TextEditor find(const std::string& pattern);
		TextEditor findPrev(const char character);
		TextEditor findPrev(const std::string& pattern);

		TextEditor til(const char character);
		TextEditor til(const std::string& pattern);
		TextEditor tilPrev(const char character);
		TextEditor tilPrev(const std::string& pattern);

		TextEditor aWord();
		TextEditor aLine();

		TextEditor beginNextWord();


		// range convenience function
		TextEditor startOfLine(const bool startOfNonBlank = true);
		TextEditor endOfLine(const bool endOfNonBlank = true);
		TextEditor firstLine();
		TextEditor lastLine();

		TextEditor beginNextWord(const bool fullWord = false);
		TextEditor endOfWord(const bool fullWord = false);

		TextEditor opposite();
		TextEditor toMark(const std::string& label = "");

		// visual
		TextEditor visual();
		TextEditor visualLineMode();

		// actions: substitute, delete, insert
		TextEditor insert(const std::string& additionalText);
		TextEditor insertLine(const std::string& additionalText, const bool below = false);
		TextEditor insertInBeginningOfEachLine(const std::string& additionalText);

		TextEditor append(const std::string& additionalText);
		
		TextEditor change(const std::string& additionalText);
		TextEditor changeLine(const std::string& additionalText);
		TextEditor changeChar(const std::string& additionalText);
		TextEditor deleteText();
		TextEditor deleteChar();
		TextEditor deleteLine();
		TextEditor replace(const std::string& oldPattern, const std::string& newPattern);
		TextEditor copy();
		TextEditor paste();
		TextEditor toggleCase();

		// extraneous functions
		TextEditor stripWhiteSpace(const bool begin = true, const bool end = true);
		TextEditor joinNextLine();
		
	private:
		// how do you wanna represent text?
		std::string text;
		std::size_t cursor;
		std::vector<std::size_t> newLineLocations;
		std::map<std::string, std::size_t> markLocations;
		bool visualMode;
		std::string copyBuffer;

		std::pair<std::size_t, std::size_t> getRowCol();

		void finishAction();
		void redoNewLines();
}

#endif
