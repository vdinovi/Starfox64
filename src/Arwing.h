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

#define NOT_YAWING 0
#define YAWING_LEFT -1
#define YAWING_RIGHT 1
#define NOT_PITCHING 0
#define PITCHING_DOWN -1
#define PITCHING_UP 1
#define NOT_ROLLING 0
#define ROLLING_LEFT -1
#define ROLLING_RIGHT 1

#define ARWING_SCALE 2.0
#define ARWING_HEIGHT 0.0
#define ARWING_DEPTH 15.0
#define ARWING_MAX_YAW 45.0
#define ARWING_MAX_PITCH 30.0
#define ARWING_MAX_ROLL 90.0
#define ARWING_MOVE_SPEED 0.5
#define ARWING_TURN_RATE 1.5
#define ARWING_ROLL_RATE 2.0
#define ARWING_BARREL_ROLL_RATE 8.0
#define ARWING_HIT_RADIUS 2.0

#define ARWING_PROJECTILE_SPEED 0.1
#define ARWING_PROJECTILE_DISTANCE 50
#define ARWING_PROJECTILE_HIT_RADIUS 1

#define ARWING_MAX_HEALTH 10
#define ARWING_NORMAL 0
#define ARWING_DAMAGED 1

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
    void draw(const std::shared_ptr<Program> textureProg, const std::shared_ptr<Program> exhaustProg,
              const std::shared_ptr<Program> crosshairProg, const std::shared_ptr<MatrixStack> P,
              const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos);

    // Code to advance arwing state. Called on each iteration of game loop
    void advance();

    void pitchUp(int action);
    void pitchDown(int action);
    void yawLeft(int action);
    void yawRight(int action);
    void rollLeft(int action);
    void rollRight(int action);
    void barrelRoll();

    void shoot();

    std::vector<std::shared_ptr<Projectile>> projectiles;
	glm::vec3 position = glm::vec3(0.0, 0.0, ARWING_DEPTH);
	double yaw = 0.0;
	double pitch = 0.0;
    double roll = 0.0;

private:
    Arwing(Arwing const &a) = delete;

    std::vector<std::shared_ptr<Shape>> shipShapes;
    glm::vec3 shipTrans;
    float shipScale;

    std::shared_ptr<vao_t> exhaustLight;

    std::vector<std::shared_ptr<Shape>> projectileShapes;
    glm::vec3 projectileTrans;
    float projectileScale;

    std::shared_ptr<Shape> crosshairShape;
    //float exhaustTime = 0.0;

    int yawing = NOT_YAWING;
    int pitching = NOT_PITCHING;
    int rolling = NOT_ROLLING;

    bool barrelRollState = false;
    double barrelRollAmnt = 0.0;


    //int state = ARWING_NORMAL;

 };

#endif // FINAL_471_ARWING_H_INCLUDED