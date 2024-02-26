#include "mapManager.h"
#include "SFML/Graphics.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cmath>
#define SPRITE_MAP_MAX_WIDTH 25

template<typename T>
T clamp(T n, T min, T max) {
	if (n > max) {
		return max;
	}
	else if (n < min) {
		return min;
	}
	else {
		return n;
	}
}

mapManager::mapManager(int x, int y, int sx, int sy) : sizeX(x), sizeY(y), scaleX(sx), scaleY(sy), posX(0), posY(0), textureSet(nullptr), textureAssetLoaded(false) {
	//this->textureSet = &this->defaultAsset;
	sf::Image nullImage = sf::Image();
	nullImage.create(sx, sy, sf::Color(255, 0, 255));
	for (int ty = 0; ty<int(sy / 2); ty++) {
		for (int tx = 0; tx<int(sx / 2); tx++) {
			nullImage.setPixel(tx, ty, sf::Color());
			nullImage.setPixel(tx + int(sx / 2), ty + int(sy / 2), sf::Color());
		}
	}

	this->nullTexture.loadFromImage(nullImage);

	for (int yy = 0; yy < y; yy++) {
		for (int xx = 0; xx < x; xx++) {
			tiles.push_back({this, xx,yy,(yy * this->sizeX) + xx, 0});
		}
	}
}
mapManager::~mapManager() {
	tiles.clear();
	this->textureSet = nullptr;
}

void mapManager::draw(sf::RenderWindow& window) {

	sf::Vector2u winSize = window.getSize();
	sf::Vector2f leftTopBound = window.mapPixelToCoords(sf::Vector2i());
	sf::Vector2f rightTopBound = window.mapPixelToCoords(sf::Vector2i(winSize.x,winSize.y));
	sf::Vector2f leftBottomBound = window.mapPixelToCoords(sf::Vector2i(0, winSize.y));
	sf::Vector2f firstTilePos = tiles[0].sprite.getPosition();
	int yMax = (this->sizeY) * this->sizeX;
	
	for (int x = 0; x < this->sizeX; x++) {
		if (firstTilePos.x > leftTopBound.x - this->scaleX && firstTilePos.x < rightTopBound.x) {
			int firstTileY = firstTilePos.y;
			for (int y = x; y < yMax; y += this->sizeX) {
				if (firstTileY > leftTopBound.y - this->scaleY && firstTileY < leftBottomBound.y) {
					window.draw(tiles[y].sprite);
					if (!tiles[y].collidable) {
						continue;
					}
					//tiles[y].drawBoundBox(window, sf::Color(255,0,0));
				}
				firstTileY += this->scaleY;
			}
		}
		firstTilePos.x += this->scaleX;
	}
}

void mapManager::getMouseWorldCoord(sf::RenderWindow& window, sf::Vector2f& worldCoord) {
	sf::Vector2f mouseWorldCoord = window.mapPixelToCoords(sf::Mouse().getPosition(window));
	worldCoord.x = mouseWorldCoord.x;
	worldCoord.y = mouseWorldCoord.y;
}

void mapManager::getCurrentTile(sf::Vector2f worldCoord, sf::Vector2i& tileCoord) {
	int tileY = int((worldCoord.y - this->posY) / this->scaleY);
	int tileX = int((worldCoord.x - this->posX) / this->scaleX);
	if (worldCoord.x < this->posX) { tileX--; }
	if (worldCoord.y < this->posY) { tileY--; }
	tileCoord.x = tileX;
	tileCoord.y = tileY;
}

int mapManager::getCurrentTileIndexPosition(sf::RenderWindow& window, sf::Vector2f position) {
	sf::Vector2i tileCoord;
	this->getCurrentTile(position, tileCoord);
	int tileIndex = (tileCoord.y * this->sizeX + tileCoord.x);

	if (((tileCoord.x >= 0) && (tileCoord.x < (this->sizeX))) && ((tileCoord.y >= 0) && (tileCoord.y < (this->sizeY)))) {
		return tileIndex;
	}
	else {
		return -1;
	}
}

int mapManager::getCurrentTileIndexMouse(sf::RenderWindow& window)
{
	sf::Vector2f mouseWorld = sf::Vector2f();
	this->getMouseWorldCoord(window, mouseWorld);

	return getCurrentTileIndexPosition(window, mouseWorld);
}

