//I promise to work on this exam on my own without receiving any help from any other persons. All of my answers will be my own.
#pragma once
#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "..\SOIL\src\SOIL.h"
#include <iostream>

//helper class that contains all texture data; NOT a wrapper class for textures (should not be instantiated)
class Texture
{
public:
	static const int numTextures = 4; //total number of textures

private:
	static const char* fileNames[numTextures]; //texture filenames
	static GLuint textures[numTextures]; //textures

	static GLint widths[numTextures]; //widths of textures
	static GLint heights[numTextures]; //heights of textures 
	static unsigned char* textureDatas[numTextures]; //texture data (returned value from SOIL_load_image())

public:

	Texture() = delete; //cannot be instantiaed


	static void initTextures() //initializes all textures
	{
		glGenTextures(numTextures, textures);

		for (int i = 0; i < numTextures; i++)
		{
			textureDatas[i] = SOIL_load_image(fileNames[i], &widths[i], &heights[i], 0, SOIL_LOAD_RGB);


			//Set the type of the allocated buffer as "TEXTURE_2D"
			glBindTexture(GL_TEXTURE_2D, textures[i]);

			//Loading the texture into the i'th allocated buffer:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widths[i], heights[i], 0, GL_RGB, GL_UNSIGNED_BYTE, textureDatas[i]);

			//Setting up parameters for the texture that recently pushed into VRAM
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}
	static inline GLuint& get(size_t t) { return textures[t]; }; //gets a texture at the index

};

//enum representing texture indexes
enum TextureID {
	grass,
	bullet,
	skybox,
	tank
};

