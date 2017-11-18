/* ZJ Wood CPE 471 Lab 3 base code - I. Dunn class re-write */
#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <cstdlib>
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14159

// Helper struct for VAOs
struct vao_t {
	unsigned vao;
	unsigned ibo;
	unsigned vbo_list[3]; // never really gonna have more than 3 vbo
};


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
    // Programs
	std::unordered_map<std::string, std::shared_ptr<Program>> programs;
	/*
	std::shared_ptr<Program> normalProg;
	std::shared_ptr<Program> matProg;
	std::shared_ptr<Program> textureProg;
	std::shared_ptr<Program> mirrorProg;
	*/


    // Shapes
	std::unordered_map<std::string, std::shared_ptr<Shape>> programs;
	/*
	std::shared_ptr<Shape> sphereShape;
	*/

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
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


	void initGeom(const std::string& resourceDirectory)
	{

	}

	void initTex(const std::string& resourceDirectory)
	{
	}

	void initProg(const std::string& resourceDirectory)
	{
	}

    void SetMaterial(int i, std::shared_ptr<Program> prog)
	{
        switch (i) {
        case 0: // shiny blue plastic
        	glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
        	glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
            glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
            glUniform1f(prog->getUniform("shine"), 120.0);
            break;
        case 1: // flat grey
            glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
            glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
            glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
            glUniform1f(prog->getUniform("shine"), 4.0);
            break;
        case 2: // brass
            glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
            glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
            glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
            glUniform1f(prog->getUniform("shine"), 27.9);
            break;
        case 3: // put in random values and got hot pink? I'm okay with that.
            glUniform3f(prog->getUniform("MatAmb"), 0.6231, 0.1111, 0.9130);
            glUniform3f(prog->getUniform("MatDif"), 0.7777, 0.1084, 0.2323);
            glUniform3f(prog->getUniform("MatSpec"), 0.7304, 0.9049, 0.1234);
            glUniform1f(prog->getUniform("shine"), 18.9);
            break;
        }
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
		glm::mat4 V = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));

		P->pushMatrix();
		if (width > height) {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		else {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		M->loadIdentity();

		P->popMatrix();
	}

};


int main(int argc, char **argv)
{
	std::string resourceDir = "../resources";
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	WindowManager * windowManager = new WindowManager();
	windowManager->init(480, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->initTex(resourceDir);
	application->initProg(resourceDir);
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
