#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <chrono>

#include "Simulation.h"
#include "Constants.h"

int main()
{
	// create a window & settings
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 32), "Blackhole Particle Simulation", sf::Style::Fullscreen);
	sf::View view = window.getDefaultView();
	window.setFramerateLimit(144);

	// simulation settings
	sf::Vector2i gridSize(200, 200);
	float radius = 1.0f;
	float spacing = 3.0f;
	float start_x = (WIDTH / 2.0f) - (gridSize.x * spacing / 2.0f) + radius + (spacing - radius * 2.0f) * 0.5f;
	float start_y = (HEIGHT / 2.0f) - (gridSize.y * spacing / 2.0f) + radius + (spacing - radius * 2.0f) * 0.5f;
	sf::Vector2f startPos(start_x, start_y);
	sf::Vector2f initialVelocity(1.0f, 0.0f);

	Simulation sim(gridSize, startPos, initialVelocity, radius, spacing);

	sim.AddGravitySource(sf::Vector2f(WIDTH / 2.f, HEIGHT / 2.f + 200), 400);
	sim.AddGravitySource(sf::Vector2f(WIDTH / 2.f, HEIGHT / 2.f - 200), 400);

	// sim control
	bool start_sim = true;

	// to zoom in/out & panning view
	sf::Vector2f old_pos;
	bool moving = false;
	float zoom = 1;

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	double total = 0.0;
	int count = 0;

	// while the window is kept open
	while (window.isOpen())
	{
		// store and loop through each event
		sf::Event event;
		while (window.pollEvent(event))
		{

			// if closing window
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyReleased:
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}

				if (event.key.code == sf::Keyboard::Space)
				{
					start_sim = !start_sim;
				}

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

		// clear frame
		window.clear(sf::Color(20, 20, 20));

		if (start_sim)
		{
			auto t1 = high_resolution_clock::now();
			sim.Step();
			auto t2 = high_resolution_clock::now();

			duration<double, std::milli> ms_double = t2 - t1;

			total += ms_double.count();
			count++;
		}

		// render
		sim.Render(window);

		// show frame / draw calls
		window.display();
	}

	std::cout << "Average time per step: " << total / (double)count << "ms" << std::endl;

	return 0;
}