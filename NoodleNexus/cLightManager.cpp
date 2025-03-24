#include "cLightManager.h"
#include <float.h>
#include <sstream>	// String stream



cLightManager::cLightManager()
{
	// Clear the lights to a known default state
	for (unsigned int index = 0; index != cLightManager::NUMBEROFLIGHTS; index++)
	{
		this->theLights[index].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		// White coloured light
		this->theLights[index].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		// White specular highlight colour, NOT shiny (4th value of 1.0f)
		this->theLights[index].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		// Reasonable attenuation
		this->theLights[index].atten.x = 0.0f;		// x = constant
		this->theLights[index].atten.y = 0.1f;		// y = linear
		this->theLights[index].atten.z = 0.01f;		// z = quadratic
		this->theLights[index].atten.w = FLT_MAX;	// w = DistanceCutOff (FLT_MAX = infinite)

		this->theLights[index].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		// 0 = pointlight
		// 1 = spot light
		// 2 = directional light
		this->theLights[index].param1.x = 0.0f;		// x = lightType
		this->theLights[index].param1.y = 0.0f;		// y = inner angle
		this->theLights[index].param1.z = 0.0f;		// z = outer angle
		this->theLights[index].param1.w = 1.0f;		// (TDB) "To Be Determined"

		this->theLights[index].param2.x = 0.0f;		// x = 0 for off, 1 for on
		this->theLights[index].param2.y = 0.0f;		// (TDB) 
		this->theLights[index].param2.z = 0.0f;		// (TDB) 
		this->theLights[index].param2.w = 0.0f;		// (TDB) 
	}//for (unsigned int index
}

