#pragma once
#ifndef FINAL_471_ARWING_H_INCLUDED
#define FINAL_471_ARWING_H_INCLUDED

#include <iostream>
#include <vector>
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>

#define ARWING_MOVE_SPEED 0.5
#define ARWING_TURN_RATE 2.0
#define ARWING_MAX_TURN 45.0
#define ARWING_SCREEN_BOUND 5.0 // @TODO rename -- more specific

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


private:
    Arwing(Arwing const &a) = delete;

    std::vector<std::shared_ptr<Shape>> shapes;
    glm::vec3 trans;
    float scale;

	glm::vec3 position = glm::vec3(0.0, 0.0, 3.0);
	std::vector<int> turning = {0, 0};
	double yaw = 0.0;
	double pitch = 0.0;

 };

#endif // FINAL_471_ARWING_H_INCLUDED