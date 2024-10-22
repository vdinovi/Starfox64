
#include "Shape.h"
#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "Program.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;


void Shape::loadMaterial(tinyobj::material_t & TOmat, string resourceDir) {
	loadTexture(resourceDir + "/" + TOmat.diffuse_texname);
	setMaterial(TOmat.ambient, TOmat.diffuse, TOmat.specular, TOmat.shininess);
};

void Shape::loadTexture(string filepath) {
	CHECKED_GL_CALL(glGenTextures(1, &texId));
	CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, texId));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	int width, height, channels;
	unsigned char * data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
	if (data) {
		CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		CHECKED_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
		stbi_image_free(data);
	}
	else {
		cout << "Error loading texture\n";
	}
};

void Shape::setMaterial(float ambient[], float diffuse[], float specular[], float shine) {
	material.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]),
	material.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]),
	material.specular = glm::vec3(specular[0], specular[1], specular[2]);
	material.shine = shine;
	useMaterial = true;
};

glm::vec3 Shape::getMax(vector<shared_ptr<Shape>> shapes) {
	glm::vec3 Gmax = glm::vec3(-numeric_limits<float>::max());
	for (size_t i = 0; i < shapes.size(); ++i) {
		Gmax[0] = glm::max(Gmax[0], shapes[i]->max[0]);
		Gmax[1] = glm::max(Gmax[1], shapes[i]->max[1]);
		Gmax[2] = glm::max(Gmax[2], shapes[i]->max[2]);
	}
	return Gmax;
}

glm::vec3 Shape::getMin(vector<shared_ptr<Shape>> shapes) {
	glm::vec3 Gmin = glm::vec3(numeric_limits<float>::min());
	for (size_t i = 0; i < shapes.size(); ++i) {
		Gmin[0] = glm::min(Gmin[0], shapes[i]->min[0]);
		Gmin[1] = glm::min(Gmin[1], shapes[i]->min[1]);
		Gmin[2] = glm::min(Gmin[2], shapes[i]->min[2]);
	}
	return Gmin;
}

void Shape::createShape(tinyobj::shape_t & shape) {
	posBuf = shape.mesh.positions;
	norBuf = shape.mesh.normals;
	texBuf = shape.mesh.texcoords;
	eleBuf = shape.mesh.indices;
}

void Shape::createShape(vector<float>& positions, vector<float>& normals, vector<unsigned>& indices) {
	posBuf = positions;
	norBuf = normals;
	eleBuf = indices;
}

void Shape::createShape(vector<float>& positions, vector<float>& normals,
						vector<float>& texcoords, vector<unsigned>& indices) {
	posBuf = positions;
	norBuf = normals;
	texBuf = texcoords;
	eleBuf = indices;
}

void Shape::measure()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	minX = minY = minZ = numeric_limits<float>::max();
	maxX = maxY = maxZ = -numeric_limits<float>::max();
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
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
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));
	CHECKED_GL_CALL(glGenBuffers(1, &posBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW));
	if (norBuf.empty()) {
		generateNormals();
	}
	CHECKED_GL_CALL(glGenBuffers(1, &norBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW));

	if (texBuf.empty()) {
		texBufID = 0;
		useMaterial = false;
	}
	else {
		CHECKED_GL_CALL(glGenBuffers(1, &texBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW));
	}
	CHECKED_GL_CALL(glGenBuffers(1, &eleBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;
	CHECKED_GL_CALL(glBindVertexArray(vaoID));
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));
	h_nor = prog->getAttribute("vertNor");
	if (h_nor != -1 && norBufID != 0) {
		GLSL::enableVertexAttribArray(h_nor);
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));
	}
	if (texBufID != 0) {
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");

		if (h_tex != -1 && texBufID != 0)
		{
			GLSL::enableVertexAttribArray(h_tex);
			CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
			CHECKED_GL_CALL(glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0));
		}
	}
	CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, texId));
	if (useMaterial) {
		CHECKED_GL_CALL(glUniform3f(prog->getUniform("Ka"), material.ambient[0], material.ambient[1], material.ambient[2]));
		CHECKED_GL_CALL(glUniform3f(prog->getUniform("Kd"), material.diffuse[0], material.diffuse[1], material.diffuse[2]));
		CHECKED_GL_CALL(glUniform3f(prog->getUniform("Ks"), material.specular[0], material.specular[1], material.specular[2]));
		CHECKED_GL_CALL(glUniform1f(prog->getUniform("Shine"), material.shine));
	}
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0));
	if (h_tex != -1) {
		GLSL::disableVertexAttribArray(h_tex);
	}
	if (h_nor != -1) {
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Shape::generateNormals() {
    norBuf = std::vector<float>(posBuf.size(), 0);
    // Accumulate the each face normal for every vertex that is a part of a given face
    for (int fndx = 0; fndx < eleBuf.size(); fndx += 3) {
        glm::vec3 a(posBuf[3*eleBuf[fndx]], posBuf[3*eleBuf[fndx]+1], posBuf[3*eleBuf[fndx]+2]);
        glm::vec3 b(posBuf[3*eleBuf[fndx+1]], posBuf[3*eleBuf[fndx+1]+1], posBuf[3*eleBuf[fndx+1]+2]);
        glm::vec3 c(posBuf[3*eleBuf[fndx+2]], posBuf[3*eleBuf[fndx+2]+1], posBuf[3*eleBuf[fndx+2]+2]);
        glm::vec3 ac = c - a;
        glm::vec3 ab = b - a;
        glm::vec3 norm = glm::cross(ab, ac);

        for (int v = 0; v < 3; ++v) {
            norBuf[3*eleBuf[fndx+v]] += norm[0];
            norBuf[3*eleBuf[fndx+v]+1] += norm[1];
            norBuf[3*eleBuf[fndx+v]+2] += norm[2];
        }
    }
    // Now normalize each normal
    for (float nndx = 0; nndx < norBuf.size(); nndx += 3) {
        glm::vec3 normal(norBuf[nndx], norBuf[nndx+1], norBuf[nndx+2]);
        glm::vec3 normalized = normalize(normal);
        norBuf[nndx] = normalized[0];
        norBuf[nndx+1] = normalized[1];
        norBuf[nndx+2] = normalized[2];
    }
}