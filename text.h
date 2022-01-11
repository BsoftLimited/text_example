#ifndef TEXT_H
	#define TEXT_H

#include "shader.h"
#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H


namespace grafx{
    std::map<GLchar, grafx::Character> characters;
    bool fondLoaded = false, shaderLoaded = false;
    Shader* textShader;
    class Text{
        private:
            std::string data;
            GLuint VAO, VBO;
            Color color;
            float fontSize, positionX, positionY;
            Window* window;
        public:
            Text(Window* window, std::string data);
            void setText(std::string data);
            void setFontSize(float size);
            void setPosition(float x, float y);
            void setColor(Color color);

            std::string getText(){ return this->data; }
            float getFontSize(){ return this->fontSize * 32.0f; }
            float getWidth();
            float getHeight();
            float getPositionX(){ return this->positionX; }
            float getPositionY(){ return this->positionY; }
            
            void draw();
    };

    int loatFont(std::map<GLchar, Character> *Characters, std::string path);
}

int grafx::loatFont(std::map<GLchar, Character> *Characters, std::string path){
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

    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 36);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 255; c++){
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

grafx::Text::Text(Window* window, std::string data){
    this->window = window;
    this->positionX = 0;
    this->positionY = 0;
    this->color = { 1.0f, 1.0f, 1.0f, 1.0f};
    this->fontSize = 1.0f;
    this->data = data;

    if(!shaderLoaded){
        textShader = new Shader("text.vs", "text.fs");
        grafx::shaderLoaded = true;
        std::cout<<"Shader loaded"<<std::endl;
    }

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);

    textShader->bind();
    
    if(!fondLoaded){
         if(grafx::loatFont(&grafx::characters, "./regular.ttf") == -1){
            std::cout<<"font loading failed"<<std::endl;
        }
        std::cout<<"Font loaded"<<std::endl;
        fondLoaded = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void grafx::Text::setText(std::string data){
    this->data = data;
}

void grafx::Text::setFontSize(float size){
    this->fontSize = size / 32.0f;
}

void grafx::Text::setPosition(float x, float y){
    this->positionX = x;
    this->positionY = y;
}

void grafx::Text::setColor(Color color){
    this->color = color;
}

float grafx::Text::getWidth(){
    std::string::const_iterator c;
    float width = 0;
    for (c = this->data.begin(); c != this->data.end(); c++) {
        grafx::Character ch = grafx::characters[*c];
        width += ch.Size.x;
    }
    return width * this->fontSize;
}

float grafx::Text::getHeight(){
    std::string::const_iterator c;
    float height = 0;
    for (c = this->data.begin(); c != this->data.end(); c++) {
        grafx::Character ch = grafx::characters[*c];
        return ch.Size.y * this->fontSize;
    }
    return height;
}

void grafx::Text::draw(){
    glBindVertexArray(this->VAO);
    glActiveTexture(GL_TEXTURE0);
    textShader->bind();

    float matrix[16];
    grafx::OrthogonalMatrix(matrix, this->window->getHeight(), 0.0f, 0.0f, this->window->getWidth());
    GLint projection = glGetUniformLocation(textShader->getProgram(), "projection");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &matrix[0]);

    GLint color = glGetUniformLocation(textShader->getProgram(), "textColor");
    glUniform3f(color, this->color.red, this->color.green, this->color.blue);

    float x = this->positionX, y = this->positionY;
    std::string::const_iterator c;
    for (c = this->data.begin(); c != this->data.end(); c++) {
        grafx::Character ch = grafx::characters[*c];
        

        float xpos = x + ch.Bearing.x * this->fontSize;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * this->fontSize;

        float w = ch.Size.x * this->fontSize;
        float h = ch.Size.y * this->fontSize;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };

        //std::cout<<ch.TextureID<<std::endl;
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * this->fontSize; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        //x += ch.Size.x;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif