#include "sharedThings.h"

cBasicFlyCamera* g_pFlyCamera = NULL;

cLightManager* g_pLightManager = NULL;
unsigned int g_selectedLightIndex = 0;
bool g_bShowDebugSpheres = false;
unsigned int g_selectedMeshIndex = 0;

glm::vec3 g_rgb_from_HTML(unsigned int red, unsigned int green, unsigned int blue)
{
	return glm::vec3((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f);
}

// HACK: Show "targeting LASER"
bool g_bShowAxis = false;

bool movingWalls = false;

std::vector<sBullet*> g_vecBullets;