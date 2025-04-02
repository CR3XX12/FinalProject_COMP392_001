//I promise to work on this exam on my own without receiving any help from any other persons. All of my answers will be my own.
#pragma once
#include "vgl.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"

/*************************************************

	This header file contains all global 
	variables that deal with the camera, 
	including the model_view matrix

	Mouse movement variables are not included

**************************************************/


namespace cam
{
	// Height of camera(player) from the level
	extern float height;

	//Player motion speed for movement and pitch/yaw
	extern float travel_speed;		//Motion speed

	extern glm::vec3 cam_pos;


	extern glm::vec3 forward_vector;
	extern glm::vec3 looking_dir_vector;
	extern glm::vec3 up_vector;
	extern glm::vec3 side_vector;


	void init();
}