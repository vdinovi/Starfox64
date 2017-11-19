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

//#include "tiny_obj_loader.h"

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
	std::map<std::string, std::shared_ptr<Program>> programs;
	//std::map<std::string, std::shared_ptr<Shape> shapes;
	//std::map<std::string, mat_t> materials;
	//std::map<std::string, GLuint> textures;

	std::vector<std::shared_ptr<Shape>> arwingShapes;
	std::vector<std::shared_ptr<Material>> arwingMaterials;

	int tex = 1;

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
	}

	void createTex(std::string filepath, ) {

	}

	void initTex(const std::string& resourceDir)
	{
		textures["arwing_back"] = 0;
		glGenTextures(1, &textures["arwing_back"]);
		glBindTexture(GL_TEXTURE_2D, textures["arwing_back"]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		std::string path = resourceDir + "/Arwing/36797B1E_c.png";
		int width, height, channels;
		unsigned char * data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error loading arwing texture\n";
		}

		textures["arwing_blue"] = 0;
		glGenTextures(1, &textures["arwing_blue"]);
		glBindTexture(GL_TEXTURE_2D, textures["arwing_blue"]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		path = resourceDir + "/Arwing/70D48B2D_c.png";
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error loading arwing texture\n";
		}

		textures["arwing_blue"] = 0;
		glGenTextures(1, &textures["arwing_body"]);
		glBindTexture(GL_TEXTURE_2D, textures["arwing_body"]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		path = resourceDir + "/Arwing/53BB3C33_c.png";
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error loading arwing texture\n";
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
		glm::mat4 V = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -5), glm::vec3(0, 1, 0));

		P->pushMatrix();
		if (width > height) {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		else {
			P->perspective(45.f, aspect, 1.f, 100.f);
		}
		M->loadIdentity();

		Shape arwing = *shapes["arwing"];
		glm::vec3 gTrans = arwing.min + 0.5f * (arwing.max - arwing.min);
		float gScale = 0;
		if (arwing.max.x > arwing.max.y && arwing.max.x > arwing.max.z)
		{
			gScale = 2.0 / (arwing.max.x - arwing.min.x);
		}
		else if (arwing.max.y > arwing.max.x && arwing.max.y > arwing.max.z)
		{
			gScale = 2.0 / (arwing.max.y - arwing.min.y);
		}
		else
		{
			gScale = 2.0 / (arwing.max.z - arwing.min.z);
		}

		programs["texture"]->bind();
		M->pushMatrix();
			M->translate(glm::vec3(0, 0, -10));
			M->rotate(glfwGetTime(), glm::vec3(1, 0, 0));
			M->scale(gScale);
			M->translate(-1.0f*gTrans);
			glUniformMatrix4fv(programs["texture"]->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(programs["texture"]->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			glUniformMatrix4fv(programs["texture"]->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glBindTexture(GL_TEXTURE_2D, textures["arwing1"]);
			glBindTexture(GL_TEXTURE_2D, textures["arwing2"]);
			arwing.draw(programs["texture"]);
		M->popMatrix();
		programs["texture"]->unbind();

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