bool mapManager::loadTextureAsset(textureAsset& texture) {
	if (!texture.isAssetLoaded()) { std::cout << "unable to load textureAsset, it's not loaded\n"; return false; }
	if (texture.sx == this->scaleX && this->scaleY) {
		this->textureAssetLoaded = true;
		this->textureSet = &texture;
		for (int i = 0; i < this->tiles.size(); i++) {
			this->tiles[i].updateTextureID(this->tiles[i].textureID);
		}
		std::cout << "loaded asset\n";
		return true;
	}
	return false;
}

bool mapManager::isTextureAssetLoaded() {
	return this->textureAssetLoaded;
}

/////////////////////////////////////////////////
/// <summary>
/// load textureID into tiles from a txt file
/// </summary>
/// <param name="path"></param>
/// <returns></returns>

bool mapManager::loadDataIntoTile(std::string path) {
	std::string line;
	// Read from the text file
	std::ifstream dataFile;
	dataFile.open(path);

	// Use a while loop together with the getline() function to read the file line by line
	int index = 0;
	if (!dataFile) { std::cout << "failed to open " << path << "\n"; return false; }
	while (getline(dataFile, line)) {
		// Output the text from the file
		if (index > (this->sizeX * this->sizeY - 1)) { std::cout << "file too big for map\n";  break; }
		std::stringstream streamRead(line);
		int output = 0;
		bool collide = false;
		streamRead >> output >> collide;
		this->tiles[index].updateTextureID(output);
		this->tiles[index].collidable = collide;
		index++;
	}

	// Close the file
	dataFile.close();
	return true;
}

bool mapManager::saveTileIntoFile(std::string path) {
	std::ofstream fileSave;
	fileSave.open(path);
	if (!fileSave) {
		std::cout << "failed to open " << path << "\n";
		return false;
	}

	for (int i = 0; i < this->tiles.size(); i++) {
		fileSave << this->tiles[i].textureID << " " << tiles[i].collidable << "\n";
	}
	fileSave.close();
	return true;
}

void mapManager::setPosition(float px, float py) {
	posX = px;
	posY = py;
	for (int i = 0; i < this->tiles.size(); i++) {
		this->tiles[i].offx = px;
		this->tiles[i].offy = py;
		this->tiles[i].updateTextureID(this->tiles[i].textureID);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 
/// </summary>
/// <param name="parentMap"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
/// <param name="ID"></param>
/// <param name="textureID"></param>

tileData::tileData(mapManager* parentMap, int posX, int posY, int ID, int textureID) : parentMap(parentMap), posX(posX), posY(posY), offx(0), offy(0), ID(ID), textureID(textureID), collidable(false) {
	this->updateTextureID(textureID);
}

void tileData::drawBoundBox(sf::RenderWindow& window, sf::Color color) {
	float tileX = sprite.getPosition().x;
	float tileY = sprite.getPosition().y;
	float boundingOff = 0;

	sf::VertexArray boundingBox(sf::LineStrip, 5);
	boundingBox[0].position = sf::Vector2f(tileX + boundingOff, tileY + boundingOff);
	boundingBox[0].color = color;
	boundingBox[1].position = sf::Vector2f(tileX + boundingOff, tileY + parentMap->scaleY - boundingOff);
	boundingBox[1].color = color;
	boundingBox[2].position = sf::Vector2f(tileX + parentMap->scaleX - boundingOff, tileY + parentMap->scaleY - boundingOff);
	boundingBox[2].color = color;
	boundingBox[3].position = sf::Vector2f(tileX + parentMap->scaleX - boundingOff, tileY + boundingOff);
	boundingBox[3].color = color;
	boundingBox[4].position = sf::Vector2f(tileX + boundingOff, tileY + boundingOff);
	boundingBox[4].color = color;
	window.draw(boundingBox);
}

void tileData::updateTextureID(int ID) {
	this->textureID = ID;

	//checking for invalid tile position
	if (!this->parentMap->textureSet || !this->parentMap->textureSet->isAssetLoaded()) {
		//null tile from main map class
		this->sprite.setTexture(this->parentMap->nullTexture);
	} else {;
		this->sprite.setTexture(*this->parentMap->textureSet->getTexture(this->textureID));
	}

	this->sprite.setPosition(sf::Vector2f((this->posX * this->parentMap->scaleX) + this->offx, (this->posY * this->parentMap->scaleY) + this->offy));
}

tileData::~tileData() {
	this->parentMap = nullptr;
}