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

#define ENEMY_SPAWN_CD 60.0

#define ENEMY_SCALE 2.0
#define ENEMY_HIT_RADIUS 2.0
#define MIN_ENEMY_INTERP_SPEED 0.006
#define MAX_ENEMY_INTERP_SPEED 0.008

#define LEFT_ENEMY_SPAWN_BOUND -50.0
#define RIGHT_ENEMY_SPAWN_BOUND 50.0
#define LOWER_ENEMY_SPAWN_BOUND -5.0
#define UPPER_ENEMY_SPAWN_BOUND 30.0
#define FRONT_ENEMY_SPAWN_BOUND 80
#define BACK_ENEMY_SPAWN_BOUND 100

#define LEFT_ENEMY_END_BOUND -2.0
#define RIGHT_ENEMY_END_BOUND 2.0
#define LOWER_ENEMY_END_BOUND -5.0
#define UPPER_ENEMY_END_BOUND 1.0
#define ENEMY_END_Z -5.0


struct EnemyUnit {
    EnemyUnit(glm::vec3 startPos, glm::vec3 endPos, double speed)
        : startPosition(startPos)
        , endPosition(endPos)
        , speed(speed)
        , travelDistance(1)
    {}

    glm::vec3 position;
	glm::vec3 startPosition;
	glm::vec3 endPosition;
    double speed;
    double travelDistance;

    void advance();
};

class Enemy {
public:
    Enemy(std::string resourceDir);
    void measure();
    void draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
              const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos);

    void spawnEnemy();

    unsigned checkCollisions(glm::vec3 position, float radius);
    unsigned checkProjectile(glm::vec3 position, float radius);

    void advance();

private:
    Enemy(Enemy const &a) = delete;

    std::vector<std::shared_ptr<EnemyUnit>> enemies;

    std::vector<std::shared_ptr<Shape>> shapes;
    glm::vec3 trans;
    float scale;

};

#endif // FINAL_471_ENEMY_H_INCLUDED