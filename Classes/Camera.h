#pragma once
#include <glad/glad.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.f;

class Camera {
public :

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 UpWorld;
	glm::vec3 Eye;
	glm::vec3 At = glm::vec3(0, 0, 0);

	float Yaw;
	float Pitch;
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	float Distance;

	Camera(glm::vec3 position = glm::vec3(0, 10.0f, 0), glm::vec3 up = glm::vec3(0.f, 1.f, 0.f),
		float yaw = YAW, float pitch = PITCH);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	void SetTarget(glm::vec3 posTarget);
	void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseScroll(float yoffset);
private:
	void updateCameraVectors();
};