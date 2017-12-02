#include "Enemy.h"
#include "GLSL.h"
#include <cstdlib>
#include <ctime>

/////////////////
///// Enemy /////
/////////////////

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

void Enemy::draw(const std::shared_ptr<Program> prog, const std::shared_ptr<Program> explosionProg,
                 const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> M,
                 const glm::mat4& V, const glm::vec3& lightPos) {
    for (auto e = enemies.begin(); e != enemies.end();) {
        if ((*e)->state == ENEMY_STATE_NORMAL && (*e)->travelDistance < 0.0) {
            enemies.erase(e);
        } else if ((*e)->state == ENEMY_STATE_DEAD && (*e)->particles.empty()) {
            enemies.erase(e);
        }  else {
	        M->pushMatrix();
                if ((*e)->state == ENEMY_STATE_NORMAL) {
                    prog->bind();
		            M->translate(glm::vec3((*e)->position.x, (*e)->position.y, (*e)->position.z));
		            M->scale(glm::vec3(ENEMY_SCALE/2, ENEMY_SCALE/2, ENEMY_SCALE/2));
                    M->rotate(glm::radians(180.0), glm::vec3(0, 1, 0));
		            M->rotate((*e)->yaw, glm::vec3(0, 1, 0));
		            M->rotate((*e)->pitch, glm::vec3(1, 0, 0));
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
                    prog->unbind();
                } else if ((*e)->state == ENEMY_STATE_DEAD) {
                    explosionProg->bind();
                    M->pushMatrix();
		                M->translate(glm::vec3((*e)->position.x, (*e)->position.y, (*e)->position.z));
                        M->scale(glm::vec3(PARTICLE_SCALE, PARTICLE_SCALE, PARTICLE_SCALE));
			            glUniformMatrix4fv(explosionProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			            glUniformMatrix4fv(explosionProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
			            glUniformMatrix4fv(explosionProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                        (*e)->updateParticles();
                        CHECKED_GL_CALL(glBindVertexArray((*e)->particleVAO));
		                CHECKED_GL_CALL(glEnable(GL_BLEND));
		                CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
       	                CHECKED_GL_CALL(glPointSize(50.0));
                        CHECKED_GL_CALL(glEnableVertexAttribArray(0));
				        CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, (*e)->pointsVBO));
				        CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0));
                        CHECKED_GL_CALL(glVertexAttribDivisor(0, 1));
                        CHECKED_GL_CALL(glDrawArraysInstanced(GL_POINTS, 0, 1, (*e)->particles.size()));
                        CHECKED_GL_CALL(glVertexAttribDivisor(0, 0));
				        CHECKED_GL_CALL(glDisableVertexAttribArray(0));
		                CHECKED_GL_CALL(glBindVertexArray(0));
		                CHECKED_GL_CALL(glDisable(GL_BLEND));
                    M->popMatrix();
                    explosionProg->unbind();
                }
            M->popMatrix();
            (*e)->advance();
            e++;
        }
    }
}

void Enemy::spawnEnemy() {
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

    double yaw = atan((endX-startX)/(endZ-startZ));
    double pitch = atan((endY-startY)/(endZ-startZ));

    enemies.push_back(std::make_shared<EnemyUnit>(
        glm::vec3(startX, startY, startZ),
        glm::vec3(endX, endY, endZ),
        speed,
        yaw,
        pitch
    ));
    auto e = enemies.back();
    e->advance();
}

unsigned Enemy::checkCollisions(glm::vec3 position, float radius) {
    unsigned collisions = 0;
    for (auto e = enemies.begin(); e != enemies.end(); ++e) {
        if ((*e)->state != ENEMY_STATE_DEAD && glm::distance((*e)->position, position) <= radius + ENEMY_HIT_RADIUS) {
            collisions++;
        }
    }
    return collisions;
}

std::vector<std::shared_ptr<EnemyUnit>> Enemy::checkProjectile(glm::vec3 position, float radius) {
    std::vector<std::shared_ptr<EnemyUnit>> enemiesHit;
    for (auto e = enemies.begin(); e != enemies.end(); ++e) {
        if ((*e)->state != ENEMY_STATE_DEAD && glm::distance((*e)->position, position) <= radius + ENEMY_HIT_RADIUS) {
            enemiesHit.push_back(*e);
        }
    }
    return enemiesHit;
}



//////////////////////
///// Enemy Unit /////
//////////////////////

EnemyUnit::EnemyUnit(glm::vec3 startPos, glm::vec3 endPos, double speed, double yaw, double pitch)
    : startPosition(startPos)
    , endPosition(endPos)
    , speed(speed)
    , yaw(yaw)
    , pitch(pitch)
    , travelDistance(1)
{
    // Setup particle data
	CHECKED_GL_CALL(glGenVertexArrays(1, &particleVAO));
	CHECKED_GL_CALL(glBindVertexArray(particleVAO));
	CHECKED_GL_CALL(glGenBuffers(1, &pointsVBO));
    CHECKED_GL_CALL(glBindVertexArray(0));
}

void EnemyUnit::updateParticles() {
    std::vector<float> positions;
    for (auto p = particles.begin(); p != particles.end(); ++p) {
        positions.push_back((*p)->pos.x);
        positions.push_back((*p)->pos.y);
        positions.push_back((*p)->pos.z);
    }
    CHECKED_GL_CALL(glBindVertexArray(particleVAO));
    CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pointsVBO));
    CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), NULL, GL_STREAM_DRAW));
    CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 3 * sizeof(float), (void*)&positions[0]));
    CHECKED_GL_CALL(glBindVertexArray(0));
};

void EnemyUnit::explode() {
    state = ENEMY_STATE_DEAD;
    for (int i = 0; i < EXPLOSION_NUM_PARTICLES; ++i) {
        particles.push_back(std::make_shared<Particle>(position, endPosition-position));
    }
}

void EnemyUnit::advance() {
    if (state == ENEMY_STATE_NORMAL) {
        travelDistance -= speed;
        float xPos = (travelDistance) * startPosition.x + (1-travelDistance) * endPosition.x;
        float yPos = (travelDistance) * startPosition.y + (1-travelDistance) * endPosition.y;
        float zPos = (travelDistance) * startPosition.z + (1-travelDistance) * endPosition.z;
        position = glm::vec3(xPos, yPos, zPos);
        /*if (position.z < 30) {
            explode();
        }*/
    } else if (state == ENEMY_STATE_DEAD) {
        for (auto p = particles.begin(); p != particles.end();) {
            if ((*p)->ttl < 0) {
                particles.erase(p);
            } else {
                (*p)->advance();
                ++p;
            }
        }
    }
}


//////////////////////
///// Particle ///////
//////////////////////

Particle::Particle(glm::vec3 position, glm::vec3 velocity)
    : pos(position)
    , vel(velocity)
    , ttl(PARTICLE_LIFETIME)
{
    vel.x *= MAX_ENEMY_INTERP_SPEED;
    vel.y *= MAX_ENEMY_INTERP_SPEED;
    vel.z *= MAX_ENEMY_INTERP_SPEED;
    pos.x += 10*(static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    pos.y += 10*(static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    pos.z += 10*(static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    vel.x += 10*(static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    vel.y += 10*(static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    vel.z += 10*(static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
    //std::cout << "Particle: (" << pos.x << ", " << pos.y << ", " << pos.z << ") ";
    //std::cout << ",  (" << vel.x << ", " << vel.y << ", " << vel.z << ") " << std::endl;;
}

void Particle::advance() {
    ttl -= 0.05;
    vel.y -= GRAVITY;

    pos += vel;
}


