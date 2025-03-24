#include "sMesh.h"


sMesh::sMesh()
{
	//for (unsigned int index = 0; index < MAX_NUM_TEXTURES; index++)
	//{
	//	this->blendRatio[MAX_NUM_TEXTURES] = 0.0f;
	//}
	this->blendRatio[0] = 1.0f;
	this->blendRatio[1] = 0.0f;
	this->blendRatio[2] = 0.0f;
	this->blendRatio[3] = 0.0f;
	this->blendRatio[4] = 0.0f;
	this->blendRatio[5] = 0.0f;
	this->blendRatio[6] = 0.0f;
	this->blendRatio[7] = 0.0f;

	this->positionXYZ = glm::vec3(0.0f);
	this->rotationEulerXYZ = glm::vec3(0.0f);
	this->objectColourRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->bTranslateTexture = false;
	this->sTranslate = 0.0f;
	this->tTranslate = 0.0f;
}

std::string sMesh::getState(void)
{
	// TODO: Make string representing state
	return "";
}

bool sMesh::loadState(std::string newState)
{
	// TODO: Take a string and decode state
	return true;
}
