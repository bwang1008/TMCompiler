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
	  markLocations{},
	  visualMode{false},
	  visualRange{std::make_pair(std::size_t{0}, std::size_t{0})},
	  copyBuffer{""} {
}

std::string TextEditor::getText() const {
	return text;
}

std::size_t TextEditor::lineNum() const {
	return std::get<0>(getRowCol());
}

std::size_t TextEditor::colNum() const {
	return std::get<1>(getRowCol());
}

TextEditor& TextEditor::down(const std::size_t num) {
	std::size_t currCol = colNum();

	for(std::size_t i = 0; i < num; ++i) {
		std::size_t newlineLocation = searchFor(std::string(1, NEWLINE), cursor);
		if(newlineLocation < text.size() && text[newlineLocation] == NEWLINE) {
			cursor = 1 + newlineLocation;
		}
	}

	return right(currCol);
}

TextEditor& TextEditor::left(const std::size_t num) {
	for(std::size_t i = 0; i < num; ++i) {
		if(cursor > 0) {
			--cursor;
		}
	}
	
	return *this;
}

TextEditor& TextEditor::right(const std::size_t num) {
	for(std::size_t i = 0; i < num; ++i) {
		if(cursor < text.size()) {
			++cursor;
		}
	}
	
	return *this;
}

TextEditor& TextEditor::up(const std::size_t num) {
	std::size_t currCol = colNum();
	for(std::size_t i = 0; i < num; ++i) {
		cursor = searchFor(std::string(1, NEWLINE), cursor, true);
	}
	
	if(colNum() > currCol) {
		return left(colNum() - currCol);
	}

	return *this;
}

TextEditor& TextEditor::goToLine(const std::size_t lineNumber) {
	return goToRowCol(lineNumber, 0);
}

