#include <iostream>
#include <vector>
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>

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
              const std::shared_ptr<MatrixStack> M, const glm::mat4& V, const glm::vec3& lightPos,
              const glm::vec2& texOffset);

    void pitchUp(int action);
    void pitchDown(int action);
    void yawLeft(int action);
    void yawRight(int action);



private:
    Arwing(Arwing const &a) = delete;

    std::vector<std::shared_ptr<Shape>> shapes;
	glm::vec3 position = glm::vec3(0.0, 0.0, 5.0);
    glm::vec3 trans;
    float scale;
	double yaw = 0.0;
	double pitch = 0.0;
};