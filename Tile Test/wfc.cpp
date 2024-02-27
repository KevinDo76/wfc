#include "wfc.h"
#include "mapManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

constraintsContainer::constraintsContainer() {}

void constraintsContainer::clone(const constraintsContainer& copy) {

	left.clear();
	right.clear();
	up.clear();
	down.clear();

	for (int i = 0; i < copy.left.size(); i++) {
		left.push_back(copy.left[i]);
	}

	for (int i = 0; i < copy.right.size(); i++) {
		right.push_back(copy.right[i]);
	}

	for (int i = 0; i < copy.up.size(); i++) {
		up.push_back(copy.up[i]);
	}

	for (int i = 0; i < copy.down.size(); i++) {
		down.push_back(copy.down[i]);
	}
}

wfc::wfc(mapManager& exampleMap_, mapManager& generateMap_) {
	exampleMap = &exampleMap_;
	generateMap = &generateMap_;
}

void wfc::setAllToConstraintContainer(constraintsContainer& constraint) {
	constraint.left.clear();
	constraint.right.clear();
	constraint.up.clear();
	constraint.down.clear();

	for (int i = 0; i < exampleMap->textureSet->textureCount; i++) {
		constraint.left.push_back(i);
		constraint.right.push_back(i);
		constraint.up.push_back(i);
		constraint.down.push_back(i);
	}
}

void wfc::computeEntropy() {
	for (int i = 0; i<generateMap->tiles.size(); i++) {
		if (mapState[i] != -1) {
			mapEntropy[i] = -1;
			continue;
		}

		sf::Vector2i tileCoord;
		constraintsContainer allowedState;
		getContraintsFromID(mapState[i], allowedState);
		generateMap->getTileCoordFromIndex(i, tileCoord);

		sf::Vector2i leftOffset(tileCoord.x - 1, tileCoord.y);
		sf::Vector2i rightOffset(tileCoord.x + 1, tileCoord.y);
		sf::Vector2i upOffset(tileCoord.x, tileCoord.y - 1);
		sf::Vector2i downtOffset(tileCoord.x, tileCoord.y + 1);

		int leftIndex = generateMap->getTileIndexFromCoord(leftOffset);
		int rightIndex = generateMap->getTileIndexFromCoord(rightOffset);
		int upIndex = generateMap->getTileIndexFromCoord(upOffset);
		int downIndex = generateMap->getTileIndexFromCoord(downtOffset);

		constraintsContainer leftAllowed;
		constraintsContainer rightAllowed;
		constraintsContainer upAllowed;
		constraintsContainer downAllowed;
		
		constraintsContainer conTemp;
		if (leftIndex != -1) {
			getContraintsFromID(mapState[leftIndex], conTemp);
			leftAllowed.clone(conTemp);
		}
		else {
			setAllToConstraintContainer(leftAllowed);
		}

		if (rightIndex != -1) {
			getContraintsFromID(mapState[rightIndex], conTemp);
			rightAllowed.clone(conTemp);
		}
		else {
			setAllToConstraintContainer(rightAllowed);
		}

		if (upIndex != -1) {
			getContraintsFromID(mapState[upIndex], conTemp);
			upAllowed.clone(conTemp);
		}
		else {
			setAllToConstraintContainer(upAllowed);
		}

		if (downIndex != -1) {
			getContraintsFromID(mapState[downIndex], conTemp);
			downAllowed.clone(conTemp);
		}
		else {
			setAllToConstraintContainer(downAllowed);
		}

		std::vector<sf::Vector2i>commonList;

		sumContraints(leftAllowed.right, commonList);
		sumContraints(rightAllowed.left, commonList);
		sumContraints(upAllowed.down, commonList);
		sumContraints(downAllowed.up, commonList);
		mapEntropy[i] = 0;
		for (int com = 0; com < commonList.size(); com++) {
			if (commonList[com].y == 4) {
				mapEntropy[i]++;
			}
		}
 	}

	for (int y = 0; y < generateMap->sizeY; y++) {
		for (int x = 0; x < generateMap->sizeX; x++) {
			std::cout << mapEntropy[y * generateMap->sizeX + x] <<" ";
		}
		std::cout << "\n";
	}
}

