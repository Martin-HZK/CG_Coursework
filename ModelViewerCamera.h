#pragma once

#include <stdio.h>
#include <glm/glm/glm.hpp>
#include "camera.h"

float cam_dist = 4.f;

void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset)
{
	/*in.Yaw -= xoffset;
	in.Pitch -= yoffset;*/
	in.Yaw -= xoffset * in.MovementSpeed;
	in.Pitch -= yoffset * in.MovementSpeed;

	// make sure that Yaw and Pitch are in the form of radius
		/*in.Yaw = glm::radians(in.Yaw);
		in.Pitch = glm::radians(in.Pitch);*/
		if (in.Pitch > 89.0f)
			in.Pitch = 89.0f;
		if (in.Pitch < -89.0f)
			in.Pitch = -89.0f;
		// calculate the new position of the camera
	in.Position.x = cam_dist * cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
	in.Position.y = cam_dist * sin(glm::radians(in.Pitch));
	in.Position.z = cam_dist * sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));


	in.Front = glm::normalize(target - in.Position);
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));


}
