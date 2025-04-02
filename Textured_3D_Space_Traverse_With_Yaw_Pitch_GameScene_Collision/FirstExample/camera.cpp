//I promise to work on this exam on my own without receiving any help from any other persons. All of my answers will be my own.
#pragma once
#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "camera.h"


namespace cam {
	float height = 0.8f;

	//Player motion speed for movement and pitch/yaw
	float travel_speed = 25.0f;		//Motion speed

	glm::vec3 unit_z_vector = glm::vec3(0, 0, 1);	//Assigning a meaningful name to (0,0,1) :-)
	glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, height);


	glm::vec3 forward_vector = glm::vec3(1, 1, 0);	//Forward vector is parallel to the level at all times (No pitch)
	glm::vec3 looking_dir_vector = glm::vec3(1, 1, 0);
	glm::vec3 up_vector = unit_z_vector;
	glm::vec3 side_vector = glm::cross(up_vector, forward_vector);

	void init()
	{
		//Normalizing all vectors
		up_vector = glm::normalize(up_vector);
		forward_vector = glm::normalize(forward_vector);
		looking_dir_vector = glm::normalize(looking_dir_vector);
		side_vector = glm::normalize(side_vector);
	}
}