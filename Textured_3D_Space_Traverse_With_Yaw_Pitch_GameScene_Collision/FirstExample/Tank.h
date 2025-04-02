//I promise to work on this exam on my own without receiving any help from any other persons. All of my answers will be my own.

/////////////////////////////////////////////////////////////////////////
////
//// By: Jody Xia
//// CODE ADAPTED FROM: Alireza Moghaddam
//// Work: Final Project
////
/////////////////////////////////////////////////////////////////////////
#pragma once
#include "GameObject.h"
#include "glm\glm.hpp"

//Tank class
class Tank : public GameObject
{
	static GLfloat vertexMesh[24][3]; //mesh of vertices;
	static GLfloat textureMesh[24][2]; //mesh of textures;
	static glm::vec3 wheelPositions[4]; //array of vectors representing the positions of the wheels as offsets from the center of the tank

	int lastBulletShot = glutGet(GLUT_ELAPSED_TIME);
public:
	Tank();

	virtual void updatePosition();

	virtual void draw();

	virtual void checkCollision(GameObject* other);

	virtual void idle();

private:
	class Wheel : public GameObject
	{
		static GLfloat vertexMesh[16][3];
		static GLfloat textureMesh[16][2];

	public:
		virtual void updatePosition() {}; //has no children; no need to update child positions

		virtual void draw(); 

		virtual void checkCollision(GameObject* other) {} //has no collision; collision is handled by the main body
	};
};
