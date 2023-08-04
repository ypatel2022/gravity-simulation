#pragma once

#include "SFML/Graphics.hpp"
#include "Particle.h"
#include "GravitySource.h"

class Simulation
{
private:
	sf::VertexArray m_Particles;
	std::vector<Particle> m_ParticleObjects;
	std::vector<GravitySource> m_GravitySources;
	float m_Radius;

public:
	Simulation(sf::Vector2i gridSize, sf::Vector2f startPos, sf::Vector2f initialVelocity, float radius, float spacing);
	~Simulation();

	void Render(sf::RenderWindow& window);

	void Step();

	void AddGravitySource(sf::Vector2f pos, float strength);
};