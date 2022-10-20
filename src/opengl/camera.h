#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

public:
	Camera();
	Camera(glm::vec3 cameraPos, glm::vec3 target, glm::vec3 upVector,
		float FOV, float aspectRatio, float near, float far);

	void createView(glm::vec3 cameraPos, glm::vec3 target, glm::vec3 upVector);
	void createPerspective(float FOV, float aspectRatio, float near, float far);

	glm::mat4 getVP();
};

inline glm::mat4 Camera::getVP()
{
	return projectionMatrix * viewMatrix;
}

