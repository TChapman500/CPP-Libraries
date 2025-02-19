#pragma once
#include "../Graphics/Textures.h"
#include "../Graphics/States.h"
#include <string>

namespace TChapman500 {
namespace GUI {

class UIFont
{
public:
	struct char_vertex
	{
		float Position[2];
		float Color[4];
		float TexCoord[2];
	};

	struct char_data
	{
		int CharWidth;
		int RectWidth;
		float ULeft;
		float VTop;
		float URight;
		float VBottom;
	};

	struct word_data
	{
		unsigned CharCount;
		unsigned TotalWidth;	// Width excluding the space.
		unsigned CharsToNext;	// Spaces between this word and the next.
		char *Start;
	};

	struct char_quad
	{
		char_vertex *Vertex[4];
		unsigned *Index[6];
	};

	struct gen_char
	{
		char CharCode;
		unsigned Width;
		char_quad Quad;
	};

	struct gen_line
	{
		unsigned CharCount;
		unsigned Width;
		gen_char *CharList;
	};

	enum class align
	{
		Left = 0,
		Top = 0,
		Center,
		Right = 2,
		Bottom = 2
	};

	struct gen_data
	{
		// Inputs
		float Width;
		float Height;
		float LineHeight;
		align HorizontalAlign;
		align VerticalAlign;

		// Outputs
		unsigned VertexCount;
		unsigned IndexCount;
		char_vertex *VertexList;
		unsigned *IndexList;
	};


	UIFont(std::wstring &path, bool pointFilter = false);
	~UIFont();

	void GenerateVertices(wchar_t *str, gen_data *data);


	std::shared_ptr<Graphics::ShaderResourceView> Texture;
	std::shared_ptr<Graphics::Sampler> Sampler;

	std::wstring Path;

	int CharHeight = 0;
	int RectHeight = 0;
	int FixedCharWidth = 0;

	bool TabWidthPixels = false;
	int TabWidth = 0;

	char_data CharData[256];


private:
	// NEW FUNCTIONS
	bool _GetLine(wchar_t *line, unsigned *strLength, unsigned *lineLength, wchar_t **nextLine = nullptr);
	wchar_t *_TruncLine(wchar_t *line, unsigned maxLength, unsigned *lineLength);

	unsigned _GetWordLength(wchar_t *start, unsigned lineLength);

	int _GetWordWidth(wchar_t *start, unsigned wordLength);		// Pixel-Perfect
	float _GetWordWidth(wchar_t *start, unsigned wordLength, float height);	// Screen Ratio

	bool _GetWord(wchar_t *word, unsigned *lineLength, unsigned *wordLength, wchar_t **nextWord = nullptr);
	wchar_t *_TruncWord(wchar_t *word, unsigned *wordLength);

	bool _AdvanceCursor(unsigned *x, unsigned *y, unsigned *lineNum, wchar_t currChar, gen_data *data);
	bool _AdvanceCursor(float *x, float *y, unsigned *lineNum, wchar_t currChar, gen_data *data);

	struct char_rect
	{
		unsigned Line;
		float Width;
		char_vertex Vertices[4];
	};

	bool _GenerateCharRect(wchar_t curr, char_rect *rect, int *x, int y);		// Pixel-Perfect
	bool _GenerateCharRect(wchar_t curr, char_rect *rect, float *x, float y, float height);	// Screen Ratio

};

}}