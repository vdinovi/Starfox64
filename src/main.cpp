/* ZJ Wood CPE 471 Lab 3 base code - I. Dunn class re-write */
#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <map>
#include <cstdlib>
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Shape.h"
#include "Arwing.h"
#include "Environment.h"
#include "common.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"


#define TURN_RATE 10
#define CAMERA_MOVE_SPEED 0.75
#define CAMERA_ORIGIN_Y -(AIRSPACE_HEIGHT/3)
#define CAMERA_ORIGIN_Z -6.0
#define CAMERA_FOLLOW 0.6

#define PI 3.14159

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
	std::map<std::string, std::shared_ptr<Program>> programs;
	std::map<std::string, std::vector<std::shared_ptr<Shape>>> shapes;
	std::map<std::string, GLuint> textures;

	// Camera
	bool cameraUnlock = false;
	bool mouseDown = false;
	std::vector<double> initialClick = { 0.0, 0.0 };
	glm::vec3 cameraPos = glm::vec3(0.0, CAMERA_ORIGIN_Y, CAMERA_ORIGIN_Z);
	glm::vec2 lookDir = glm::vec2(glm::radians(90.0), 0.0);
	glm::vec2 newLook = lookDir;

	// Arwing
	std::shared_ptr<Arwing> arwing;
	// Environment
	std::shared_ptr<Environment> environment;

	// Light
	glm::vec3 lightPos = {1.0, 0.0, 0.0};



	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_U && action == GLFW_PRESS) {
			if (cameraUnlock) {
				cameraUnlock = false;
				cameraPos = glm::vec3(0.0, CAMERA_ORIGIN_Y, CAMERA_ORIGIN_Z);
				lookDir = newLook = glm::vec2(glm::radians(90.0), 0.0);
			}
			else {
				cameraUnlock = true;
			}
		}

		// Camera controls
		if (cameraUnlock) {
			if(key == GLFW_KEY_W) {
				cameraPos += glm::vec3(
					CAMERA_MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					CAMERA_MOVE_SPEED*sin(lookDir[1]),
					CAMERA_MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
			}
        	else if(key == GLFW_KEY_S) {
				cameraPos -= glm::vec3(
					CAMERA_MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					CAMERA_MOVE_SPEED*sin(lookDir[1]),
					CAMERA_MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
			}
        	else if( key == GLFW_KEY_A) {
				glm::vec3 scaledLook = glm::vec3(
					CAMERA_MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					CAMERA_MOVE_SPEED*sin(lookDir[1]),
					CAMERA_MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
				cameraPos -= glm::cross(scaledLook, glm::vec3(0, 1, 0));
			}
       		else if( key == GLFW_KEY_D) {
				glm::vec3 scaledLook = glm::vec3(
					CAMERA_MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					CAMERA_MOVE_SPEED*sin(lookDir[1]),
					CAMERA_MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
				cameraPos += glm::cross(scaledLook, glm::vec3(0, 1, 0));
			}
		}

		// Arwing Controls
		if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			arwing->yawLeft(KEY_PRESS);
		}
		if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
			arwing->yawLeft(KEY_RELEASE);
		}
		if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			arwing->yawRight(KEY_PRESS);
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
			arwing->yawRight(KEY_RELEASE);
		}
		if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			arwing->pitchDown(KEY_PRESS);
		}
		if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
			arwing->pitchDown(KEY_RELEASE);
		}
		if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			arwing->pitchUp(KEY_PRESS);
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
			arwing->pitchUp(KEY_RELEASE);
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS) {
			glfwGetCursorPos(window, &(initialClick[0]), &(initialClick[1]));
			mouseDown = true;
		}
		if (action == GLFW_RELEASE) {
			lookDir = newLook;
			mouseDown = false;
		}
	}

	void scrollCallback(GLFWwindow *window, double dX, double dY)
	{
	}

	void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
	{
		if (cameraUnlock && mouseDown) {
        	int width, height;
        	glfwGetWindowSize(window, &width, &height);
			newLook[0] = lookDir[0] + glm::radians(-90 * (xpos - initialClick[0])/(width/2));
			newLook[1] = lookDir[1] + glm::radians(glm::clamp(80.0 * (ypos - initialClick[1])/(height/2), -80.0, 80.0));
		}
	}

	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	void initGeom(const std::string& resourceDir)
	{
		std::vector<tinyobj::shape_t> TOshapes;
		std::vector<tinyobj::material_t> TOmats;
		std::string err;
		std::string basePath = resourceDir + "/";

		arwing = std::make_shared<Arwing>(basePath);
		arwing->measure();

		environment = std::make_shared<Environment>(basePath);
		//environment->measure();
	}

	void initTex(const std::string& resourceDir)
	{
	}

	void initProg(const std::string& resourceDir)
	{
		GLSL::checkVersion();
		glClearColor(0.3f, 0.5f, 0.65f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glBlendColor(1,1,1,1);

		programs["texture"] = std::make_shared<Program>();
		programs["texture"]->setVerbose(true);
		programs["texture"]->setShaderNames(resourceDir + "/texture_vert_shader.glsl",
    										resourceDir + "/texture_frag_shader.glsl");
		programs["texture"]->init();
		programs["texture"]->addUniform("uTexture");
		programs["texture"]->addUniform("P");
		programs["texture"]->addUniform("V");
		programs["texture"]->addUniform("M");
		programs["texture"]->addUniform("lightPosition");
		programs["texture"]->addUniform("La");
		programs["texture"]->addUniform("Ld");
		programs["texture"]->addUniform("Ls");
		programs["texture"]->addUniform("Ka");
		programs["texture"]->addUniform("Kd");
		programs["texture"]->addUniform("Ks");
		programs["texture"]->addUniform("Shine");
		programs["texture"]->addUniform("texOffset");
		programs["texture"]->addAttribute("vertPos");
		programs["texture"]->addAttribute("vertNor");
		programs["texture"]->addAttribute("vertTex");

	}

	void render()
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto P = std::make_shared<MatrixStack>();
		auto M = std::make_shared<MatrixStack>();
		glm::mat4 V;
		if (cameraUnlock) {
			glm::vec3 lookV = glm::vec3(
				cos(newLook[1])*cos(newLook[0]),
				sin(newLook[1]),
				cos(newLook[1])*sin(newLook[0])
			);
			V = glm::lookAt(cameraPos, cameraPos + lookV , glm::vec3(0, 1, 0));
		}
		else {
			glm::vec3 newCameraPos = cameraPos + glm::vec3(0.5*arwing->position.x, 0.5*arwing->position.y, 0);
			glm::vec3 newCameraLook = glm::vec3(
				CAMERA_FOLLOW*cos(glm::radians(arwing->yaw-90.0)),
				CAMERA_FOLLOW*cos(glm::radians(arwing->pitch)),
				arwing->position.z
			);
			V = glm::lookAt(newCameraPos, newCameraPos + newCameraLook, glm::vec3(0, 1, 0));
		}


		P->pushMatrix();
		if (width > height) {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		else {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		M->loadIdentity();

		arwing->draw(programs["texture"], P, M, V, lightPos);
		arwing->advance();

		environment->draw(programs["texture"], P, M, V, lightPos);
		environment->advance();

		P->popMatrix();
	}

};


int main(int argc, char **argv)
{
	std::string shaderDir = "../shaders";
	std::string resourceDir = "../resources";

	Application *application = new Application();

	WindowManager * windowManager = new WindowManager();
	windowManager->init(1024, 768);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->initProg(shaderDir);
	application->initTex(resourceDir);
	application->initGeom(resourceDir);

	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		application->render();
		glfwSwapBuffers(windowManager->getHandle());
		glfwPollEvents();
	}

	windowManager->shutdown();
	return 0;
}
