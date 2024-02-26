#include "textureAsset.h"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <string>
#include <vector>

textureAsset::textureAsset(int sx, int sy, int spriteMaxWidth) : sx(sx), sy(sy), spriteMaxWidth(spriteMaxWidth), isLoaded(false), texturesArray(nullptr), textureArraySize(0) {
	sf::Image nullImage = sf::Image();
	nullImage.create(sx, sy, sf::Color(255, 0, 255));
	for (int ty = 0; ty<int(sy / 2); ty++) {
		for (int tx = 0; tx<int(sx / 2); tx++) {
			nullImage.setPixel(tx, ty, sf::Color());
			nullImage.setPixel(tx + int(sx / 2), ty + int(sy / 2), sf::Color());
		}
	}
	this->nullTexture.loadFromImage(nullImage);
}

sf::Texture* textureAsset::getTexture(int id) {
	if (id<0 || id>textureArraySize - 1 || !isLoaded || !texturesArray) {
		return &nullTexture;
	}
	return &texturesArray[id];
}

textureAsset::~textureAsset() {
	delete[] texturesArray;
}

bool textureAsset::isAssetLoaded() {
	return this->isLoaded;
}

int textureAsset::getSize() {
	return textureArraySize;
}

bool textureAsset::loadTextureMap(std::string path) {
	sf::Image masterImage;
	std::vector<sf::Image> tempImages;
	if (!masterImage.loadFromFile(path)) { return false; }
	
	sf::Vector2u imgSize = masterImage.getSize();
	int tileXCount = imgSize.x / this->sx;
	int tileYCount = imgSize.y / this->sy;

	for (int i = 0; i < tileXCount * tileYCount; i++) {
		int tileTextureX = (i % this->spriteMaxWidth) * this->sx;
		int tileTextureY = int(i / this->spriteMaxWidth) * this->sy;

		sf::Image isolatedImage = sf::Image();
		isolatedImage.create(this->sx, this->sy);
		for (int yy = 0; yy < this->sy; yy++) {
			for (int xx = 0; xx < this->sx; xx++) {
				sf::Color color = masterImage.getPixel(xx + tileTextureX, yy + tileTextureY);
				isolatedImage.setPixel(xx, yy, color);
			}
		}
		tempImages.push_back(isolatedImage);
	}
	textureArraySize = (int)tempImages.size();

	delete[] texturesArray;
	texturesArray = new sf::Texture[tempImages.size()];

	for (int i = 0; i < tempImages.size(); i++) {
		this->texturesArray[i].loadFromImage(tempImages[i]);
	};

	this->isLoaded = true;
	std::cout << textureArraySize << " tile textures loaded\n";
	return true;
}