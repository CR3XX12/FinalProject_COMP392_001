//I promise to work on this exam on my own without receiving any help from any other persons. All of my answers will be my own.

#include "Texture.h"
#include "glm\glm.hpp"


//texture filenames
const char* Texture::fileNames[Texture::numTextures] =
{
	"grass.png",
	"bullet.png",
	"skybox_test.png",
	"tank.png"
};



//initialization of static members

GLuint Texture::textures[Texture::numTextures]{};
GLint Texture::widths[Texture::numTextures]{};
GLint Texture::heights[Texture::numTextures]{};
unsigned char* Texture::textureDatas[Texture::numTextures]{};