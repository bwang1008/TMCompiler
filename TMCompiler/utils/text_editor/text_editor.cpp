#include "text_editor.hpp"

#include <algorithm>  // std::max
#include <cstddef>	  // std::size_t
#include <map>		  // std::map
#include <string>	  // std::string
#include <utility>	  // std::pair
#include <vector>	  // std::vector

const char TextEditor::NEWLINE = '\n';
const std::set<char> TextEditor::WHITESPACE = {NEWLINE, ' ', '\r', '\t'};
const std::set<std::pair<char, char> > TextEditor::OPPOSITES = {{'(', ')'}, {'[', ']'}, {'{', '}'}};

TextEditor::TextEditor(const std::string& text)
	: text{text},
	  cursor{0},
	  visualMode{false},
	  visualRange{std::make_pair(std::size_t{0}, std::size_t{0})},
	  copyBuffer{""} {
}

auto TextEditor::getText() const -> std::string {
	return text;
}

auto TextEditor::lineNum() const -> std::size_t {
	return std::get<0>(getRowCol());
}

auto TextEditor::colNum() const -> std::size_t {
	return std::get<1>(getRowCol());
}

auto TextEditor::down(const std::size_t num) -> TextEditor& {
	std::size_t currCol = colNum();

	for(std::size_t i = 0; i < num; ++i) {
		std::size_t newlineLocation = searchFor(std::string(1, NEWLINE), cursor);
		if(newlineLocation < text.size() && text[newlineLocation] == NEWLINE) {
			cursor = 1 + newlineLocation;
		}
	}

	return right(currCol);
}

auto TextEditor::left(const std::size_t num) -> TextEditor& {
	for(std::size_t i = 0; i < num; ++i) {
		if(cursor > 0) {
			--cursor;
		}
	}
	
	return *this;
}

auto TextEditor::right(const std::size_t num) -> TextEditor& {
	for(std::size_t i = 0; i < num; ++i) {
		if(cursor < text.size()) {
			++cursor;
		}
	}
	
	return *this;
}

auto TextEditor::up(const std::size_t num) -> TextEditor& {
	std::size_t currCol = colNum();
	for(std::size_t i = 0; i < num; ++i) {
		cursor = searchFor(std::string(1, NEWLINE), cursor, true);
	}
	
	if(colNum() > currCol) {
		return left(colNum() - currCol);
	}

	return *this;
}

auto TextEditor::goToLine(const std::size_t lineNumber) -> TextEditor& {
	return goToRowCol(lineNumber, 0);
}

auto TextEditor::goToRowCol(const std::size_t row, const std::size_t col) -> TextEditor& {
	std::size_t currRow = 0;
	std::size_t currCol = 0;
	
	for(cursor = 1; cursor < text.size() && currRow < row && currCol < col; ++cursor) {
		if(text[cursor - 1] == NEWLINE) {
			++currRow;
			currCol = 0;
		}
		else {
			++currCol;
		}
	}
	
	return *this;
}

auto TextEditor::absolutePosition(const std::size_t position) -> TextEditor& {
	cursor = std::min(position, text.size());
	return *this;
}

auto TextEditor::setMark(const std::string& label) -> TextEditor& {
	return *this;
}

auto TextEditor::find(const char character) -> TextEditor& {
	cursor = searchFor(std::string(1, character), cursor);
	return *this;
}

auto TextEditor::find(const std::string& pattern) -> TextEditor& {
	cursor = searchFor(pattern, cursor);
	return *this;
}

auto TextEditor::findPrev(const char character) -> TextEditor& {
	cursor = searchFor(std::string(1, character), cursor, true);
	return *this;
}

auto TextEditor::findPrev(const std::string& pattern) -> TextEditor& {
	cursor = searchFor(pattern, cursor, true);
	return *this;
}

auto TextEditor::til(const char character) -> TextEditor& {
	std::size_t searchIndex = searchFor(std::string(1, character), cursor);
	if(cursor != searchIndex) {
		cursor = searchIndex - 1;
	}
	
	return *this;
}

auto TextEditor::til(const std::string& pattern) -> TextEditor& {
	std::size_t searchIndex = searchFor(pattern, cursor);
	if(cursor != searchIndex) {
		cursor = searchIndex - 1;
	}
	
	return *this;
}

auto TextEditor::tilPrev(const char character) -> TextEditor& {
	std::size_t searchIndex = searchFor(std::string(1, character), cursor, true);
	if(cursor != searchIndex) {
		cursor = searchIndex + 1;
	}
	
	return *this;
}

