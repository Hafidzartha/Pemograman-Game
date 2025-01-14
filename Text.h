#ifndef TEXT_H
#define TEXT_H

#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>
#include <iostream>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include "Shader.h"

using namespace glm;
using namespace std;

namespace Engine {
	struct TextCharacter {
		GLuint TextureID; // ID handle of the glyph texture
		ivec2 Size; // Size of glyph
		ivec2 Bearing; // Offset from baseline to left/top of glyph
		GLuint Advance; // Offset to advance to next glyph
	};

	class Text
	{
	public:
		Text(const char* fontName, uint fontSize, Shader* shader);
		Text* SetScale(float scale);
		Text* SetColor(float r, float g, float b);
		Text* SetPosition(float x, float y);
		Text* SetText(string text);
		void Draw();
		uint GetFontSize();
		float GetScale();
		float GetTextWidth() const;
	private:
		void Init();
		map<GLchar, TextCharacter> TextCharacters;
		uint fontSize = 12;
		const char* fontName = NULL;
		GLuint vbo, ebo, vao;
		Shader* shader;
		vec3 color = vec3(1.0f, 1.0f, 1.0f);
		float scale = 1.0f;
		vec2 position = vec2(0, 0);
		string text = "";
	};

}

#endif

