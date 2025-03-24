#pragma once

#include "sharedThings.h"

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>

class BehaviourTreeNode {
public:
	virtual bool execute() = 0;
};

class ActionNode : public BehaviourTreeNode {
private:
	std::function<bool()> action;
public:
	ActionNode(std::function<bool()> action);
	bool execute() override;
};

class SequenceNode : public BehaviourTreeNode {
private:
	std::vector<BehaviourTreeNode*> children;
public:
	void addChild(BehaviourTreeNode* child);
	bool execute() override;
};

class SelectorNode : public BehaviourTreeNode {
private:
	std::vector<BehaviourTreeNode*> children;
public:
	void addChild(BehaviourTreeNode* child);
	bool execute() override;
};

class Enemy {
private:
	float speed;
	int currentWaypointIndex;
	std::vector<std::pair<float, float>> patrolPath;
	SelectorNode* behaviourTreeSelector;
	SequenceNode* behaviourTreeSequence;
	sMesh* pPlayerMesh;
	sMesh* pAssociatedMesh;

	sBullet* closestBullet;
	glm::vec3 spawnLocation;

	float moveX;
	float moveZ;

	float timer = 0.0f;

	bool isSelectorTree;

	bool moveTowardsWaypoint();
	void move();
public:
	Enemy(float speed, sMesh* pPlayerMesh, sMesh* pAssociatedMesh);

	void initializePatrolPath();

	void buildBehaviourTree_Enemy1();
	void buildBehaviourTree_Enemy2();
	void buildBehaviourTree_Enemy3();
	void buildBehaviourTree_Enemy4();

	bool executeBehaviourTreeSelector();
	bool executeBehaviourTreeSequence();

	bool enemyWithinCircle(float radius);

	bool isObjectACloserThanObjectB(sBullet* pA, sBullet* pB);

	bool isBulletClose(float distance);
	bool moveAwayFromBullet();

	float distanceToPlayer() const;

	void moveTowardsPlayer();
	void turnTowardsPlayer();

	void moveAwayFromPlayer();

	bool isPlayerFacingSoldier();

	bool selectorOrSequence();
	glm::vec3 getPosition();
	void resetPosition();

	std::string getName();
};
