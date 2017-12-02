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

    std::vector<float> exhaustPos = { 0.0, 0.0, 0.0 };
    exhaustLight = std::make_shared<vao_t>();
	glGenVertexArrays(1, &exhaustLight->vaoId);
	glBindVertexArray(exhaustLight->vaoId);
	glGenBuffers(1, &exhaustLight->vboId[0]);
	glBindBuffer(GL_ARRAY_BUFFER, exhaustLight->vboId[0]);
	glBufferData(GL_ARRAY_BUFFER, exhaustPos.size() * sizeof(float), &exhaustPos[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    exhaustLight->numElements = exhaustPos.size();
    glBindVertexArray(0);


    // Set up crosshair shape
    crosshairShape = std::make_shared<Shape>();
    std::vector<float> crosshairVerts = { -1.0, -1.0, 0.0, 1.0, -1.0,
                                           0.0, 1.0, 1.0, 0.0, -1.0,  1.0, 0.0 };
    std::vector<float> crosshairNorms;
    std::vector<float> crosshairTex = { 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,};
    std::vector<unsigned> crosshairInds = { 0, 1, 2, 0, 2, 3 };
    crosshairShape->createShape(crosshairVerts, crosshairNorms, crosshairTex, crosshairInds);
    crosshairShape->loadTexture(basePath + "crosshair.jpg");
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

void Arwing::draw(const std::shared_ptr<Program> textureProg, const std::shared_ptr<Program> exhaustProg,
                  const std::shared_ptr<Program> crosshairProg, const std::shared_ptr<MatrixStack> P,
                  const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos)
{
    textureProg->bind();
    // Draw Ship
	M->pushMatrix();
		M->translate(glm::vec3(position.x, position.y, position.z));
		M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
		M->rotate(glm::radians(roll), glm::vec3(0, 0, 1));
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
        textureProg->unbind();

        exhaustProg->bind();
        M->pushMatrix();
            M->translate(glm::vec3(0, 0.2, -1.2));
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    	    glPointSize(200.0);
		    glUniformMatrix4fv(exhaustProg->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		    glUniformMatrix4fv(exhaustProg->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
		    glUniformMatrix4fv(exhaustProg->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M->topMatrix()));
		    glBindVertexArray(exhaustLight->vaoId);
			glBindBuffer(GL_ARRAY_BUFFER, exhaustLight->vboId[0]);
			glDrawArrays(GL_POINTS, 0, exhaustLight->numElements);
		    glBindVertexArray(0);
		    glDisable(GL_BLEND);
        M->popMatrix();
        exhaustProg->unbind();
    M->popMatrix();

    // Draw Projctiles
    textureProg->bind();
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

    // Draw CrossHairs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    crosshairProg->bind();
	M->pushMatrix();
		M->translate(glm::vec3(position.x+50*glm::sin(glm::radians(yaw)), position.y-50*glm::sin(glm::radians(pitch)), 50));
        M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
        M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		M->scale(glm::vec3(2, 2, 0));
	    glUniformMatrix4fv(crosshairProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(crosshairProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(crosshairProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		crosshairShape->draw(crosshairProg);
    M->popMatrix();
	M->pushMatrix();
		M->translate(glm::vec3(position.x+22*glm::sin(glm::radians(yaw)), position.y-22*glm::sin(glm::radians(pitch)), 30));
        M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
        M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		M->scale(glm::vec3(2, 2, 0));
	    glUniformMatrix4fv(crosshairProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(crosshairProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(crosshairProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		crosshairShape->draw(crosshairProg);
    M->popMatrix();
    crosshairProg->unbind();
    glDisable(GL_BLEND);
}

void Arwing::pitchUp(int action) {
    if (action == KEY_PRESS) {
        pitching = PITCHING_UP;
    }
    if (action == KEY_RELEASE) {
        pitching = NOT_PITCHING;
    }
}

void Arwing::pitchDown(int action) {
    if (action == KEY_PRESS) {
        pitching = PITCHING_DOWN;
	}
    if (action == KEY_RELEASE) {
        pitching = NOT_PITCHING;
    }
}

void Arwing::yawLeft(int action) {
    if (action == KEY_PRESS) {
        yawing = YAWING_LEFT;
    }
    if (action == KEY_RELEASE) {
        yawing = NOT_YAWING;
    }
}

void Arwing::yawRight(int action) {
    if (action == KEY_PRESS) {
        yawing = YAWING_RIGHT;
    }
    if (action == KEY_RELEASE) {
        yawing = NOT_YAWING;
    }
}

void Arwing::rollLeft(int action) {
    if (action == KEY_PRESS) {
        rolling = ROLLING_LEFT;
    }
    if (action == KEY_RELEASE) {
        rolling = NOT_ROLLING;
    }
}

void Arwing::rollRight(int action) {
    if (action == KEY_PRESS) {
        rolling = ROLLING_RIGHT;
    }
    if (action == KEY_RELEASE) {
        rolling = NOT_ROLLING;
    }
}

void Arwing::barrelRoll() {
    if (!barrelRollState) {
        barrelRollState = true;
        barrelRollAmnt = 360.0;
    }
}

void Arwing::advance() {
    switch(yawing) {
    case NOT_YAWING:
        yaw = yaw < 0 ?
            glm::min(yaw + 2*ARWING_TURN_RATE, 0.0) :
            (yaw > 0 ? glm::max(yaw - 2*ARWING_TURN_RATE, 0.0) : 0.0);
        break;
    case YAWING_LEFT:
        yaw = glm::min(yaw + ARWING_TURN_RATE, ARWING_MAX_YAW);
        break;
    case YAWING_RIGHT:
        yaw = glm::max(yaw - ARWING_TURN_RATE, -ARWING_MAX_YAW);
        break;
    }
    switch(pitching) {
    case NOT_PITCHING:
        pitch = pitch < 0 ?
            glm::min(pitch + 2*ARWING_TURN_RATE, 0.0) :
            (pitch > 0 ? glm::max(pitch - 2*ARWING_TURN_RATE, 0.0) : 0.0);
        break;
    case PITCHING_DOWN:
        pitch = glm::min(pitch + ARWING_TURN_RATE, ARWING_MAX_PITCH);
        break;
    case PITCHING_UP:
        pitch = glm::max(pitch - ARWING_TURN_RATE, -ARWING_MAX_PITCH);
        break;
    }
    switch(rolling) {
    case NOT_ROLLING:
        roll = roll < 0 ?
               glm::min(roll + 2*ARWING_ROLL_RATE, 0.0) :
               (roll > 0 ? glm::max(roll - 2*ARWING_ROLL_RATE, 0.0) : 0.0);
        break;
    case ROLLING_LEFT:
        roll = glm::max(roll - 3*ARWING_ROLL_RATE, -ARWING_MAX_ROLL);
        break;
    case ROLLING_RIGHT:
        roll = glm::min(roll + 3*ARWING_ROLL_RATE, ARWING_MAX_ROLL);
        break;
    }

    if (barrelRollState) {
        roll = barrelRollAmnt;
        barrelRollAmnt -= ARWING_BARREL_ROLL_RATE;
        if (barrelRollAmnt < 0.0) {
            barrelRollState = false;
        }
    }


    position.x = glm::clamp(position.x + ARWING_MOVE_SPEED*glm::sin(glm::radians(yaw)),
                            -AIRSPACE_WIDTH, AIRSPACE_WIDTH);
    position.y = glm::clamp(position.y + ARWING_MOVE_SPEED*-glm::sin(glm::radians(pitch)),
                            -AIRSPACE_HEIGHT, AIRSPACE_HEIGHT);
}

void Arwing::shoot() {
    // @NOTE this is really frustrating to tune; dont fuck with it.
    // -- I fixed some stuff and it's less frustrating now...
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



