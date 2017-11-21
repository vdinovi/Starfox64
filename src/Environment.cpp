#include "Environment.h"
#include "stb_image.h"
#include "common.h"

Environment::Environment(std::string resourceDir) {
	std::vector<tinyobj::shape_t> TOshapes;
	std::vector<tinyobj::material_t> TOmats;
	std::string err;
	std::string basePath = resourceDir + "/";

	bool rc = tinyobj::LoadObj(TOshapes, TOmats, err, (basePath + "quad.obj").c_str(), basePath.c_str());
	if (!rc) {
		std::cerr << err << std::endl;
	}
	else {
		for (int i = 0; i < TOshapes.size(); ++i) {
			std::shared_ptr<Shape> shape = std::make_shared<Shape>();
			shape->createShape(TOshapes[0]);
			shape->measure();
			shape->init();
            groundShapes.push_back(shape);
			if (TOshapes[i].mesh.material_ids[0] >= 0) {
				shape->loadMaterial(TOmats[TOshapes[i].mesh.material_ids[0]], basePath);
				shape->useMaterial = true;
			}
			else {
                // materials must be available
                std::cout << "Error loading materials" << std::endl;
			}
		}
    }
}

void Environment::draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
                       const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos) {
    prog->bind();
    // Draw Ground
	M->pushMatrix();
		M->translate(glm::vec3(0, -5, 2));
		M->scale(glm::vec3(20, 20, 20));
        M->rotate(glm::radians(80.0), glm::vec3(1, 0, 0));
        for (auto shape = groundShapes.begin(); shape != groundShapes.end(); ++shape) {
	        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
	        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		    glUniform3f(prog->getUniform("lightPos"), 5.0, 0.0, 0.0);
		    glUniform2f(prog->getUniform("texOffset"), 0.0, texOffset.y);
		    (*shape)->draw(prog);
        }
	M->popMatrix();
	prog->unbind();
}

void Environment::advance() {
	texOffset.y += TEX_SCROLL_RATE;
}