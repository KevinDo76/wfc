#pragma once
#include <vector>
#include "SFML/Graphics.hpp"
#include "string"
#include "textureAsset.h"
class mapManager; //thx chatgpt, didn't know about forward declaration until u <3

class tileData {
public:
	int posX;
	int posY;
	float offx;
	float offy;
	const int ID;
	int textureID;
	bool collidable;
	sf::Sprite sprite;
	sf::Texture internal;
	mapManager* parentMap;
	tileData(mapManager* parentMap, int posX, int posY, int ID, int textureID);
	~tileData();

	void updateTextureID(int ID);
	void drawBoundBox(sf::RenderWindow& window, sf::Color color);
private:
};

class mapManager {
public:
	//variables
	const int sizeX;
	const int sizeY;
	const int scaleX;
	const int scaleY;
	float posX;
	float posY;
	textureAsset* textureSet;
	sf::Texture nullTexture;
	std::vector<tileData> tiles;

public:
	//methods
	mapManager(int x, int y, int sx, int sy);
	~mapManager();

	void buildTextureCache();
	void getMouseWorldCoord(sf::RenderWindow& window, sf::Vector2f& worldCoord); //get the world coord of the user's mouse based on pixel position
	int getCurrentTileIndexMouse(sf::RenderWindow& window); //get the current tile index that the mouse is hovering over
	int getCurrentTileIndexPosition(sf::RenderWindow& window, sf::Vector2f pos); //get the tile position x-y of the tile that the mouse is hovering over
	int getTileIndexFromCoord(sf::Vector2i& tileCoord); // get the index of a tile based on x-y coord
	void getTileCoordFromIndex(int index, sf::Vector2i& tileCoord); // get the x-y coord of a tile based on index
	void getCurrentTile(sf::Vector2f worldCoord, sf::Vector2i& tileCoord); //get the current tile based on position
	bool loadTextureAsset(textureAsset& texture); //load in a texture map
	bool loadDataIntoTile(std::string path); // load tile index configuration
	bool saveTileIntoFile(std::string path); // save tile index configuration
	bool isTextureAssetLoaded(); //check if a textureAsset has been loaded into the Manager or not
	void setPosition(float px, float py); // change position

	void draw(sf::RenderWindow& window);
private:
	bool textureAssetLoaded;
};