void cLightManager::loadUniformLocations(GLuint shaderProgram)
{
//	GLint light_00_position_UL = glGetUniformLocation(shaderProgram, "theLights[0].position");
	// and so on...

	this->theLights[0].position_UL = glGetUniformLocation(shaderProgram, "theLights[0].position");
	this->theLights[0].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[0].diffuse");
	this->theLights[0].specular_UL = glGetUniformLocation(shaderProgram, "theLights[0].specular");
	this->theLights[0].atten_UL = glGetUniformLocation(shaderProgram, "theLights[0].atten");
	this->theLights[0].direction_UL = glGetUniformLocation(shaderProgram, "theLights[0].direction");
	this->theLights[0].param1_UL = glGetUniformLocation(shaderProgram, "theLights[0].param1");
	this->theLights[0].param2_UL = glGetUniformLocation(shaderProgram, "theLights[0].param2");

	this->theLights[1].position_UL = glGetUniformLocation(shaderProgram, "theLights[1].position");
	this->theLights[1].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[1].diffuse");
	this->theLights[1].specular_UL = glGetUniformLocation(shaderProgram, "theLights[1].specular");
	this->theLights[1].atten_UL = glGetUniformLocation(shaderProgram, "theLights[1].atten");
	this->theLights[1].direction_UL = glGetUniformLocation(shaderProgram, "theLights[1].direction");
	this->theLights[1].param1_UL = glGetUniformLocation(shaderProgram, "theLights[1].param1");
	this->theLights[1].param2_UL = glGetUniformLocation(shaderProgram, "theLights[1].param2");

	this->theLights[2].position_UL = glGetUniformLocation(shaderProgram, "theLights[2].position");
	this->theLights[2].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[2].diffuse");
	this->theLights[2].specular_UL = glGetUniformLocation(shaderProgram, "theLights[2].specular");
	this->theLights[2].atten_UL = glGetUniformLocation(shaderProgram, "theLights[2].atten");
	this->theLights[2].direction_UL = glGetUniformLocation(shaderProgram, "theLights[2].direction");
	this->theLights[2].param1_UL = glGetUniformLocation(shaderProgram, "theLights[2].param1");
	this->theLights[2].param2_UL = glGetUniformLocation(shaderProgram, "theLights[2].param2");

	this->theLights[3].position_UL = glGetUniformLocation(shaderProgram, "theLights[3].position");
	this->theLights[3].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[3].diffuse");
	this->theLights[3].specular_UL = glGetUniformLocation(shaderProgram, "theLights[3].specular");
	this->theLights[3].atten_UL = glGetUniformLocation(shaderProgram, "theLights[3].atten");
	this->theLights[3].direction_UL = glGetUniformLocation(shaderProgram, "theLights[3].direction");
	this->theLights[3].param1_UL = glGetUniformLocation(shaderProgram, "theLights[3].param1");
	this->theLights[3].param2_UL = glGetUniformLocation(shaderProgram, "theLights[3].param2");

	this->theLights[4].position_UL = glGetUniformLocation(shaderProgram, "theLights[4].position");
	this->theLights[4].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[4].diffuse");
	this->theLights[4].specular_UL = glGetUniformLocation(shaderProgram, "theLights[4].specular");
	this->theLights[4].atten_UL = glGetUniformLocation(shaderProgram, "theLights[4].atten");
	this->theLights[4].direction_UL = glGetUniformLocation(shaderProgram, "theLights[4].direction");
	this->theLights[4].param1_UL = glGetUniformLocation(shaderProgram, "theLights[4].param1");
	this->theLights[4].param2_UL = glGetUniformLocation(shaderProgram, "theLights[4].param2");

	this->theLights[5].position_UL = glGetUniformLocation(shaderProgram, "theLights[5].position");
	this->theLights[5].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[5].diffuse");
	this->theLights[5].specular_UL = glGetUniformLocation(shaderProgram, "theLights[5].specular");
	this->theLights[5].atten_UL = glGetUniformLocation(shaderProgram, "theLights[5].atten");
	this->theLights[5].direction_UL = glGetUniformLocation(shaderProgram, "theLights[5].direction");
	this->theLights[5].param1_UL = glGetUniformLocation(shaderProgram, "theLights[5].param1");
	this->theLights[5].param2_UL = glGetUniformLocation(shaderProgram, "theLights[5].param2");

	this->theLights[6].position_UL = glGetUniformLocation(shaderProgram, "theLights[6].position");
	this->theLights[6].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[6].diffuse");
	this->theLights[6].specular_UL = glGetUniformLocation(shaderProgram, "theLights[6].specular");
	this->theLights[6].atten_UL = glGetUniformLocation(shaderProgram, "theLights[6].atten");
	this->theLights[6].direction_UL = glGetUniformLocation(shaderProgram, "theLights[6].direction");
	this->theLights[6].param1_UL = glGetUniformLocation(shaderProgram, "theLights[6].param1");
	this->theLights[6].param2_UL = glGetUniformLocation(shaderProgram, "theLights[6].param2");

	this->theLights[7].position_UL = glGetUniformLocation(shaderProgram, "theLights[7].position");
	this->theLights[7].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[7].diffuse");
	this->theLights[7].specular_UL = glGetUniformLocation(shaderProgram, "theLights[7].specular");
	this->theLights[7].atten_UL = glGetUniformLocation(shaderProgram, "theLights[7].atten");
	this->theLights[7].direction_UL = glGetUniformLocation(shaderProgram, "theLights[7].direction");
	this->theLights[7].param1_UL = glGetUniformLocation(shaderProgram, "theLights[7].param1");
	this->theLights[7].param2_UL = glGetUniformLocation(shaderProgram, "theLights[7].param2");

	this->theLights[8].position_UL = glGetUniformLocation(shaderProgram, "theLights[8].position");
	this->theLights[8].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[8].diffuse");
	this->theLights[8].specular_UL = glGetUniformLocation(shaderProgram, "theLights[8].specular");
	this->theLights[8].atten_UL = glGetUniformLocation(shaderProgram, "theLights[8].atten");
	this->theLights[8].direction_UL = glGetUniformLocation(shaderProgram, "theLights[8].direction");
	this->theLights[8].param1_UL = glGetUniformLocation(shaderProgram, "theLights[8].param1");
	this->theLights[8].param2_UL = glGetUniformLocation(shaderProgram, "theLights[8].param2");

	this->theLights[9].position_UL = glGetUniformLocation(shaderProgram, "theLights[9].position");
	this->theLights[9].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[9].diffuse");
	this->theLights[9].specular_UL = glGetUniformLocation(shaderProgram, "theLights[9].specular");
	this->theLights[9].atten_UL = glGetUniformLocation(shaderProgram, "theLights[9].atten");
	this->theLights[9].direction_UL = glGetUniformLocation(shaderProgram, "theLights[9].direction");
	this->theLights[9].param1_UL = glGetUniformLocation(shaderProgram, "theLights[9].param1");
	this->theLights[9].param2_UL = glGetUniformLocation(shaderProgram, "theLights[9].param2");

	this->theLights[10].position_UL = glGetUniformLocation(shaderProgram, "theLights[10].position");
	this->theLights[10].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[10].diffuse");
	this->theLights[10].specular_UL = glGetUniformLocation(shaderProgram, "theLights[10].specular");
	this->theLights[10].atten_UL = glGetUniformLocation(shaderProgram, "theLights[10].atten");
	this->theLights[10].direction_UL = glGetUniformLocation(shaderProgram, "theLights[10].direction");
	this->theLights[10].param1_UL = glGetUniformLocation(shaderProgram, "theLights[10].param1");
	this->theLights[10].param2_UL = glGetUniformLocation(shaderProgram, "theLights[10].param2");

	this->theLights[11].position_UL = glGetUniformLocation(shaderProgram, "theLights[11].position");
	this->theLights[11].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[11].diffuse");
	this->theLights[11].specular_UL = glGetUniformLocation(shaderProgram, "theLights[11].specular");
	this->theLights[11].atten_UL = glGetUniformLocation(shaderProgram, "theLights[11].atten");
	this->theLights[11].direction_UL = glGetUniformLocation(shaderProgram, "theLights[11].direction");
	this->theLights[11].param1_UL = glGetUniformLocation(shaderProgram, "theLights[11].param1");
	this->theLights[11].param2_UL = glGetUniformLocation(shaderProgram, "theLights[11].param2");

	this->theLights[12].position_UL = glGetUniformLocation(shaderProgram, "theLights[12].position");
	this->theLights[12].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[12].diffuse");
	this->theLights[12].specular_UL = glGetUniformLocation(shaderProgram, "theLights[12].specular");
	this->theLights[12].atten_UL = glGetUniformLocation(shaderProgram, "theLights[12].atten");
	this->theLights[12].direction_UL = glGetUniformLocation(shaderProgram, "theLights[12].direction");
	this->theLights[12].param1_UL = glGetUniformLocation(shaderProgram, "theLights[12].param1");
	this->theLights[12].param2_UL = glGetUniformLocation(shaderProgram, "theLights[12].param2");

	this->theLights[13].position_UL = glGetUniformLocation(shaderProgram, "theLights[13].position");
	this->theLights[13].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[13].diffuse");
	this->theLights[13].specular_UL = glGetUniformLocation(shaderProgram, "theLights[13].specular");
	this->theLights[13].atten_UL = glGetUniformLocation(shaderProgram, "theLights[13].atten");
	this->theLights[13].direction_UL = glGetUniformLocation(shaderProgram, "theLights[13].direction");
	this->theLights[13].param1_UL = glGetUniformLocation(shaderProgram, "theLights[13].param1");
	this->theLights[13].param2_UL = glGetUniformLocation(shaderProgram, "theLights[13].param2");

	this->theLights[14].position_UL = glGetUniformLocation(shaderProgram, "theLights[14].position");
	this->theLights[14].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[14].diffuse");
	this->theLights[14].specular_UL = glGetUniformLocation(shaderProgram, "theLights[14].specular");
	this->theLights[14].atten_UL = glGetUniformLocation(shaderProgram, "theLights[14].atten");
	this->theLights[14].direction_UL = glGetUniformLocation(shaderProgram, "theLights[14].direction");
	this->theLights[14].param1_UL = glGetUniformLocation(shaderProgram, "theLights[14].param1");
	this->theLights[14].param2_UL = glGetUniformLocation(shaderProgram, "theLights[14].param2");

	this->theLights[15].position_UL = glGetUniformLocation(shaderProgram, "theLights[15].position");
	this->theLights[15].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[15].diffuse");
	this->theLights[15].specular_UL = glGetUniformLocation(shaderProgram, "theLights[15].specular");
	this->theLights[15].atten_UL = glGetUniformLocation(shaderProgram, "theLights[15].atten");
	this->theLights[15].direction_UL = glGetUniformLocation(shaderProgram, "theLights[15].direction");
	this->theLights[15].param1_UL = glGetUniformLocation(shaderProgram, "theLights[15].param1");
	this->theLights[15].param2_UL = glGetUniformLocation(shaderProgram, "theLights[15].param2");

	this->theLights[16].position_UL = glGetUniformLocation(shaderProgram, "theLights[16].position");
	this->theLights[16].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[16].diffuse");
	this->theLights[16].specular_UL = glGetUniformLocation(shaderProgram, "theLights[16].specular");
	this->theLights[16].atten_UL = glGetUniformLocation(shaderProgram, "theLights[16].atten");
	this->theLights[16].direction_UL = glGetUniformLocation(shaderProgram, "theLights[16].direction");
	this->theLights[16].param1_UL = glGetUniformLocation(shaderProgram, "theLights[16].param1");
	this->theLights[16].param2_UL = glGetUniformLocation(shaderProgram, "theLights[16].param2");

	this->theLights[17].position_UL = glGetUniformLocation(shaderProgram, "theLights[17].position");
	this->theLights[17].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[17].diffuse");
	this->theLights[17].specular_UL = glGetUniformLocation(shaderProgram, "theLights[17].specular");
	this->theLights[17].atten_UL = glGetUniformLocation(shaderProgram, "theLights[17].atten");
	this->theLights[17].direction_UL = glGetUniformLocation(shaderProgram, "theLights[17].direction");
	this->theLights[17].param1_UL = glGetUniformLocation(shaderProgram, "theLights[17].param1");
	this->theLights[17].param2_UL = glGetUniformLocation(shaderProgram, "theLights[17].param2");

	this->theLights[18].position_UL = glGetUniformLocation(shaderProgram, "theLights[18].position");
	this->theLights[18].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[18].diffuse");
	this->theLights[18].specular_UL = glGetUniformLocation(shaderProgram, "theLights[18].specular");
	this->theLights[18].atten_UL = glGetUniformLocation(shaderProgram, "theLights[18].atten");
	this->theLights[18].direction_UL = glGetUniformLocation(shaderProgram, "theLights[18].direction");
	this->theLights[18].param1_UL = glGetUniformLocation(shaderProgram, "theLights[18].param1");
	this->theLights[18].param2_UL = glGetUniformLocation(shaderProgram, "theLights[18].param2");

	this->theLights[19].position_UL = glGetUniformLocation(shaderProgram, "theLights[19].position");
	this->theLights[19].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[19].diffuse");
	this->theLights[19].specular_UL = glGetUniformLocation(shaderProgram, "theLights[19].specular");
	this->theLights[19].atten_UL = glGetUniformLocation(shaderProgram, "theLights[19].atten");
	this->theLights[19].direction_UL = glGetUniformLocation(shaderProgram, "theLights[19].direction");
	this->theLights[19].param1_UL = glGetUniformLocation(shaderProgram, "theLights[19].param1");
	this->theLights[19].param2_UL = glGetUniformLocation(shaderProgram, "theLights[19].param2");

	return;
}


