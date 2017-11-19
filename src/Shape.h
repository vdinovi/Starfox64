
#pragma once

#ifndef FINAL_471_SHAPE_H_INCLUDED
#define FINAL_471_SHAPE_H_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include "tiny_obj_loader.h"

class Program;

struct Material
{
	unsigned texId;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	unsigned shine;
};

class Shape
{

public:

	void createShape(tinyobj::shape_t & shape);
	void loadMaterial(tinyobj::material_t & material, std::string resourceDir);
	void setMaterial(unsigned texId, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shine);
	void init();
	void measure();
	void draw(const std::shared_ptr<Program> prog) const;

	static glm::vec3 getMax(std::vector<std::shared_ptr<Shape>> shapes);
	static glm::vec3 getMin(std::vector<std::shared_ptr<Shape>> shapes);

	glm::vec3 min = glm::vec3(0);
	glm::vec3 max = glm::vec3(0);
	bool useMaterial = false;
	Material material;

private:

	std::vector<unsigned int> eleBuf;
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	unsigned int eleBufID = 0;
	unsigned int posBufID = 0;
	unsigned int norBufID = 0;
	unsigned int texBufID = 0;
	unsigned int vaoID = 0;
};

#endif // FINAL_471_SHAPE_H_INCLUDED
