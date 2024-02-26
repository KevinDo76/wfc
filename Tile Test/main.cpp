#include <SFML/Graphics.hpp>
#include "mapManager.h"
#include "textureAsset.h"
#include "sfmlPanZoomHandler.h"
#include "Entity.h"
#include <sstream>
#include <iostream>
#include <cmath>
#define WORLD_RES_X 1000
#define WORLD_RES_Y 1000
int main()
{
    sfmlPanZoomHandler winObj(sf::VideoMode(WORLD_RES_X, WORLD_RES_Y), "Tile Engine");

    mapManager map = mapManager(20,20,16, 16);
    textureAsset mainMap = textureAsset(16, 16, 10);
    mainMap.loadTextureMap("test.png");
    map.loadTextureAsset(mainMap);

    winObj.window.setFramerateLimit(240);
    map.loadDataIntoTile("save.txt");


    while (winObj.window.isOpen())
    {
        while (winObj.window.pollEvent(winObj.event)) {
            if (winObj.window.hasFocus() && map.isTextureAssetLoaded()) {
                if (winObj.event.type == sf::Event::MouseButtonPressed && winObj.event.mouseButton.button == sf::Mouse::Button::Left) {
                    int index = map.getCurrentTileIndexMouse(winObj.window);
                    if (index >= 0) {
                        map.tiles[index].updateTextureID((map.tiles[index].textureID + 1) % mainMap.getSize());
                    }
                }
                else if (winObj.event.type == sf::Event::MouseButtonPressed && winObj.event.mouseButton.button == sf::Mouse::Button::Right) {
                    int index = map.getCurrentTileIndexMouse(winObj.window);
                    if (index >= 0) {
                        map.tiles[index].updateTextureID(std::abs((map.tiles[index].textureID - 1)) % mainMap.getSize());
                    }
                }
                else if (winObj.event.type == sf::Event::KeyPressed && winObj.event.key.code == sf::Keyboard::S) {
                    map.saveTileIntoFile("save.txt");
                    std::cout << "saved\n";
                }
            }
            
            winObj.handleEventPanZoom();
        }


        winObj.window.clear();
        map.draw(winObj.window);
        winObj.window.display();


    }

    return 0;
}