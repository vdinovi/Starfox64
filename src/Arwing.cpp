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
            shipShapes.push_back(shape);
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
    std::vector<tinyobj::shape_t> TOshapes1;
	std::vector<tinyobj::material_t> TOmats1;

    rc = tinyobj::LoadObj(TOshapes1, TOmats1, err, (basePath + "arwing_projectile.obj").c_str(), basePath.c_str());
    if (!rc) {
        std::cerr << err << std::endl;
    }
    else {
        for (int i = 0; i < TOshapes1.size(); ++i) {
            std::shared_ptr<Shape> shape = std::make_shared<Shape>();
            shape->createShape(TOshapes1[i]);
            shape->measure();
            shape->init();
            projectileShapes.push_back(shape);
            if (TOshapes1[i].mesh.material_ids[0] >= 0) {
                shape->loadMaterial(TOmats1[TOshapes1[i].mesh.material_ids[0]], basePath);
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
	glm::vec3 arwingMin = Shape::getMin(shipShapes);
	glm::vec3 arwingMax = Shape::getMax(shipShapes);

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
    shipTrans = arwingTrans;
    shipScale = arwingScale;

    projectileTrans = glm::vec3(1, 1, 1);
    projectileScale = 1;
}

void Arwing::draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
                  const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos) {

    prog->bind();
    // Draw Ship
	M->pushMatrix();
		M->translate(glm::vec3(position.x, position.y, position.z));
		M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
		M->scale(glm::vec3(ARWING_SCALE, ARWING_SCALE, ARWING_SCALE));
		M->scale(shipScale);
		M->translate(-1.0f*shipTrans);
        for (auto shape = shipShapes.begin(); shape != shipShapes.end(); ++shape) {
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glUniform3f(prog->getUniform("lightPosition"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(prog->getUniform("La"), 1, 1, 1);
			glUniform3f(prog->getUniform("Ld"), 1, 1, 1);
			glUniform3f(prog->getUniform("Ls"), 1, 1, 1);
			glUniform2f(prog->getUniform("texOffset"), 0.0, 0.0);
			(*shape)->draw(prog);
        }
    M->popMatrix();

    // Draw Projctiles
    for (auto p = projectiles.begin(); p != projectiles.end();) {
        if ((*p)->travelDistance < 0.0) {
            projectiles.erase(p);
        }
        else {
	        M->pushMatrix();
		        M->translate(glm::vec3((*p)->position.x, (*p)->position.y, (*p)->position.z));
                M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
                M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
                M->rotate(glm::radians(45.0), glm::vec3(0, 0, 1));
                M->scale(glm::vec3(0.3, 0.3, 5));
		        //M->scale(projectileScale);
		        //M->translate(-1.0f*projectileTrans);
                for (auto shape = projectileShapes.begin(); shape != projectileShapes.end(); ++shape) {
		    	    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		    	    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		    	    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		    	    glUniform3f(prog->getUniform("lightPosition"), lightPos.x, lightPos.y, lightPos.z);
		    	    glUniform3f(prog->getUniform("La"), 1, 1, 1);
		    	    glUniform3f(prog->getUniform("Ld"), 1, 1, 1);
		    	    glUniform3f(prog->getUniform("Ls"), 1, 1, 1);
		    	    glUniform2f(prog->getUniform("texOffset"), 0.0, 0.0);
		    	    (*shape)->draw(prog);
                }
            M->popMatrix();
            (*p)->advance();
            p++;
        }
    }
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
        yaw = glm::min(yaw + ARWING_TURN_RATE, ARWING_MAX_YAW);
        break;
    case 1: // turning right
        yaw = glm::max(yaw - ARWING_TURN_RATE, -ARWING_MAX_YAW);
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
        pitch = glm::min(pitch + ARWING_TURN_RATE, ARWING_MAX_PITCH);
        break;
    case 1: // turning up
        pitch = glm::max(pitch - ARWING_TURN_RATE, -ARWING_MAX_PITCH);
        break;
    }

    position.x = glm::clamp(position.x + ARWING_MOVE_SPEED*glm::sin(glm::radians(yaw)),
                            -AIRSPACE_WIDTH, AIRSPACE_WIDTH);
    position.y = glm::clamp(position.y + ARWING_MOVE_SPEED*-glm::sin(glm::radians(pitch)),
                            -AIRSPACE_HEIGHT, AIRSPACE_HEIGHT);
}

void Arwing::shoot() {
    // @NOTE this is really frustrating to tune; dont fuck with it.
    glm::vec3 shootPos = glm::vec3(
        position.x + ARWING_SCALE/2*glm::sin(glm::radians(yaw)),
        position.y - ARWING_SCALE/3*glm::sin(glm::radians(pitch)),
        4
    );
    glm::vec3 shootDir = glm::vec3(100*sin(glm::radians(yaw)), -100*sin(glm::radians(pitch)), 100);
    //std::cout << "Firing at <" << shootDir.x << ", " << shootDir.y << ", " << shootDir.z << ">" << std::endl;
    projectiles.push_back(std::make_shared<Projectile>(
        shootPos,
        shootDir,
        ARWING_PROJECTILE_SPEED,
        yaw,
        pitch
    ));
    projectiles.back()->advance();
}


void Projectile::advance() {
    travelDistance -= speed;
    float xPos = (travelDistance) * startPosition.x + (1-travelDistance) * endPosition.x;
    float yPos = (travelDistance) * startPosition.y + (1-travelDistance) * endPosition.y;
    float zPos = (travelDistance) * startPosition.z + (1-travelDistance) * endPosition.z;
    position = glm::vec3(xPos, yPos, zPos);
};



