
#include "Shape.h"
#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "Program.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;


void Shape::loadMaterial(tinyobj::material_t & TOmat, std::string resourceDir) {
	CHECKED_GL_CALL(glGenTextures(1, &material.texId));
	CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, material.texId));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	int width, height, channels;
	unsigned char * data = stbi_load((resourceDir + TOmat.diffuse_texname).c_str(), &width, &height, &channels, 0);
	if (data) {
		CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		CHECKED_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
		stbi_image_free(data);
	}
	else {
		std::cout << "Error loading arwing texture\n";
	}
	material.ambient = glm::vec3(TOmat.ambient[0], TOmat.ambient[1], TOmat.ambient[2]),
	material.diffuse = glm::vec3(TOmat.diffuse[0], TOmat.diffuse[1], TOmat.diffuse[2]),
	material.specular = glm::vec3(TOmat.specular[0], TOmat.specular[1], TOmat.specular[2]);
	material.shine = TOmat.shininess;
};

void Shape::setMaterial(unsigned texId, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shine) {
	material.texId = texId;
	material.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]),
	material.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]),
	material.specular = glm::vec3(specular[0], specular[1], specular[2]);
	material.shine = shine;
};

glm::vec3 Shape::getMax(std::vector<std::shared_ptr<Shape>> shapes) {
	glm::vec3 Gmax = glm::vec3(-std::numeric_limits<float>::max());
	for (size_t i = 0; i < shapes.size(); ++i) {
		Gmax[0] = glm::max(Gmax[0], shapes[i]->max[0]);
		Gmax[1] = glm::max(Gmax[1], shapes[i]->max[1]);
		Gmax[2] = glm::max(Gmax[2], shapes[i]->max[2]);
	}
	return Gmax;
}

glm::vec3 Shape::getMin(std::vector<std::shared_ptr<Shape>> shapes) {
	glm::vec3 Gmin = glm::vec3(std::numeric_limits<float>::min());
	for (size_t i = 0; i < shapes.size(); ++i) {
		Gmin[0] = glm::min(Gmin[0], shapes[i]->min[0]);
		Gmin[1] = glm::min(Gmin[1], shapes[i]->min[1]);
		Gmin[2] = glm::min(Gmin[2], shapes[i]->min[2]);
	}
	return Gmin;
}

// copy the data from the shape to this object
void Shape::createShape(tinyobj::shape_t & shape) {
	posBuf = shape.mesh.positions;
	norBuf = shape.mesh.normals;
	texBuf = shape.mesh.texcoords;
	eleBuf = shape.mesh.indices;
}

void Shape::measure()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = minY = minZ = std::numeric_limits<float>::max();
	maxX = maxY = maxZ = -std::numeric_limits<float>::max();

	//Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < posBuf.size() / 3; v++)
	{
		if (posBuf[3*v+0] < minX) minX = posBuf[3 * v + 0];
		if (posBuf[3*v+0] > maxX) maxX = posBuf[3 * v + 0];

		if (posBuf[3*v+1] < minY) minY = posBuf[3 * v + 1];
		if (posBuf[3*v+1] > maxY) maxY = posBuf[3 * v + 1];

		if (posBuf[3*v+2] < minZ) minZ = posBuf[3 * v + 2];
		if (posBuf[3*v+2] > maxZ) maxZ = posBuf[3 * v + 2];
	}

	min.x = minX;
	min.y = minY;
	min.z = minZ;
	max.x = maxX;
	max.y = maxY;
	max.z = maxZ;
}

void Shape::init()
{
	// Initialize the vertex array object
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Send the position array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &posBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW));

	// Send the normal array to the GPU
	if (norBuf.empty())
	{
		norBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &norBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW));
	}

	// Send the texture array to the GPU
	if (texBuf.empty())
	{
		texBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &texBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW));
	}

	// Send the element array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &eleBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW));

	// Unbind the arrays
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));

	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if (h_nor != -1 && norBufID != 0)
	{
		GLSL::enableVertexAttribArray(h_nor);
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));
	}

	if (texBufID != 0)
	{
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");

		if (h_tex != -1 && texBufID != 0)
		{
			GLSL::enableVertexAttribArray(h_tex);
			CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
			CHECKED_GL_CALL(glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0));
		}
	}

	if (useMaterial) {
		CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, material.texId));
		CHECKED_GL_CALL(glUniform3f(prog->getUniform("MatAmb"), material.ambient[0], material.ambient[1], material.ambient[2]));
		CHECKED_GL_CALL(glUniform3f(prog->getUniform("MatDif"), material.diffuse[0], material.diffuse[1], material.diffuse[2]));
		CHECKED_GL_CALL(glUniform3f(prog->getUniform("MatSpec"), material.specular[0], material.specular[1], material.specular[2]));
		CHECKED_GL_CALL(glUniform1f(prog->getUniform("Shine"), material.shine));
	}

	// Bind element buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));

	// Draw
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0));

	// Disable and unbind
	if (h_tex != -1)
	{
		GLSL::disableVertexAttribArray(h_tex);
	}
	if (h_nor != -1)
	{
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
