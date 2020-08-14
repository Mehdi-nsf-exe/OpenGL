#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch) :
	position(pos),
	worldUp(up),
	yaw(yaw),
	pitch(pitch),
	movementSpeed(DEFLT_CAM_SPEED),
	mouseSensitivity(DEFLT_MOUSE_SENSITIVITY),
	zoom(DEFLT_ZOOM)
{
	updateCameraVectors();
}

void Camera::move(CameraMovement movementDirection, float deltaTime) {
	float velocity = movementSpeed * deltaTime;
	switch (movementDirection) {
	case CameraMovement::FORWARD:
		position += front * velocity;
		break;
	case CameraMovement::BACKWARD:
		position -= front * velocity;
		break;
	case CameraMovement::LEFT:
		position -= right * velocity;
		break;
	case CameraMovement::RIGHT:
		position += right * velocity;
		break;
	default:
		break;
	}
}

void Camera::updateOrientation(float xOffset, float yOffset, bool constrainPitch) {
	yaw += xOffset * mouseSensitivity;
	pitch += yOffset * mouseSensitivity;

	if (constrainPitch) {
		if (pitch > 89.0f) {
			pitch = 89.0f;
		}
		else if (pitch < -89.0f) {
			pitch = -89.0f;
		}
	}
	updateCameraVectors();
}

void Camera::updateZoom(float yOffset) {
	zoom -= yOffset;
	if (zoom < 1.0f) {
		zoom = 1.0f;
	}
	else if (zoom > 45.0f) {
		zoom = 45.0f;
	}
}

void Camera::updateCameraVectors() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}