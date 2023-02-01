#pragma once
#include <SFML/Graphics.hpp>

class Charge
{
public:
	float q;
	sf::Vector2f position;
	
	Charge(float q, sf::Vector2f position);
	~Charge();
};

