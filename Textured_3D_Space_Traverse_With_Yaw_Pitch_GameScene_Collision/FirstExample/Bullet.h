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
#include "Texture.h"


class Bullet : public GameObject
{
	static GLfloat vertexMesh[24][3];
	static GLfloat textureMesh[24][2];
	int spawnedTime = 0; // time when the bullet was spawned
	GameObject* source { nullptr }; //pointer to the source of the bullet (was fired by X)
public:
	Bullet(glm::vec3 loc, glm::vec3 dir, GameObject* src = nullptr) : GameObject(loc, dir) {
		setDimensions(0.25, 0.25, 0.25);
		setMoveSpeed(10.f);
		source = src;
		spawnedTime = glutGet(GLUT_ELAPSED_TIME);
	};

	
	virtual void updatePosition() {
		//moves the bullet in the direction it was fired 
		setPosition(getPosition() + (getDirection() * (getMoveSpeed() * deltaTime / 1000.f)));
	}

	//draws the bullet
	virtual void draw()
	{	
		
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexMesh), vertexMesh, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureMesh), textureMesh, GL_STATIC_DRAW);

		glm::mat4 model_view = glm::mat4(1);

		model_view = glm::translate(model_view, getPosition()); //translate draw at object's location
		model_view = glm::scale(model_view, glm::vec3(0.25, 0.25, 0.25));
		glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
		glBindTexture(GL_TEXTURE_2D, Texture::get(TextureID::bullet));
		glDrawArrays(GL_QUADS, 0, 24);
	}

	//idle
	virtual void idle()
	{
		if (glutGet(GLUT_ELAPSED_TIME) - spawnedTime >= 2000) //if 2 seconds have passed since the bullet's spawn
			willBeErased = true; //mark the object for erasure 
		if (source != nullptr && source->willBeErased) //disassociate from the source if the source will be deleted (idle check is done before deletion check); may run into issues when there is willBeErased is marked at the idle check, but that is out of the scope of this work for now.
			source = nullptr;
	}

	inline GameObject* getSource() {return source;}

	//collision detection
	virtual void checkCollision(GameObject*) {};
};
