#include "cPhysics.h"

cPhysics::cBroad_Cube::cBroad_Cube(
	glm::vec3 minXYZ, glm::vec3 maxXYZ,
	float sizeOrWidth,
	unsigned long long indexXYZ)
{
	this->m_minXYZ = minXYZ;
	this->m_maxXYZ = maxXYZ;
	this->m_size = sizeOrWidth;
	this->m_indexXYZ = indexXYZ;
}

unsigned long long cPhysics::cBroad_Cube::getGridIndexID(void)
{
	return this->m_indexXYZ;
}

float cPhysics::cBroad_Cube::getSize(void)
{
	return this->m_size;
}

glm::vec3 cPhysics::cBroad_Cube::getMinXYZ(void)
{
	return this->m_minXYZ;
}

glm::vec3 cPhysics::cBroad_Cube::getMaxXYZ(void)
{
	return this->m_maxXYZ;
}

glm::vec3 cPhysics::calcBP_MinXYZ_FromID(unsigned long long BP_CubeID, float sizeOrWidth)
{
	const unsigned long long MULTIPLIER = 1'000'000;

	long long xIndex = BP_CubeID / (MULTIPLIER * MULTIPLIER);

	long long yIndex = BP_CubeID - (xIndex * MULTIPLIER * MULTIPLIER);	// Strip away the x digits
	yIndex = yIndex / MULTIPLIER;

	long long zIndex = BP_CubeID - (xIndex * MULTIPLIER * MULTIPLIER) - (yIndex * MULTIPLIER);
		
	glm::vec3 minXYZ = glm::vec3(0.0f);

	// The 1st digit indicates +ve or -ve, 100,000 --> -ve 
	if (xIndex > 99'999)
	{
		// It's negative, so strip away the 1st digit
		minXYZ.x = -(((xIndex + 1) - 100'000) * sizeOrWidth);
	}
	else
	{
		minXYZ.x = xIndex * sizeOrWidth;
	}

	if (yIndex > 99'999)
	{
		// It's negative
		minXYZ.y = -(((yIndex + 1) - 100'000) * sizeOrWidth);
	}
	else
	{
		minXYZ.y = yIndex * sizeOrWidth;
	}

	if (zIndex > 99'999)
	{
		// It's negative
		minXYZ.z = -(((zIndex + 1) - 100'000) * sizeOrWidth);
	}
	else
	{
		minXYZ.z = zIndex * sizeOrWidth;
	}

	return minXYZ;
}


unsigned long long cPhysics::calcBP_GridIndex(
	float x, float y, float z, float sizeOrWidth)
{
	// Calculate the "rounded down" index based on the AABB size:
	int xIndexMin = (int)(x / sizeOrWidth);
	int yIndexMin = (int)(y / sizeOrWidth);
	int zIndexMin = (int)(z / sizeOrWidth);

	// "shift" this value by 6 digits
	// 
	// unsigned long long: 
	// 
	// xxx,xxx   yyy,yyy   zzz,zzz
	// 
	// Where 1st digit is sign
	//
	const unsigned long long NEGATIVE_DIGIT = 100'000;
	if (x < 0)
	{
		xIndexMin = abs(xIndexMin) + NEGATIVE_DIGIT;
	}
	if (y < 0)
	{
		yIndexMin = abs(yIndexMin) + NEGATIVE_DIGIT;
	}
	if (z < 0)
	{
		zIndexMin = abs(zIndexMin) + NEGATIVE_DIGIT;
	}

	const unsigned long long MULTIPLIER = 1'000'000;
	unsigned long long gridIndexID =
		xIndexMin * MULTIPLIER * MULTIPLIER
		+ yIndexMin * MULTIPLIER
		+ zIndexMin;

	return gridIndexID;
}


bool cPhysics::generateBroadPhaseGrid(std::string meshModelName, float AABBCubeSize_or_Width)
{
	// 1. Get the mesh (triangle) information
	// 2. For each triangle, and each vertex, 
	//    see what AABB they would be in
	// 3. If there's no AABB there, make one
	// 4. Add those triangles to that AABB

	if (!this->m_pVAOManager)
	{
		// Haven't set the VAO manager pointer, yet
		return false;
	}

	std::vector<cVAOManager::sTriangle> vecVAOTriangles;
	if (!this->m_pVAOManager->getTriangleMeshInfo(meshModelName, vecVAOTriangles))
	{
		// Can't find mesh
		return false;
	}


	for (std::vector<cVAOManager::sTriangle>::iterator itVAOTri = vecVAOTriangles.begin();
		itVAOTri != vecVAOTriangles.end(); itVAOTri++)
	{
		cVAOManager::sTriangle curVAOTri = *itVAOTri;

		sTriangle curTri;
		curTri.vertices[0] = curVAOTri.vertices[0];
		curTri.vertices[1] = curVAOTri.vertices[1];
		curTri.vertices[2] = curVAOTri.vertices[2];
		curTri.normal = curVAOTri.normal;

		// For each vertex, calculate the AABB index it would be in
		unsigned long long vert0_AABB_ID =
			this->calcBP_GridIndex(curTri.vertices[0].x, curTri.vertices[0].y, curTri.vertices[0].z, AABBCubeSize_or_Width);
		// Store the triangle this vertex is in inside the approprirate AABB
		// Is there already an AABB there
		std::map< unsigned long long /*index*/, cBroad_Cube* >::iterator itAAAB_V0 = this->map_BP_CubeGrid.find(vert0_AABB_ID);
		// There?
		if (itAAAB_V0 == this->map_BP_CubeGrid.end())
		{
			// Nope, so make one

			glm::vec3 minXYZ = this->calcBP_MinXYZ_FromID(vert0_AABB_ID, AABBCubeSize_or_Width);
			glm::vec3 maxXYZ = minXYZ + glm::vec3(AABBCubeSize_or_Width);

			cBroad_Cube* pCube = new cBroad_Cube(minXYZ, maxXYZ, AABBCubeSize_or_Width, vert0_AABB_ID);

			// Add it to the map
			this->map_BP_CubeGrid[vert0_AABB_ID] = pCube;
		}
		// Add that triangle to the AABB 
		this->map_BP_CubeGrid[vert0_AABB_ID]->vec_pTriangles.push_back(curTri);

//		unsigned long long vert1_AABB_ID =
//			this->calcBP_GridIndex(curTri.vertices[1].x, curTri.vertices[1].y, curTri.vertices[1].z, AABBCubeSize_or_Width);
//		unsigned long long vert2_AABB_ID =
//			this->calcBP_GridIndex(curTri.vertices[1].x, curTri.vertices[2].y, curTri.vertices[2].z, AABBCubeSize_or_Width);





	}//for (...sTriangle>::iterator itVAOTri...

	return true;
}


//void cPhysics::initBroadPhaseGrid(void)
//{
//	unsigned int GRID_SIZE = 100;
//	//std::vector< std::vector< std::vector< sBroad_Cube* > > > vec_3D_CubeArray;
//	for (unsigned int x = 0; x != GRID_SIZE; x++)
//	{
//		std::vector< std::vector< sBroad_Cube* > > vecTempYRow;
//		for (unsigned int y = 0; y != GRID_SIZE; y++)
//		{
//			std::vector< sBroad_Cube* > vecTempZRow;
//			for (unsigned int z = 0; z != GRID_SIZE; z++)
//			{
//				vecTempZRow.push_back(new sBroad_Cube());
//			}
//			vecTempYRow.push_back(vecTempZRow);
//		}
//		this->vec_3D_CubeArray.push_back(vecTempYRow);
//	}
//	return;
//}

