#pragma once
#ifndef FINAL_471_ENVIRONMENT_H_INCLUDED
#define FINAL_471_ENVIRONMENT_H_INCLUDED

#include <iostream>
#include <vector>
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>

#define TEX_SCROLL_RATE 0.003
#define GROUND_WIDTH 50
#define GROUND_DEPTH 30

#define SKY_WIDTH 80
#define SKY_HEIGHT 40


class Environment {
public:
    Environment(std::string resourceDir);
    //~Environment();


    void draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P,
              const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos);

    // Code to advance environment. Called on each iteration of the game loop
    void advance();


private:
    Environment(Environment const &a) = delete;
	glm::vec2 texOffset = {0.0, 0.0};
    std::vector<std::shared_ptr<Shape>> groundShapes;
    std::vector<std::shared_ptr<Shape>> skyShapes;

};

#endif // FINAL_471_ENVIRONMENT_H_INCLUDED