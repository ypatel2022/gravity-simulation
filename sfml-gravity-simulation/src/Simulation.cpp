#include "Simulation.h"

#include <execution>


#define MULTI_THREADING 1


Simulation::Simulation(sf::Vector2i gridSize, sf::Vector2f startPos, sf::Vector2f initialVelocity, float radius, float spacing)
	: m_Radius(radius)
{
	srand(time(NULL));

	sf::VertexArray particles(sf::Quads);

	// create particles
	for (int i = 0; i < gridSize.x; i++)
	{
		for (int j = 0; j < gridSize.y; j++)
		{
			float x = startPos.x + i * spacing;
			float y = startPos.y + j * spacing;

			sf::Vertex v1(sf::Vector2f(x - radius, y - radius));
			sf::Vertex v2(sf::Vector2f(x - radius, y + radius));
			sf::Vertex v3(sf::Vector2f(x + radius, y + radius));
			sf::Vertex v4(sf::Vector2f(x + radius, y - radius));

			// random colors
			v1.color = v2.color = v3.color = v4.color = sf::Color(rand() % 255, rand() % 255, rand() % 255, 255);

			// add to vertex array
			particles.append(v1);
			particles.append(v2);
			particles.append(v3);
			particles.append(v4);

			// create particle object
			m_ParticleObjects.push_back(Particle(x, y, initialVelocity.x, initialVelocity.y));
		}
	}

	// set vertex array
	this->m_Particles = particles;
}

Simulation::~Simulation() {}

void Simulation::Render(sf::RenderWindow& window)
{
	// send vertex array to draw
	window.draw(m_Particles);

	// render gravity sources
	for (size_t i = 0; i < m_GravitySources.size(); i++)
	{
		m_GravitySources[i].Render(window);
	}
}

void Simulation::Step()
{
	// update each particle's physics
	for (int i = 0; i < m_ParticleObjects.size(); i++)
	{
		// get particle
		Particle& particle = m_ParticleObjects[i];


#if MULTI_THREADING
		// multithread physics updates
		std::for_each(std::execution::par, m_GravitySources.begin(), m_GravitySources.end(), [&](GravitySource& gravitySource) {
			particle.UpdatePhysics(gravitySource);
			});
#else
		// update the physics for each gravity source
		for (auto gravitySource : m_GravitySources)
		{
			particle.UpdatePhysics(gravitySource);
		}
#endif

		// update vertex positions
		float x = particle.GetPos().x;
		float y = particle.GetPos().y;

		if (!particle.isDone)
		{
			m_Particles[i * 4].position = sf::Vector2f(x - m_Radius, y - m_Radius);
			m_Particles[i * 4 + 1].position = sf::Vector2f(x - m_Radius, y + m_Radius);
			m_Particles[i * 4 + 2].position = sf::Vector2f(x + m_Radius, y + m_Radius);
			m_Particles[i * 4 + 3].position = sf::Vector2f(x + m_Radius, y - m_Radius);
		}
		else
		{
			sf::Vector2f zero(0, 0);
			m_Particles[i * 4].position = zero;
			m_Particles[i * 4 + 1].position = zero;
			m_Particles[i * 4 + 2].position = zero;
			m_Particles[i * 4 + 3].position = zero;
		}
	}
}

void Simulation::AddGravitySource(sf::Vector2f pos, float strength)
{
	m_GravitySources.push_back(GravitySource(pos.x, pos.y, strength));
}
