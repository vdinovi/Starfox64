#pragma once
#ifndef FINAL_471_ARWING_H_INCLUDED
#define FINAL_471_ARWING_H_INCLUDED

#include <iostream>
#include <vector>
#include "Shape.h"
#include "Program.h"
#include "common.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>

#define ARWING_HIT_RADIUS 2.0
#define ARWING_MOVE_SPEED 0.5
#define ARWING_TURN_RATE 2.0
#define ARWING_MAX_YAW 45.0
#define ARWING_MAX_PITCH 30.0
#define ARWING_SCALE 2.0
#define ARWING_DEPTH 6.0
#define ARWING_HEIGHT 3.0


#define ARWING_PROJECTILE_SPEED 0.1
#define ARWING_PROJECTILE_DISTANCE 50
#define ARWING_PROJECTILE_HIT_RADIUS 1

struct Projectile {
    Projectile(glm::vec3 startPos, glm::vec3 endPos, double speed, float yaw, float pitch)
        : startPosition(startPos)
        , endPosition(endPos)
        , speed(speed)
        , travelDistance(1)
        , yaw(yaw)
        , pitch(pitch)
    {}
    glm::vec3 position;
	glm::vec3 startPosition;
	glm::vec3 endPosition;
    double speed;
    double travelDistance;
    float yaw;
    float pitch;

    void advance();
};

class Arwing {

public:
    Arwing(std::string resourceDir);
    //~Arwing();


    void measure();

    /* Draws the arwing.
       Preconditions:
            Uniforms (P, V, M, MatAmb, MatDiff, MatSpec, Shine, lightPos, texOffset) have been set.
            Attributes (vertPos, vertNor, vertTex) have been set
    */
    void draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
              const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos);

    // Code to advance arwing state. Called on each iteration of game loop
    void advance();

    void pitchUp(int action);
    void pitchDown(int action);
    void yawLeft(int action);
    void yawRight(int action);

    void shoot();

    std::vector<std::shared_ptr<Projectile>> projectiles;
	glm::vec3 position = glm::vec3(0.0, 0.0, ARWING_DEPTH);
	double yaw = 0.0;
	double pitch = 0.0;

private:
    Arwing(Arwing const &a) = delete;

    std::vector<std::shared_ptr<Shape>> shipShapes;
    glm::vec3 shipTrans;
    float shipScale;

    std::vector<std::shared_ptr<Shape>> projectileShapes;
    glm::vec3 projectileTrans;
    float projectileScale;

	std::vector<int> turning = {0, 0};
 };

#endif // FINAL_471_ARWING_H_INCLUDED