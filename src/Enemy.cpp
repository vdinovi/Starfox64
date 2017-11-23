#include "Enemy.h"
#include <cstdlib>
#include <ctime>

Enemy::Enemy(std::string resourceDir) {
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
                shape->loadMaterial(TOmats[TOshapes[i].mesh.material_ids[0]], basePath);
                shape->useMaterial = true;
            }
            else {
                // materials must be available
                std::cout << "Error loading materials" << std::endl;
            }
        }
    }
    spawnEnemy();
}

void Enemy::measure() {
	glm::vec3 min = Shape::getMin(shapes);
	glm::vec3 max = Shape::getMax(shapes);

    glm::vec3 lTrans = min + 0.5f * (max - min);
	float lScale = 0;
	if (max.x > max.y && max.x > max.z) {
		lScale = 2.0 / (max.x - min.x);
	}
	else if (max.y > max.x && max.y > max.z) {
		lScale = 2.0 / (max.y - min.y);
	}
	else {
	    lScale = 2.0 / (max.z - min.z);
	}
    trans = lTrans;
    scale = lScale;
}

void Enemy::draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
                 const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos) {
    prog->bind();
    for (auto e = enemies.begin(); e != enemies.end();) {
        if ((*e)->travelDistance < 0.0) {
            //std::cout << "Travel Distance is " << (*e)->travelDistance << " -- Erasing.." << std::endl;
            //EnemyUnit eu = **e;
            enemies.erase(e);
        } else {
            //std::cout << "Travel Distance is " << (*e)->travelDistance << std::endl;
	        M->pushMatrix();
		        M->translate(glm::vec3((*e)->position.x, (*e)->position.y, (*e)->position.z));
		        //M->rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
		        //M->rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
                M->rotate(glm::radians(180.0), glm::vec3(0, 1, 0));
		        M->scale(glm::vec3(ENEMY_SCALE/2, ENEMY_SCALE/2, ENEMY_SCALE/2));
		        M->scale(scale);
		        M->translate(-1.0f*trans);
                for (auto shape = shapes.begin(); shape != shapes.end(); ++shape) {
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
            (*e)->advance();
            e++;
        }
    }
    prog->unbind();
}

void EnemyUnit::advance() {
    travelDistance -= speed;
    float xPos = (travelDistance) * startPosition.x + (1-travelDistance) * endPosition.x;
    float yPos = (travelDistance) * startPosition.y + (1-travelDistance) * endPosition.y;
    float zPos = (travelDistance) * startPosition.z + (1-travelDistance) * endPosition.z;
    position = glm::vec3(xPos, yPos, zPos);
}


void Enemy::spawnEnemy() {
    srand (static_cast <unsigned> (time(0)));

    rand();
    float speed = MIN_ENEMY_INTERP_SPEED + (MIN_ENEMY_INTERP_SPEED - MAX_ENEMY_INTERP_SPEED)
                 * (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    float startX = LEFT_ENEMY_SPAWN_BOUND + (RIGHT_ENEMY_SPAWN_BOUND - LEFT_ENEMY_SPAWN_BOUND)
                 * (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    float startY = LOWER_ENEMY_SPAWN_BOUND + (UPPER_ENEMY_SPAWN_BOUND - LOWER_ENEMY_END_BOUND)
                 * (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    float startZ = FRONT_ENEMY_SPAWN_BOUND + (BACK_ENEMY_SPAWN_BOUND - FRONT_ENEMY_SPAWN_BOUND)
                 * (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    float endX = LEFT_ENEMY_END_BOUND + (RIGHT_ENEMY_END_BOUND - LEFT_ENEMY_END_BOUND)
                 * (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    float endY = LOWER_ENEMY_END_BOUND + (UPPER_ENEMY_END_BOUND - LOWER_ENEMY_END_BOUND)
                 * (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    float endZ = ENEMY_END_Z;


    enemies.push_back(std::make_shared<EnemyUnit>(
        glm::vec3(startX, startY, startZ),
        glm::vec3(endX, endY, endZ),
        speed
    ));
    auto e = enemies.back();
    e->advance();
    //std::cout << "New unit (" <<  e->startPosition.x << ", " << e->startPosition.y << ", " << e->startPosition.z;
    //std::cout << ") -> (" <<  e->endPosition.x << ", " << e->endPosition.y << ", " << e->endPosition.z;
    //std::cout << ") with travel distance: " << e->travelDistance << " and speed: " << e->speed << std::endl;
}


bool Enemy::checkCollisions(glm::vec3 position, float radius) {
    for (auto e = enemies.begin(); e != enemies.end(); ++e) {
        if (glm::distance((*e)->position, position) <= radius) {
            return true;
        }
    }
    return false;
}

