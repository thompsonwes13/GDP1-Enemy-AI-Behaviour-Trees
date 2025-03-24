#include "GLCommon.h"

#include "sharedThings.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <algorithm>

#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cLightHelper/cLightHelper.h"

extern cVAOManager* g_pMeshManager;
extern cBasicTextureManager* g_pTextures;

sMesh* pDebugSphere = NULL;

bool isObjectACloserThanObjectB(sMesh* pA, sMesh* pB)
{
    float distanceToA = glm::distance(::g_pFlyCamera->getEyeLocation(), pA->positionXYZ);
    float distanceToB = glm::distance(::g_pFlyCamera->getEyeLocation(), pB->positionXYZ);

    if (distanceToA > distanceToB)
    {
        return true;
    }
    return false;
}

// Texture set up
void SetUpTextures(sMesh* pCurMesh, GLuint program)
{
    GLuint MissingTexture_ID = ::g_pTextures->getTextureIDFromName("UV_Test_750x750.bmp");

    {
        GLuint textureID_00 = ::g_pTextures->getTextureIDFromName(pCurMesh->textures[0]);

        GLint textureScale00_UL = glGetUniformLocation(program, "textureScale00");
        glUniform1f(textureScale00_UL, (GLfloat)pCurMesh->textureScale);

        GLint sTranslate_UL = glGetUniformLocation(program, "sTranslate");
        glUniform1f(sTranslate_UL, (GLfloat)pCurMesh->sTranslate);

        GLint tTranslate_UL = glGetUniformLocation(program, "tTranslate");
        glUniform1f(tTranslate_UL, (GLfloat)pCurMesh->tTranslate);

        if (textureID_00 == 0)
        {
            textureID_00 = MissingTexture_ID;
        }        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, textureID_00);

        //glBindTextureUnit(0, textureID_00);

        GLint texture00_UL = glGetUniformLocation(program, "texture00");
        glUniform1i(texture00_UL, 0);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
    }

    {
        GLuint textureID_01 = ::g_pTextures->getTextureIDFromName(pCurMesh->textures[1]);

        GLint textureScale01_UL = glGetUniformLocation(program, "textureScale01");
        glUniform1f(textureScale01_UL, (GLfloat)pCurMesh->textureScale);

        if (textureID_01 == 0)
        {
            textureID_01 = MissingTexture_ID;

        }        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, textureID_01);
        GLint texture01_UL = glGetUniformLocation(program, "texture01");
        glUniform1i(texture01_UL, 1);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
    }

    {
        GLuint textureID_02 = ::g_pTextures->getTextureIDFromName(pCurMesh->textures[2]);

        GLint textureScale02_UL = glGetUniformLocation(program, "textureScale02");
        glUniform1f(textureScale02_UL, (GLfloat)pCurMesh->textureScale);

        if (textureID_02 == 0)
        {
            textureID_02 = MissingTexture_ID;

        }
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, textureID_02);
        GLint texture02_UL = glGetUniformLocation(program, "texture02");
        glUniform1i(texture02_UL, 2);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
    }

    {
        GLuint textureID_03 = ::g_pTextures->getTextureIDFromName(pCurMesh->textures[3]);

        GLint textureScale03_UL = glGetUniformLocation(program, "textureScale03");
        glUniform1f(textureScale03_UL, (GLfloat)pCurMesh->textureScale);

        if (textureID_03 == 0)
        {
            textureID_03 = MissingTexture_ID;

        }
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, textureID_03);
        GLint texture03_UL = glGetUniformLocation(program, "texture03");
        glUniform1i(texture03_UL, 3);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
    }    

    // Now the ratios
    // uniform vec4 texRatio_0_to_3;
    GLint texRatio_0_to_3_UL = glGetUniformLocation(program, "texRatio_0_to_3");
    glUniform4f(texRatio_0_to_3_UL,
        pCurMesh->blendRatio[0],
        pCurMesh->blendRatio[1],
        pCurMesh->blendRatio[2],
        pCurMesh->blendRatio[3]);
    
    return;
}


