#include "Entity.h"
#include "mapManager.h"
#include "SFML/Graphics.hpp"
#include <iostream>
#define DRAW_DEBUG 1
#include <bitset>
Entity::Entity(float posX, float posY, int sX, int sY, textureAsset& textureA) : posX(posX), posY(posY), sizeX(sX), sizeY(sY), velX(0), velY(0), boundX(sizeX - 2), boundY(sizeY - 2), offsetBoundX(1), offsetBoundY(1), collidableWithMap(true), mapCollisionSubject(nullptr), textureSet(&textureA), textureID(0) {}

Entity::~Entity() {}

void Entity::renderSelf(sf::RenderWindow& win) {
	sf::VertexArray shape(sf::TriangleStrip, 4);
	shape[0].position = sf::Vector2f(posX, posY);
	shape[1].position = sf::Vector2f(posX + sizeX, posY);
	shape[2].position = sf::Vector2f(posX, posY + sizeY);
	shape[3].position = sf::Vector2f(posX + sizeX, posY + sizeY);


	shape[0].texCoords = sf::Vector2f(0, 0);
	shape[1].texCoords = sf::Vector2f(sizeX, 0);
	shape[2].texCoords = sf::Vector2f(0, sizeY);
	shape[3].texCoords = sf::Vector2f(sizeX, sizeY);
	sf::RenderStates state;
	win.draw(shape, textureSet->getTexture(textureID));
	if (DRAW_DEBUG) {
		sf::VertexArray boundingBox(sf::LineStrip, 5);
		boundingBox[0].position = sf::Vector2f(posX + offsetBoundX, posY + offsetBoundY);
		boundingBox[0].color = sf::Color(255, 0, 0);
		boundingBox[1].position = sf::Vector2f(posX + offsetBoundX + boundX, posY + offsetBoundY);
		boundingBox[1].color = sf::Color(255, 0, 0);
		boundingBox[2].position = sf::Vector2f(posX + offsetBoundX + boundX, posY + offsetBoundY + boundY);
		boundingBox[2].color = sf::Color(255, 0, 0);
		boundingBox[3].position = sf::Vector2f(posX + offsetBoundX, posY + offsetBoundY + boundY);
		boundingBox[3].color = sf::Color(255, 0, 0);
		boundingBox[4].position = sf::Vector2f(posX + offsetBoundX, posY + offsetBoundY);
		boundingBox[4].color = sf::Color(255, 0, 0);
		win.draw(boundingBox);
	}

}

void Entity::resolveVelocity(float elapsedTime) {
	posX += velX * elapsedTime;
	posY += velY * elapsedTime;
}

void Entity::resolveCollisionWithMap(float elapsedTime, mapManager& physicAgainst, sf::RenderWindow& window) {
	if (velX == 0 && velY == 0) return;
	if (!collidableWithMap) return;
	int collideCheckIndex[9];

	float collisionX = posX + offsetBoundX;
	float collisionY = posY + offsetBoundY;
	float halfBoundX = boundX / 2;
	float halfBoundY = boundY / 2;

	collideCheckIndex[0] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX + physicAgainst.scaleX + halfBoundX, collisionY + halfBoundY)));
	collideCheckIndex[1] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX - physicAgainst.scaleX + halfBoundX, collisionY + halfBoundY)));
	collideCheckIndex[2] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX + halfBoundX, collisionY + physicAgainst.scaleY + halfBoundY)));
	collideCheckIndex[3] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX + halfBoundX, collisionY - physicAgainst.scaleY + halfBoundY)));
	collideCheckIndex[4] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX + physicAgainst.scaleX + halfBoundX, collisionY + physicAgainst.scaleY + halfBoundY)));
	collideCheckIndex[5] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX + physicAgainst.scaleX + halfBoundX, collisionY - physicAgainst.scaleY + halfBoundY)));
	collideCheckIndex[6] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX - physicAgainst.scaleX + halfBoundX, collisionY + physicAgainst.scaleY + halfBoundY)));
	collideCheckIndex[7] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX - physicAgainst.scaleX + halfBoundX, collisionY - physicAgainst.scaleY + halfBoundY)));
	collideCheckIndex[8] = (physicAgainst.getCurrentTileIndexPosition(window, sf::Vector2f(collisionX + halfBoundX, collisionY + halfBoundY)));

	for (int i = 0; i < 9; i++) {
		if (collideCheckIndex[i] == -1) continue;
		if (DRAW_DEBUG) physicAgainst.tiles[collideCheckIndex[i]].drawBoundBox(window, sf::Color(255, 0, 255));
		if (!physicAgainst.tiles[collideCheckIndex[i]].collidable) continue;

		sf::Vector2f collisionSubPos = physicAgainst.tiles[collideCheckIndex[i]].sprite.getPosition();
		sf::FloatRect collisionSubject(sf::Vector2f(collisionSubPos.x - boundX / 2, collisionSubPos.y - boundY / 2), sf::Vector2f(physicAgainst.scaleX + boundX, physicAgainst.scaleY + boundY));
		sf::Vector2f rayOrigin = sf::Vector2f(posX + offsetBoundX + boundX / 2, posY + offsetBoundY + boundY / 2);
		sf::Vector2f rayDir = sf::Vector2f(velX * elapsedTime, velY * elapsedTime);
		sf::Vector2f faceNormal;
		sf::Vector2f contactPoint;

		if (DRAW_DEBUG) {
			sf::VertexArray line(sf::Lines, 2);
			line[0].color = (sf::Color(0, 255, 0));
			line[1].color = (sf::Color(255, 0, 255));
			line[0].position = rayOrigin;
			line[1].position = rayOrigin + (rayDir + rayDir + rayDir + rayDir);
			window.draw(line);
		}

		float scaleContact;
		if (RayAgainstRectCollision(collisionSubject, rayOrigin, rayDir, faceNormal, contactPoint, scaleContact)) {
			if (DRAW_DEBUG) physicAgainst.tiles[collideCheckIndex[i]].drawBoundBox(window, sf::Color(0, 255, 255));
			if (faceNormal.x == 1) {
				posX = contactPoint.x - boundX / 2 - offsetBoundX + 0.0001f;
			}
			else if (faceNormal.x == -1) {
				posX = contactPoint.x - boundX / 2 - offsetBoundX - 0.0001f;
			}

			if (faceNormal.y == 1) {
				posY = contactPoint.y - boundY / 2 - offsetBoundY + 0.0001f;
			}
			else if (faceNormal.y == -1) {
				posY = contactPoint.y - boundY / 2 - offsetBoundY - 0.0001f;
			}
		}
	}
}

