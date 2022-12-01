#ifndef TEXT_EDITOR_HPP
#define TEXT_EDITOR_HPP

#include <cstddef>	// std::size_t
#include <set>		// std::set
#include <map>		// std::map
#include <string>	// std::string
#include <utility>	// std::pair
#include <vector>	// std::vector

class TextEditor {
public:
	// constructor and convenience functions
	TextEditor(const std::string& text);
	std::string getText() const;
	std::size_t lineNum() const;
	std::size_t colNum() const;

	// cursor movement
	TextEditor& down(const std::size_t num = 1);
	TextEditor& left(const std::size_t num = 1);
	TextEditor& right(const std::size_t num = 1);
	TextEditor& up(const std::size_t num = 1);

	TextEditor& goToLine(const std::size_t lineNumber);
	TextEditor& goToRowCol(const std::size_t row, const std::size_t col);
	TextEditor& absolutePosition(const std::size_t position);

	TextEditor& setMark(const std::string& label = "");

	// range
	TextEditor& find(const char character);
	TextEditor& find(const std::string& pattern);
	TextEditor& findPrev(const char character);
	TextEditor& findPrev(const std::string& pattern);

	TextEditor& til(const char character);
	TextEditor& til(const std::string& pattern);
	TextEditor& tilPrev(const char character);
	TextEditor& tilPrev(const std::string& pattern);

	TextEditor& aWord();
	TextEditor& aLine();

	// range convenience function
	TextEditor& startOfLine(const bool startOfNonBlank = true);
	TextEditor& endOfLine(const bool endOfNonBlank = true);
	TextEditor& firstLine();
	TextEditor& lastLine();

	TextEditor& beginNextWord();
	TextEditor& beginCurrWord();
	TextEditor& endOfWord();

	TextEditor& opposite();
	TextEditor& toMark(const std::string& label = "");

	// visual
	TextEditor& visual();

	// actions: substitute, delete, insert
	TextEditor& insert(const std::string& additionalText);
	TextEditor& insertLine(const std::string& additionalText,
						   const bool above = false);
	TextEditor& insertInBeginningOfEachLine(const std::string& additionalText);

	TextEditor& append(const std::string& additionalText);

	TextEditor& change(const std::string& additionalText);
	TextEditor& changeLine(const std::string& additionalText);
	TextEditor& changeChar(const std::string& additionalText);
	TextEditor& deleteText();
	TextEditor& deleteChar();
	TextEditor& deleteLine();
	TextEditor& replace(const std::string& oldPattern,
						const std::string& newPattern);
	TextEditor& copy();
	TextEditor& paste();
	TextEditor& toggleCase();

	// extraneous functions
	TextEditor& stripWhiteSpace(const bool begin = true, const bool end = true);
	TextEditor& joinNextLine();

private:
	// class constants
	static const char NEWLINE;
	static const std::set<char> WHITESPACE;
	static const std::set<std::pair<char, char> > OPPOSITES;

	// data representation
	std::string text;
	std::size_t cursor;			// 0 <= cursor <= text.size() (fence-post around letters)
	std::map<std::string, std::size_t> markLocations;
	bool visualMode;
	std::pair<std::size_t, std::size_t> visualRange;
	std::string copyBuffer;

	// helper functions
	std::pair<std::size_t, std::size_t> getRowCol() const;
	void finishAction();

	bool onLastLine() const;
	bool stringMatches(const std::string& pattern, const std::size_t index) const;
	std::size_t searchFor(const std::string& searchText,
						  const std::size_t searchIndex,
						  const bool backwards = false) const;
	std::size_t searchForWhitespace(const std::size_t searchIndex, const bool backwards = false) const;
	std::size_t searchForNonWhitespace(const std::size_t searchIndex, const bool backwards = false) const;
};

#endif