void DrawMesh(sMesh* pCurMesh, GLuint program, glm::vec3 eyelocation, bool SetTexturesFromMeshInfo = true)
{
    // Is it visible? 
    if (!pCurMesh->bIsVisible)
    {
        // Continue in loops jumps to the end of this loop
        // (for, while, do)
        return;
    }

    // HACK:
    // For the discard example
	// uniform bool bUseStencilTexture;
    GLint bUseStencilTexture_UL = glGetUniformLocation(program, "bUseStencilTexture");

    if (pCurMesh->uniqueFriendlyName == "WallR3" ||
        pCurMesh->uniqueFriendlyName == "WallR6" ||
        pCurMesh->uniqueFriendlyName == "WallR9" ||
        pCurMesh->uniqueFriendlyName == "WallF2" || 
        pCurMesh->uniqueFriendlyName == "WallF5")
    {
        glUniform1f(bUseStencilTexture_UL, (GLfloat)GL_TRUE);

        // uniform sampler2D stencilTexture;
        GLuint stencilTextureID = ::g_pTextures->getTextureIDFromName("window_cutout.bmp");

        glActiveTexture(GL_TEXTURE0 + 40);
        glBindTexture(GL_TEXTURE_2D, stencilTextureID);

        GLint stencilTexture_UL = glGetUniformLocation(program, "stencilTexture");
        glUniform1i(stencilTexture_UL, 40);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
    }
    else
    {
        glUniform1f(bUseStencilTexture_UL, (GLfloat)GL_FALSE);
    }

    // HACK:
    // For the discard example
    // uniform bool bUseStencilTexture;
    GLint bUseStencilTextureInverse_UL = glGetUniformLocation(program, "bUseStencilTextureInverse");

    if (pCurMesh->uniqueFriendlyName == "WindowPane1" ||
        pCurMesh->uniqueFriendlyName == "WindowPane2" ||
        pCurMesh->uniqueFriendlyName == "WindowPane3" ||
        pCurMesh->uniqueFriendlyName == "WindowPane4" ||
        pCurMesh->uniqueFriendlyName == "WindowPane5")
    {
        glUniform1f(bUseStencilTextureInverse_UL, (GLfloat)GL_TRUE);

        // uniform sampler2D stencilTexture;
        GLuint stencilTextureID = ::g_pTextures->getTextureIDFromName("window_cutout.bmp");

        glActiveTexture(GL_TEXTURE0 + 41);
        glBindTexture(GL_TEXTURE_2D, stencilTextureID);

        GLint stencilTexture_UL = glGetUniformLocation(program, "stencilTexture");
        glUniform1i(stencilTexture_UL, 41);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
    }
    else
    {
        glUniform1f(bUseStencilTextureInverse_UL, (GLfloat)GL_FALSE);
    }

    GLint bModulateTexture_UL = glGetUniformLocation(program, "bModulateTexture");
    if (pCurMesh->bModulateTexture)
    {
        glUniform1f(bModulateTexture_UL, (GLfloat)GL_TRUE);  // True
    }
    else
    {
        glUniform1f(bModulateTexture_UL, (GLfloat)GL_FALSE);  // False
    }

    GLint bTranslateTexture_UL = glGetUniformLocation(program, "bTranslateTexture");
    if (pCurMesh->bTranslateTexture)
    {
        glUniform1f(bTranslateTexture_UL, (GLfloat)GL_TRUE);  // True
    }
    else
    {
        glUniform1f(bTranslateTexture_UL, (GLfloat)GL_FALSE);  // False
    }

    GLint bIsMirrorFBO_UL = glGetUniformLocation(program, "bIsMirrorFBO");
    if (pCurMesh->uniqueFriendlyName == "WallFBO")
    {
        glUniform1f(bIsMirrorFBO_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bIsMirrorFBO_UL, (GLfloat)GL_FALSE);
    }

    GLint bIsTVFBO_UL = glGetUniformLocation(program, "bIsTVFBO");
    if (pCurMesh->uniqueFriendlyName == "TV_Screen")
    {
        glUniform1f(bIsTVFBO_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bIsTVFBO_UL, (GLfloat)GL_FALSE);
    }

    //glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);

    // Use lighting or not
    // uniform bool bDoNotLight;	
    GLint bDoNotLight_UL = glGetUniformLocation(program, "bDoNotLight");
    if (pCurMesh->bDoNotLight)
    {
        //glUniform1f(bDoNotLight_UL, 1.0f);  // True
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);  // True
    }
    else
    {
        //                glUniform1f(bDoNotLight_UL, 0.0f);  // False
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);  // False
    }

    // Set up the textures for THIS mesh
    // uniform bool bUseTextureAsColour;	// If true, then sample the texture
    GLint bUseTextureAsColour_UL = glGetUniformLocation(program, "bUseTextureAsColour");
    glUniform1f(bUseTextureAsColour_UL, (GLfloat)GL_TRUE);

    if (SetTexturesFromMeshInfo)
    {
        SetUpTextures(pCurMesh, program);
    }

    // Could be called the "model" or "world" matrix
    glm::mat4 matModel = glm::mat4(1.0f);

    // Translation (movement, position, placement...)
    glm::mat4 matTranslate
        = glm::translate(glm::mat4(1.0f),
            glm::vec3(pCurMesh->positionXYZ.x,
                pCurMesh->positionXYZ.y,
                pCurMesh->positionXYZ.z));

    // Rotation...
    // Caculate 3 Euler acix matrices...
    glm::mat4 matRotateX =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(pCurMesh->rotationEulerXYZ.x), // Angle in radians
            glm::vec3(1.0f, 0.0, 0.0f));

    glm::mat4 matRotateY =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(pCurMesh->rotationEulerXYZ.y), // Angle in radians
            glm::vec3(0.0f, 1.0, 0.0f));

    glm::mat4 matRotateZ =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(pCurMesh->rotationEulerXYZ.z), // Angle in radians
            glm::vec3(0.0f, 0.0, 1.0f));


    // Scale
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(pCurMesh->uniformScale,
            pCurMesh->uniformScale,
            pCurMesh->uniformScale));


    // Calculate the final model/world matrix
    matModel *= matTranslate;     // matModel = matModel * matTranslate;
    matModel *= matRotateX;
    matModel *= matRotateY;
    matModel *= matRotateZ;
    matModel *= matScale;

    //           matRoationOnly = matModel * matRotateX * matRotateY * matRotateZ;


               //mat4x4_mul(mvp, p, m);
               //mvp = p * v * m;
   //            glm::mat4 matMVP = matProjection * matView * matModel;

               //const GLint mvp_location = glGetUniformLocation(program, "MVP");
               //glUniformMatrix4fv(mvp_location, 
               //    1,
               //    GL_FALSE,
               //    (const GLfloat*)&matMVP);

    const GLint mvp_location = glGetUniformLocation(program, "matModel");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matModel);

    //const GLint mvp_location = glGetUniformLocation(program, "matView");
    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matMVP);

    //const GLint mvp_location = glGetUniformLocation(program, "matProjection");
    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&matMVP);

    // uniform bool bUseObjectColour;
    GLint bUseObjectColour = glGetUniformLocation(program, "bUseObjectColour");

    if (pCurMesh->bOverrideObjectColour)
    {
        // bool doesn't really exist, it's a float...
        glUniform1f(bUseObjectColour, (GLfloat)GL_TRUE);    // or 1.0f
    }
    else
    {
        glUniform1f(bUseObjectColour, (GLfloat)GL_FALSE);   // or 0.0f
    }

    // Set the object colour
    // uniform vec4 objectColour;
    GLint objectColour_UL = glGetUniformLocation(program, "objectColour");
    glUniform4f(objectColour_UL,
        pCurMesh->objectColourRGBA.r,
        pCurMesh->objectColourRGBA.g,
        pCurMesh->objectColourRGBA.b,
        1.0f);

    // eye location
    GLint eyeLocation_UL = glGetUniformLocation(program, "eyeLocation");
    glUniform4f(eyeLocation_UL,
        eyelocation.x,
        eyelocation.y,
        eyelocation.z,
        1.0f);

    ///uniform float wholeObjectTransparencyAlpha;
    GLint wholeObjectTransparencyAlpha_UL = glGetUniformLocation(program, "wholeObjectTransparencyAlpha");
    glUniform1f(wholeObjectTransparencyAlpha_UL, pCurMesh->alphaTransparency);


    // solid or wireframe, etc.
