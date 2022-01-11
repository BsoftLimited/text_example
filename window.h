#ifndef WINDOW_H
	#define WINDOW_H
	
#include "utils.h"
#include <GL/glew.h>
#include<GL/freeglut.h>

namespace grafx{
	class Window{
		private:
			char* title;
		public:
			int width, height;
			
			Window(char* title, int width, int height);
			virtual void init() = 0;
			virtual void render() = 0;
			virtual void update(float deltaTime) = 0;
			virtual void resize(int width, int height) = 0;
			int getWidth();
			int getHeight();
			char* getTitle();
			virtual void dispose() = 0;
	};
	
	void run(int argc, char **argv, Window* win);
}

grafx::Window::Window(char* title, int width, int height){
	this->title = title;
	this->width = width;
	this->height = height;
}

int grafx::Window::getWidth(){ return this->width; }
int grafx::Window::getHeight(){ return this->height; }
char* grafx::Window::getTitle(){ return this->title; }

grafx::Window* window;
int oldTimeSinceStart = 0;

void display(){
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	window->render();
	
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;
	window->update(deltaTime/1000.0);
	
	glutSwapBuffers();
}

void changeSize(int width, int height){
	glViewport(0, 0, width, height);
	window->width = width;
	window->height = height;
	window->resize(width, height);
}

void grafx::run(int argc, char **argv, Window* win){
	window = win;

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion (3, 1);
    glutInitContextFlags (GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitContextProfile(GLUT_FORWARD_COMPATIBLE);
	glutInitWindowSize(window->getWidth(), window->getHeight());
	glutCreateWindow(window->getTitle());
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	std::cout<<"\tUsing glew "<<glewGetString(GLEW_VERSION)<<std::endl;
	std::cout<<"\tVendor: "<<glGetString (GL_VENDOR)<<std::endl;
	std::cout<<"\tRenderer: "<<glGetString (GL_RENDERER)<<std::endl;
	std::cout<<"\tVersion: "<<glGetString (GL_VERSION)<<std::endl;
	std::cout<<"\tGLSL: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;
	
	if (GLEW_OK != err){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	} else {
		glutDisplayFunc(display);
		glutIdleFunc(display);
		glutReshapeFunc(changeSize);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        win->init();
		glutMainLoop();
		win->dispose();
	}
	
}

#endif