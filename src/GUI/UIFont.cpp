#include "UIFont.h"
#include <fstream>
#include <vector>
#include "../Graphics/ImageLoader.h"
#include "UISystem.h"

using namespace TChapman500::Graphics;

namespace TChapman500 {
namespace GUI {

UIFont::UIFont(std::wstring &path, bool pointFilter)
{
	// Initialize character data to zero.
	ZeroMemory(CharData, sizeof(char_data) * 256);

	// Must be supplied.
	if (path.empty()) return;

	// Allocate space for the name.
	Path = path;

	if (pointFilter) Sampler = UISystem::_PointFilter;
	else Sampler = UISystem::_LinearFilter;
	
	// TGA File
	{
		// Get the file name.
		std::wstring tgaPath = Path + L".tga";
		Texture = CreateShaderResource(tgaPath);
	}

	// Get the font file.
	std::wstring fontPath = Path + L".font";

	// Read the file.
	std::fstream file(fontPath, std::ios::in | std::ios::binary);
	if (!file.is_open()) return;

	// Validate Version
	unsigned version;
	file.read((char *)&version, 4);	// Version
	if (version != 1) return;

	// Read the rest of the header.
	file.read((char *)&FixedCharWidth, 4);
	file.read((char *)&CharHeight, 4);
	file.read((char *)&RectHeight, 4);
	file.read((char *)&TabWidthPixels, 4);
	file.read((char *)&TabWidth, 4);

	// Populate Character Data.
	for (unsigned i = 32; i < 256; i++)
	{
		// Read character properties.
		file.read((char *)&CharData[i].CharWidth, 4);
		file.read((char *)&CharData[i].RectWidth, 4);

		// Read character texture coords.
		file.read((char *)&CharData[i].ULeft, 4);
		file.read((char *)&CharData[i].VTop, 4);
		file.read((char *)&CharData[i].URight, 4);
		file.read((char *)&CharData[i].VBottom, 4);
	}

	// We're done here.
	file.close();
}

UIFont::~UIFont()
{
}

void UIFont::GenerateVertices(wchar_t *str, gen_data *data)
{
	// These must be provided!
	if (!str || !data) return;

	// Truncate the string.
	wchar_t *cStr = str;
	unsigned strLen = (unsigned)wcslen(str);
	if (strLen == 0) return;	// Nothing to generate!

	// Get the number of lines from the string.
	unsigned currLineNum = 0;

	// The working line
	struct str_line
	{
		wchar_t *Start;
		unsigned Length;
		float Width;
	};
	str_line currLine;
	wchar_t *nextLine = cStr;

	// The working word
	struct str_word
	{
		wchar_t *Start;
		unsigned Length;
		float Width;
	};
	str_word currWord;
	wchar_t *nextWord = cStr;

	// The generated characters
	std::vector<char_rect> charRects;
	charRects.reserve(strLen);
	char_rect currRect;

	// Current screen position.
	float currX = 0.0f;
	float currY = 0.0f;

	bool overranHeight = false;

	// Print each line one at a time.
	do
	{
		// Reset the cursor position.
		currX = 0.0f;

		// Get the first character of the line.
		currLine.Start = nextLine;
		_GetLine(currLine.Start, &strLen, &currLine.Length, &nextLine);
		currLine.Start = _TruncLine(currLine.Start, currLine.Length, &currLine.Length);

		// There's nothing on this line.
		if (currLine.Length == 0)
		{
			currLineNum++;
			currY += data->LineHeight;
			if (currY + data->LineHeight > data->Height)
			{
				overranHeight = true;
				break;
			}
			continue;
		}
		nextWord = currLine.Start;

		// Print each word one at a time.
		do
		{
			// Get the first character of the next word.
			currWord.Start = nextWord;
			_GetWord(currWord.Start, &currLine.Length, &currWord.Length, &nextWord);
			currWord.Start = _TruncWord(currWord.Start, &currWord.Length);

			// Get width of the word.
			currWord.Width = _GetWordWidth(currWord.Start, currWord.Length, data->LineHeight);

			// Word is wider than the element.  So keep printing until we reach the end of the word (or element).
			if (currX == 0.0f && currWord.Width > data->Width)
			{
				for (unsigned i = 0; i < currWord.Length; i++)
				{
					float width = data->LineHeight * ((float)CharData[currWord.Start[i]].CharWidth / (float)CharHeight);
					// Don't overrun the width of the element.
					if (currX + width > data->Width)
					{
						// Advance the line number.
						currLineNum++;

						// Reset cursor position.
						currX = 0.0f;
						currY += data->LineHeight;

						// Overran the height of the element.
						if (currY + data->LineHeight > data->Height)
						{
							overranHeight = true;
							break;
						}
					}

					// Generate rect and advance X position.
					if (_GenerateCharRect(currWord.Start[i], &currRect, &currX, currY, data->LineHeight))
					{
						currRect.Line = currLineNum;
						charRects.push_back(currRect);
					}
				}
			}
			// Word fits onto the element.
			else
			{
				// Generate the rectangle for every character in the word.
				for (unsigned i = 0; i < currWord.Length; i++)
				{
					if (_GenerateCharRect(currWord.Start[i], &currRect, &currX, currY, data->LineHeight))
					{
						currRect.Line = currLineNum;
						charRects.push_back(currRect);
					}
				}
			}

			// Out of words for this line.
			if (currLine.Length == 0)
			{
				// Advance line number and cursor.
				currLineNum++;
				currX = 0.0f;
				currY += data->LineHeight;

				// Overran the element height
				if (currY + data->LineHeight > data->Height)
					overranHeight = true;

				// Current line has come to an end.
				break;
			}

			// Get the width of the next word (including preceeding whitespace.
			float nextWidth = _GetWordWidth(nextWord, currLine.Length, data->LineHeight);

			// The next word will overrun the width of the element.
			if (currX + nextWidth > data->Width)
			{
				// Advance line number.
				currLineNum++;

				// Advance cursor position.
				currX = 0.0f;
				currY += data->LineHeight;

				// Overran the height of the element.
				if (currY + data->LineHeight > data->Height)
				{
					overranHeight = true;
					break;
				}
				continue;
			}
			// Next word will not overrun the width of the element.
			else
			{
				// Print the whitespace.
				wchar_t *whitespace = nextWord;
				while (*whitespace <= ' ')
				{
					if (_GenerateCharRect(*whitespace, &currRect, &currX, currY, data->LineHeight))
					{
						currRect.Line = currLineNum;
						charRects.push_back(currRect);
					}
					whitespace++;
				}
			}
			// Time to advance to the next word.
		} while (nextWord);


		// Ran out of space to print stuff.
		if (overranHeight) break;
	} while (nextLine);

	// Done printing the string.  Finalize the vertices and indices.
	data->VertexCount = (unsigned)charRects.size() * 4;
	data->IndexCount = (unsigned)charRects.size() * 6;

	// Generate Index List.
	data->IndexList = new(std::nothrow) unsigned[data->IndexCount];
	if (data->IndexList)
	{
		for (unsigned i = 0, v = 0; i < data->IndexCount; i += 6, v += 4)
		{
			data->IndexList[i] = v;
			data->IndexList[i + 1] = v + 1;
			data->IndexList[i + 2] = v + 2;
			data->IndexList[i + 3] = v;
			data->IndexList[i + 4] = v + 2;
			data->IndexList[i + 5] = v + 3;
		}
	}

	// Non-left alignment is expensive!
	if (data->HorizontalAlign != align::Left)
	{
		unsigned lineCount = currLineNum;
		unsigned startRect = 0;
		unsigned nextStart = 0;
		currLineNum = 0;

		do
		{
			// Width of the current line.
			float lineWidth = 0;

			// Rect to start at for the next line.
			startRect = nextStart;

			// Get width of line.
			for (unsigned i = startRect; i < (unsigned)charRects.size(); i++)
			{
				// End of the line.
				if (charRects[i].Line != currLineNum)
				{
					nextStart = i;
					break;
				}
				lineWidth += charRects[i].Width;
			}

			// Ran out of characters!
			if (nextStart == startRect)
				nextStart = (unsigned)charRects.size();

			// How much to shift the line by.
			float shift = 0.0f;

			// Default Shifting.
			//*
			// Center alignment
			if (data->HorizontalAlign == align::Center)
				shift = (data->Width - lineWidth) * 0.5f;
			// Right alignment
			else shift = data->Width - lineWidth;
			//*/

			// Shift the text over by the specified amount.
			for (unsigned i = startRect; i < nextStart; i++)
			{
				charRects[i].Vertices[0].Position[0] += shift;
				charRects[i].Vertices[1].Position[0] += shift;
				charRects[i].Vertices[2].Position[0] += shift;
				charRects[i].Vertices[3].Position[0] += shift;
			}

			// Next line!
			currLineNum++;
		} while (currLineNum < lineCount);
	}

	// Non-top alignment is expensive!
	if (data->VerticalAlign != align::Top)
	{
		unsigned lineCount = currLineNum;
		float textHeight = data->LineHeight * (float)lineCount;

		float shift = 0.0f;
		// Center Alignment
		if (data->VerticalAlign == align::Center)
			shift = (data->Height - textHeight) * 0.5f;
		// Bottom Alignment
		else shift = data->Height - textHeight;

		for (unsigned i = 0; i < (unsigned)charRects.size(); i++)
		{
			charRects[i].Vertices[0].Position[1] += shift;
			charRects[i].Vertices[1].Position[1] += shift;
			charRects[i].Vertices[2].Position[1] += shift;
			charRects[i].Vertices[3].Position[1] += shift;
		}
	}

	// Generate Vertex List.
	data->VertexList = new(std::nothrow) char_vertex[data->VertexCount];
	if (data->VertexList)
	{
		for (unsigned i = 0, v = 0; i < (unsigned)charRects.size(); i++, v += 4)
			memcpy(&data->VertexList[v], charRects[i].Vertices, sizeof(char_vertex) * 4);
	}
}

bool UIFont::_GetLine(wchar_t *line, unsigned *strLength, unsigned *lineLength, wchar_t **nextLine)
{
	// All parameters are required.
	if (!line || !strLength || !*strLength || !lineLength) return false;
	unsigned startLength = *strLength;

	// Go until newline character.
	unsigned lineLen = 0;
	for (unsigned i = 0; i < startLength; i++)
	{
		// Length includes new line sequence.
		lineLen++;

		// Found a newline sequence.
		if ((line[i] == L'\r' && line[i + 1] != L'\n') || line[i] == L'\n')
			break;
	}

	// Output the remaining string length
	*strLength = startLength - lineLen;

	// We want a next line if available.
	if (nextLine)
	{
		// There is no next line.
		if (lineLen == startLength) *nextLine = nullptr;
		// There is a next line.
		else *nextLine = line + lineLen;
	}

	// Output the length of the line.
	*lineLength = lineLen;
	return true;
}

wchar_t *UIFont::_TruncLine(wchar_t *line, unsigned maxLength, unsigned *lineLength)
{
	// All parameters required.
	if (!line || !maxLength || !lineLength) return nullptr;

	// Remove all leading whitespace.
	wchar_t *result = line;
	while (*result < L' ' && maxLength > 0)
	{
		result++;
		maxLength--;
	}
	*lineLength = maxLength;

	// This should not have happened (line has zero length).
	if (maxLength == 0) return nullptr;

	// Remove all trailing whitespace.
	wchar_t *lChar = result + (maxLength - 1);
	while (*lChar < L' ')
	{
		lChar--;
		maxLength--;
	}
	*lineLength = maxLength;
	return result;
}

bool UIFont::_GenerateCharRect(wchar_t curr, char_rect *rect, int *x, int y)
{
	// No printable character or invalid parameter.
	if (curr < L' ' || !rect) return false;

	char_data *charData = &CharData[curr];
	rect->Width = (float)charData->CharWidth;

	int cursorX = *x;
	int cursorY = y;

	int left = cursorX - ((charData->RectWidth - charData->CharWidth) >> 1);
	int right = left + charData->RectWidth;
	int top = cursorY - ((RectHeight - CharHeight) >> 1);
	int bottom = top + RectHeight;


	// Top Left
	rect->Vertices[0].Position[0] = (float)left;
	rect->Vertices[0].Position[1] = (float)top;
	rect->Vertices[0].Color[0] = 1.0f;
	rect->Vertices[0].Color[1] = 1.0f;
	rect->Vertices[0].Color[2] = 1.0f;
	rect->Vertices[0].Color[3] = 1.0f;
	rect->Vertices[0].TexCoord[0] = charData->ULeft;
	rect->Vertices[0].TexCoord[1] = charData->VTop;

	// Top Right
	rect->Vertices[1].Position[0] = (float)right;
	rect->Vertices[1].Position[1] = (float)top;
	rect->Vertices[1].Color[0] = 1.0f;
	rect->Vertices[1].Color[1] = 1.0f;
	rect->Vertices[1].Color[2] = 1.0f;
	rect->Vertices[1].Color[3] = 1.0f;
	rect->Vertices[1].TexCoord[0] = charData->URight;
	rect->Vertices[1].TexCoord[1] = charData->VTop;

	// Bottom Right
	rect->Vertices[2].Position[0] = (float)right;
	rect->Vertices[2].Position[1] = (float)bottom;
	rect->Vertices[2].Color[0] = 1.0f;
	rect->Vertices[2].Color[1] = 1.0f;
	rect->Vertices[2].Color[2] = 1.0f;
	rect->Vertices[2].Color[3] = 1.0f;
	rect->Vertices[2].TexCoord[0] = charData->URight;
	rect->Vertices[2].TexCoord[1] = charData->VBottom;

	// Bottom Left
	rect->Vertices[3].Position[0] = (float)left;
	rect->Vertices[3].Position[1] = (float)bottom;
	rect->Vertices[3].Color[0] = 1.0f;
	rect->Vertices[3].Color[1] = 1.0f;
	rect->Vertices[3].Color[2] = 1.0f;
	rect->Vertices[3].Color[3] = 1.0f;
	rect->Vertices[3].TexCoord[0] = charData->ULeft;
	rect->Vertices[3].TexCoord[1] = charData->VBottom;

	*x += charData->CharWidth;
	return true;
}

bool UIFont::_GenerateCharRect(wchar_t curr, char_rect *rect, float *x, float y, float height)
{
	// No printable character or invalid parameter.
	if (curr < L' ' || !rect || height == 0.0f) return false;

	char_data *charData = &CharData[curr];

	float charWidth = height * ((float)charData->CharWidth / (float)CharHeight);
	float rectWidth = charWidth * ((float)charData->RectWidth / (float)charData->CharWidth);

	float charHeight = height;
	float rectHeight = charHeight * ((float)RectHeight / (float)CharHeight);

	rect->Width = charWidth;

	float cursorX = *x;
	float cursorY = y;

	float left = cursorX - ((rectWidth - charWidth) * 0.5f);
	float right = left + rectWidth;
	float top = cursorY - ((rectHeight - charHeight) * 0.5f);
	float bottom = top + rectHeight;

	// Top Left
	rect->Vertices[0].Position[0] = left;
	rect->Vertices[0].Position[1] = top;
	rect->Vertices[0].Color[0] = 1.0f;
	rect->Vertices[0].Color[1] = 1.0f;
	rect->Vertices[0].Color[2] = 1.0f;
	rect->Vertices[0].Color[3] = 1.0f;
	rect->Vertices[0].TexCoord[0] = charData->ULeft;
	rect->Vertices[0].TexCoord[1] = charData->VTop;

	// Top Right
	rect->Vertices[1].Position[0] = right;
	rect->Vertices[1].Position[1] = top;
	rect->Vertices[1].Color[0] = 1.0f;
	rect->Vertices[1].Color[1] = 1.0f;
	rect->Vertices[1].Color[2] = 1.0f;
	rect->Vertices[1].Color[3] = 1.0f;
	rect->Vertices[1].TexCoord[0] = charData->URight;
	rect->Vertices[1].TexCoord[1] = charData->VTop;

	// Bottom Right
	rect->Vertices[2].Position[0] = right;
	rect->Vertices[2].Position[1] = bottom;
	rect->Vertices[2].Color[0] = 1.0f;
	rect->Vertices[2].Color[1] = 1.0f;
	rect->Vertices[2].Color[2] = 1.0f;
	rect->Vertices[2].Color[3] = 1.0f;
	rect->Vertices[2].TexCoord[0] = charData->URight;
	rect->Vertices[2].TexCoord[1] = charData->VBottom;

	// Bottom Left
	rect->Vertices[3].Position[0] = left;
	rect->Vertices[3].Position[1] = bottom;
	rect->Vertices[3].Color[0] = 1.0f;
	rect->Vertices[3].Color[1] = 1.0f;
	rect->Vertices[3].Color[2] = 1.0f;
	rect->Vertices[3].Color[3] = 1.0f;
	rect->Vertices[3].TexCoord[0] = charData->ULeft;
	rect->Vertices[3].TexCoord[1] = charData->VBottom;

	*x += charWidth;
	return true;
}

// Pixel-Perfect
int UIFont::_GetWordWidth(wchar_t *start, unsigned wordLength)
{
	// All parameters required.
	if (!start || !wordLength || !*start) return 0;

	// The width of the text from the current position to the next word.
	int result = 0;

	// Find the first non whitespace character.
	while (*start <= L' ')
	{
		result += CharData[*start].CharWidth;
		start++;
		wordLength--;
	}

	// Go to the first whitespace character.
	while (*start > L' ' && wordLength > 0)
	{
		result += CharData[*start].CharWidth;
		start++;
		wordLength--;
	}

	// We're done here.
	return result;
}

// Screen Ratio
float UIFont::_GetWordWidth(wchar_t *start, unsigned wordLength, float height)
{
	// All parameters required.
	if (!start || !wordLength || !*start || height == 0.0f) return 0.0f;

	// The width of the text from the current position to the next word.
	float result = 0.0f;

	// Find the first non whitespace character.
	while (*start <= L' ' && *start != L'\0')
	{
		result += height * ((float)CharData[*start].CharWidth / (float)CharHeight);
		start++;
		wordLength--;
	}

	// Go to the first whitespace character.
	while (*start > L' ' && *start != L'\0' && wordLength > 0)
	{
		result += height * ((float)CharData[*start].CharWidth / (float)CharHeight);
		start++;
		wordLength--;
	}

	// We're done here.
	return result;
}

bool UIFont::_GetWord(wchar_t *word, unsigned *lineLength, unsigned *wordLength, wchar_t **nextWord)
{
	// All parameters are required.
	if (!word || !lineLength || !*lineLength || !wordLength) return false;
	unsigned startLength = *lineLength;

	// The length of the word (including whitespace).
	unsigned wordLen = 0;

	// Go until the first non-whitespace character.
	while (*word <= L' ' && startLength > 0)
	{
		word++;
		wordLen++;
		startLength--;
	}
	*lineLength = startLength;
	*wordLength = wordLen;

	// Go until a whitespace character.
	while (*word > L' ' && startLength > 0)
	{
		word++;
		wordLen++;
		startLength--;
	}
	*lineLength = startLength;
	*wordLength = wordLen;

	// Next word param provided.
	if (nextWord)
	{
		// Length ran out.
		if (startLength == 0) *nextWord = nullptr;
		// Output the start of the next word (the first whitespace character that we find).
		else *nextWord = word;
	}

	// Got it.
	return true;
}

wchar_t *UIFont::_TruncWord(wchar_t *word, unsigned *wordLength)
{
	// All parameters required.
	if (!word || !wordLength) return nullptr;
	unsigned wordLen = *wordLength;

	wchar_t *result = word;
	while (*result <= L' ' && wordLen > 0)
	{
		result++;
		wordLen--;
	}
	*wordLength = wordLen;
	return result;
}

}}