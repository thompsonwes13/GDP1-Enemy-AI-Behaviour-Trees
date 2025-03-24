#include "EnemyBehaviourTree.h"

ActionNode::ActionNode(std::function<bool()> action) : action(action) {}

bool ActionNode::execute() {
	return action();
}

void SequenceNode::addChild(BehaviourTreeNode* child) {
	children.push_back(child);
}

bool SequenceNode::execute() {
	for (auto& child : children) {
		if (!child->execute()) {
			return false;
		}
	}

	return true;
}

void SelectorNode::addChild(BehaviourTreeNode* child) {
	children.push_back(child);
}

bool SelectorNode::execute() {
	for (auto& child : children) {
		if (child->execute()) {
			return true;
		}
	}

	return false;
}

Enemy::Enemy(float speed, sMesh* pPlayerMesh, sMesh* pAssociatedMesh) :
			 speed(speed), pPlayerMesh(pPlayerMesh), pAssociatedMesh(pAssociatedMesh)
{
	currentWaypointIndex = rand() % 4;
	spawnLocation = pAssociatedMesh->positionXYZ;
	initializePatrolPath();
}

void Enemy::initializePatrolPath()
{
	patrolPath.push_back({ 100.0f,-100.0f });
	patrolPath.push_back({ -100.0f,-100.0f });
	patrolPath.push_back({ -100.0f, 100.0f });
	patrolPath.push_back({ 100.0f, 100.0f });
}

bool Enemy::moveTowardsWaypoint() 
{
	std::pair<float, float> waypoint = patrolPath[currentWaypointIndex];

	glm::vec2 currentXY = glm::vec2(pAssociatedMesh->positionXYZ.x, pAssociatedMesh->positionXYZ.z);
	glm::vec2 waypointXY = glm::vec2(waypoint.first, waypoint.second);

	float distToWaypoint = glm::distance(currentXY, waypointXY);

	if (distToWaypoint < 1.0f) {
		currentWaypointIndex = (currentWaypointIndex + 1) % patrolPath.size();
	}

	moveX = (waypoint.first - pAssociatedMesh->positionXYZ.x) * speed / distToWaypoint;
	moveZ = (waypoint.second - pAssociatedMesh->positionXYZ.z) * speed / distToWaypoint;

	// Rotate Mesh
	pAssociatedMesh->rotationEulerXYZ.y = -(atan(moveZ / moveX) * 56);
	if (moveX < 0)
	{
		pAssociatedMesh->rotationEulerXYZ.y += 180;
	}

	move();
	return distToWaypoint < 1.0f;
}

void Enemy::move() 
{
	pAssociatedMesh->positionXYZ.x += moveX;
	pAssociatedMesh->positionXYZ.z += moveZ;
}

bool Enemy::enemyWithinCircle(float radius)
{
	if ((pAssociatedMesh->positionXYZ.x - pPlayerMesh->positionXYZ.x) * (pAssociatedMesh->positionXYZ.x - pPlayerMesh->positionXYZ.x) +
		(pAssociatedMesh->positionXYZ.z - pPlayerMesh->positionXYZ.z) * (pAssociatedMesh->positionXYZ.z - pPlayerMesh->positionXYZ.z) <= radius * radius)
	{
		return true; // Within circle
	}
	return false; // Not within circle
}

bool Enemy::isObjectACloserThanObjectB(sBullet* pA, sBullet* pB)
{
	float distanceToA = glm::distance(pAssociatedMesh->positionXYZ, pA->pAssociatedMesh->positionXYZ); // Soldier mesh, bullet mesh
	float distanceToB = glm::distance(pAssociatedMesh->positionXYZ, pB->pAssociatedMesh->positionXYZ); // Soldier mesh, bullet mesh

	if (distanceToA > distanceToB)
	{
		return true;
	}
	return false;
}

