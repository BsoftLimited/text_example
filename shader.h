#ifndef SHADER_H
	#define SHADER_H

#include<iostream>
#include <fstream>
#include <sstream>  
#include <GL/glew.h>
#include<GL/freeglut.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace grafx{
    struct Vector2{ int x; int y; };

    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        Vector2   Size;      // Size of glyph
        Vector2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

	class Shader{
		private:
			GLuint program;
			GLuint BuildShader(const char *source, GLenum shaderType);
		public:
            Shader(const char* vertexShader, const char* fragmentShader);
			GLuint getAttributeLocation(const char* name);
			void setUniformMatrix(const char* name, float* matrix);
            GLuint getProgram();
            void bind();
			void dispose();
			~Shader();
	};

    int loatFont(std::map<GLchar, Character> *Characters, std::string path){
        FT_Library ft;
        if (FT_Init_FreeType(&ft)){
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return -1;
        }

        // find path to font
        if (path.empty()){
            std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
            return -1;
        }
        
        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, path.c_str(), 0, &face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return -1;
        }

        FT_Set_Pixel_Sizes(face, 0, 128);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++){
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,  GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            grafx::Character character = {
                texture,
                (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows,
                face->glyph->bitmap_left, face->glyph->bitmap_top,
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters->insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return 0;
    }

    void OrthogonalMatrix(float matrix[], float top, float bottom, float left, float right){
        for(int i = 0; i < 16; i++){ matrix[i] = 0; }
        matrix[0] = 2.0f/(right - left);      matrix[3] =  -(right + left) / (right - left);
        matrix[5] = 2.0f/(top - bottom);      matrix[7] = -(top + bottom) / (top - bottom);
        matrix[10] = -2.0f/(100.0f - 0.1f);  matrix[11] = -(100.0f + 0.1f) / (100.0f - 0.1f);
        matrix[15] = 1.0f;
    }
}

GLuint grafx::Shader::BuildShader(const char *source, GLenum shaderType){
	GLuint shaderHandle = glCreateShader(shaderType);
	glShaderSource(shaderHandle, 1, &source, 0);
	glCompileShader(shaderHandle);
	GLint compileSuccess;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);

	if (compileSuccess == GL_FALSE){
		GLchar messages[256];
		glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
		std::cout<<"compile glsl error : "<<messages<<std::endl;
		std::cout<<source<<std::endl;
	}
	return shaderHandle;
}

grafx::Shader::Shader(const char* vertexShader, const char* fragmentShader){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexShader);
        fShaderFile.open(fragmentShader);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }catch(std::ifstream::failure e){
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();


	GLuint vertex = BuildShader(vShaderCode, GL_VERTEX_SHADER);
	GLuint fragment = BuildShader(fShaderCode, GL_FRAGMENT_SHADER);

	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);

	GLint linkSuccess;
	glGetProgramiv(this->program, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess == GL_FALSE){
		GLchar messages[256];
		glGetProgramInfoLog(this->program, sizeof(messages), 0, &messages[0]);
		std::cout<<"linking glsl error : "<<messages<<std::endl;
	}
}

grafx::Shader::~Shader(){
	this->dispose();
}

void grafx::Shader::bind(){
	glUseProgram(this->program);
}

void grafx::Shader::dispose(){
	glDeleteProgram(this->program);
	this->program = 0;
}

GLuint grafx::Shader::getProgram(){
	return this->program;
}

GLuint grafx::Shader::getAttributeLocation(const char* name){
	return glGetAttribLocation(this->program, name);
}

#endif
