#pragma once

#include "SFML/Graphics.hpp"
#include "GravitySource.h"

class Particle
{
private:
	sf::Vector2f m_Pos;
	sf::Vector2f m_Vel;
	const float m_Threshold = 2.0f;

public:
	bool isDone = false;

public:
	Particle(float pos_x, float pos_y, float vel_x, float vel_y);

	void UpdatePhysics(GravitySource& s);

	~Particle();

	sf::Vector2f GetPos();
};