#pragma once
#ifndef FINAL_471_ENEMY_H_INCLUDED
#define FINAL_471_ENEMY_H_INCLUDED

#include <iostream>
#include <vector>
#include "Shape.h"
#include "Program.h"
#include "common.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>

#define PARTICLE_LIFETIME 10
#define EXPLOSION_NUM_PARTICLES 10
#define PARTICLE_SCALE 0.2
#define GRAVITY 0.2
#define WIND_RESISTANCE 0.1

#define ENEMY_SPAWN_CD 60.0

#define ENEMY_SCALE 3.0
#define ENEMY_HIT_RADIUS 3.0
#define MIN_ENEMY_INTERP_SPEED 0.004
#define MAX_ENEMY_INTERP_SPEED 0.006

#define LEFT_ENEMY_SPAWN_BOUND 60.0
#define RIGHT_ENEMY_SPAWN_BOUND -60.0
#define LOWER_ENEMY_SPAWN_BOUND -8.0
#define UPPER_ENEMY_SPAWN_BOUND 50.0
#define FRONT_ENEMY_SPAWN_BOUND 80
#define BACK_ENEMY_SPAWN_BOUND 100

#define LEFT_ENEMY_END_BOUND -2.0
#define RIGHT_ENEMY_END_BOUND 2.0
#define LOWER_ENEMY_END_BOUND -5.0
#define UPPER_ENEMY_END_BOUND 2.0
#define ENEMY_END_Z 0.0

#define ENEMY_STATE_NORMAL 0
#define ENEMY_STATE_DEAD 1
#define ENEMY_STATE_DONE 2

struct Particle {
    Particle(glm::vec3 pos, glm::vec3 vel);

    void advance();

    glm::vec3 pos;
    glm::vec3 vel;

    double ttl;
};

struct EnemyUnit {
    EnemyUnit(glm::vec3 startPos, glm::vec3 endPos, double speed);

    glm::vec3 currentPosition;
    std::vector<glm::vec3> targetPositions;
    unsigned nextPosition;
    size_t numPositions;
    double speed;
    double yaw;
    double pitch;
    double travelDistance;

    int state = ENEMY_STATE_NORMAL;


    std::vector<std::shared_ptr<Particle>> particles;
    unsigned particleVAO;
    unsigned pointsVBO;

    void advance();
    void explode();
    void updateParticles();
};

class Enemy {
public:
    Enemy(std::string resourceDir);
    void measure();
    void draw(const std::shared_ptr<Program> prog, const std::shared_ptr<Program> explosionProg,
              const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> M,
              const glm::mat4& V, const glm::vec3& lightPos);

    void spawnEnemy();

    unsigned checkCollisions(glm::vec3 position, float radius);

    std::vector<std::shared_ptr<EnemyUnit>> checkProjectile(glm::vec3 position, float radius);

    void advance();

private:
    Enemy(Enemy const &a) = delete;

    std::vector<std::shared_ptr<EnemyUnit>> enemies;

    std::vector<std::shared_ptr<Shape>> shapes;
    glm::vec3 trans;
    float scale;
};

#endif // FINAL_471_ENEMY_H_INCLUDED