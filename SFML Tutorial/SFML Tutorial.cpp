#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int WIDTH = 1920;
int HEIGHT = 1080;
sf::Color red(252, 63, 63);
sf::Color blue(63, 120, 252);

class GravitySource
{
    sf::Vector2f pos;
    float strength;
    sf::CircleShape s;

public:
    GravitySource(float pos_x, float pos_y, float strength)
    {
        pos.x = pos_x;
        pos.y = pos_y;
        this->strength = strength;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(4);
    }

    void update_pos(sf::Vector2i pos)
    {
        this->pos.x = pos.x;
        this->pos.y = pos.y;
        s.setPosition(this->pos);
    }

    void render(sf::RenderWindow &window)
    {
        window.draw(s);
    }

    sf::Vector2f get_pos()
    {
        return pos;
    }

    float get_strength()
    {
        return strength;
    }
};

class Particle
{
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape s;
    bool isDone = false;
    const float threshold = 2.f;

public:
    Particle(float radius, float pos_x, float pos_y, float vel_x, float vel_y)
    {
        pos.x = pos_x;
        pos.y = pos_y;
        vel.x = vel_x;
        vel.y = vel_y;

        s.setPosition(pos);
        sf::Color randomColor(rand() % 255, rand() % 255, rand() % 255, 255);
        s.setFillColor(randomColor);
        s.setRadius(radius);
    }

    void render(sf::RenderWindow& window)
    {
        if (isDone)
        {
            return;
        }
        
        s.setPosition(pos);
        window.draw(s);
    }

    void update_physics(GravitySource &s)
    {
        if (isDone)
        {
            return;
        }

        float velocity_cap = 2;

        // newtons law of physics
        float dist_x = s.get_pos().x - pos.x;
        float dist_y = s.get_pos().y - pos.y;
        
		float dist = sqrt(dist_x * dist_x + dist_y * dist_y);
        
        float inverse_dist = 1.f / dist;

        float normalized_x = inverse_dist * dist_x;
		float normalized_y = inverse_dist * dist_y;

        float inverse_square_dropoff = inverse_dist * inverse_dist;

        float acceleration_x = std::min(velocity_cap, normalized_x * s.get_strength() * inverse_square_dropoff);
		float acceleration_y = std::min(velocity_cap, normalized_y * s.get_strength() * inverse_square_dropoff);

        vel.x += acceleration_x;
        vel.y += acceleration_y;

        pos.x += vel.x;
        pos.y += vel.y;
           
        if (abs(pos.x - s.get_pos().x) < threshold && abs(pos.y - s.get_pos().y) < threshold)
        {
            isDone = true;
            this->s.setRadius(0);
            this->~Particle();
        }
    }

	~Particle() {}

    sf::Vector2f get_pos()
    {
        return pos;
    }
};

int main()
{
    // create a window & settings
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 32), "Blackhole Particle Simulation", sf::Style::Fullscreen);
    sf::View view = window.getDefaultView();
    window.setFramerateLimit(144);

    // create 2 gravity sources
    std::vector<GravitySource> sources;

    sources.push_back(GravitySource(WIDTH / 2.f, HEIGHT / 2.f + 200, 400));
    sources.push_back(GravitySource(WIDTH / 2.f, HEIGHT / 2.f - 200, 400));


    // for source that follows mouse
    GravitySource mouse_source(WIDTH / 2, HEIGHT / 2, 400);
    bool mouse_enabled = false;


    // particle settings
    std::vector<Particle> particles;
    int x = 50;
    int y = 50;
    int n = x * y;
    int m = 6;
    float radius = 2.f;
	float start_x = WIDTH / 2.f - ((x-1) * m / 2.f);
	float start_y = HEIGHT / 2.f - ((y-1) * m / 2.f);
    float vel_x = 1.f;
    float vel_y = 0.f;

    // create particles
    for (size_t i = 0; i < x; i++)
    {
        for (size_t j = 0; j < y; j++)
        {
			particles.push_back(Particle(radius, start_x + i * m, start_y + j * m, vel_x, vel_y));
        }
    }

    // for zooming in/out & panning view
    sf::Vector2f old_pos;
    bool moving = false;
    float zoom = 1;

    // while the window is kept open
    while (window.isOpen())
	{
        // store and loop through each event
        sf::Event event;
		while (window.pollEvent(event))
		{
            // if escape key is pressed
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }

            // if closing window
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::MouseButtonPressed:
                // Mouse button is pressed, get the position and set moving as active
                if (event.mouseButton.button == 0)
                {
                    moving = true;
                    old_pos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
                break;

            case  sf::Event::MouseButtonReleased:
                // Mouse button is released, no longer move
                if (event.mouseButton.button == 0)
                {
                    moving = false;
                }
                break;

            case sf::Event::MouseMoved:
            {
                // Ignore mouse movement unless a button is pressed (see above)
                if (!moving)
                    break;
                // Determine the new position in world coordinates
                const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                // Determine how the cursor has moved
                // Swap these to invert the movement direction
                const sf::Vector2f deltaPos = old_pos - newPos;

                // Move our view accordingly and update the window
                view.setCenter(view.getCenter() + deltaPos);
                window.setView(view);

                // Save the new position as the old one
                // We're recalculating this, since we've changed the view
                old_pos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                break;

            }
            case sf::Event::MouseWheelScrolled:
                // Ignore the mouse wheel unless we're not moving
                if (moving)
                    break;

                // Determine the scroll direction and adjust the zoom level
                // Again, you can swap these to invert the direction
                if (event.mouseWheelScroll.delta <= -1)
                    zoom = std::min(2.f, zoom + .1f);
                else if (event.mouseWheelScroll.delta >= 1)
                    zoom = std::max(.5f, zoom - .1f);

                // Update our view
                view.setSize(window.getDefaultView().getSize()); // Reset the size
                view.zoom(zoom); // Apply the zoom level (this transforms the view)
                window.setView(view);
                break;
            }
        }

        if (mouse_enabled)
        {
            mouse_source.update_pos(sf::Mouse::getPosition(window));
        }

        // clear frame
        window.clear(sf::Color(36, 36, 36));

        for (size_t i = 0; i < n; i++)
        {
            if (mouse_enabled)
            {
                particles[i].update_physics(mouse_source);
            }
            
            for (size_t j = 0; j < sources.size(); j++)
            {
                particles[i].update_physics(sources[j]);
            }

            // render the particle on the window
			particles[i].render(window);
        }

        // render all sources
        if (mouse_enabled)
        {
            mouse_source.render(window);
        }
        for (size_t i = 0; i < sources.size(); i++)
        {
            sources[i].render(window);
        }

        // show frame / draw calls
        window.display();
    }

    return 0;
}