//        glPointSize(10.0f);
    if (pCurMesh->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glDisable(GL_CULL_FACE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //glEnable(GL_CULL_FACE);
    }

    //        glDrawArrays(GL_TRIANGLES, 0, 3);
//            glDrawArrays(GL_TRIANGLES, 0, numberOfVertices_TO_DRAW);

    sModelDrawInfo meshToDrawInfo;
    if (::g_pMeshManager->FindDrawInfoByModelName(pCurMesh->modelFileName, meshToDrawInfo))
    {
        // Found the model
        glBindVertexArray(meshToDrawInfo.VAO_ID); 		// enable VAO(and everything else)
        //https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
        glDrawElements(GL_TRIANGLES,
            meshToDrawInfo.numberOfIndices,
            GL_UNSIGNED_INT,
            (void*)0);

        glBindVertexArray(0); 			//disable VAO(and everything else)
    }


//    // Check if there are child meshes
//    for (unsigned int childIndex = 0; childIndex != pCurMesh->vec_pChildMeshes.size(); childIndex++)
//    {
//        DrawMesh(pCurMesh->vec_pChildMeshes[childIndex], program);
//    }

	return;
}

void DrawSkyBox(GLuint program,
                glm::vec3 eyeLocation,
                std::string skyboxName,
                glm::mat4 matProjection,
                glm::mat4 matView)
{
    GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

    GLint matView_UL = glGetUniformLocation(program, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);

    // **************************************************************
     // Sky box
    // Move the sky sphere with the camera
    sMesh* pSkySphere = g_pFindMeshByFriendlyName("SkySphere");
    pSkySphere->positionXYZ = eyeLocation;

    // Disable backface culling (so BOTH sides are drawn)
    glDisable(GL_CULL_FACE);
    // Don't perform depth buffer testing
    glDisable(GL_DEPTH_TEST);
    // Don't write to the depth buffer when drawing to colour (back) buffer
    //        glDepthMask(GL_FALSE);
    //        glDepthFunc(GL_ALWAYS);// or GL_LESS (default)
    // GL_DEPTH_TEST : do or not do the test against what's already on the depth buffer

    pSkySphere->bIsVisible = true;
    //        pSkySphere->bDoNotLight = true;

    pSkySphere->uniformScale = 50.0f;

    // Space skybox

    // Tell the shader this is the skybox, so use the cube map
    // uniform samplerCube skyBoxTexture;
    // uniform bool bIsSkyBoxObject;
    GLuint bIsSkyBoxObject_UL = glGetUniformLocation(program, "bIsSkyBoxObject");
    glUniform1f(bIsSkyBoxObject_UL, (GLfloat)GL_TRUE);

    // Set the cube map texture, just like we do with the 2D
    GLuint cubeSamplerID = ::g_pTextures->getTextureIDFromName(skyboxName);
    //        GLuint cubeSamplerID = ::g_pTextures->getTextureIDFromName("SunnyDay");
            // Make sure this is an unused texture unit
    glActiveTexture(GL_TEXTURE0 + 42);
    // *****************************************
    // NOTE: This is a CUBE_MAP, not a 2D
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeSamplerID);
    //        glBindTexture(GL_TEXTURE_2D, cubeSamplerID);
            // *****************************************
    GLint skyBoxTextureSampler_UL = glGetUniformLocation(program, "skyBoxTextureSampler");
    glUniform1i(skyBoxTextureSampler_UL, 42);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here

    // Interior skybox

    // Set the cube map texture, just like we do with the 2D
    GLuint interiorCubeSamplerID = ::g_pTextures->getTextureIDFromName("Interior");
    //        GLuint cubeSamplerID = ::g_pTextures->getTextureIDFromName("SunnyDay");
            // Make sure this is an unused texture unit
    glActiveTexture(GL_TEXTURE0 + 43);
    // *****************************************
    // NOTE: This is a CUBE_MAP, not a 2D
    glBindTexture(GL_TEXTURE_CUBE_MAP, interiorCubeSamplerID);
    //        glBindTexture(GL_TEXTURE_2D, cubeSamplerID);
            // *****************************************
    GLint interiorTextureSampler_UL = glGetUniformLocation(program, "interiorTextureSampler");
    glUniform1i(interiorTextureSampler_UL, 43);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here


    DrawMesh(pSkySphere, program, eyeLocation);

    pSkySphere->bIsVisible = false;

    glUniform1f(bIsSkyBoxObject_UL, (GLfloat)GL_FALSE);

    glEnable(GL_CULL_FACE);
    // Enable depth test and write to depth buffer (normal rendering)
    glEnable(GL_DEPTH_TEST);
    //        glDepthMask(GL_FALSE);
    //        glDepthFunc(GL_LESS);
    // **************************************************************
}