auto TextEditor::tilPrev(const std::string& pattern) -> TextEditor& {
	std::size_t searchIndex = searchFor(pattern, cursor, true);
	if(cursor != searchIndex) {
		cursor = searchIndex + 1;
	}

	return *this;
}

auto TextEditor::aWord() -> TextEditor& {
	if(WHITESPACE.find(text[cursor]) == WHITESPACE.end()) {
		// if not on whitespace, go to beginning of current word
		beginCurrWord();
	}
	visualRange.first = cursor; 
	
	endOfWord();
	visualRange.second = cursor;
	
	return *this;
}

auto TextEditor::aLine() -> TextEditor& {
	startOfLine();
	visualRange.first = cursor;
	
	endOfLine();
	visualRange.second = cursor;
	
	return *this;
}

auto TextEditor::startOfLine(const bool startOfNonBlank) -> TextEditor& {
	if(text.empty()) {
		cursor = 0;
		return *this;
	}

	std::size_t prevNewline = searchFor(std::string(1, NEWLINE), cursor, true);
	if(startOfNonBlank) {
		cursor = searchForNonWhitespace(prevNewline, true);
	}
	else {
		cursor = prevNewline + 1;
	}
	
	return *this;
}

auto TextEditor::endOfLine(const bool endOfNonBlank) -> TextEditor& {
	std::size_t nextNewline = searchFor(std::string(1, NEWLINE), cursor);
	if(endOfNonBlank) {
		cursor = searchForNonWhitespace(nextNewline, true);
	}
	else{
		cursor = nextNewline;
	}
	
	return *this;
}

auto TextEditor::firstLine() -> TextEditor& {
	cursor = 0;
	return *this;
}

auto TextEditor::lastLine() -> TextEditor& {
	if(text.empty()) {
		cursor = 0;
		return *this;
	}
	
	std::size_t lastPosition = text.size() - 1;
	std::size_t lastNewline = searchFor(std::string(1, NEWLINE), lastPosition, true);
	
	cursor = lastNewline + 1;
	return *this;
}

auto TextEditor::beginNextWord() -> TextEditor& {
	std::size_t whitespacePosition = searchForWhitespace(cursor);
	cursor = searchForNonWhitespace(whitespacePosition);
	return *this;
}

auto TextEditor::beginCurrWord() -> TextEditor& {
	if(text.empty()) {
		return *this;
	}
	
	std::size_t prevWhitespacePosition = searchForWhitespace(cursor, true);
	if(prevWhitespacePosition == cursor) {
		cursor = 0;
	}
	else {
		cursor = prevWhitespacePosition + 1;
	}
	
	return *this;
}


auto TextEditor::endOfWord() -> TextEditor& {
	if(text.empty()) {
		return *this;
	}

	if(WHITESPACE.find(text[cursor]) != WHITESPACE.end()) {
		cursor = searchForNonWhitespace(cursor);
	}
	
	std::size_t nextWhitespace = searchForWhitespace(cursor);
	if(nextWhitespace != cursor) {
		cursor = nextWhitespace - 1;
	}
	
	return *this;
}

auto TextEditor::opposite() -> TextEditor& {
	if(text.empty()) {
		return *this;
	}

	const char currentCharacter = text[cursor];
	for(std::pair<char, char> pairOfOpposites : OPPOSITES) {
		if(pairOfOpposites.first == currentCharacter) {
			cursor = searchFor(std::string(1, pairOfOpposites.second), cursor);
		}
		else if(pairOfOpposites.second == currentCharacter) {
			cursor = searchFor(std::string(1, pairOfOpposites.first), cursor, true);
		}
	}

	return *this;
}

auto TextEditor::toMark(const std::string& label) -> TextEditor& {
	return *this;
}

auto TextEditor::visual() -> TextEditor& {
	visualMode = !visualMode;
	
	if(visualMode) {
		visualRange.first = visualRange.second = cursor;
	}
	
	return *this;
}

auto TextEditor::insert(const std::string& additionalText) -> TextEditor& {
	text.insert(cursor, additionalText);
	return *this;
}

auto TextEditor::insertLine(const std::string& additionalText,
								  const bool above) -> TextEditor& {

	std::size_t insertIndex = 0;
	if(above) {
		std::size_t prevNewline = searchFor(std::string(1, NEWLINE), cursor, true);
		if(prevNewline == cursor) {
			insertIndex = 0;
		}
		else {


		}

	}

	return *this;
}

