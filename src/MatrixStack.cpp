
#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>

static const int MaxMatrixSize = 100;


MatrixStack::MatrixStack()
{
	stack.push(glm::mat4(1.0));
}

void MatrixStack::pushMatrix()
{
	const glm::mat4 &top = stack.top();
	stack.push(top);
	assert(stack.size() < MaxMatrixSize);
}

void MatrixStack::popMatrix()
{
	assert(!stack.empty());
	stack.pop();

	// There should always be one matrix left.
	assert(!stack.empty());
}

void MatrixStack::loadIdentity()
{
	glm::mat4 &top = stack.top();
	top = glm::mat4(1.f);
}

 void MatrixStack::perspective(float fovy, float aspect, float zNear, float zFar)
{
	glm::mat4 &top = stack.top();
	top *= glm::perspective(fovy, aspect, zNear, zFar);
}

void MatrixStack::translate(const glm::vec3 &offset)
{
	glm::mat4 &top = stack.top();
	glm::mat4 t = glm::translate(glm::mat4(1.f), offset);
	top *= t;
}

void MatrixStack::scale(const glm::vec3 &scaleV)
{
	glm::mat4 &top = stack.top();
	glm::mat4 s = glm::scale(glm::mat4(1.f), scaleV);
	top *= s;
}

void MatrixStack::scale(float size)
{
	glm::mat4 &top = stack.top();
	glm::mat4 s = glm::scale(glm::mat4(1.f), glm::vec3(size));
	top *= s;
}

void MatrixStack::rotate(float angle, const glm::vec3 &axis)
{
	glm::mat4 &top = stack.top();
	glm::mat4 r = glm::rotate(glm::mat4(1.0), angle, axis);
	top *= r;
}

void MatrixStack::multMatrix(const glm::mat4 &matrix)
{
	glm::mat4 &top = stack.top();
	top *= matrix;
}

void MatrixStack::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	assert(left != right);
	assert(bottom != top);
	assert(zFar != zNear);

	glm::mat4 &ctm = stack.top();
	ctm *= glm::ortho(left, right, bottom, top, zNear, zFar);
}

void MatrixStack::frustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	glm::mat4 &ctm = stack.top();
	ctm *= glm::frustum(left, right, bottom, top, zNear, zFar);
}

void MatrixStack::lookAt(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up)
{
	glm::mat4 &top = stack.top();
	top *= glm::lookAt(eye, target, up);
}

const glm::mat4 &MatrixStack::topMatrix() const
{
	return stack.top();
}

void MatrixStack::print(const glm::mat4 &mat, const char *name)
{
	if (name)
	{
		printf("%s = [\n", name);
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			printf("%- 5.2f ", mat[i][j]);
		}
		printf("\n");
	}

	if (name)
	{
		printf("];");
	}
	printf("\n");
}

void MatrixStack::print(const char *name) const
{
	print(stack.top(), name);
}
