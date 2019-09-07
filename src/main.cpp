#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Material.h"
#include "Light.h"

using namespace std;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from


shared_ptr<Camera> camera;
shared_ptr<Program> prog;
shared_ptr<Program> prog2;
shared_ptr<Shape> shape;

// Declaration/Intialization for Materials
Material M1(0.2, 0.2, 0.2, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 200.0);
Material M2(0.0, 0.0, 0.0, 0.5, 0.5, 0.7, 0.1, 0.1, 0.1, 25.0);
Material M3(0.2, 0.2, 0.2, 0.8, 0.7, 0.7, 1.0, 0.9, 0.8, 200.0);
vector<Material> MaterialList = { M1, M2, M3 };

// Declaration/Intialization for Lights
Light l1(1.0, 1.0, 1.0, 0.8);
Light l2(-1.0, 1.0, 1.0, 0.2);
vector<Light> LightList = { l1, l2 };

int MPos = 0;
int LPos = 0;
int LPos2 = 1;
bool Phong = true;
bool keyToggles[256] = {false}; // only for English keyboards!

// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;		// sets to true if shift key is being pressed
		bool ctrl  = (mods & GLFW_MOD_CONTROL) != 0;	// sets to true if ctrl key is being pressed
		bool alt   = (mods & GLFW_MOD_ALT) != 0;		// sets to true if alt key is being pressed
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if(state == GLFW_PRESS) {
		camera->mouseMoved((float)xmouse, (float)ymouse);
	}
}

// This function is called when a key is pushed
static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	if (key == 'm')
	{
		if (MPos < 2)
		{
			MPos++;
		}
	}
	else if (key == 'M')
	{
		if (MPos > 0)
		{
			MPos--;
		}
	}
	else if (key == 'l')
	{
		if (LPos == 0)
		{
			LPos = 1;
			LPos2 = 0;
		}
	}
	else if (key == 'L')
	{
		if (LPos == 1)
		{
			LPos = 0;
			LPos2 = 1;
		}
	}
	else if (key == 'x')
	{
		LightList.at(LPos).setXPos(-0.01);
	}
	else if (key == 'X')
	{
		LightList.at(LPos).setXPos(0.01);
	}
	else if (key == 'y')
	{
		LightList.at(LPos).setYPos(-0.01);
	}
	else if (key == 'Y')
	{
		LightList.at(LPos).setYPos(0.01);
	}
	else if (key == 's')
	{
		Phong = true;
	}
	else if (key == 'S')
	{
		Phong = false;
	}
	else
	{
		//do nothing
	}
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "frag.glsl");
	prog->setVerbose(true);
	prog->init();
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->addUniform("MV");
	prog->addUniform("P");
	prog->addUniform("lightPos1");
	prog->addUniform("lightPos2");
	prog->addUniform("ka");
	prog->addUniform("kd");
	prog->addUniform("ks");
	prog->addUniform("s");
	prog->addUniform("i1");
	prog->addUniform("i2");
	prog->setVerbose(false);

	prog2 = make_shared<Program>();
	prog2->setShaderNames(RESOURCE_DIR + "vert2.glsl", RESOURCE_DIR + "frag2.glsl");
	prog2->setVerbose(true);
	prog2->init();
	prog2->addAttribute("aPos");
	prog2->addAttribute("aNor");
	prog2->addUniform("MV");
	prog2->addUniform("P");
	prog2->setVerbose(false);

	camera = make_shared<Camera>();
	camera->setInitDistance(2.0f);
	
	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "bunny.obj");
	shape->fitToUnitBox();
	shape->init();
	
	GLSL::checkError(GET_FILE_LINE);

}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	/*
	if(keyToggles[(unsigned)'l']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	*/
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	
	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	camera->applyViewMatrix(MV);
	if (Phong)
	{
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		// light position
		glUniform3f(prog->getUniform("lightPos1"), LightList.at(LPos).getLightPosition().x, 
			LightList.at(LPos).getLightPosition().y, LightList.at(LPos).getLightPosition().z);
		glUniform3f(prog->getUniform("lightPos2"), LightList.at(LPos2).getLightPosition().x, 
			LightList.at(LPos2).getLightPosition().y, LightList.at(LPos2).getLightPosition().z);
		// ambient component
		glUniform3f(prog->getUniform("ka"), MaterialList.at(MPos).getAmbient().x,
			MaterialList.at(MPos).getAmbient().y, MaterialList.at(MPos).getAmbient().z);
		// diffuse component
		glUniform3f(prog->getUniform("kd"), MaterialList.at(MPos).getDiffuse().x,
			MaterialList.at(MPos).getDiffuse().y, MaterialList.at(MPos).getDiffuse().z);
		// specular component
		glUniform3f(prog->getUniform("ks"), MaterialList.at(MPos).getSpecular().x,
			MaterialList.at(MPos).getSpecular().y, MaterialList.at(MPos).getSpecular().z);
		// shininess factor
		glUniform1f(prog->getUniform("s"), MaterialList.at(MPos).getShiny());
		// light intensity factor
		glUniform1f(prog->getUniform("i1"), LightList.at(LPos).getIntensity());
		glUniform1f(prog->getUniform("i2"), LightList.at(LPos2).getIntensity());
		shape->draw(prog);
		prog->unbind();
	}
	else
	{
		prog2->bind();
		glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		shape->draw(prog2);
		prog2->unbind();
	}
	
	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "YOUR NAME", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