/// <summary>
/// check and resolve collision with self and entity
/// a map is require in order to check for collision with entity that resolved in collision with map
/// </summary>
/// <param name="elapsedTime"></param>
/// <param name="physicAgainst"></param>
/// <param name="mapPhysicAgainst"></param>
/// <param name="window"></param>
/// <param name="SoftCollisionResponse"></param>

bool Entity::checkBoundingTouchEntity(Entity& physicAgainst) {
	sf::FloatRect selfBoundBox(this->posX + this->offsetBoundX, this->posY + this->offsetBoundY, this->boundX, this->boundY);
	sf::FloatRect AgainstBoundBox(physicAgainst.posX + physicAgainst.offsetBoundX, physicAgainst.posY + physicAgainst.offsetBoundY, physicAgainst.boundX, physicAgainst.boundY);
	//aabb https://tutorialedge.net/gamedev/aabb-collision-detection-tutorial/
	if (selfBoundBox.left < AgainstBoundBox.left + AgainstBoundBox.width &&
		selfBoundBox.left + selfBoundBox.width > AgainstBoundBox.left &&
		selfBoundBox.top < AgainstBoundBox.top + AgainstBoundBox.height &&
		selfBoundBox.top + selfBoundBox.height > AgainstBoundBox.top)
	{
		return true;
	}

	return false;
}