void RenderScene(
    GLuint program,
    glm::mat4 matProjection,
    glm::mat4 matView,
    float ratio,
    glm::vec3 eyeLocation)
{
    GLint matView_UL = glGetUniformLocation(program, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);

    GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

    //DrawSkyBox(program, eyeLocation);

    ::g_pLightManager->updateShaderWithLightInfo();
    // *******************************************************************
    //    ____                       _                      
    //   |  _ \ _ __ __ ___      __ | |    ___   ___  _ __  
    //   | | | | '__/ _` \ \ /\ / / | |   / _ \ / _ \| '_ \ 
    //   | |_| | | | (_| |\ V  V /  | |__| (_) | (_) | |_) |
    //   |____/|_|  \__,_| \_/\_/   |_____\___/ \___/| .__/ 
    //                                               |_|    
    // Draw all the objects

    // Split to solid and transparent

    std::vector<sMesh*> solidObjects;
    std::vector<sMesh*> transparentObjects;

    for (sMesh* pCurMesh : g_vecMeshesToDraw)
    {
        /*if (pCurMesh->uniqueFriendlyName == "WallFBO")
        {
            continue;
        }*/

        if (pCurMesh->textures[0] == "wall_texture_01.bmp")
        {
            pCurMesh->textures[1] = "dirt_texture.bmp";
            pCurMesh->bModulateTexture = true;
            pCurMesh->bTranslateTexture = true;

            if (movingWalls)
            {
                pCurMesh->sTranslate += 0.001;
                pCurMesh->tTranslate += 0.001;
            }

            //pCurMesh->blendRatio[0] = 0.5;
            //pCurMesh->blendRatio[1] = 0.5;
        }

        if (pCurMesh->alphaTransparency < 1.0)
        {
            // Transparent
            transparentObjects.push_back(pCurMesh);
        }
        else
        {
            // Solid
            solidObjects.push_back(pCurMesh);
        }
    }

    // Solid
    for (sMesh* pCurMesh : solidObjects)
    {
        if (pCurMesh->uniqueFriendlyName == "Basketball" ||
            pCurMesh->uniqueFriendlyName == "Cabinet1" ||
            pCurMesh->uniqueFriendlyName == "Cabinet2" ||
            pCurMesh->uniqueFriendlyName == "Cabinet3")
        {
            pCurMesh->textures[1] = "scratches_texture.bmp";
            pCurMesh->bModulateTexture = true;

            pCurMesh->blendRatio[0] = 1.0f;
            pCurMesh->blendRatio[1] = 1.0f;
        }

        DrawMesh(pCurMesh, program, eyeLocation);
    }

    // Sort transparent objects
    std::sort(transparentObjects.begin(), transparentObjects.end(), isObjectACloserThanObjectB);

    // Transparent
    for (sMesh* pCurMesh : transparentObjects)
    {
        DrawMesh(pCurMesh, program, eyeLocation);
    }

    // Rotate the skybox
    if (g_vecMeshesToDraw[0]->rotationEulerXYZ.y < 360)
    {
        g_vecMeshesToDraw[0]->rotationEulerXYZ.y += 0.002;
    }
    else
    {
        g_vecMeshesToDraw[0]->rotationEulerXYZ.y = 0;
    }

    // **********************************************************************************
    if (::g_bShowDebugSpheres)
    {
        cLightHelper TheLightHelper;

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

        const float DEBUG_LIGHT_BRIGHTNESS = 0.3f;

        const float ACCURACY = 0.1f;       // How many units distance
        float distance_75_percent =
            TheLightHelper.calcApproxDistFromAtten(0.75f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(DEBUG_LIGHT_BRIGHTNESS, 0.0f, 0.0f, 1.0f),
            distance_75_percent,
            program);


        float distance_50_percent =
            TheLightHelper.calcApproxDistFromAtten(0.5f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(0.0f, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
            distance_50_percent,
            program);

        float distance_25_percent =
            TheLightHelper.calcApproxDistFromAtten(0.25f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(0.0f, 0.0f, DEBUG_LIGHT_BRIGHTNESS, 1.0f),
            distance_25_percent,
            program);

        float distance_05_percent =
            TheLightHelper.calcApproxDistFromAtten(0.05f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(DEBUG_LIGHT_BRIGHTNESS, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
            distance_05_percent,
            program);

    }
    // **********************************************************************************

}


void DrawDebugSphere(glm::vec3 position, glm::vec4 RGBA, float scale, GLuint program)
{
    // Created the debug sphere, yet?
    if (!pDebugSphere)           // Same as if ( pDebugSphere == NULL )
    {
        pDebugSphere = new sMesh();
//        pDebugSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
        pDebugSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
        pDebugSphere->positionXYZ = glm::vec3(0.0f, 5.0f, 0.0f);
        pDebugSphere->bIsWireframe = true;
        pDebugSphere->objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        pDebugSphere->uniqueFriendlyName = "Debug_Sphere";
        pDebugSphere->uniformScale = 10.0f;
        pDebugSphere->bDoNotLight = true;
    }
    // At this point the debug sphere is created

    pDebugSphere->bIsVisible = true;
    pDebugSphere->positionXYZ = position;
    pDebugSphere->bOverrideObjectColour = true;
    pDebugSphere->objectColourRGBA = RGBA;
    pDebugSphere->uniformScale = scale;

    DrawMesh(pDebugSphere, program, g_pFlyCamera->getEyeLocation());

    pDebugSphere->bIsVisible = false;

    return;
}