TextEditor& TextEditor::goToRowCol(const std::size_t row, const std::size_t col) {
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

TextEditor& TextEditor::absolutePosition(const std::size_t position) {
	cursor = std::min(position, text.size());
	return *this;
}

TextEditor& TextEditor::setMark(const std::string& label) {
	return *this;
}

TextEditor& TextEditor::find(const char character) {
	cursor = searchFor(std::string(1, character), cursor);
	return *this;
}

TextEditor& TextEditor::find(const std::string& pattern) {
	cursor = searchFor(pattern, cursor);
	return *this;
}

TextEditor& TextEditor::findPrev(const char character) {
	cursor = searchFor(std::string(1, character), cursor, true);
	return *this;
}

TextEditor& TextEditor::findPrev(const std::string& pattern) {
	cursor = searchFor(pattern, cursor, true);
	return *this;
}

TextEditor& TextEditor::til(const char character) {
	std::size_t searchIndex = searchFor(std::string(1, character), cursor);
	if(cursor != searchIndex) {
		cursor = searchIndex - 1;
	}
	
	return *this;
}

TextEditor& TextEditor::til(const std::string& pattern) {
	std::size_t searchIndex = searchFor(pattern, cursor);
	if(cursor != searchIndex) {
		cursor = searchIndex - 1;
	}
	
	return *this;
}

TextEditor& TextEditor::tilPrev(const char character) {
	std::size_t searchIndex = searchFor(std::string(1, character), cursor, true);
	if(cursor != searchIndex) {
		cursor = searchIndex + 1;
	}
	
	return *this;
}

TextEditor& TextEditor::tilPrev(const std::string& pattern) {
	std::size_t searchIndex = searchFor(pattern, cursor, true);
	if(cursor != searchIndex) {
		cursor = searchIndex + 1;
	}

	return *this;
}

TextEditor& TextEditor::aWord() {
	if(WHITESPACE.find(text[cursor]) == WHITESPACE.end()) {
		// if not on whitespace, go to beginning of current word
		beginCurrWord();
	}
	visualRange.first = cursor; 
	
	endOfWord();
	visualRange.second = cursor;
	
	return *this;
}

TextEditor& TextEditor::aLine() {
	startOfLine();
	visualRange.first = cursor;
	
	endOfLine();
	visualRange.second = cursor;
	
	return *this;
}

TextEditor& TextEditor::startOfLine(const bool startOfNonBlank) {
	if(text.size() == 0) {
		cursor = 0;
		return *this;
	}

	std::size_t prevNewline = searchFor(std::string(1, NEWLINE), cursor, true);
	if(startOfNonBlank) {
		cursor = searchForNonWhitespace(prevNewline, cursor);
	}
	else {
		cursor = prevNewline + 1;
	}
	
	return *this;
}

TextEditor& TextEditor::endOfLine(const bool endOfNonBlank) {
	std::size_t nextNewline = searchFor(std::string(1, NEWLINE), cursor);
	if(endOfNonBlank) {
		cursor = searchForNonWhitespace(nextNewline, true);
	}
	else{
		cursor = nextNewline;
	}
	
	return *this;
}

TextEditor& TextEditor::firstLine() {
	cursor = 0;
	return *this;
}

TextEditor& TextEditor::lastLine() {
	if(text.size() == 0) {
		cursor = 0;
		return *this;
	}
	
	std::size_t lastPosition = text.size() - 1;
	std::size_t lastNewline = searchFor(std::string(1, NEWLINE), lastPosition, true);
	
	cursor = lastNewline + 1;
	return *this;
}

TextEditor& TextEditor::beginNextWord() {
	std::size_t whitespacePosition = searchForWhitespace(cursor);
	cursor = searchForNonWhitespace(whitespacePosition);
	return *this;
}

TextEditor& TextEditor::beginCurrWord() {
	if(text.size() == 0) {
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


TextEditor& TextEditor::endOfWord() {
	if(text.size() == 0) {
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

TextEditor& TextEditor::opposite() {
	if(text.size() == 0) {
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

TextEditor& TextEditor::toMark(const std::string& label) {
	return *this;
}

TextEditor& TextEditor::visual() {
	visualMode = !visualMode;
	
	if(visualMode) {
		visualRange.first = visualRange.second = cursor;
	}
	
	return *this;
}

TextEditor& TextEditor::insert(const std::string& additionalText) {
	text.insert(cursor, additionalText);
	return *this;
}

TextEditor& TextEditor::insertLine(const std::string& additionalText,
								  const bool above) {

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

TextEditor& TextEditor::insertInBeginningOfEachLine(
	const std::string& additionalText) {
	return *this;
}

TextEditor& TextEditor::append(const std::string& additionalText) {
	return *this;
}

TextEditor& TextEditor::change(const std::string& additionalText) {
	return *this;
}
TextEditor& TextEditor::changeLine(const std::string& additionalText) {
	return *this;
}
TextEditor& TextEditor::changeChar(const std::string& additionalText) {
	return *this;
}
TextEditor& TextEditor::deleteText() {
	return *this;
}
TextEditor& TextEditor::deleteChar() {
	return *this;
}
TextEditor& TextEditor::deleteLine() {
	return *this;
}
TextEditor& TextEditor::replace(const std::string& oldPattern,
							   const std::string& newPattern) {
	return *this;
}
TextEditor& TextEditor::copy() {
	return *this;
}
TextEditor& TextEditor::paste() {
	return *this;
}
TextEditor& TextEditor::toggleCase() {
	return *this;
}

TextEditor& TextEditor::stripWhiteSpace(const bool begin, const bool end) {
	return *this;
}
TextEditor& TextEditor::joinNextLine() {
	return *this;
}

std::pair<std::size_t, std::size_t> TextEditor::getRowCol() const {
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

bool TextEditor::onLastLine() const {
	std::size_t newlineLocation = searchFor(std::string(1, NEWLINE), cursor);
	// if cursor was at end, or you did not find a new line at index
	return newlineLocation == text.size() || text[newlineLocation] != NEWLINE;
}

bool TextEditor::stringMatches(const std::string& pattern,
							   const std::size_t index) const {
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

std::size_t TextEditor::searchFor(const std::string& pattern,
								  const std::size_t searchIndex,
								  const bool backwards) const {
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

std::size_t TextEditor::searchForWhitespace(
								  const std::size_t searchIndex,
								  const bool backwards) const {
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

std::size_t TextEditor::searchForNonWhitespace(
								  const std::size_t searchIndex,
								  const bool backwards) const {
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
