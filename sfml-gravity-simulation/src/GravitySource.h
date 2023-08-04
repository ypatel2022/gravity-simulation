#pragma once

#include <SFML/Graphics.hpp>

class GravitySource
{
private:
	sf::Vector2f m_Pos;
	float m_Strength;
	sf::CircleShape m_Shape;

public:
	GravitySource(float pos_x, float pos_y, float strength);

	void UpdatePos(sf::Vector2i pos);

	void Render(sf::RenderWindow& window);

	sf::Vector2f GetPos();

	float GetStrength();
};