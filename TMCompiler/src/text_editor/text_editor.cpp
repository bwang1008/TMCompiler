#include "text_editor.hpp"

#include <cdtddef>		// std::size_t
#include <string>		// std::string
#include <map>			// std::map
#include <utility>		// std::pair
#include <vector>		// std::vector


TextEditor::TextEditor(const std::string text): text{text}, cursor{0}, newLineLocations{}, markLocations{}, visualMode{false}, copyBuffer{""} {
	redoNewLines();
}

std::string TextEditor::getText() const {
	return text;
}

std::size_t TextEditor::lineNum() const {
	if(newLineLocations.size() == 0) {
		return 0;
	}

	std::size_t low = 0;
	std::size_t high = newLineLocations.size() - 1;

	while(low <= high) {
		std::size_t mid = (low + high) / 2;
		if(newLineLocations[mid] < cursor) {
			low = mid + 1;
		}
		else {
			high = mid - 1;
		}
	}

	return low;
}

std::size_t TextEditor::colNum() const {
	return cursor - newLineLocations[lineNum()];
}

TextEditor TextEditor::down(const int num) {
	if(num < 0) {
		return up(-num);
	}
	
	bool hasNextLine = 


}

TextEditor TextEditor::left(const int num);
TextEditor TextEditor::right(const int num);
TextEditor TextEditor::up(const int num);

TextEditor TextEditor::line(const int lineNumber);
TextEditor TextEditor::rowCol(const int row, const int col);
TextEditor TextEditor::absolutePosition(const int position);

TextEditor TextEditor::setMark(const std::string label);

TextEditor TextEditor::find(const char character);
TextEditor TextEditor::find(const std::string pattern);
TextEditor TextEditor::findPrev(const char character);
TextEditor TextEditor::findPrev(const std::string pattern);

TextEditor TextEditor::til(const char character);
TextEditor TextEditor::til(const std::string pattern);
TextEditor TextEditor::tilPrev(const char character);
TextEditor TextEditor::tilPrev(const std::string pattern);

TextEditor TextEditor::aWord();
TextEditor TextEditor::aLine();

TextEditor TextEditor::beginNextWord();


TextEditor TextEditor::startOfLine(const bool startOfNonBlank);
TextEditor TextEditor::endOfLine(const bool endOfNonBlank);
TextEditor TextEditor::firstLine();
TextEditor TextEditor::lastLine();

TextEditor TextEditor::beginNextWord(const bool fullWord);
TextEditor TextEditor::endOfWord(const bool fullWord);

TextEditor TextEditor::opposite();
TextEditor TextEditor::toMark(const std::string label);

TextEditor TextEditor::visual();
TextEditor TextEditor::visualLineMode();

TextEditor TextEditor::insert(const std::string additionalText);
TextEditor TextEditor::insertLine(const std::string additionalText, const bool below);
TextEditor TextEditor::insertInBeginningOfEachLine(const std::string additionalText);

TextEditor TextEditor::append(const std::string additionalText);

TextEditor TextEditor::change(const std::string additionalText);
TextEditor TextEditor::changeLine(const std::string additionalText);
TextEditor TextEditor::changeChar(const std::string additionalText);
TextEditor TextEditor::deleteText();
TextEditor TextEditor::deleteChar();
TextEditor TextEditor::deleteLine();
TextEditor TextEditor::replace(const std::string oldPattern, const std::string newPattern);
TextEditor TextEditor::copy();
TextEditor TextEditor::paste();
TextEditor TextEditor::toggleCase();

TextEditor TextEditor::stripWhiteSpace(const bool begin, const bool end);
TextEditor TextEditor::joinNextLine();

std::pair<std::size_t, std::size_t> TextEditor::getRowCol() {

}

void TextEditor::finishAction();
void TextEditor::redoNewLines() {
	newLineLocations.clear();
	for(std::size_t i = 0; i < text.size(); ++i) {
		if(text[i] == '\n') {
			newLineLocations.push_back(i);
		}
	}
}
