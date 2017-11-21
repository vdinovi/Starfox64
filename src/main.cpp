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
#include "common.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"


#define TURN_RATE 10
#define MOVE_SPEED 0.75

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
	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, -5.0);
	glm::vec2 lookDir = glm::vec2(glm::radians(90.0), 0.0);
	glm::vec2 newLook = lookDir;

	// Arwing
	std::shared_ptr<Arwing> arwing;

	// Environment
	glm::vec3 lightPos = {1.0, 1.0, 1.0};
	glm::vec2 texOffset = {0.0, 0.0};



	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_U && action == GLFW_PRESS) {
			if (cameraUnlock) {
				cameraUnlock = false;
				cameraPos = glm::vec3(0.0, 0.0, -5.0);
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
					MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					MOVE_SPEED*sin(lookDir[1]),
					MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
			}
        	else if(key == GLFW_KEY_S) {
				cameraPos -= glm::vec3(
					MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					MOVE_SPEED*sin(lookDir[1]),
					MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
			}
        	else if( key == GLFW_KEY_A) {
				glm::vec3 scaledLook = glm::vec3(
					MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					MOVE_SPEED*sin(lookDir[1]),
					MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
				cameraPos -= glm::cross(scaledLook, glm::vec3(0, 1, 0));
			}
       		else if( key == GLFW_KEY_D) {
				glm::vec3 scaledLook = glm::vec3(
					MOVE_SPEED*cos(lookDir[1])*cos(lookDir[0]),
					MOVE_SPEED*sin(lookDir[1]),
					MOVE_SPEED*cos(lookDir[1])*sin(lookDir[0])
				);
				cameraPos += glm::cross(scaledLook, glm::vec3(0, 1, 0));
			}
		}

		// Arwing Controls
		if (key == GLFW_KEY_LEFT) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				arwing->yawLeft(KEY_PRESS);
			}
			else {
				arwing->yawLeft(KEY_RELEASE);
			}
		}
		if (key == GLFW_KEY_RIGHT) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				arwing->yawRight(KEY_PRESS);
			}
			else {
				arwing->yawRight(KEY_RELEASE);
			}
		}
		if (key == GLFW_KEY_UP) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				arwing->pitchUp(KEY_PRESS);
			}
			else {
				arwing->pitchUp(KEY_RELEASE);
			}
		}
		if (key == GLFW_KEY_DOWN) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				arwing->pitchDown(KEY_PRESS);
			}
			else {
				arwing->pitchDown(KEY_RELEASE);
			}
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
        {
			glfwGetCursorPos(window, &(initialClick[0]), &(initialClick[1]));
            mouseDown = true;
        }
        if (action == GLFW_RELEASE)
        {
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

		// Load Quad
		bool rc = tinyobj::LoadObj(TOshapes, TOmats, err, (basePath + "quad.obj").c_str(), basePath.c_str());
		if (!rc) {
			std::cerr << err << std::endl;
		}
		else {
			std::string objName("quad");
			for (int i = 0; i < TOshapes.size(); ++i) {
				std::shared_ptr<Shape> shape = std::make_shared<Shape>();
				shape->createShape(TOshapes[0]);
				shape->measure();
				shape->init();
				if (TOshapes[i].mesh.material_ids[0] >= 0) {
					shape->loadMaterial(TOmats[TOshapes[i].mesh.material_ids[0]], basePath);
					shape->useMaterial = true;
				}
				else {
					shape->setMaterial(textures["white"], glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5),
									   glm::vec3(0.5, 0.5, 0.5), 1.0);
					shape->useMaterial = true;
				}
				shapes[objName].push_back(shape);
			}
		}
	}

	void initTex(const std::string& resourceDir)
	{
		// Load in plain white texture to use in absence of mtl file
		textures["white"] = 0;
		glGenTextures(1, &textures["white"]);
		glBindTexture(GL_TEXTURE_2D, textures["white"]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, channels;
		unsigned char * data = stbi_load((resourceDir + "/white.png").c_str(), &width, &height, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error loading plain white texture\n";
		}
	}

	void initProg(const std::string& resourceDir)
	{
		GLSL::checkVersion();
		glClearColor(0.3f, 0.5f, 0.65f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glBlendColor(1,1,1,1);

		programs["normal"] = std::make_shared<Program>();
		programs["normal"]->setVerbose(true);
		programs["normal"]->setShaderNames(resourceDir + "/vert_shader.glsl",
    		                       		   resourceDir + "/normal_frag_shader.glsl");
		programs["normal"]->init();
		programs["normal"]->addUniform("P");
		programs["normal"]->addUniform("V");
		programs["normal"]->addUniform("M");
		programs["normal"]->addAttribute("vertPos");
		programs["normal"]->addAttribute("vertNor");

		programs["material"] = std::make_shared<Program>();
		programs["material"]->setVerbose(true);
		programs["material"]->setShaderNames(resourceDir + "/phong_vert_shader.glsl",
    										 resourceDir + "/phong_frag_shader.glsl");
		programs["material"]->init();
		programs["material"]->addUniform("P");
		programs["material"]->addUniform("V");
		programs["material"]->addUniform("M");
		programs["material"]->addUniform("lightPos");
		//programs["material"]->addUniform("lightDir");
		programs["material"]->addUniform("MatAmb");
		programs["material"]->addUniform("MatDif");
		programs["material"]->addUniform("MatSpec");
		programs["material"]->addUniform("shine");
		programs["material"]->addAttribute("vertPos");
		programs["material"]->addAttribute("vertNor");

		programs["texture"] = std::make_shared<Program>();
		programs["texture"]->setVerbose(true);
		programs["texture"]->setShaderNames(resourceDir + "/texture_vert_shader.glsl",
    										resourceDir + "/texture_frag_shader.glsl");
		programs["texture"]->init();
		programs["texture"]->addUniform("uTexture");
		programs["texture"]->addUniform("P");
		programs["texture"]->addUniform("V");
		programs["texture"]->addUniform("M");
		programs["texture"]->addUniform("lightPos");
		//programs["texture"]->addUniform("lightDir");
		programs["texture"]->addUniform("MatAmb");
		programs["texture"]->addUniform("MatDif");
		programs["texture"]->addUniform("MatSpec");
		programs["texture"]->addUniform("Shine");
		programs["texture"]->addUniform("texOffset");
		programs["texture"]->addAttribute("vertPos");
		programs["texture"]->addAttribute("vertNor");
		programs["texture"]->addAttribute("vertTex");

		programs["gblur"] = std::make_shared<Program>();
		programs["gblur"]->setVerbose(true);
		programs["gblur"]->setShaderNames(resourceDir + "/gblur_vert_shader.glsl",
    							  		  resourceDir + "/gblur_frag_shader.glsl");
		programs["gblur"]->init();
		programs["gblur"]->addUniform("uTexture");
		programs["gblur"]->addUniform("P");
		programs["gblur"]->addUniform("V");
		programs["gblur"]->addUniform("M");
		programs["gblur"]->addAttribute("vertPos");
		programs["gblur"]->addAttribute("vertNor");
		programs["gblur"]->addAttribute("vertTex");
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
		glm::vec3 lookV = glm::vec3(
			cos(newLook[1])*cos(newLook[0]),
			sin(newLook[1]),
			cos(newLook[1])*sin(newLook[0])
		);
		glm::mat4 V = glm::lookAt(cameraPos, cameraPos + lookV , glm::vec3(0, 1, 0));

		P->pushMatrix();
		if (width > height) {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		else {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		M->loadIdentity();

		arwing->draw(programs["texture"], P, M, V, lightPos, texOffset);


		/*
		programs["texture"]->bind();
		M->pushMatrix();
			M->translate(glm::vec3(0, -5, 2));
			M->scale(glm::vec3(20, 20, 20));
            M->rotate(glm::radians(80.0), glm::vec3(1, 0, 0));
			glUniformMatrix4fv(programs["texture"]->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(programs["texture"]->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			glUniformMatrix4fv(programs["texture"]->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glUniform3f(programs["texture"]->getUniform("lightPos"), 5.0, 0.0, 0.0);
			glUniform2f(programs["texture"]->getUniform("texOffset"), 0.0, texOffset);
			shapes["quad"][0]->draw(programs["texture"]);
		M->popMatrix();
		programs["texture"]->unbind();

		texOffset.y += 0.003;
		*/

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
