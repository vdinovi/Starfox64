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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14159

// Helper struct for VAOs
struct vao_t {
	unsigned vao;
	unsigned ibo;
	unsigned vbo_list[3]; // never really gonna have more than 3 vbo
};

struct material_t {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shine;
};

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
	std::map<std::string, std::shared_ptr<Program>> programs;
	std::map<std::string, std::shared_ptr<Shape>> shapes;
	std::map<std::string, GLuint> textures;
	std::map<std::string, material_t> materials;

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


	void initGeom(const std::string& resourceDir)
	{
		shapes["sphere"] = std::make_shared<Shape>();
		shapes["sphere"]->loadMesh(resourceDir + "/sphere.obj");
		shapes["sphere"]->resize();
		shapes["sphere"]->init();
	}

	void initTex(const std::string& resourceDir)
	{
		textures["cat"] = 1;
		glGenTextures(1, &textures["cat"]);
		glBindTexture(GL_TEXTURE_2D, textures["cat"]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		std::string path = resourceDir + "/cat.jpg";
		int width, height, channels;
		unsigned char * data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error loading texture\n";
		}
	}

	void initProg(const std::string& resourceDir)
	{
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
		programs["texture"]->addAttribute("vertPos");
		programs["texture"]->addAttribute("vertNor");
		programs["texture"]->addAttribute("vertTexCoord");

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
		programs["gblur"]->addAttribute("vertTexCoord");
	}

    void initMat()
	{
		materials["shiny_blue_plastic"] = {
        	glm::vec3(0.02, 0.04, 0.2),
        	glm::vec3(0.0, 0.16, 0.9),
            glm::vec3(0.14, 0.2, 0.8),
            120.0
		};
		materials["flat_grey"] = {
        	glm::vec3(0.13, 0.13, 0.14),
        	glm::vec3(0.3, 0.3, 0.4),
            glm::vec3(0.3, 0.3, 0.4),
            4.0
		};
		materials["brass"] = {
        	glm::vec3(0.3294, 0.2235, 0.02745),
        	glm::vec3(0.7804, 0.5686, 0.11373),
            glm::vec3(0.9922, 0.941176, 0.80784),
            27.9
		};
		materials["pink"] = {
        	glm::vec3(0.6231, 0.1111, 0.9130),
        	glm::vec3(0.7777, 0.1084, 0.2323),
            glm::vec3(0.7304, 0.9049, 0.1234),
            15.0
		};
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
		//glm::mat4 V = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));

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
	std::string textureDir = "../textures";
	std::string meshDir = "../meshes";
	std::string shaderDir = "../shaders";

	Application *application = new Application();

	WindowManager * windowManager = new WindowManager();
	windowManager->init(480, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->initProg(shaderDir);
	application->initTex(textureDir);
	application->initGeom(meshDir);
	application->initMat();

	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		application->render();
		glfwSwapBuffers(windowManager->getHandle());
		glfwPollEvents();
	}

	windowManager->shutdown();
	return 0;
}
