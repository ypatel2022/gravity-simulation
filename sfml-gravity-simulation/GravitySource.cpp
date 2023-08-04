#include "GravitySource.h"

GravitySource::GravitySource(float pos_x, float pos_y, float strength)
	: m_Strength(strength)
{
	const float radius = strength / 50.0f;

	m_Pos.x = pos_x;
	m_Pos.y = pos_y;

	m_Shape.setPosition({ m_Pos.x - radius, m_Pos.y - radius });
	m_Shape.setFillColor(sf::Color::White);
	m_Shape.setRadius(radius);
}

void GravitySource::UpdatePos(sf::Vector2i pos)
{
	this->m_Pos.x = pos.x;
	this->m_Pos.y = pos.y;
	m_Shape.setPosition(this->m_Pos);
}

void GravitySource::Render(sf::RenderWindow& window)
{
	window.draw(m_Shape);
}

sf::Vector2f GravitySource::GetPos()
{
	return m_Pos;
}

float GravitySource::GetStrength()
{
	return m_Strength;
}
