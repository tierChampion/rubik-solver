#include "camera.h"

/* Default values: looks from 0, 0, 1 at 0, 0, 0 */
Camera::Camera() : viewMatrix(glm::mat4(0.0f)), projectionMatrix(glm::mat4(0.0f)) {}

Camera::Camera(glm::vec3 cameraPos, glm::vec3 target, glm::vec3 upVector,
	float FOV, float aspectRatio, float near, float far)
{
	createView(cameraPos, target, upVector);
	createPerspective(FOV, aspectRatio, near, far);
}

void Camera::createView(glm::vec3 cameraPos, glm::vec3 target, glm::vec3 upVector)
{
	viewMatrix = glm::lookAt(
		cameraPos,
		target,
		upVector);
}

void Camera::createPerspective(float FOV, float aspectRatio, float near, float far)
{
	projectionMatrix = glm::perspective(
		glm::radians(FOV),
		aspectRatio,
		near, far);
}
