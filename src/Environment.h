#pragma once
#ifndef FINAL_471_ENVIRONMENT_H_INCLUDED
#define FINAL_471_ENVIRONMENT_H_INCLUDED

#include <iostream>
#include <vector>
#include "Shape.h"
#include "Program.h"
#include "common.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>

#define TEX_SCROLL_RATE 0.003
#define GROUND_WIDTH 80
#define GROUND_DEPTH 40

#define SKY_WIDTH 90
#define SKY_HEIGHT 50


class Environment {
public:
    Environment(std::string resourceDir);
    //~Environment();


    void draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
              const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos);

    // Code to advance environment. Called on each iteration of the game loop
    void advance();

    //bool checkGroundCollision(glm::vec3 position, double radius);


private:
    Environment(Environment const &a) = delete;
	glm::vec2 texOffset = {0.0, 0.0};
    std::vector<std::shared_ptr<Shape>> groundShapes;
    std::vector<std::shared_ptr<Shape>> skyShapes;
    std::map<std::string, unsigned> textures;
    //float groundHeight = -(AIRSPACE_HEIGHT+1);

};

#endif // FINAL_471_ENVIRONMENT_H_INCLUDED