auto TextEditor::insertInBeginningOfEachLine(
	const std::string& additionalText) -> TextEditor& {
	return *this;
}

auto TextEditor::append(const std::string& additionalText) -> TextEditor& {
	return *this;
}

auto TextEditor::change(const std::string& additionalText) -> TextEditor& {
	return *this;
}
auto TextEditor::changeLine(const std::string& additionalText) -> TextEditor& {
	return *this;
}
auto TextEditor::changeChar(const std::string& additionalText) -> TextEditor& {
	return *this;
}
auto TextEditor::deleteText() -> TextEditor& {
	return *this;
}
auto TextEditor::deleteChar() -> TextEditor& {
	return *this;
}
auto TextEditor::deleteLine() -> TextEditor& {
	return *this;
}
auto TextEditor::replace(const std::string& oldPattern,
							   const std::string& newPattern) -> TextEditor& {
	return *this;
}
auto TextEditor::copy() -> TextEditor& {
	return *this;
}
auto TextEditor::paste() -> TextEditor& {
	return *this;
}
auto TextEditor::toggleCase() -> TextEditor& {
	return *this;
}

auto TextEditor::stripWhiteSpace(const bool begin, const bool end) -> TextEditor& {
	return *this;
}
auto TextEditor::joinNextLine() -> TextEditor& {
	return *this;
}

auto TextEditor::getRowCol() const -> std::pair<std::size_t, std::size_t> {
	std::size_t row = 0;
	std::size_t col = 0;
	for(std::size_t i = 0; i < cursor; ++i) {
		if(text[i] == NEWLINE) {
			++row;
			col = 0;
		} else {
			++col;
		}
	}

	return std::make_pair(row, col);
}

void TextEditor::finishAction() {
	visualMode = false;
}

auto TextEditor::onLastLine() const -> bool {
	std::size_t newlineLocation = searchFor(std::string(1, NEWLINE), cursor);
	// if cursor was at end, or you did not find a new line at index
	return newlineLocation == text.size() || text[newlineLocation] != NEWLINE;
}

auto TextEditor::stringMatches(const std::string& pattern,
							   const std::size_t index) const -> bool {
	if(index + pattern.size() > text.size()) {
		return false;
	}

	for(std::size_t i = 0; i < pattern.size(); ++i) {
		if(text[index + i] != pattern[i]) {
			return false;
		}
	}

	return true;
}

auto TextEditor::searchFor(const std::string& pattern,
								  const std::size_t searchIndex,
								  const bool backwards) const -> std::size_t {
	if(backwards) {
		for(std::size_t searchCandidate = searchIndex; searchCandidate--;) {
			if(stringMatches(pattern, searchCandidate)) {
				return searchCandidate;
			}
		}
	}
	else {
		for(std::size_t searchCandidate = searchIndex + 1;
			searchCandidate < text.size(); ++searchCandidate) {
			if(stringMatches(pattern, searchCandidate)) {
				return searchCandidate;
			}
		}
	}

	return searchIndex;
}

auto TextEditor::searchForWhitespace(
								  const std::size_t searchIndex,
								  const bool backwards) const -> std::size_t {
	if(backwards) {
		for(std::size_t searchCandidate = searchIndex; searchCandidate--;) {
			if(WHITESPACE.find(text[searchIndex]) != WHITESPACE.end()) {
				return searchCandidate;
			}
		}
	}
	else {
		for(std::size_t searchCandidate = searchIndex + 1;
			searchCandidate < text.size(); ++searchCandidate) {
			if(WHITESPACE.find(text[searchIndex]) != WHITESPACE.end()) {
				return searchCandidate;
			}
		}
	}

	return searchIndex;
}

auto TextEditor::searchForNonWhitespace(
								  const std::size_t searchIndex,
								  const bool backwards) const -> std::size_t {
	if(backwards) {
		for(std::size_t searchCandidate = searchIndex; searchCandidate--;) {
			if(WHITESPACE.find(text[searchIndex]) == WHITESPACE.end()) {
				return searchCandidate;
			}
		}
	}
	else {
		for(std::size_t searchCandidate = searchIndex + 1;
			searchCandidate < text.size(); ++searchCandidate) {
			if(WHITESPACE.find(text[searchIndex]) == WHITESPACE.end()) {
				return searchCandidate;
			}
		}
	}

	return searchIndex;
}
