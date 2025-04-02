
#include "Tank.h"
#include "GameObject.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "camera.h"
#include "Texture.h"
#include <memory>
#include <vector>
#include "Player.h"
#include "Bullet.h"

extern std::vector<std::shared_ptr<GameObject>> gameScene;
extern int gs; //gamestate

Tank::Tank() : GameObject(glm::vec3(), glm::vec3(1, 0, 0))
{
	float x = randomFloat(-25, 25); //spawns at a random point no further than 25 units from the center
	float y = randomFloat(-25, 25); //spawns at a random point no further than 25 units from the center
	setPosition(glm::vec3(x, y, 0.5f)); //sets its position
	setMoveSpeed(randomFloat(0.75,1.25)); //sets its move speed
	setDimensions(4.0, 2.4, 3.5); //sets the dimensions it will use for collision (x,y,z)

	for (int i = 0; i < 4; i++)
	{
		getChildNodes().push_back(std::shared_ptr<GameObject>(new Wheel())); //adds 4 wheels
	}


	glm::vec3 v = glm::normalize(glm::vec3(randomFloat(-1, 1), randomFloat(-1, 1), 0.0)); //random direction, normalized

	//normalization turns 0's into nan's for some reason... turn them back to 0's
	if (glm::isnan(v.x))
		v.x = 0;
	if (glm::isnan(v.y))
		v.y = 0;
	if (glm::isnan(v.z))
		v.z = 0;

	setDirection(v); //set direction to the calculated direction
};

void Tank::updatePosition()
{

	//setting position and direction of wheels
	for (size_t i = 0; i < 4; i++)
	{	
		getChildNodes().at(i)->setPosition(getPosition() + (glm::rotateZ(wheelPositions[i], atan2(getDirection().x, -getDirection().y)))); //wheel positions rotate to match the main body
		getChildNodes().at(i)->setDirection(getDirection()); //wheels face the same direction as the main body
	}

	//updates position of all children
	for (size_t i = 0; i < getChildNodes().size(); i++)
		getChildNodes().at(i)->updatePosition();

	
	//moves the tank forward
	setPosition(getPosition() + (getDirection() * (getMoveSpeed() * deltaTime / 1000.f)));
};

void Tank::draw(){
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexMesh), vertexMesh, GL_STATIC_DRAW); //assigning the tank's vertex mesh to the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureMesh), textureMesh, GL_STATIC_DRAW); //assigning the tank's texture mesh to the texture buffer

	glm::mat4 model_view = glm::mat4(1);

	model_view = glm::translate(model_view, getPosition()); //translate draw at object's location
	//rotates tank to face its direction
	model_view = glm::rotate(model_view, atan2(-getDirection().x, getDirection().y), glm::vec3(0, 0, 1));
	model_view = glm::scale(model_view, glm::vec3(2.0, 2.0, 2.0));
	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);


	glBindTexture(GL_TEXTURE_2D, Texture::get(TextureID::tank));
	glDrawArrays(GL_QUADS, 0, 24);
}

void Tank::checkCollision(GameObject* other)
{
	//assignment will return NULL if cast is invalid
	if (auto c_player = dynamic_cast<Player*>(other)) { //if being compared with a player...
		if (isColliding(c_player)) {//if the two bodies intersect...
			willBeErased = true; //mark the object for deletion
		}
	}
	else if (auto bullet = dynamic_cast<Bullet*>(other)) { //if being compared with a bullet...
		if (isColliding(bullet)) {//if the two bodies intersect...
			if (bullet->getSource() != nullptr) {
				if (auto source = dynamic_cast<Player*>(bullet->getSource())) //it is colliding with the PLAYER'S bullet
				{
					willBeErased = true; //mark the object for deletion
					::player->incTanksShot(); //increment number of tanks shot
				}
			}
		}
	}
}

void Tank::idle()
{
	int time = glutGet(GLUT_ELAPSED_TIME);

	if (time - lastBulletShot > 1000) //every second..
	{
		glm::vec3 v = glm::normalize(glm::vec3(player->getPosition() - getPosition()));
		//normalization turns 0's into nan's for some reason... turn them back to 0's
		if (glm::isnan(v.x))
			v.x = 0;
		if (glm::isnan(v.y))
			v.y = 0;
		if (glm::isnan(v.z))
			v.z = 0;
		gameScene.push_back(std::shared_ptr<GameObject>(new Bullet(getPosition(),v, this)));
		lastBulletShot = time;
	}
}

