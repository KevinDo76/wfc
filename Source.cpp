#include <SFML/Graphics.hpp>
#include "mapManager.h"
#include <iostream>
#include <cmath>
#define WORLD_RES_X 320
#define WORLD_RES_Y 320
int main()
{
    sf::RenderWindow window(sf::VideoMode(WORLD_RES_X, WORLD_RES_Y), "Tile Engine");
    sf::View view = sf::View();
    view.reset(sf::FloatRect(0,0, WORLD_RES_X, WORLD_RES_Y));

    mapManager map = mapManager(10,10,16, 16);
    bool panning = false;
    float zoom = 1;
    sf::Clock Clock;
    
    sf::Vector2f lastMouse;
    map.loadTileMap("stardewtiles.png");

    window.setFramerateLimit(150);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                sf::Vector2u windowSize = window.getSize();
                //std::cout << (double)windowSize.x / (double)windowSize.y << "\n";;
                //map.loadTileMap("test.png");
                //view.reset(sf::FloatRect(0, 0, int(((double)windowSize.x / (double)windowSize.y)* WORLD_RES_X), WORLD_RES_Y));
                view.setSize(sf::Vector2f(int(((double)windowSize.x / (double)windowSize.y) * WORLD_RES_X), WORLD_RES_Y));
                view.zoom(zoom);
            }

            if (window.hasFocus()) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
                    int index = 0;
                    map.getCurrentTileIndexMouse(window, index);
                    map.tiles[index].updateTextureID((map.tiles[index].textureID + 1) % 300);
                } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Right) {
                    int index = 0;
                    map.getCurrentTileIndexMouse(window, index);
                    map.tiles[index].updateTextureID(std::abs((map.tiles[index].textureID - 1)) % 300);
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Middle) {
                    panning = true;
                    lastMouse = sf::Vector2f(sf::Mouse::getPosition(window));
                } 
                else if (event.type == sf::Event::MouseMoved) {
                    if (!panning) { break; }
                    sf::Vector2u winSize = window.getSize();
                    sf::Vector2f changePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                    sf::Vector2f different = lastMouse - changePos;
                    different = sf::Vector2f(different.x, different.y);
                    float scaleFactor = (((double)winSize.y / WORLD_RES_Y));
                    different /= scaleFactor;
                    different *= zoom;
                    //different = sf::Vector2f(std::roundf(different.x), std::roundf(different.y));
                    view.move(different);
                    lastMouse = changePos;
                }
                else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Middle) {
                    panning = false;
                }
                else if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.delta >= 1 && !panning) {
                    sf::Vector2u winSize = window.getSize();
                    view.setSize(sf::Vector2f(int(((double)winSize.x / (double)winSize.y) * WORLD_RES_X), WORLD_RES_Y));
                    zoom *= 0.97f;
                    view.zoom(zoom);
                }
                else if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.delta <= -1 && !panning) {
                    sf::Vector2u winSize = window.getSize();
                    view.setSize(sf::Vector2f(int(((double)winSize.x / (double)winSize.y) * WORLD_RES_X), WORLD_RES_Y));
                    zoom *= 1.03f;
                    view.zoom(zoom);
                }
            }
        }

        window.clear();
        map.draw(window);
        window.setView(view);
        window.display();

        //fps calc
        sf::Time Time = Clock.getElapsedTime();
        Clock.restart();
        std::cout << 1000/Time.asMilliseconds() << "FPS\n";
    }

    return 0;
}