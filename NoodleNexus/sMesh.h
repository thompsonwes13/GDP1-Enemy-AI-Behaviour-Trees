#pragma once

// This is the 3D drawing information
//	to draw a single mesh (single PLY file)

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <string>

struct sMesh
{
	sMesh();
	std::string modelFileName;			// "bunny.ply"
	std::string uniqueFriendlyName;		// "Fred", "Ali", "Bunny_007"

	glm::vec3 positionXYZ;
	glm::vec3 rotationEulerXYZ;		// 90 degrees around the x axis
	float uniformScale = 1.0f;				// Same for each axis

	glm::vec4 objectColourRGBA;		// 0 - 1.0 
	// If true, it uses the colour above
	bool bOverrideObjectColour = true;

	bool bIsWireframe = false;
	bool bIsVisible = true;
	bool bDoNotLight = false;

	unsigned int uniqueID = 0;

//	std::string textureName;
	static const unsigned int MAX_NUM_TEXTURES = 8;
	std::string textures[MAX_NUM_TEXTURES];
	float blendRatio[MAX_NUM_TEXTURES];
	float textureScale;

	// 0.0 = invisible
	// 1.0 = solid 
	float alphaTransparency = 1.0f;

	bool bModulateTexture;
	bool bTranslateTexture;
	float sTranslate;
	float tTranslate;

	std::string getState(void);
	bool loadState(std::string newState);

//	sMesh* pChildMeshes[100];
	std::vector< sMesh* > vec_pChildMeshes;


};