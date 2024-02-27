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
	void computeEntropy();
	void setAllToConstraintContainer(constraintsContainer& constraint);
	void sumContraints(std::vector<int>& constraint, std::vector<sf::Vector2i>& commonList);
public:
	std::vector<constraintsContainer>constraints;
	std::vector<int>mapEntropy; // 0 is no possibility and we're in trouble;
	std::vector<int>mapState;// -1 means not collapsed yet;
private:
	mapManager* exampleMap;
	mapManager* generateMap;
};

