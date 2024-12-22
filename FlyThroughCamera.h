#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include "camera.h"

void MoveCamera(SCamera& in, SCamera::Camera_Movement direction, float deltaTime)
{
    float velocity = in.MovementSpeed * deltaTime * 5;

    if (direction == SCamera::FORWARD)
        in.Position += in.Front * velocity;
    if (direction == SCamera::BACKWARD)
		in.Position -= in.Front * velocity;
    if (direction == SCamera::LEFT)
		in.Position -= in.Right * velocity;
    if (direction == SCamera::RIGHT)
        in.Position += in.Right * velocity;
}

void OrientCamera(SCamera& in, float xoffset, float yoffset, float deltaTime)
{
    in.Yaw += xoffset * in.MouseSensitivity * deltaTime;
    in.Pitch += yoffset * in.MouseSensitivity * deltaTime;

    if (in.Pitch > 89.0f)
        in.Pitch = 89.0f;
    if (in.Pitch < -89.0f)
        in.Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
    front.y = sin(glm::radians(in.Pitch));
    front.z = sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
    in.Front = glm::normalize(front);

    in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
    in.Up = glm::normalize(glm::cross(in.Right, in.Front));

}
