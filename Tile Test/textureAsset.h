#pragma once
#include <string>
#include "SFML/Graphics.hpp"
#include <vector>
class textureAsset {
public:
	//std::vector<sf::Texture>textureVector;
	sf::Texture* texturesArray;
	const int sx;
	const int sy;
	int spriteMaxWidth;
	int textureCount;
	sf::Texture nullTexture;
public:
	textureAsset(int sx, int sy);
	sf::Texture* getTexture(int id);
	int getSize();
	bool isAssetLoaded();
	~textureAsset();
	bool loadTextureMap(std::string path);
private:
	bool isLoaded;
	int textureArraySize;
};