void Entity::resolveCollisionWithEntity(float elapsedTime, Entity& physicAgainst, mapManager& mapPhysicAgainst, sf::RenderWindow& window, bool SoftCollisionResponse) {
	if (!SoftCollisionResponse) {
		float dirX = velX * elapsedTime;
		float dirY = velY * elapsedTime;
		if (velX == 0 || velY == 0) {
			dirX = (physicAgainst.posX + physicAgainst.offsetBoundX + physicAgainst.boundX / 2) - (posX + offsetBoundX + boundX / 2);
			dirY = (physicAgainst.posY + physicAgainst.offsetBoundY + physicAgainst.boundY / 2) - (posY + offsetBoundY + boundY / 2);
			float l = std::sqrtf(dirX * dirX + dirY * dirY);
			dirX /= l;
			dirY /= l;
			dirX *= 0.0001f;
			dirY *= 0.0001f;
		}

		sf::Vector2f collisionSubPos = sf::Vector2f(physicAgainst.posX + physicAgainst.offsetBoundX, physicAgainst.posY + physicAgainst.offsetBoundY);
		sf::FloatRect collisionSubject(sf::Vector2f(collisionSubPos.x - boundX / 2, collisionSubPos.y - boundY / 2), sf::Vector2f(physicAgainst.boundX + boundX, physicAgainst.boundY + boundY));
		sf::Vector2f rayOrigin = sf::Vector2f(posX + offsetBoundX + boundX / 2, posY + offsetBoundY + boundY / 2);
		sf::Vector2f rayDir = sf::Vector2f(dirX, dirY);
		sf::Vector2f faceNormal;
		sf::Vector2f contactPoint;

		if (DRAW_DEBUG) {
			sf::VertexArray line(sf::Lines, 2);
			line[0].color = (sf::Color(0, 255, 0));
			line[1].color = (sf::Color(255, 0, 255));
			line[0].position = rayOrigin;
			line[1].position = rayOrigin + (rayDir + rayDir + rayDir + rayDir);
			window.draw(line);
		}

		float scaleContact;
		if (RayAgainstRectCollision(collisionSubject, rayOrigin, rayDir, faceNormal, contactPoint, scaleContact)) {
			if (faceNormal.x == 1) {
				posX = contactPoint.x - boundX / 2 - offsetBoundX + 0.0001f;
			}
			else if (faceNormal.x == -1) {
				posX = contactPoint.x - boundX / 2 - offsetBoundX - 0.0001f;
			}

			if (faceNormal.y == 1) {
				posY = contactPoint.y - boundY / 2 - offsetBoundY + 0.0001f;
			}
			else if (faceNormal.y == -1) {
				posY = contactPoint.y - boundY / 2 - offsetBoundY - 0.0001f;
			}
		}
	}
	else {
		sf::FloatRect subject = sf::FloatRect(posX + offsetBoundX, posY + offsetBoundY, boundX, boundY);
		if (subject.intersects(sf::FloatRect(physicAgainst.posX + physicAgainst.offsetBoundX, physicAgainst.posY + physicAgainst.offsetBoundY, physicAgainst.boundX, physicAgainst.boundY))) {
			float dirX = (physicAgainst.posX + physicAgainst.offsetBoundX + physicAgainst.boundX / 2) - (posX + offsetBoundX + boundX / 2);
			float dirY = (physicAgainst.posY + physicAgainst.offsetBoundY + physicAgainst.boundY / 2) - (posY + offsetBoundY + boundY / 2);
			float subjectLength = std::sqrt(physicAgainst.boundX * physicAgainst.boundX + physicAgainst.boundY * physicAgainst.boundY);
			float rayLength = std::sqrtf(dirX * dirX + dirY * dirY);
			dirX /= rayLength;
			dirY /= rayLength;
			dirX *= (25 * (subjectLength / rayLength));
			dirY *= (25 * (subjectLength / rayLength));

			velX += -dirX;
			velY += -dirY;
			computePhysic(elapsedTime, mapPhysicAgainst, window);
		}
	}
}

void Entity::computePhysic(float elapsedTime, mapManager& physicAgainst, sf::RenderWindow& window, bool resetVelocity) {
	resolveVelocity(elapsedTime);
	resolveCollisionWithMap(elapsedTime, physicAgainst, window);
	if (resetVelocity) {
		velX = 0;
		velY = 0;
	}
}

bool Entity::RayAgainstRectCollision(const sf::FloatRect Rect, const sf::Vector2f rayOrigin, const sf::Vector2f rayDirection, sf::Vector2f& faceNormal, sf::Vector2f& contactPoint, float& sc) {
	//learned this from a onelonecoder vid, good vid
	sf::Vector2f NearRayScale;
	sf::Vector2f FarRayScale;
	NearRayScale.x = (Rect.left - rayOrigin.x) / rayDirection.x;
	NearRayScale.y = (Rect.top - rayOrigin.y) / rayDirection.y;

	FarRayScale.x = (Rect.left + Rect.width - rayOrigin.x) / rayDirection.x;
	FarRayScale.y = (Rect.top + Rect.height - rayOrigin.y) / rayDirection.y;

	if (NearRayScale.x > FarRayScale.x) std::swap(NearRayScale.x, FarRayScale.x);
	if (NearRayScale.y > FarRayScale.y) std::swap(NearRayScale.y, FarRayScale.y);

	if (NearRayScale.x > FarRayScale.y || NearRayScale.y > FarRayScale.x) return false;

	float scaleNear = std::max(NearRayScale.x, NearRayScale.y);
	float scaleFar = std::min(FarRayScale.x, FarRayScale.y);

	if (scaleFar < 0 || scaleNear > 1) return false;
	sc = scaleNear;
	contactPoint.x = rayOrigin.x + rayDirection.x * sc;
	contactPoint.y = rayOrigin.y + rayDirection.y * sc;

	if (NearRayScale.x > NearRayScale.y) {
		if (rayDirection.x < 0) {
			faceNormal.x = 1;
			faceNormal.y = 0;
		}
		else {
			faceNormal.x = -1;
			faceNormal.y = 0;
		}
	}
	else if (NearRayScale.x < NearRayScale.y) {
		if (rayDirection.y < 0) {
			faceNormal.x = 0;
			faceNormal.y = 1;
		}
		else {
			faceNormal.x = 0;
			faceNormal.y = -1;
		}
	}

	return true;
}