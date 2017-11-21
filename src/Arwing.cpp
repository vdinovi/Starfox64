#include "Arwing.h"
#include "stb_image.h"
#include "common.h"

Arwing::Arwing(std::string resourceDir) {
    std::vector<tinyobj::shape_t> TOshapes;
	std::vector<tinyobj::material_t> TOmats;
	std::string err;
	std::string basePath = resourceDir + "/";

    bool rc = tinyobj::LoadObj(TOshapes, TOmats, err, (basePath + "arwing.obj").c_str(), basePath.c_str());
    if (!rc) {
        std::cerr << err << std::endl;
    }
    else {
        for (int i = 0; i < TOshapes.size(); ++i) {
            std::shared_ptr<Shape> shape = std::make_shared<Shape>();
            shape->createShape(TOshapes[i]);
            shape->measure();
            shape->init();
            shapes.push_back(shape);
            if (TOshapes[i].mesh.material_ids[0] >= 0) {
                shape->loadMaterial(TOmats[TOshapes[i].mesh.material_ids[0]],
                                    basePath);
                shape->useMaterial = true;
            }
            else {
                // materials must be available
                std::cout << "Error loading materials" << std::endl;
            }
        }
    }
}

void Arwing::measure() {
	glm::vec3 arwingMin = Shape::getMin(shapes);
	glm::vec3 arwingMax = Shape::getMax(shapes);

    glm::vec3 arwingTrans = arwingMin + 0.5f * (arwingMax - arwingMin);
	float arwingScale = 0;
	if (arwingMax.x > arwingMax.y && arwingMax.x > arwingMax.z) {
		arwingScale = 2.0 / (arwingMax.x - arwingMin.x);
	}
	else if (arwingMax.y > arwingMax.x && arwingMax.y > arwingMax.z) {
		arwingScale = 2.0 / (arwingMax.y - arwingMin.y);
	}
	else {
	    arwingScale = 2.0 / (arwingMax.z - arwingMin.z);
	}
    trans = arwingTrans;
    scale = arwingScale;
}

void Arwing::draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
                  const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos) {

    prog->bind();
	M->pushMatrix();
		M->translate(glm::vec3(position.x, position.y, position.z));
		M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
		M->scale(glm::vec3(2.0, 2.0, 2.0));
		M->scale(scale);
		M->translate(-1.0f*trans);
        for (auto shape = shapes.begin(); shape != shapes.end(); ++shape) {
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glUniform3f(prog->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
			glUniform2f(prog->getUniform("texOffset"), 0.0, 0.0);
			(*shape)->draw(prog);
        }
    M->popMatrix();
    prog->unbind();
}

void Arwing::pitchUp(int action) {
    if (action == KEY_PRESS) {
        turning[1] = 1;
    }
    if (action == KEY_RELEASE) {
        turning[1] = 0;
    }
}

void Arwing::pitchDown(int action) {
    if (action == KEY_PRESS) {
        turning[1] = -1;
	}
    if (action == KEY_RELEASE) {
        turning[1] = 0;
    }
}

void Arwing::yawLeft(int action) {
    if (action == KEY_PRESS) {
        turning[0] = -1;
    }
    if (action == KEY_RELEASE) {
        turning[0] = 0;
    }

}

void Arwing::yawRight(int action) {
    if (action == KEY_PRESS) {
        turning[0] = 1;
    }
    if (action == KEY_RELEASE) {
        turning[0] = 0;
    }
}



void Arwing::advance() {
    // Handle yaw
    switch(turning[0]) {
    case 0: // not turning
        yaw = yaw < 0 ?
            glm::min(yaw + 2*ARWING_TURN_RATE, 0.0) :
            (yaw > 0 ? glm::max(yaw - 2*ARWING_TURN_RATE, 0.0) : 0.0);
        break;
    case -1: // turning left
        yaw = glm::min(yaw + ARWING_TURN_RATE, ARWING_MAX_TURN);
        break;
    case 1: // turning right
        yaw = glm::max(yaw - ARWING_TURN_RATE, -ARWING_MAX_TURN);
        break;
    }
    // Handle pitch
    switch(turning[1]) {
    case 0: // not turning
        pitch = pitch < 0 ?
            glm::min(pitch + 2*ARWING_TURN_RATE, 0.0) :
            (pitch > 0 ? glm::max(pitch - 2*ARWING_TURN_RATE, 0.0) : 0.0);
        break;
    case -1: // turning down
        pitch = glm::min(pitch + ARWING_TURN_RATE, ARWING_MAX_TURN);
        break;
    case 1: // turning up
        pitch = glm::max(pitch - ARWING_TURN_RATE, -ARWING_MAX_TURN);
        break;
    }

    position.x = glm::clamp(position.x + ARWING_MOVE_SPEED*glm::sin(glm::radians(yaw)), -BOUND, BOUND);
    position.y = glm::clamp(position.y + ARWING_MOVE_SPEED*-glm::sin(glm::radians(pitch)), -BOUND, BOUND);
}