void cLightManager::updateShaderWithLightInfo(void)
{
	// Called every frame, updating the light information in the shader


	//glUniform4f(light_00_position_UL, this->theLights[0].position.x,
	//	this->theLights[0].position.y,
	//	this->theLights[0].position.z,
	//	1.0f);

	for (unsigned int index = 0; index != cLightManager::NUMBEROFLIGHTS; index++)
	{
		glUniform4f(
			this->theLights[index].position_UL,
			this->theLights[index].position.x,
			this->theLights[index].position.y,
			this->theLights[index].position.z,
			this->theLights[index].position.w);

		glUniform4f(
			this->theLights[index].diffuse_UL,
			this->theLights[index].diffuse.x,
			this->theLights[index].diffuse.y,
			this->theLights[index].diffuse.z,
			this->theLights[index].diffuse.w);

		glUniform4f(
			this->theLights[index].specular_UL,
			this->theLights[index].specular.x,
			this->theLights[index].specular.y,
			this->theLights[index].specular.z,
			this->theLights[index].specular.w);

		glUniform4f(
			this->theLights[index].atten_UL,
			this->theLights[index].atten.x,
			this->theLights[index].atten.y,
			this->theLights[index].atten.z,
			this->theLights[index].atten.w);

		glUniform4f(
			this->theLights[index].direction_UL,
			this->theLights[index].direction.x,
			this->theLights[index].direction.y,
			this->theLights[index].direction.z,
			this->theLights[index].direction.w);

		glUniform4f(
			this->theLights[index].param1_UL,
			this->theLights[index].param1.x,
			this->theLights[index].param1.y,
			this->theLights[index].param1.z,
			this->theLights[index].param1.w);

		glUniform4f(
			this->theLights[index].param2_UL,
			this->theLights[index].param2.x,
			this->theLights[index].param2.y,
			this->theLights[index].param2.z,
			this->theLights[index].param2.w);
	}//for (unsigned int index...

	return;
}

std::string cLightManager::sLight::getState(void)
{
	std::stringstream ssLightState;

	// "position 922.3  8882.9   22.3"
	ssLightState << "position "
		<< this->position.x << " "
		<< this->position.y << " "
		<< this->position.z << std::endl;
	// And so on...

	// return as a string
	return ssLightState.str();
}

bool cLightManager::sLight::loadState(std::string stateString)
{
	std::stringstream ssLightState;
	ssLightState << stateString;

	std::string discard;
	ssLightState >> discard;		// "position"
	ssLightState >> this->position.x;
	ssLightState >> this->position.y;
	ssLightState >> this->position.z;

	// And so on...

	// If it worked
	return true;
}
