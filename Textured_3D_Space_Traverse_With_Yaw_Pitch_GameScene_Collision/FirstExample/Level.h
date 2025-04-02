//I promise to work on this exam on my own without receiving any help from any other persons. All of my answers will be my own.

#pragma once
#include "vgl.h"
#include "glm\glm.hpp"
#include <iostream>
using namespace std;
extern GLuint Buffers[2];

class Level {

	static GLfloat vertices[28][3];
	static GLfloat textureCoordinates[28][2];

public:
	Level() = delete;

	static void draw()
	{
		glm::mat4 model_view = glm::mat4(1);
		glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);

		glBindTexture(GL_TEXTURE_2D, Texture::get(TextureID::skybox)); //drawing skybox
		glDrawArrays(GL_QUADS, 4, 24);

		glBindTexture(GL_TEXTURE_2D, Texture::get(TextureID::grass)); //drawing ground
		glDrawArrays(GL_QUADS, 0, 4);
	}
};

GLfloat Level::vertices[28][3] = {

		{ -50, -50.0, 0.0 }, //Plane to walk on
		{ 50.0, -50.0, 0.0 },
		{ 50.0, 50.0, 0.0 },
		{ -50.0, 50.0, 0.0 },

		{ 75, 75 ,-75 },
		{ -75, 75 ,-75 }, //skybox
		{ -75, -75 ,-75 },
		{ 75, -75 ,-75 },

		{ 75, 75 ,75 },
		{ -75, 75 ,75 },
		{ -75, -75 ,75 },
		{ 75, -75 ,75},

		{ -75, 75 , -75 },
		{ -75, -75, -75 },
		{ -75, -75 ,75 },
		{ -75, 75 ,75 },

		{ 75, -75 , -75 },
		{ 75, 75 , -75 },
		{ 75, 75 ,75 },
		{ 75, -75 ,75 },

		{ 75, 75 , -75 },
		{ -75, 75 , -75 },
		{ -75, 75 , 75 },
		{ 75, 75 , 75},

		{ -75, -75 , -75 },
		{ 75, -75 , -75 },
		{ 75, -75 , 75 },
		{ -75, -75 , 75 }
};

GLfloat Level::textureCoordinates[28][2] = {
		0.0f, 0.0f,
		200.f, 0.0f,
		200.f, 200.f,
		0.0f, 200.f,

		0.25f, 1.0f, 0.5f, 1.0f, 0.5f, 0.666f, 0.25f, 0.666f,
		0.25f, 0.334f,0.5f, 0.334f, 0.5f, 0.0f, 0.25f, 0.0f,
		0.5f, 0.666f,0.75f, 0.666f, 0.75f, 0.334f, 0.5f, 0.334f,
		0.0f, 0.666f, 0.25f, 0.666f, 0.25f, 0.334f, 0.0f, 0.334f,
		0.25f, 0.666f, 0.5f, 0.666f, 0.5f, 0.334f, 0.25f, 0.334f,
		0.75f, 0.666f, 1.0f, 0.666f, 1.0f, 0.334f, 0.75f, 0.334f,
};