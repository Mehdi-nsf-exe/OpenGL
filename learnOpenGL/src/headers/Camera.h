#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#define DEFLT_PITCH					0.0f
#define DEFLT_YAW						-90.0f
#define DEFLT_CAM_SPEED				2.5f
#define DEFLT_MOUSE_SENSITIVITY	0.1f
#define DEFLT_ZOOM					45.0f

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
private:

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;

public:
	/**
		Creates and initializes a new camera instance.

		@param pos		The starting position of the camera.
		@param up		The up vector of the camara.
		@param yaw		The starting yaw of the camera.
		@param pitch	The starting pitch of the camera.
	 */
	Camera(
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float yaw = DEFLT_YAW, 
		float pitch = DEFLT_PITCH
	);

	/**
		Gets the position of the camera in world space.
	 */
	inline glm::vec3 getPosition() const {
		return position;
	}

	/**
		Gets the front of the camera.
	 */
	inline glm::vec3 getFront() const {
		return front;
	}

	/**
		Gets the zoom of the camera.
	 */
	inline float getZoom() const {
		return zoom;
	}

	/**
		gets the view matrix of the camera.

		@return the view matrix of the camera.
	 */
	inline glm::mat4 getViewMatrix() const {
		return glm::lookAt(position, position + front, up);
	}

	/**
		Moves the camera in the indicated direction.

		@param movementDirecion	The direction towards the camera is to be moved.
		@param deltaTime			The duration of he last frame.
	 */
	void move(CameraMovement movementDirection, float deltaTime);

	/**
		Updates the orientation of the camera based on mouse-like input systems (mouse, joystick, etc.)

		@param xOffset			The x axis offset of the mouse-like input device.
		@param yOffset			The y axis offset of the mouse-like input device.
		@param constrainPich	indicates whether to constrain the pitch of the camera.
	*/
	void updateOrientation(float xOffset, float yOffset, bool constrainPitch = true);

	/**
		Updates the zoom of the camera based on the offset of a mouse scroll weel-like input system.

		@param yOffset the y axis offset of hte input device.
	 */
	void updateZoom(float yOffset);

private:
	/**
		Calculates and updates the camera vectors from the camera's (updated) Euler Angles.
	 */
	void updateCameraVectors();
};