#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
using namespace std;

class Light
{
public:
Light::Light(float x, float y, float z, float i)
	:LightPos(x,y,z)
{
	LightIntensity = i;
}
~Light()
{
}
// Setter Functions
void setXPos(float x) 
{ 
	/*
	glm::vec3 &XPos = LightPos;
	XPos += glm::vec3(x, 0.0f, 0.0f);
	*/
	LightPos.x += x;
}
void setYPos(float y) 
{ 
	glm::vec3 &YPos = LightPos;
	YPos += glm::vec3(0.0f, y, 0.0f);
}
// Getter Functions
glm::vec3 getLightPosition() { return LightPos; }
float getIntensity() { return LightIntensity; }

private:
glm::vec3 LightPos;
float LightIntensity;
};