#pragma once
#include "mapManager.h"
#include "textureAsset.h"
#include "textureAsset.h"
#include "SFML/Graphics.hpp"
class Entity {
public:
	//variable
	float posX;
	float posY;

	float velX;
	float velY;

	int sizeX;
	int sizeY;

	float boundX; // size
	float boundY; // size
	float offsetBoundX; //offset to entity position origin(top left corner)
	float offsetBoundY; //offset to entity position origin(top left corner)

	bool collidableWithMap;
	int textureID;
	textureAsset* textureSet;
	mapManager* mapCollisionSubject;

public:
	Entity(float posX, float posY, int sX, int sY, textureAsset& textureA);
	~Entity();

	void resolveCollisionWithMap(float elapsedTime, mapManager& physicAgainst, sf::RenderWindow& window);
	void resolveCollisionWithEntity(float elapsedTime, Entity& physicAgainst, mapManager& mapPhysicAgainst, sf::RenderWindow& window, bool SoftCollisionResponse = true);
	void computePhysic(float elapsedTime, mapManager& physicAgainst, sf::RenderWindow& window, bool resetVelocity = true);
	void resolveVelocity(float elapsedTime);
	bool checkBoundingTouchEntity(Entity& physicAgainst);
	void renderSelf(sf::RenderWindow& win);
private:
	static bool RayAgainstRectCollision(const sf::FloatRect Rect, const sf::Vector2f rayOrigin, const sf::Vector2f rayDirection, sf::Vector2f& faceNormal, sf::Vector2f& contactPoint, float& sc);
};