#include"Camera.h"

Camera:: Camera(glm::vec3 position, glm::vec3 up,
	float yaw, float pitch ) :Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
	this->Position = position;
	this->UpWorld = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	UpWorld = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(this->Position, this->Position + this->Front, this->UpWorld);
}
void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
	xOffset *= MouseSensitivity;
	yOffset *= MouseSensitivity;

	this->Yaw += xOffset;
	this->Pitch += yOffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}
	updateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
	float velocity = this->MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
	Position.y = 0;
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

void Camera::updateCameraVectors() {

	glm::vec3 front;
	front.y = sin(glm::radians(Pitch));
	front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
	front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, this->UpWorld));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}