#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include "camera.h"

void MoveCamera(SCamera& in, SCamera::Camera_Movement direction)
{
    // ���ղ�ͬ�ķ����ƶ����
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
    // ����ƫ���Ǻ͸�����
    in.Yaw += xoffset * in.MouseSensitivity;
    in.Pitch += yoffset * in.MouseSensitivity;

    // ���Ƹ����ǣ����ⳬ�� 89 ��
    if (in.Pitch > 89.0f)
        in.Pitch = 89.0f;
    if (in.Pitch < -89.0f)
        in.Pitch = -89.0f;

    // ���������ǰ����
    glm::vec3 front;
    front.x = cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
    front.y = sin(glm::radians(in.Pitch));
    front.z = sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
    in.Front = glm::normalize(front);

    // �������������
    in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
    in.Up = glm::normalize(glm::cross(in.Right, in.Front));

}
