#include<iostream>
#include "text.h"

class Test : public grafx::Window{
	private:
		grafx::Text* name;
        grafx::Text* copy;
        float deltaX, deltaY;
	public:
		Test() : Window("Text Test", 800, 480){}
		void init(){
             this->name = new grafx::Text(this, "Nobel Okelekekele");
             this->name->setFontSize(36);
             this->name->setColor({ 0.0, 0.0, 1.0, 1.0 });

             this->copy = new grafx::Text(this, "copy 2022 Bosoft Limited");
             this->copy->setFontSize(16);
             this->copy->setPosition(10, 15);

             deltaX = 80;
             deltaY = 80;
		}

		void render(){
			this->name->draw();
            this->copy->draw();
		}

		void resize(int width, int height){}
		void update(float deltaTime){
            this->name->setPosition(
                this->name->getPositionX() + deltaX * deltaTime,
                this->name->getPositionY() + deltaY * deltaTime);
            
            if(this->name->getPositionX() <= 0 || this->name->getPositionX() + this->name->getWidth() >= this->getWidth()){
                deltaX *= -1;
            }

            if(this->name->getPositionY() <= 0 || this->name->getPositionY() + this->name->getHeight() >= this->getHeight()){
                deltaY *= -1;
            }
        }

		void dispose(){}
};

int main(int argc,char **argv){
	Test test;
	grafx::run(argc, argv, &test);
	return 0;
}