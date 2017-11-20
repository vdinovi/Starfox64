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

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"


#define TURN_RATE 10

#define PI 3.14159

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
	std::map<std::string, std::shared_ptr<Program>> programs;
	std::map<std::string, std::vector<std::shared_ptr<Shape>>> shapes;
	std::map<std::string, GLuint> textures;



	glm::vec3 cameraPos = glm::vec3(0.f, 0.f, -5.f);
	glm::vec3 cameraLook = glm::vec3(0.f, 0.f, 1.f);

	// arwing
	glm::vec3 arwingPos = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 arwingVel = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 arwingAcc = glm::vec3(0.0, 0.0, 0.0);
	double pitch = 0;
	double yaw = 0;


	float texOffset = 0;



	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				yaw = glm::min(yaw + TURN_RATE, 40.0);
			}
			else {
				yaw = 0;
			}
			arwingAcc.x = glm::cos(yaw);
		}
		else if (key == GLFW_KEY_D) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				yaw = glm::max(yaw - TURN_RATE, -40.0);
			}
			else {
				yaw = 0;
			}
			arwingAcc.x = glm::cos(yaw);
		}
		else if (key == GLFW_KEY_W) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				pitch = glm::max(pitch - TURN_RATE, -40.0);
			}
			else {
				pitch = 0;
			}
			arwingAcc.y = glm::sin(pitch);
		}
		else if (key == GLFW_KEY_S) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				pitch = glm::min(pitch + TURN_RATE, 40.0);
			}
			else {
				pitch = 0;
			}
			arwingAcc.y = glm::sin(pitch);
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
	}

	void scrollCallback(GLFWwindow *window, double dX, double dY)
    {
    }

	void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
	{
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

		// Load Arwing
		bool rc = tinyobj::LoadObj(TOshapes, TOmats, err, (basePath + "arwing.obj").c_str(), basePath.c_str());
		if (!rc) {
			std::cerr << err << std::endl;
		}
		else {
			std::string objName("arwing");
			for (int i = 0; i < TOshapes.size(); ++i) {
				std::shared_ptr<Shape> shape = std::make_shared<Shape>();
				shape->createShape(TOshapes[i]);
				shape->measure();
				shape->init();
				shapes[objName].push_back(shape);
				if (TOshapes[i].mesh.material_ids[0] >= 0) {
					shape->loadMaterial(TOmats[TOshapes[i].mesh.material_ids[0]], basePath);
					shape->useMaterial = true;
				}
				else {
					shape->setMaterial(textures["white"], glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5),
									   glm::vec3(0.5, 0.5, 0.5), 1.0);
					shape->useMaterial = true;
				}
			}
		}

		// Load Quad
		rc = tinyobj::LoadObj(TOshapes, TOmats, err, (basePath + "quad.obj").c_str(), basePath.c_str());
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
		programs["material"]->addUniform("lightDir");
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
		programs["texture"]->addUniform("lightDir");
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
		glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraLook, glm::vec3(0, 1, 0));

		P->pushMatrix();
		if (width > height) {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		else {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		M->loadIdentity();


		glm::vec3 arwingMin = Shape::getMin(shapes["arwing"]);
		glm::vec3 arwingMax = Shape::getMax(shapes["arwing"]);

		glm::vec3 arwingTrans = arwingMin + 0.5f * (arwingMax - arwingMin);
		float arwingScale = 0;
		if (arwingMax.x > arwingMax.y && arwingMax.x > arwingMax.z)
		{
			arwingScale = 2.0 / (arwingMax.x - arwingMin.x);
		}
		else if (arwingMax.y > arwingMax.x && arwingMax.y > arwingMax.z)
		{
			arwingScale = 2.0 / (arwingMax.y - arwingMin.y);
		}
		else
		{
			arwingScale = 2.0 / (arwingMax.z - arwingMin.z);
		}

		arwingVel += arwingAcc;

		// Draw arwing parts
		programs["texture"]->bind();
		for (auto shape = shapes["arwing"].begin(); shape != shapes["arwing"].end(); ++shape) {
			M->pushMatrix();
				M->translate(glm::vec3(arwingPos.x, arwingPos.y, arwingPos.z));
				M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
				M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
				M->scale(arwingScale);
				M->translate(-1.0f*arwingTrans);
				glUniformMatrix4fv(programs["texture"]->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
				glUniformMatrix4fv(programs["texture"]->getUniform("V"), 1, GL_FALSE, value_ptr(V));
				glUniformMatrix4fv(programs["texture"]->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
				glUniform3f(programs["texture"]->getUniform("lightDir"), 1.0, 0.0, 0.0);
				glUniform2f(programs["texture"]->getUniform("texOffset"), 0.0, 0.0);
				(*shape)->draw(programs["texture"]);
			M->popMatrix();
		}
		programs["texture"]->unbind();


		programs["texture"]->bind();
		M->pushMatrix();
			M->translate(glm::vec3(0, -5, 10));
			M->scale(glm::vec3(20, 20, 0));
			M->rotate(glm::radians(80.0), glm::vec3(1, 0, 0));
			glUniformMatrix4fv(programs["texture"]->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(programs["texture"]->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			glUniformMatrix4fv(programs["texture"]->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glUniform3f(programs["texture"]->getUniform("lightDir"), 1.0, 0.0, 0.0);
			glUniform2f(programs["texture"]->getUniform("texOffset"), 0.0, texOffset);
			shapes["quad"][0]->draw(programs["texture"]);
		M->popMatrix();
		programs["texture"]->unbind();

		texOffset += 0.003;

		P->popMatrix();
	}

};


int main(int argc, char **argv)
{
	std::string shaderDir = "../shaders";
	std::string resourceDir = "../resources";

	Application *application = new Application();

	WindowManager * windowManager = new WindowManager();
	windowManager->init(480, 480);
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