bool Enemy::isBulletClose(float distance)
{
	std::vector<sBullet*> vec_playerBullets;
	for (sBullet* pCurrentBullet : g_vecBullets) // Copy player bullets
	{
		if (pCurrentBullet->pSoldierMesh->uniqueFriendlyName == "Player")
		{
			vec_playerBullets.push_back(pCurrentBullet);
		}
	}

	if (vec_playerBullets.size() == 0)
		return false;

	std::sort(vec_playerBullets.begin(), vec_playerBullets.end(), [&](sBullet* pA, sBullet* pB) // Sort by distance to soldier
		{
			float distanceToA = glm::distance(pAssociatedMesh->positionXYZ, pA->pAssociatedMesh->positionXYZ); // Soldier mesh, bullet mesh
			float distanceToB = glm::distance(pAssociatedMesh->positionXYZ, pB->pAssociatedMesh->positionXYZ); // Soldier mesh, bullet mesh

			if (distanceToA < distanceToB)
			{
				return true;
			}
			return false;
		}); 
	closestBullet = vec_playerBullets[0];

	if (glm::distance(closestBullet->pAssociatedMesh->positionXYZ, pAssociatedMesh->positionXYZ) < distance) // Closest bullet is close enough to dodge
	{
		return true;
	}

	return false;
}
bool Enemy::moveAwayFromBullet()
{
	glm::vec2 distance_to_bullet = closestBullet->pAssociatedMesh->positionXYZ - pAssociatedMesh->positionXYZ;

	moveX = -distance_to_bullet.x * speed * 0.4;
	moveZ = -distance_to_bullet.y * speed * 0.4;

	// Rotate Mesh
	pAssociatedMesh->rotationEulerXYZ.y = -(atan(moveZ / moveX) * 56);
	if (moveX < 0)
	{
		pAssociatedMesh->rotationEulerXYZ.y += 180;
	}

	move();

	return false;
}

float Enemy::distanceToPlayer() const {
	return sqrt(pow(pAssociatedMesh->positionXYZ.x - pPlayerMesh->positionXYZ.x, 2) + pow(pAssociatedMesh->positionXYZ.y - pPlayerMesh->positionXYZ.y, 2) + pow(pAssociatedMesh->positionXYZ.z - pPlayerMesh->positionXYZ.z, 2));
}

void Enemy::moveTowardsPlayer()
{
	float directionX = pPlayerMesh->positionXYZ.x - pAssociatedMesh->positionXYZ.x;
	float directionZ = pPlayerMesh->positionXYZ.z - pAssociatedMesh->positionXYZ.z;

	float distanceToCube = sqrt(directionX * directionX + directionZ * directionZ);

	if (distanceToCube != 0) {
		directionX /= distanceToCube;
		directionZ /= distanceToCube;
	}

	moveX = directionX * speed;
	moveZ = directionZ * speed;

	// Rotate Mesh
	pAssociatedMesh->rotationEulerXYZ.y = -(atan(moveZ / moveX) * 56);
	if (moveX < 0)
	{
		pAssociatedMesh->rotationEulerXYZ.y += 180;
	}

	move();
}

void Enemy::turnTowardsPlayer()
{
	float directionX = pPlayerMesh->positionXYZ.x - pAssociatedMesh->positionXYZ.x;
	float directionZ = pPlayerMesh->positionXYZ.z - pAssociatedMesh->positionXYZ.z;

	float distanceToCube = sqrt(directionX * directionX + directionZ * directionZ);

	if (distanceToCube != 0) {
		directionX /= distanceToCube;
		directionZ /= distanceToCube;
	}

	moveX = directionX * speed;
	moveZ = directionZ * speed;

	// Rotate Mesh
	pAssociatedMesh->rotationEulerXYZ.y = -(atan(moveZ / moveX) * 56);
	if (moveX < 0)
	{
		pAssociatedMesh->rotationEulerXYZ.y += 180;
	}
}

void Enemy::moveAwayFromPlayer()
{
	float directionX = pPlayerMesh->positionXYZ.x - pAssociatedMesh->positionXYZ.x;
	float directionZ = pPlayerMesh->positionXYZ.z - pAssociatedMesh->positionXYZ.z;

	float distanceToCube = sqrt(directionX * directionX + directionZ * directionZ);

	if (distanceToCube != 0) {
		directionX /= distanceToCube;
		directionZ /= distanceToCube;
	}

	moveX = -directionX * speed;
	moveZ = -directionZ * speed;

	// Rotate Mesh
	pAssociatedMesh->rotationEulerXYZ.y = -(atan(moveZ / moveX) * 56);
	if (moveX < 0)
	{
		pAssociatedMesh->rotationEulerXYZ.y += 180;
	}

	move();
}

