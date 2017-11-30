#include "Interface.h"
#include "Arwing.h"
#include "GLSL.h"

Interface::Interface(unsigned maxHealth)
    //: maxHealth(maxHealth)
{
    (void) maxHealth;
    std::vector<float> positions = {
        -1.0, -0.5, 0.0,
         0.0, -0.5, 0.0,
         1.0, -0.5, 0.0,

        -1.0,  0.5, 0.0,
         0.0,  0.5, 0.0,
         1.0,  0.5, 0.0,
    };
    std::vector<float> colors = {
        1.0, 0.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 0.0,
    };
    std::vector<unsigned> indices = {
        0, 1, 4,
        0, 4, 3,
        1, 2, 5,
        1, 5, 4,
    };

    hbarShape = std::make_shared<vao_t>();
    CHECKED_GL_CALL(glGenVertexArrays(1, &hbarShape->vaoId));
    CHECKED_GL_CALL(glBindVertexArray(hbarShape->vaoId));
	CHECKED_GL_CALL(glGenBuffers(1, &hbarShape->vboId[0]));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, hbarShape->vboId[0]));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(float), &positions[0], GL_STATIC_DRAW));

	CHECKED_GL_CALL(glGenBuffers(1, &hbarShape->vboId[1]));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, hbarShape->vboId[1]));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(float), &colors[0], GL_STATIC_DRAW));

	CHECKED_GL_CALL(glGenBuffers(1, &hbarShape->iboId));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hbarShape->iboId));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
    hbarShape->numElements = indices.size();

	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Interface::draw(const std::shared_ptr<Program> colorProg,
                     const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> M,
                     const glm::mat4& V, unsigned health)
{
    (void)health;
    colorProg->bind();
	M->pushMatrix();
        M->translate(glm::vec3(1, 2, ARWING_DEPTH+5));
        M->scale(glm::vec3(0.5, 0.5, 0));
		glUniformMatrix4fv(colorProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(colorProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(colorProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        render(colorProg);
    M->popMatrix();
    colorProg->unbind();
}

void Interface::render(const std::shared_ptr<Program> colorProg)
{
	CHECKED_GL_CALL(glBindVertexArray(hbarShape->vaoId));

    int h_pos = colorProg->getAttribute("vertPos");
    GLSL::enableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, hbarShape->vboId[0]));
	CHECKED_GL_CALL(glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));

    int h_col = colorProg->getAttribute("vertCol");
    GLSL::enableVertexAttribArray(h_col);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, hbarShape->vboId[1]));
	CHECKED_GL_CALL(glVertexAttribPointer(h_col, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));

    CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hbarShape->iboId));
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, hbarShape->numElements, GL_UNSIGNED_INT, (const void *)0));

	GLSL::disableVertexAttribArray(colorProg->getAttribute("vertPos"));
	GLSL::disableVertexAttribArray(colorProg->getAttribute("vertCol"));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}




void Interface::advance() {}

