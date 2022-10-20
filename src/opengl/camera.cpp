#include "camera.h"

Camera::Camera() : _viewMatrix(glm::mat4(0.0f)), _projectionMatrix(glm::mat4(0.0f)) {}

/**
* @param cameraPos - position of the camera
* @param target - position that the camera looks at
* @param upVector - upwards direction of the camera
* @param FOV - field of view in degrees
* @param aspectRatio - ratio of the screen
* @param near - distance to the near plane
* @param far - distance to the far plane
*/
Camera::Camera(glm::vec3 cameraPos, glm::vec3 target, glm::vec3 upVector,
	float FOV, float aspectRatio, float near, float far)
{
	createView(cameraPos, target, upVector);
	createPerspective(FOV, aspectRatio, near, far);
}

/**
* Compute the desired view matrix.
* @param cameraPos - position of the camera
* @param target - location to look at
* @param upVector - upwards direction of the camera
*/
void Camera::createView(glm::vec3 cameraPos, glm::vec3 target, glm::vec3 upVector)
{
	_viewMatrix = glm::lookAt(
		cameraPos,
		target,
		upVector);
}

/**
* Compute the desired perspective matrix.
* @param FOV - field of view in degrees
* @param aspectRatio - ratio of the screen
* @param near - distance to the near plane
* @param far - distance to the far plane
*/
void Camera::createPerspective(float FOV, float aspectRatio, float near, float far)
{
	_projectionMatrix = glm::perspective(
		glm::radians(FOV),
		aspectRatio,
		near, far);
}
