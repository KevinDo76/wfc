#include "wfc.h"
#include "mapManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

constraintsContainer::constraintsContainer() {}

void constraintsContainer::clone(const constraintsContainer& copy) {

	left = copy.left;
	right = copy.right;
	up = copy.up;
	down = copy.down;
}

wfc::wfc(mapManager& exampleMap_, mapManager& generateMap_) : reAttempt(0), lastRe(0), iterated(0) {
	exampleMap = &exampleMap_;
	generateMap = &generateMap_;
}

bool wfc::isComputeFinished() {
	for (int i = 0; i < mapEntropy.size(); i++) {
		if (mapEntropy[i] != -1) {
			return false;
		}
	}
	return true;
}

void wfc::setAllToConstraintContainer(constraintsContainer& constraint) {
	std::vector<int>temp;
	for (int i = 0; i < exampleMap->textureSet->textureCount; i++) {
		temp.push_back(i);
	}
	constraint.left = temp;
	constraint.right = temp;
	constraint.up = temp;
	constraint.down = temp;
}

void wfc::computeEntropy() {
	for (int i = 0; i<generateMap->tiles.size(); i++) {
		if (mapState[i] != -1) {
			possibleState[i].clear();
			possibleState[i].emplace_back(mapState[i]);
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
		//constraintsContainer all

		if (leftIndex != -1) {
			leftAllowed = convertedLookup[mapState[leftIndex]+1];
		}
		else {
			setAllToConstraintContainer(leftAllowed);
		}

		if (rightIndex != -1) {
			rightAllowed = convertedLookup[mapState[rightIndex]+1];
		}
		else {
			setAllToConstraintContainer(rightAllowed);
		}

		if (upIndex != -1) {
			upAllowed = convertedLookup[mapState[upIndex]+1];
		}
		else {
			setAllToConstraintContainer(upAllowed);
		}

		if (downIndex != -1) {
			downAllowed = convertedLookup[mapState[downIndex]+1];
		}
		else {
			setAllToConstraintContainer(downAllowed);
		}

		std::vector<sf::Vector2i>commonList;

		commonList.reserve(sizeof(int) * constraints.size() * 3);
		for (int i = 0; i < constraints.size(); i++) {
			commonList.emplace_back( i,0 );
		}

		sumContraints(leftAllowed.right, commonList);
		sumContraints(rightAllowed.left, commonList);
		sumContraints(upAllowed.down, commonList);
		sumContraints(downAllowed.up, commonList);
		mapEntropy[i] = 0;
		possibleState[i].clear();
		possibleState[i].reserve(sizeof(int) * commonList.size());
		for (int com = 0; com < commonList.size(); com++) {
			if (commonList[com].y == 4) {
				mapEntropy[i]++;
				possibleState[i].emplace_back(commonList[com].x);
			}
		}
 	}

	for (int y = 0; y < generateMap->sizeY; y++) {
		for (int x = 0; x < generateMap->sizeX; x++) {
			//std::cout << mapEntropy[y * generateMap->sizeX + x] <<" ";
		}
		//std::cout << "\n";
	}
}

void wfc::sumContraints(std::vector<int>& constraint, std::vector<sf::Vector2i>& commonList) {
	for (int i = 0; i < constraint.size(); i++) {
		commonList[constraint[i]].y++;
	}
}

void wfc::getContraintsFromID(int id, constraintsContainer& result) {
	result.left.clear();
	result.left.reserve(sizeof(int) * 12);
	if (id == -1) {
		for (int i = 0; i < constraints.size(); i++) {
			result.left.push_back(i);
		}
		result.right = result.left;
		result.up = result.left;
		result.down = result.left;
	}
	else {
		result = constraints[id];
	}
}

void wfc::generateConstraints() {
	for (int i = 0; i < exampleMap->textureSet->textureCount; i++) {
		constraints.push_back({});
		weightedIndex.push_back(0);
	}

	for (int i = 0; i < exampleMap->tiles.size(); i++) {
		int currentTileType = exampleMap->tiles[i].textureID;

		weightedIndex[currentTileType]++;

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
		possibleState.push_back({});
		for (int type = 0; type < exampleMap->textureSet->textureCount; type++) {
			possibleState[i].push_back(type);
		}
	}

	constraintsContainer blankAll;
	setAllToConstraintContainer(blankAll);
	convertedLookup.push_back(blankAll);
	for (int i = 0; i < constraints.size(); i++) {
		convertedLookup.push_back(constraints[i]);
	}

	computeEntropy();

	std::cout << "done\n";
}

void wfc::computeReset() {
	mapEntropy.clear();
	mapState.clear();
	possibleState.clear();
	for (int i = 0; i < generateMap->tiles.size(); i++) {
		mapEntropy.push_back(0);
		mapState.push_back(-1);
		possibleState.push_back({});
		for (int type = 0; type < exampleMap->textureSet->textureCount; type++) {
			possibleState[i].push_back(type);
		}
		generateMap->tiles[i].updateTextureID(0);
	}
	iterated = 0;
	computeEntropy();
}

int wfc::weightedPick(std::vector<int> possibleOpt) {
	int possibleSum = 0;
	std::vector<int> probIndex;
	for (int i = 0; i < possibleOpt.size(); i++) {
		possibleSum+=weightedIndex[possibleOpt[i]];
	}
	probIndex.reserve(sizeof(int) * possibleSum);
	for (int i = 0; i < possibleOpt.size(); i++) {
		for (int p = 0; p < weightedIndex[possibleOpt[i]]; p++) {
			probIndex.push_back(i);
		}
	}
	return probIndex[(((float)rand()/(float)RAND_MAX) * (probIndex.size() - 1))];
}

void wfc::computeIterate() {
	if (isComputeFinished()) {
		std::cout << "fin\n";
		return;
	}
	int lowestEntropy = exampleMap->textureSet->textureCount + 1;
	std::vector<int>indexWithLowestEntrophy;
	for (int i = 0; i < mapEntropy.size(); i++) {
		if (mapEntropy[i] > -1 && mapEntropy[i] < lowestEntropy) {
			lowestEntropy = mapEntropy[i];
		}
	}

	for (int i = 0; i < mapEntropy.size(); i++) {
		if (mapEntropy[i] == lowestEntropy) {
			indexWithLowestEntrophy.push_back(i);
			//std::cout << "adding " << i << "\n";
		}
	}
	rand();
	int selectedSwitch = int(rand() / (float)RAND_MAX * ((float)indexWithLowestEntrophy.size()-1));
	//selectedSwitch = 0;
	int selectedTile = (((float)rand() / (float)RAND_MAX) * ((float)possibleState[indexWithLowestEntrophy[selectedSwitch]].size())-1);
	selectedTile = weightedPick(possibleState[indexWithLowestEntrophy[selectedSwitch]]);
	mapState[indexWithLowestEntrophy[selectedSwitch]] = possibleState[indexWithLowestEntrophy[selectedSwitch]][selectedTile];
	generateMap->tiles[indexWithLowestEntrophy[selectedSwitch]].updateTextureID(possibleState[indexWithLowestEntrophy[selectedSwitch]][selectedTile]);
	//std::cout << "selected tile " << indexWithLowestEntrophy[selectedSwitch] << " with possible index of " << selectedTile << " which is " << possibleState[indexWithLowestEntrophy[selectedSwitch]][selectedTile] << "\n";
	computeEntropy();
	iterated++;
	collapseHistory.push_back(indexWithLowestEntrophy[selectedSwitch]);
	for (int i = 0; i < mapEntropy.size(); i++) {
		if (mapEntropy[i] == 0) {
			std::cout << reAttempt << "\n";
			if (lastRe == indexWithLowestEntrophy[selectedSwitch]) {
				reAttempt++;
			}
			else {
				reAttempt = 0;
			}
			for (int re = 0; re<int(reAttempt / 2) + 1; re++) {
				int indexToRid = collapseHistory[collapseHistory.size() - 1];
				collapseHistory.pop_back();
				lastRe = indexWithLowestEntrophy[selectedSwitch];
				generateMap->tiles[indexToRid].updateTextureID(0);
				mapState[indexToRid] = -1;
			}
			computeEntropy();
			if (iterated > generateMap->sizeX * generateMap->sizeY * 2) {
				computeReset();
			}
		}
	}
;}