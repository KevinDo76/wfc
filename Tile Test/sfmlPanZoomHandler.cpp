#include "sfmlPanZoomHandler.h"
#include <string>
#include <iostream>
sfmlPanZoomHandler::sfmlPanZoomHandler(sf::VideoMode videoMode, std::string title) : defaultHeight(videoMode.height), defaultWidth(videoMode.width), zoom(1), panning(0), event() {
	this->window.create(videoMode, title);
	this->view.reset(sf::FloatRect(0.f, 0.f, defaultHeight, defaultWidth));
	window.setView(view);
}

sfmlPanZoomHandler::~sfmlPanZoomHandler() {

}

void sfmlPanZoomHandler::handleEventPanZoom() {
	switch (this->event.type) {
	case sf::Event::Closed:
		this->window.close();
	case sf::Event::Resized:
		sf::Vector2u windowSize = this->window.getSize();
		view.setSize(sf::Vector2f(int(((double)windowSize.x / (double)windowSize.y) * this->defaultWidth), this->defaultWidth));
		view.zoom(this->zoom);
	}



	if (this->window.hasFocus()) {
		switch (this->event.type) {
		case sf::Event::MouseButtonPressed:
			if (this->event.mouseButton.button == sf::Mouse::Button::Middle) {
				this->panning = true;
				this->lastMouse = sf::Vector2f(sf::Mouse::getPosition(window));
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (this->event.mouseButton.button == sf::Mouse::Button::Middle) {
				this->panning = false;
			}
			break;
		case sf::Event::MouseWheelScrolled:
			if (panning) { break; }
			if (event.mouseWheelScroll.delta >= 1) {
				sf::Vector2u winSize = this->window.getSize();
				this->view.setSize(sf::Vector2f(int(((float)winSize.x / winSize.y) * this->defaultWidth), (float)this->defaultHeight));
				this->zoom *= 0.97f;
				this->view.zoom(this->zoom);
			}
			else if (event.mouseWheelScroll.delta <= -1) {
				sf::Vector2u winSize = this->window.getSize();
				this->view.setSize(sf::Vector2f(int(((float)winSize.x / winSize.y) * this->defaultWidth), this->defaultHeight));
				this->zoom *= 1.03f;
				this->view.zoom(this->zoom);
			}
			break;
		case sf::Event::MouseMoved:
			if (!panning) { break; }
			sf::Vector2u winSize = this->window.getSize();
			sf::Vector2f newPos = sf::Vector2f((float)this->event.mouseMove.x, (float)this->event.mouseMove.y);
			sf::Vector2f different = this->lastMouse - newPos;
			float scaleFactor = (((float)winSize.y / this->defaultHeight));
			different /= scaleFactor;
			different *= this->zoom;
			this->view.move(different);
			this->lastMouse = newPos;
			break;
		}
	}
	this->window.setView(view);
}