bool Enemy::isPlayerFacingSoldier()
{
	int camQuadrant = 0;
	int enemyQaudrant = 0;

	if (g_pFlyCamera->getTarget().x > 0 && g_pFlyCamera->getTarget().z > 0) // +x +z
	{
		camQuadrant = 0;
	}
	else if (g_pFlyCamera->getTarget().x > 0 && g_pFlyCamera->getTarget().z < 0) // +x -z
	{
		camQuadrant = 1;
	}
	else if (g_pFlyCamera->getTarget().x < 0 && g_pFlyCamera->getTarget().z > 0) // -x +z
	{
		camQuadrant = 2;
	}
	else if (g_pFlyCamera->getTarget().x < 0 && g_pFlyCamera->getTarget().z < 0) // -x -z
	{
		camQuadrant = 3;
	}

	float directionX = pAssociatedMesh->positionXYZ.x - pPlayerMesh->positionXYZ.x;
	float directionZ = pAssociatedMesh->positionXYZ.z - pPlayerMesh->positionXYZ.z;

	if (directionX > 0 && directionZ > 0) // +x +z
	{
		enemyQaudrant = 0;
	}
	else if (directionX > 0 && directionZ < 0) // +x -z
	{
		enemyQaudrant = 1;
	}
	else if (directionX < 0 && directionZ > 0) // -x +z
	{
		enemyQaudrant = 2;
	}
	else if (directionX < 0 && directionZ < 0) // -x -z
	{
		enemyQaudrant = 3;
	}

	float inverseMoveX = abs(moveX / speed);
	float inverseMoveZ = abs(moveZ / speed);

	if (camQuadrant == enemyQaudrant) // If enemy is in quadrant that camera is facing
	{
		if (abs(g_pFlyCamera->getTarget().x) - inverseMoveX < 0.1 &&
			abs(g_pFlyCamera->getTarget().z) - inverseMoveZ < 0.1) // If enemy is in "vision cone"
		{
			return true;
		}
	}

	return false;
}

// ===================================== BEHAVIOUR TREES ===================================== //

bool Enemy::executeBehaviourTreeSelector()
{
	return behaviourTreeSelector->execute();
}

bool Enemy::executeBehaviourTreeSequence()
{
	return behaviourTreeSequence->execute();
}

void Enemy::buildBehaviourTree_Enemy1() // Enemy 1: Chase and Flee
{
	behaviourTreeSelector = new SelectorNode();
	isSelectorTree = true;

	SequenceNode* fleeSequence = new SequenceNode();
	fleeSequence->addChild(new ActionNode([this]() { // Check if player is facing soldier
		return (isPlayerFacingSoldier());
		}));
	fleeSequence->addChild(new ActionNode([this]() { // Evade!
		this->moveAwayFromPlayer();
		return true;
		}));

	behaviourTreeSelector->addChild(fleeSequence);

	behaviourTreeSelector->addChild(new ActionNode([this]() { // Move towards player
		this->moveTowardsPlayer();
		return true;
		}));
}

void Enemy::buildBehaviourTree_Enemy2() // Enemy 2: Chases and Dodges Bullets
{
	behaviourTreeSelector = new SelectorNode();
	isSelectorTree = true;

	SequenceNode* dodgeSequence = new SequenceNode();
	dodgeSequence->addChild(new ActionNode([this]() { // Check if bullet is close
		return (isBulletClose(10.0f));
		}));
	dodgeSequence->addChild(new ActionNode([this]() { // Evade!
		this->moveAwayFromBullet();
		return true;
		}));

	behaviourTreeSelector->addChild(dodgeSequence);

	behaviourTreeSelector->addChild(new ActionNode([this]() { // Move towards player
		this->moveTowardsPlayer();
		return true; 
	}));
}

void Enemy::buildBehaviourTree_Enemy3() // Enemy 3: Chases and Shoots
{
	behaviourTreeSequence = new SequenceNode();
	isSelectorTree = false;

	behaviourTreeSequence->addChild(new ActionNode([this]() { // Move towards player
		if (enemyWithinCircle(40.0f))
		{
			return true;
		}
		timer = -1;
		this->moveTowardsPlayer();
		return false;
		}));

	behaviourTreeSequence->addChild(new ActionNode([this]() { // Shoot at player
		timer++;
		turnTowardsPlayer();
		if ((int)timer % 150 == 0)
		{
			glm::vec2 direction = glm::vec2(moveX, moveZ);
			g_ShootBullet(pAssociatedMesh, direction);
		}
		return true;
		}));
}

void Enemy::buildBehaviourTree_Enemy4() // Enemy 4: Wanders and Idles
{
	behaviourTreeSequence = new SequenceNode();
	isSelectorTree = false;

	behaviourTreeSequence->addChild(new ActionNode([this]() { // Wander across path
		timer += 0.015;
		
		if (timer < 6.0f)
		{
			this->moveTowardsWaypoint(); // Move while timer < 6 seconds
			return false;
		}
		return true;
		}));

	behaviourTreeSequence->addChild(new ActionNode([this]() { // Idle

		if (timer >= 9.0f) // Reset timer if over 9 seconds (begin moving again)
		{
			timer = 0.0f;
			return false;
		}
		return true; // Idle while timer < 9 seconds
		}));
}

bool Enemy::selectorOrSequence()
{
	return isSelectorTree;
}

glm::vec3 Enemy::getPosition()
{
	return pAssociatedMesh->positionXYZ;
}

void Enemy::resetPosition()
{
	pAssociatedMesh->positionXYZ = spawnLocation;
}

std::string Enemy::getName()
{
	return pAssociatedMesh->uniqueFriendlyName;
}