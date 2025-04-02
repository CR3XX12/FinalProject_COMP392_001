#pragma once
#include <string>
#include "glm\glm.hpp"
class GameObject
{
	std::string goType;
	glm::vec3 goLocation = glm::vec3(0, 0, 0);
	glm::vec3 goDirection = glm::vec3(0, 0, 0);
	//exam code added
	glm::vec3 posToMove = glm::vec3(0, 0, 0);
	float spd = 0;
	float size = 0;
	float xOff;
	float yOff;
	bool isAlive = true;
	bool noRot = false;
public:
	GameObject();
	GameObject(std::string t, glm::vec3 loc);
	void SetLocation(glm::vec3 loc);
	glm::vec3 RetLocation();
	void SetDirection(glm::vec3 dir);
	glm::vec3 RetDirection();
	bool isColliding(GameObject g);
	void setSize(float si);
	std::string RetType();
	void setType(std::string t);
	void setXYOff(float x, float y);
	float retXOff();
	float retYOff();
	void setAlive(bool a);
	bool getAlive();
	void setRot(bool r)
	{
		noRot = r;
	}
	bool getRot()
	{
		return noRot;
	}
	//exam code added
	void setSpeed(float s)
	{
		spd = s;
	}
	float retSpeed()
	{
		return spd;
	}
	void setToMove(glm::vec3 m)
	{
		posToMove = m;
	}
	glm::vec3 retToMove()
	{
		return posToMove;
	}
};

GameObject::GameObject()
{
	goType = "";
	goLocation = glm::vec3(0, 0, 0);
	goDirection = glm::vec3(0, 0, 0); 
}

GameObject::GameObject(std::string t, glm::vec3 loc)
{
	goType = t;
	goLocation = loc;
}

void GameObject::SetLocation(glm::vec3 loc)
{
	goLocation = loc;
}

glm::vec3 GameObject::RetLocation()
{
	return goLocation;
}

void GameObject::SetDirection(glm::vec3 dir)
{
	goDirection = dir;
}

glm::vec3 GameObject::RetDirection()
{
	return goDirection;
}

void GameObject::setSize(float si)
{
	size = si;
}

bool GameObject::isColliding(GameObject g)
{
	bool flag = false;
	if (abs(this->RetLocation().x - g.RetLocation().x) <= ((size + g.size) / 2.0f) && abs(this->RetLocation().y - g.RetLocation().y) <= ((size + g.size) / 2.0f))
	{
		flag = true;
	}
	return flag;
}

std::string GameObject::RetType()
{
	return goType;
}

void GameObject::setType(std::string t)
{
	goType = t;
}

void GameObject::setXYOff(float x, float y)
{
	xOff = x;
	yOff = y;
}
float GameObject::retXOff()
{
	return xOff;
}
float GameObject::retYOff()
{
	return yOff;
}

void GameObject::setAlive(bool a)
{
	isAlive = a;
}

bool GameObject::getAlive()
{
	return isAlive;
}