void wfc::sumContraints(std::vector<int>& constraint, std::vector<sf::Vector2i>& commonList) {
	for (int i = 0; i < constraint.size(); i++) {
		bool found = false;
		int indexFound = 0;
		for (int com = 0; com < commonList.size(); com++) {
			if (commonList[com].x == constraint[i]) {
				found = true;
				indexFound = com;
				break;
			}
		}
		if (found) {
			commonList[indexFound].y++;
		}
		else {
			commonList.push_back({ constraint[i],1 });
		}
	}
}

void wfc::getContraintsFromID(int id, constraintsContainer& result) {
	result.left.clear();
	result.right.clear();
	result.up.clear();
	result.down.clear();
	if (id == -1) {
		for (int i = 0; i < constraints.size(); i++) {
			result.left.push_back(i);
			result.right.push_back(i);
			result.up.push_back(i);
			result.down.push_back(i);
		}
	}
	else {
		result.clone(constraints[id]);
	}
}

void wfc::generateConstraints() {
	for (int i = 0; i < exampleMap->textureSet->textureCount; i++) {
		std::cout << i << "\n";

		constraints.push_back({});
	}

	for (int i = 0; i < exampleMap->tiles.size(); i++) {
		int currentTileType = exampleMap->tiles[i].textureID;

		sf::Vector2i result;
		exampleMap->getTileCoordFromIndex(i, result);
		//std::cout << result.x << " " << result.y << " " << exampleMap->tiles[i].textureID << "\n";
		sf::Vector2i leftOffset(result.x - 1, result.y);
		sf::Vector2i rightOffset(result.x + 1, result.y);
		sf::Vector2i upOffset(result.x, result.y - 1);
		sf::Vector2i downtOffset(result.x, result.y + 1);

		int leftIndex = exampleMap->getTileIndexFromCoord(leftOffset);
		int rightIndex = exampleMap->getTileIndexFromCoord(rightOffset);
		int upIndex = exampleMap->getTileIndexFromCoord(upOffset);
		int downIndex = exampleMap->getTileIndexFromCoord(downtOffset);


		//std::cout << leftIndex << " " << leftOffset.x << " " << leftOffset.y << "\n";
		if (leftIndex != -1) {
			int testId = exampleMap->tiles[leftIndex].textureID;
			if (std::find(constraints[currentTileType].left.begin(), constraints[currentTileType].left.end(), exampleMap->tiles[leftIndex].textureID) == constraints[currentTileType].left.end()) {
				constraints[currentTileType].left.push_back(exampleMap->tiles[leftIndex].textureID);
				std::cout << exampleMap->tiles[leftIndex].textureID << "\n";
			}
		}

		if (rightIndex != -1) {
			int testId = exampleMap->tiles[rightIndex].textureID;
			if (std::find(constraints[currentTileType].right.begin(), constraints[currentTileType].right.end(), exampleMap->tiles[rightIndex].textureID) == constraints[currentTileType].right.end()) {
				constraints[currentTileType].right.push_back(exampleMap->tiles[rightIndex].textureID);
				std::cout << exampleMap->tiles[rightIndex].textureID << "\n";
			}
		}

		if (upIndex != -1) {
			int testId = exampleMap->tiles[upIndex].textureID;
			if (std::find(constraints[currentTileType].up.begin(), constraints[currentTileType].up.end(), exampleMap->tiles[upIndex].textureID) == constraints[currentTileType].up.end()) {
				constraints[currentTileType].up.push_back(exampleMap->tiles[upIndex].textureID);
				std::cout << exampleMap->tiles[upIndex].textureID << "\n";
			}
		}

		if (downIndex != -1) {
			int testId = exampleMap->tiles[downIndex].textureID;
			if (std::find(constraints[currentTileType].down.begin(), constraints[currentTileType].down.end(), exampleMap->tiles[downIndex].textureID) == constraints[currentTileType].down.end()) {
				constraints[currentTileType].down.push_back(exampleMap->tiles[downIndex].textureID);
				std::cout << exampleMap->tiles[downIndex].textureID << "\n";
			}
		}
	}

	for (int i = 0; i < exampleMap->textureSet->textureCount; i++) {
		std::cout << "tile " << i << " constraints computed | " << constraints[i].left.size() << " " << constraints[i].right.size() << " " << constraints[i].up.size() << " " << constraints[i].down.size() << "\n";
	}

	for (int i = 0; i < generateMap->tiles.size(); i++) {
		mapEntropy.push_back(0);
		mapState.push_back(-1);
	}

	mapState[30] = 4;

	computeEntropy();
	computeEntropy();

	std::cout << "done\n";
}