#pragma once
#ifndef FINAL_471_INTERFACE_H_INCLUDED
#define FINAL_471_INTERFACE_H_INCLUDED

#include "Shape.h"
#include "common.h"
#include "Program.h"
#include "MatrixStack.h"

class Interface {
public:
    Interface(unsigned maxHealth);
    //~Arwing();

    void draw(const std::shared_ptr<Program> colorProg,
              const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> M,
              const glm::mat4& V, unsigned health);

    void advance();


private:
    Interface(Interface const &a) = delete;

    void render(const std::shared_ptr<Program> colorProg);

    // "hbar" == "health bar"
    std::shared_ptr<vao_t> hbarShape;

    unsigned maxHealth;

};



#endif // FINAL_471_INTERFACE_H_INCLUDED
