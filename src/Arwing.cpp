#include "Arwing.h"
#include "stb_image.h"
#include "common.h"

Arwing::Arwing(std::string resourceDir) {
    std::vector<tinyobj::shape_t> TOshapes;
	std::vector<tinyobj::material_t> TOmats;
	std::string err;
	std::string basePath = resourceDir + "/";

    // Set up ship shape
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
    // Set up projectile shape
    rc = tinyobj::LoadObj(TOshapes, TOmats, err, (basePath + "arwing_projectile.obj").c_str(), basePath.c_str());
    if (!rc) {
        std::cerr << err << std::endl;
    }
    else {
        for (int i = 0; i < TOshapes.size(); ++i) {
            std::shared_ptr<Shape> shape = std::make_shared<Shape>();
            shape->createShape(TOshapes[i]);
            shape->measure();
            shape->init();
            projectileShapes.push_back(shape);
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
    // Set up crosshair shape
    crosshairShape = std::make_shared<Shape>();
    std::vector<float> crosshairVerts =
        { -1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, -1.0,  1.0, 0.0 };
    std::vector<unsigned> crosshairInds = { 0, 1, 2, 0, 2, 3 };
    std::vector<float> crosshairNorms;
    crosshairShape->createShape(crosshairVerts, crosshairNorms, crosshairInds);
    crosshairShape->init();
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

void Arwing::draw(const std::shared_ptr<Program> textureProg, const std::shared_ptr<Program> crosshairProg,
                  const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> M,
                  const glm::mat4& V, const glm::vec3& lightPos)
{
    textureProg->bind();
    // Draw Ship
	M->pushMatrix();
		M->translate(glm::vec3(position.x, position.y, position.z));
		M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
		M->scale(glm::vec3(ARWING_SCALE, ARWING_SCALE, ARWING_SCALE));
		M->scale(shipScale);
		M->translate(-1.0f*shipTrans);
        for (auto shape = shipShapes.begin(); shape != shipShapes.end(); ++shape) {
			glUniformMatrix4fv(textureProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(textureProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			glUniformMatrix4fv(textureProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glUniform3f(textureProg->getUniform("lightPosition"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(textureProg->getUniform("La"), 1, 1, 1);
			glUniform3f(textureProg->getUniform("Ld"), 1, 1, 1);
			glUniform3f(textureProg->getUniform("Ls"), 1, 1, 1);
			glUniform2f(textureProg->getUniform("texOffset"), 0.0, 0.0);
			(*shape)->draw(textureProg);
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
                M->rotate(glm::radians((*p)->yaw), glm::vec3(0, 1, 0));
                M->rotate(glm::radians((*p)->pitch), glm::vec3(1, 0, 0));
                M->scale(glm::vec3(0.3, 0.3, 20));
                for (auto shape = projectileShapes.begin(); shape != projectileShapes.end(); ++shape) {
		    	    glUniformMatrix4fv(textureProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		    	    glUniformMatrix4fv(textureProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		    	    glUniformMatrix4fv(textureProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		    	    glUniform3f(textureProg->getUniform("lightPosition"), lightPos.x, lightPos.y, lightPos.z);
		    	    glUniform3f(textureProg->getUniform("La"), 1, 1, 1);
		    	    glUniform3f(textureProg->getUniform("Ld"), 1, 1, 1);
		    	    glUniform3f(textureProg->getUniform("Ls"), 1, 1, 1);
		    	    glUniform2f(textureProg->getUniform("texOffset"), 0.0, 0.0);
		    	    (*shape)->draw(textureProg);
                }
            M->popMatrix();
            (*p)->advance();
            p++;
        }
    }
    textureProg->unbind();

    // Draw Projctiles
    crosshairProg->bind();
	M->pushMatrix();
		M->translate(glm::vec3(position.x+30*glm::sin(glm::radians(yaw)), position.y-30*glm::sin(glm::radians(pitch)), 30));
		M->scale(glm::vec3(2, 2, 0));
        M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
        M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		glUniformMatrix4fv(crosshairProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(crosshairProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(crosshairProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		crosshairShape->draw(crosshairProg);
    M->popMatrix();
	M->pushMatrix();
		M->translate(glm::vec3(position.x+50*glm::sin(glm::radians(yaw)), position.y-50*glm::sin(glm::radians(pitch)), 50));
		M->scale(glm::vec3(2, 2, 0));
        M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
        M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
	    glUniformMatrix4fv(crosshairProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(crosshairProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(crosshairProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		crosshairShape->draw(crosshairProg);
    M->popMatrix();
    crosshairProg->unbind();
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
        position.x - 3*ARWING_SCALE*glm::sin(glm::radians(yaw)),
        position.y + 3*ARWING_SCALE*glm::sin(glm::radians(pitch)),
        position.z - 2*ARWING_SCALE
    );
    glm::vec3 shootDir = glm::vec3(
        position.x + ARWING_PROJECTILE_DISTANCE*sin(glm::radians(yaw)),
        position.y + -ARWING_PROJECTILE_DISTANCE*sin(glm::radians(pitch)),
        ARWING_PROJECTILE_DISTANCE
    );
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



