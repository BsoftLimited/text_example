#ifndef SHADER_H
	#define SHADER_H

#include "window.h"
#include<iostream>
#include <fstream>
#include <sstream>  

namespace grafx{
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
