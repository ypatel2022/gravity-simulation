#include "Particle.h"

Particle::Particle(float pos_x, float pos_y, float vel_x, float vel_y)
{
	m_Pos.x = pos_x;
	m_Pos.y = pos_y;
	m_Vel.x = vel_x;
	m_Vel.y = vel_y;
}

void Particle::UpdatePhysics(GravitySource& s)
{
	if (isDone)
	{
		return;
	}

	float velocity_cap = 2;

	// newtons law of physics
	float dist_x = s.GetPos().x - m_Pos.x;
	float dist_y = s.GetPos().y - m_Pos.y;

	float dist = sqrt(dist_x * dist_x + dist_y * dist_y);

	float inverse_dist = 1.f / dist;

	float normalized_x = inverse_dist * dist_x;
	float normalized_y = inverse_dist * dist_y;

	float inverse_square_dropoff = inverse_dist * inverse_dist;

	float acceleration_x = std::min(velocity_cap, normalized_x * s.GetStrength() * inverse_square_dropoff);
	float acceleration_y = std::min(velocity_cap, normalized_y * s.GetStrength() * inverse_square_dropoff);

	m_Vel.x += acceleration_x;
	m_Vel.y += acceleration_y;

	m_Pos.x += m_Vel.x;
	m_Pos.y += m_Vel.y;

	if (abs(m_Pos.x - s.GetPos().x) < m_Threshold && abs(m_Pos.y - s.GetPos().y) < m_Threshold)
	{
		isDone = true;
		this->~Particle();
	}
}

Particle::~Particle() {}

sf::Vector2f Particle::GetPos()
{
	return m_Pos;
}
