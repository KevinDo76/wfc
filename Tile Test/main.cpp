#include <SFML/Graphics.hpp>
#include "mapManager.h"
#include "textureAsset.h"
#include "sfmlPanZoomHandler.h"
#include "Entity.h"
#include <sstream>
#include <iostream>
#include <cmath>
#include "wfc.h"
#define WORLD_RES_X 1000
#define WORLD_RES_Y 1000
int main()
{
    srand(2);
    bool gen = false;
    int selectedTileIndex = 0;
    sfmlPanZoomHandler winObj(sf::VideoMode(WORLD_RES_X, WORLD_RES_Y), "Tile Engine");
    textureAsset mainMap = textureAsset(56, 56);
    mainMap.loadTextureMap("circuit.png");

    mapManager exampleMap = mapManager(20, 5, 56, 56);
    mapManager SelectionMap(mainMap.textureCount, 1,56,56);
    mapManager genMap(40, 40, 56, 56);

    wfc wfcObj(exampleMap, genMap);

    SelectionMap.setPosition(0, -70);
     genMap.setPosition(0, 2000);
    SelectionMap.loadTextureAsset(mainMap);
    genMap.loadTextureAsset(mainMap);

    for (int i = 0; i < mainMap.textureCount; i++) {
        SelectionMap.tiles[i].updateTextureID(i);
    }

    exampleMap.loadTextureAsset(mainMap);
    exampleMap.loadDataIntoTile("save.txt");

    wfcObj.generateConstraints();

    winObj.window.setFramerateLimit(240);


    while (winObj.window.isOpen())
    {
        while (winObj.window.pollEvent(winObj.event)) {
            if (winObj.window.hasFocus() && exampleMap.isTextureAssetLoaded()) {
                if (winObj.event.type == sf::Event::MouseButtonPressed && winObj.event.mouseButton.button == sf::Mouse::Button::Right) {
                    int indexSel = SelectionMap.getCurrentTileIndexMouse(winObj.window);
                    int indexMap = exampleMap.getCurrentTileIndexMouse(winObj.window);
                    if (indexSel > -1) {
                        selectedTileIndex = indexSel;
                    }
                    else if (indexMap>-1) {
                        exampleMap.tiles[indexMap].updateTextureID(selectedTileIndex);
                    }
                }
                else if (winObj.event.type == sf::Event::KeyPressed && winObj.event.key.code == sf::Keyboard::S) {
                    exampleMap.saveTileIntoFile("save.txt");
                    std::cout << "saved\n";
                }
                else if (winObj.event.type == sf::Event::KeyPressed && winObj.event.key.code == sf::Keyboard::I) {
                    wfcObj.computeIterate();
                    gen = true;
                }
            }
            
            winObj.handleEventPanZoom();
        }
        if (gen) {
            //while (!wfcObj.isComputeFinished()) {
                wfcObj.computeIterate();
            //}
        }

        winObj.window.clear();
        SelectionMap.draw(winObj.window);
        exampleMap.draw(winObj.window);
        genMap.draw(winObj.window);
        winObj.window.display();


    }

    return 0;
}