void Tank::Wheel::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexMesh), vertexMesh, GL_STATIC_DRAW); //assigning the tank wheel's vertex mesh to the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureMesh), textureMesh, GL_STATIC_DRAW); //assigning the tank wheel's texture mesh to the texture buffer

	glm::mat4 model_view = glm::mat4(1);

	glm::vec3 side_vector = glm::cross(getDirection(), glm::vec3(0, 0, 1)); //side vector, used for rotating the wheel

	model_view = glm::translate(model_view, getPosition()); //draw at wheel's position
	if (gs == 0) {
		model_view = glm::rotate(model_view, -glutGet(GLUT_ELAPSED_TIME) / 500.f, side_vector); //constant rotation of the wheel
	}
	model_view = glm::scale(model_view, glm::vec3(0.5, 0.5, 0.5)); 


	for (size_t i = 0; i < 6; i++) //drawing each face of the wheel individually, rotating and reusing the vertices
	{
		model_view = glm::rotate(model_view, glm::radians(60.f), side_vector); 
		glm::mat4 model_view2 = glm::rotate(model_view, atan2(-getDirection().x, getDirection().y), glm::vec3(0, 0, 1)); //making a second matrix to hold the rotation that faces its direction, this is done so that it does not mess with other rotations
		glUniformMatrix4fv(location, 1, GL_FALSE, &model_view2[0][0]);
		glDrawArrays(GL_QUADS, 12, 4);
	}

	//rotate the wheel to face its direction
	model_view = glm::rotate(model_view, atan2(-getDirection().x, getDirection().y), glm::vec3(0, 0, 1));
	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

	glDrawArrays(GL_POLYGON, 0, 6); //draw face of wheel
	glDrawArrays(GL_POLYGON, 6, 6); 

}


//VERTEX AND TEXTURE MESH DATA

GLfloat Tank::vertexMesh[24][3] = {
	{-0.6, -1,  0},
	{0.6, -1,  0},	//bottom face
	{0.6, 1, 0},
	{-0.6, 1, 0},

	{-0.6, -0.8, 0.75},
	{0.6, -0.8, 0.75},	//top face
	{0.6, 0.8, 0.75},
	{-0.6, 0.8, 0.75},

	{-0.6, -0.8, 0.75},
	{-0.6, 0.8, 0.75}, //left 
	{-0.6, 1, 0 },
	{-0.6, -1, 0},

	{0.6, -0.8, 0.75},
	{0.6, 0.8, 0.75}, //right
	{0.6, 1, 0 },
	{0.6, -1, 0},

	{0.6, 0.8, 0.75},
	{-0.6, 0.8, 0.75},
	{-0.6, 1, 0},
	{0.6, 1, 0},


	{0.6, -0.8, 0.75},
	{-0.6, -0.8, 0.75},
	{-0.6, -1, 0},
	{0.6, -1, 0}

};
GLfloat Tank::textureMesh[24][2] = {
	0, 0.224,
	1, 0.224,  //bottom face
	1, 0.743,
	0, 0.743,

	0, 0.224,
	1, 0.224, //top face
	1, 0.743,
	0, 0.743,

	0.870, 0, //left face
	0.128, 0,
	0, 0.224,
	1, 0.224,

	0.128, 0,
	0.870, 0, //right face
	1, 0.224,
	0, 0.224,

	0, 0.744,
	0.227, 0.744, //front 
	0.227, 1,
	0, 1,

	0, 0.744,
	0.227, 0.744, //back
	0.227, 1,
	0, 1
};

glm::vec3 Tank::wheelPositions[4]{
	{-1.25, -1.35, 0},
	{-1.25, 1.35, 0},
	{1.25, 1.35, 0},
	{1.25, -1.35, 0}
};

GLfloat Tank::Wheel::vertexMesh[][3] = {
	{0.25, 0, 1},
	{0.25, 0.87, 0.5},
	{0.25, 0.87, -0.5},
	{0.25, 0, -1},			//front
	{0.25, -0.87, -0.5},
	{0.25, -0.87, 0.5},

	{-0.25, 0, 1},
	{-0.25, 0.87, 0.5},
	{-0.25, 0.87, -0.5},
	{-0.25, 0, -1},			//back
	{-0.25, -0.87, -0.5},
	{-0.25, -0.87, 0.5},

	{0.25, 0, 1},
	{0.25, 0.87, 0.5}, //side
	{-0.25, 0.87, 0.5},
	{-0.25, 0, 1,},
	
};

GLfloat Tank::Wheel::textureMesh[16][2] = {
	0.44, 0.82,
	0.54, 0.82,
	0.59, 0.91, //front
	0.541, 0.994,
	0.44, 0.994,
	0.391, 0.908,

	0.44, 0.82,
	0.54, 0.82,
	0.59, 0.91,
	0.541, 0.994, //back
	0.44, 0.994,
	0.391, 0.908,

	0.609, 0.88,
	0.654, 0.88,
	0.654, 0.981,
	0.609, 0.981
};