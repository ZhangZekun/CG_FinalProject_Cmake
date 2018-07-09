//#include"Camera.h"
//#include<iostream>
//
//Camera::Camera(glm::vec3 position, glm::vec3 up , float yaw , float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//{
//	Position = position;
//	WorldUp = up;
//	Yaw = yaw;
//	Pitch = pitch;
//	updateCameraVectors();
//}
//
//
//glm::mat4 Camera::GetViewMatrix()
//{
//	return glm::lookAt(Position, Position + Front, Up);
//}
//
//void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
//{
//	xoffset *= MouseSensitivity;
//	yoffset *= MouseSensitivity;
//
//	Yaw += xoffset;
//	Pitch += yoffset;
//	std::cout << Yaw << " " << Pitch << std::endl;
//	if (constrainPitch)
//	{
//		if (Pitch > 89.0f)
//			Pitch = 89.0f;
//		if (Pitch < -89.0f)
//			Pitch = -89.0f;
//	}
//
//	updateCameraVectors();
//}
//
//void Camera::ProcessMouseScroll(float yoffset)
//{
//	if (Zoom >= 1.0f && Zoom <= 45.0f)
//		Zoom -= yoffset;
//	if (Zoom <= 1.0f)
//		Zoom = 1.0f;
//	if (Zoom >= 45.0f)
//		Zoom = 45.0f;
//}
//
//void Camera::moveForward(float deltaTime) {
//	float velocity = MovementSpeed * deltaTime;
//	Position += Front * velocity;
//}
//void Camera::moveBack(float deltaTime) {
//	float velocity = MovementSpeed * deltaTime;
//	Position -= Front * velocity;
//}
//void Camera::moveRight(float deltaTime) {
//	float velocity = MovementSpeed * deltaTime;
//	Position += Right * velocity;
//}
//void Camera::moveLeft(float deltaTime) {
//	float velocity = MovementSpeed * deltaTime;
//	Position -= Right * velocity;
//}
//
//
//void Camera::updateCameraVectors()
//{
//	// Calculate the new Front vector
//	glm::vec3 front;
//	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//	front.y = sin(glm::radians(Pitch));
//	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//	Front = glm::normalize(front);
//	Right = glm::normalize(glm::cross(Front, WorldUp));
//	Up = glm::normalize(glm::cross(Right, Front));
//}



#include"Camera.h"
#include<iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}


glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;
	std::cout << Yaw << " " << Pitch << std::endl;
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

void Camera::moveForward(float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	Position += Front * velocity;
}
void Camera::moveBack(float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	Position -= Front * velocity;
}
void Camera::moveRight(float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	Position += Right * velocity;
}
void Camera::moveLeft(float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	Position -= Right * velocity;
}


void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front1;
	glm::vec3 front2;
	//yaw, then pitch, then roll, which means rotate around y, x, z in order, is correct.
	//first Roll, then pitch, then yaw. Rotate around z, x, y
	front1.x = -sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front1.y = sin(glm::radians(Pitch));
	front1.z = -cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	//first yaw, then pitch, then roll. Rotate around y, x, z
	front2.x = -sin(glm::radians(Yaw));
	front2.y = sin(glm::radians(Pitch)) * cos(glm::radians(Yaw));
	front2.z = -cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));

	Front = glm::normalize(front1);
	glm::vec3 Front2 = glm::normalize(front2);
	std::cout << "pitch yaw: " << Pitch << " " << Yaw << std::endl;
	std::cout << Front.x << " " << Front.y << " " << Front.z << std::endl;
	std::cout << Front2.x << " " << Front2.y << " " << Front2.z << std::endl;

	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}