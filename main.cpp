#include <map>
#include <string>

#include "shader.h"

unsigned int VAO, VBO;
std::map<GLchar, grafx::Character> characters;
grafx::Shader* shader;

grafx::Vector2 getSize(std::string text, float scale){
    grafx::Vector2 size = {0};
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        grafx::Character ch = characters[*c];
        size.x += ch.Size.x * scale;
        size.y = ch.Size.y * scale;
    }
    return size;
}

void RenderText(std::string text, float x, float y, float scale){
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    shader->bind();

    float matrix[16];
    grafx::OrthogonalMatrix(matrix, 0.0f, 480.0f, 0.0f, 800.0f);
    GLint projection = glGetUniformLocation(shader->getProgram(), "projection");
    glUniformMatrix4fv(projection, 1, GL_FALSE, &matrix[0]);

    GLint color = glGetUniformLocation(shader->getProgram(), "textColor");
    glUniform3f(color, 0.0f, 0.0f, 1.0f);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        grafx::Character ch = characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

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
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        //x += ch.Size.x;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void init(){
    shader = new grafx::Shader("text.vs", "text.fs");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    shader->bind();
    
    if(grafx::loatFont(&characters, "./regular.ttf") == -1){
        std::cout<<"font loading failed"<<std::endl;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void display(){
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    std::string name = "Nobel Okelekekele";
    std::string copy = "(C) Bsoft Limited.com";
    RenderText( name, -0.96f, 9.6f, 0.0005f);
    RenderText( copy , -0.96f, -0.9f, 0.0005f);

	glutSwapBuffers();
}

void changeSize(int width, int height){
	glViewport(0, 0, width, height);
}

int main(int argc, char **argv){
    glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion (3, 1);
    glutInitContextFlags (GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitContextProfile(GLUT_FORWARD_COMPATIBLE);
	glutInitWindowSize(800, 480);
	glutCreateWindow("Text Test");
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	
	if (GLEW_OK != err){
		std::cout<<glewGetErrorString(err)<<std::endl;
	} else {
		glutDisplayFunc(display);
		glutIdleFunc(display);
		glutReshapeFunc(changeSize);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        init();
		glutMainLoop();
	}
}