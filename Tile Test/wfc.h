#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "mapManager.h"

class constraintsContainer {
public:
	constraintsContainer();
	void clone(const constraintsContainer& copy);
	void computeEntropy();
public:
	std::vector<int> left;
	std::vector<int> right;
	std::vector<int> up;
	std::vector<int> down;
};

class wfc
{
public:
	wfc(mapManager& exampleMap_, mapManager& generateMap_);
	void getContraintsFromID(int id, constraintsContainer& result);
	void generateConstraints();
	void computeEntropy(bool narrowRange = false, int mid = 0);
	void setAllToConstraintContainer(constraintsContainer& constraint);
	void sumContraints(std::vector<int>& constraint, std::vector<sf::Vector2i>& commonList);
	void computeIterate();
	void computeReset();
	bool isComputeFinished();
	int weightedPick(std::vector<int> possibleOpt);
public:
	std::vector<constraintsContainer>constraints;
	std::vector<int>mapEntropy; // 0 is no possibility and we're in trouble;
	std::vector<std::vector<int>>possibleState;
	std::vector<int>mapState;// -1 means not collapsed yet;
	std::vector<int>collapseHistory;
	std::vector<constraintsContainer> convertedLookup;
	std::vector<int> weightedIndex;
	constraintsContainer blankAll;
	int reAttempt;
	int lastRe;
	int iterated;
private:
	mapManager* exampleMap;
	mapManager* generateMap;
};

