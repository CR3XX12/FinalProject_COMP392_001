//I promise to work on this exam on my own without receiving any help from any other persons. All of my answers will be my own.

/////////////////////////////////////////////////////////////////////////
////
//// By: Jody Xia
//// CODE ADAPTED FROM: Alireza Moghaddam
//// Work: Lab 08
//// Due: 14/11/2021 (DD/MM/YY)
////
/////////////////////////////////////////////////////////////////////////
#pragma once
#include "GameObject.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include <iostream>

#include "camera.h"
#include "Tank.h"
#include "Bullet.h"

extern int gs;

class Player : public GameObject
{
	size_t tanksShot = 0;
	size_t spawnTime = 0;
public:
	Player() : GameObject(cam::cam_pos, cam::looking_dir_vector) {
		setDimensions(0.1, 0.1, 0.4);
	};

	void incTanksShot() { tanksShot++; }
	size_t getTanksShot() { return tanksShot; }

	//keeps the player gameObject synchronized with the camera
	virtual void updatePosition() {
		setPosition(cam::cam_pos);
		setDirection(cam::looking_dir_vector);
	}

	//no need to draw the player
	virtual void draw(){}

	//collision detection
	virtual void checkCollision(GameObject* obj) {
		
		if (auto bullet = dynamic_cast<Bullet*>(obj))
		{
			if (isColliding(bullet))
			{
				if (bullet->getSource() != nullptr)
					if (auto tank = dynamic_cast<Tank*>(bullet->getSource()))
						gs = 2; //defeat
			}
		}
	
	};

	virtual void idle() {
		spawnTime += deltaTime;
		if (getTanksShot() >= 5)
		{
			if (spawnTime < 20000)
				gs = 1; //victory
			else
				gs = 3; //shot but not fast enough
		}
	}


};