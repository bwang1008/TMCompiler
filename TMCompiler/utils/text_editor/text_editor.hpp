#ifndef TEXT_EDITOR_HPP
#define TEXT_EDITOR_HPP

#include <cstddef>	// std::size_t
#include <map>		// std::map
#include <set>		// std::set
#include <string>	// std::string
#include <utility>	// std::pair
#include <vector>	// std::vector

class TextEditor {
public:
	// constructor and convenience functions
	explicit TextEditor(const std::string& text);
	auto getText() const -> std::string;
	auto lineNum() const -> std::size_t;
	auto colNum() const -> std::size_t;

	// cursor movement
	auto down(std::size_t num = 1) -> TextEditor&;
	auto left(std::size_t num = 1) -> TextEditor&;
	auto right(std::size_t num = 1) -> TextEditor&;
	auto up(std::size_t num = 1) -> TextEditor&;

	auto goToLine(std::size_t lineNumber) -> TextEditor&;
	auto goToRowCol(std::size_t row, std::size_t col) -> TextEditor&;
	auto absolutePosition(std::size_t position) -> TextEditor&;

	auto setMark(const std::string& label = "") -> TextEditor&;

	// range
	auto find(char character) -> TextEditor&;
	auto find(const std::string& pattern) -> TextEditor&;
	auto findPrev(char character) -> TextEditor&;
	auto findPrev(const std::string& pattern) -> TextEditor&;

	auto til(char character) -> TextEditor&;
	auto til(const std::string& pattern) -> TextEditor&;
	auto tilPrev(char character) -> TextEditor&;
	auto tilPrev(const std::string& pattern) -> TextEditor&;

	auto aWord() -> TextEditor&;
	auto aLine() -> TextEditor&;

	// range convenience function
	auto startOfLine(bool startOfNonBlank = true) -> TextEditor&;
	auto endOfLine(bool endOfNonBlank = true) -> TextEditor&;
	auto firstLine() -> TextEditor&;
	auto lastLine() -> TextEditor&;

	auto beginNextWord() -> TextEditor&;
	auto beginCurrWord() -> TextEditor&;
	auto endOfWord() -> TextEditor&;

	auto opposite() -> TextEditor&;
	auto toMark(const std::string& label = "") -> TextEditor&;

	// visual
	auto visual() -> TextEditor&;

	// actions: substitute, delete, insert
	auto insert(const std::string& additionalText) -> TextEditor&;
	auto insertLine(const std::string& additionalText, bool above = false)
		-> TextEditor&;
	auto insertInBeginningOfEachLine(const std::string& additionalText)
		-> TextEditor&;

	auto append(const std::string& additionalText) -> TextEditor&;

	auto change(const std::string& additionalText) -> TextEditor&;
	auto changeLine(const std::string& additionalText) -> TextEditor&;
	auto changeChar(const std::string& additionalText) -> TextEditor&;
	auto deleteText() -> TextEditor&;
	auto deleteChar() -> TextEditor&;
	auto deleteLine() -> TextEditor&;
	auto replace(const std::string& oldPattern, const std::string& newPattern)
		-> TextEditor&;
	auto copy() -> TextEditor&;
	auto paste() -> TextEditor&;
	auto toggleCase() -> TextEditor&;

	// extraneous functions
	auto stripWhiteSpace(bool begin = true, bool end = true) -> TextEditor&;
	auto joinNextLine() -> TextEditor&;

private:
	// class constants
	static const char NEWLINE;
	static const std::set<char> WHITESPACE;
	static const std::set<std::pair<char, char> > OPPOSITES;

	// data representation
	std::string text;
	std::size_t
		cursor;	 // 0 <= cursor <= text.size() (fence-post around letters)
	std::map<std::string, std::size_t> markLocations;
	bool visualMode;
	std::pair<std::size_t, std::size_t> visualRange;
	std::string copyBuffer;

	// helper functions
	auto getRowCol() const -> std::pair<std::size_t, std::size_t>;
	auto finishAction() -> void;

	auto onLastLine() const -> bool;
	auto stringMatches(const std::string& pattern, std::size_t index) const
		-> bool;
	auto searchFor(const std::string& searchText, std::size_t searchIndex,
				   bool backwards = false) const -> std::size_t;
	auto searchForWhitespace(std::size_t searchIndex,
							 bool backwards = false) const -> std::size_t;
	auto searchForNonWhitespace(std::size_t searchIndex,
								bool backwards = false) const -> std::size_t;
};

#endif
