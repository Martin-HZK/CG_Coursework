#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include "camera.h"

void MoveCamera(SCamera& in, SCamera::Camera_Movement direction)
{
    // 按照不同的方向移动相机
    if (direction == SCamera::FORWARD)
        in.Position += in.Front * in.MovementSpeed;
    if (direction == SCamera::BACKWARD)
        in.Position -= in.Front * in.MovementSpeed;
    if (direction == SCamera::LEFT)
        in.Position -= in.Right * in.MovementSpeed;
    if (direction == SCamera::RIGHT)
        in.Position += in.Right * in.MovementSpeed;
    /*if (direction == SCamera::UP)
        in.Position += in.Up * in.MovementSpeed;
    if (direction == SCamera::DOWN)
        in.Position -= in.Up * in.MovementSpeed;*/
}

void OrientCamera(SCamera& in, float xoffset, float yoffset)
{
    // 调整偏航角和俯仰角
    in.Yaw += xoffset * in.MouseSensitivity;
    in.Pitch += yoffset * in.MouseSensitivity;

    // 限制俯仰角，避免超出 89 度
    if (in.Pitch > 89.0f)
        in.Pitch = 89.0f;
    if (in.Pitch < -89.0f)
        in.Pitch = -89.0f;

    // 更新相机的前向方向
    glm::vec3 front;
    front.x = cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
    front.y = sin(glm::radians(in.Pitch));
    front.z = sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
    in.Front = glm::normalize(front);

    // 更新右向和上向
    in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
    in.Up = glm::normalize(glm::cross(in.Right, in.Front));

}
