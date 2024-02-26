#pragma once
#include "SFML/Graphics.hpp"
#include <string>
class sfmlPanZoomHandler
{ 
public:
	sfmlPanZoomHandler(sf::VideoMode videoMode, std::string title);
	~sfmlPanZoomHandler();

	void handleEventPanZoom();
public:
	const int defaultWidth;
	const int defaultHeight;
	float zoom;
	bool panning;
	sf::RenderWindow window;
	sf::View view;
	sf::Event event;

private:
	sf::Vector2f lastMouse;
};

