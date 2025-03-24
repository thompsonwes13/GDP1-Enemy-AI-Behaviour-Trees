//#define GLAD_GL_IMPLEMENTATION
//#include <glad/glad.h>
//
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "GLCommon.h"

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>     // "input output" stream
#include <fstream>      // "file" stream
#include <sstream>      // "string" stream ("string builder" in Java c#, etc.)
#include <string>
#include <vector>
#include <algorithm>

//void ReadPlyModelFromFile(std::string plyFileName);
#include "PlyFileLoaders.h"
#include "Basic_Shader_Manager/cShaderManager.h"
#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include "sharedThings.h"       // Fly camera
#include "cPhysics.h"
#include "cLightManager.h"
#include <windows.h>    // Includes ALL of windows... MessageBox
#include "cLightHelper/cLightHelper.h"
//
#include "cBasicTextureManager/cBasicTextureManager.h"

// Frame Buffer Object (FBO)
#include "cFBO/cFBO_RGB_depth.h"

#include "EnemyBehaviourTree.h"

std::vector<sMesh*> g_vecMeshesToDraw;

cPhysics* g_pPhysicEngine = NULL;
// This loads the 3D models for drawing, etc.
cVAOManager* g_pMeshManager = NULL;

cBasicTextureManager* g_pTextures = NULL;

int g_total_bullet_count = 0;
int g_enemies_killed = 0;
int g_shoot_timer = 50;
int g_spawn_timer = 0;
int g_enemies_spawned = 5;

//cLightManager* g_pLightManager = NULL;

void AddModelsToScene(cVAOManager* pMeshManager, GLuint shaderProgram);
void AddModelsToScene();

void DrawMesh(sMesh* pCurMesh, GLuint program, glm::vec3 eyelocation, bool SetTexturesFromMeshInfo = true);

void RenderScene(
    GLuint program,
    glm::mat4 matProjection,
    glm::mat4 matView,
    float ratio,
    glm::vec3 eyeLocation);

void DrawSkyBox(GLuint program,
    glm::vec3 eyeLocation,
    std::string skyboxName,
    glm::mat4 matProjection,
    glm::mat4 matView);

//std::vector<sBullet*> g_vecBullets;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

bool isControlDown(GLFWwindow* window);

// Returns NULL if NOT found
sMesh* g_pFindMeshByFriendlyName(std::string theNameToFind)
{
    for (unsigned int index = 0; index != ::g_vecMeshesToDraw.size(); index++)
    {
        if (::g_vecMeshesToDraw[index]->uniqueFriendlyName == theNameToFind)
        {
            return ::g_vecMeshesToDraw[index];
        }
    }
    // Didn't find it
    return NULL;
}

void SaveMeshes()
{
    // Save state to file

    if (MessageBox(NULL, L"Would you like to save the mesh locations?", L"SAVE", MB_YESNO) == IDYES)
    {
        std::ofstream meshInfo("meshInfo.txt");

        for (int i = 0; i < g_vecMeshesToDraw.size(); i++)
        {
            if (::g_vecMeshesToDraw[i] == g_pFindMeshByFriendlyName("Light_Sphere"))
            {
            }
            else if (::g_vecMeshesToDraw[i] == g_pFindMeshByFriendlyName("Debug_Sphere"))
            {
            }
            else
            {
                meshInfo << ::g_vecMeshesToDraw[i]->uniqueFriendlyName << "\n";

                meshInfo << "Position - ";
                meshInfo << "x: " << ::g_vecMeshesToDraw[i]->positionXYZ.x << " ";
                meshInfo << "y: " << ::g_vecMeshesToDraw[i]->positionXYZ.y << " ";
                meshInfo << "z: " << ::g_vecMeshesToDraw[i]->positionXYZ.z << "\n";

                meshInfo << "Rotation - ";
                meshInfo << "x: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.x << " ";
                meshInfo << "y: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.y << " ";
                meshInfo << "z: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.z << "\n";

                meshInfo << "Scale - ";
                meshInfo << ::g_vecMeshesToDraw[i]->uniformScale << "\n\n";
            }
        }

        meshInfo.close();

        std::cout << "Saved Meshes!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Save" << std::endl;
    }

    return;
}

void LoadMeshes()
{
    // Load state to file
            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox

    if (MessageBox(NULL, L"Would you like to load the mesh locations?", L"LOAD", MB_YESNO) == IDYES)
    {
        std::ifstream meshInfo("meshInfo.txt");

        for (int i = 0; i < g_vecMeshesToDraw.size(); i++)
        {
            std::string name;

            float xPos;
            float yPos;
            float zPos;

            float xRot;
            float yRot;
            float zRot;

            float scale;

            std::string discard;

            if (::g_vecMeshesToDraw[i] == g_pFindMeshByFriendlyName("Light_Sphere"))
            {
            }
            else if (::g_vecMeshesToDraw[i] == g_pFindMeshByFriendlyName("Debug_Sphere"))
            {
            }
            else
            {
                meshInfo >> name    // Mesh Name
                    >> discard    // "Position"
                    >> discard    // "-"
                    >> discard    // "x:"
                    >> xPos       // x val
                    >> discard    // "y:"
                    >> yPos       // y val
                    >> discard    // "z:"
                    >> zPos       // z val

                    >> discard    // "Rotation"
                    >> discard    // "-"
                    >> discard    // "x:"
                    >> xRot       // x val
                    >> discard    // "y:"
                    >> yRot       // y val
                    >> discard    // "z:"
                    >> zRot       // z val

                    >> discard    // "Scale"
                    >> discard    // "-"
                    >> scale;     // scale
                if (name != "SkySphere")
                {
                    ::g_vecMeshesToDraw[i]->positionXYZ.x = xPos;
                    ::g_vecMeshesToDraw[i]->positionXYZ.y = yPos;
                    ::g_vecMeshesToDraw[i]->positionXYZ.z = zPos;
                }

                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.x = xRot;
                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.y = yRot;
                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.z = zRot;

                ::g_vecMeshesToDraw[i]->uniformScale = scale;
            }
        }

        std::cout << "Loaded Meshes!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Load" << std::endl;
    }

    return;
}

void SaveLights()
{
    if (MessageBox(NULL, L"Would you like to save the light locations?", L"SAVE", MB_YESNO) == IDYES)
    {
        std::ofstream lightInfo("lightInfo.txt");
        std::string lightType;
        std::string on_off;

        for (int i = 0; i < cLightManager::NUMBEROFLIGHTS; i++)
        {
            if (::g_pLightManager->theLights[i].param1.x == 0.0)
            {
                lightType = "Point";
            }
            else if (::g_pLightManager->theLights[i].param1.x == 1.0)
            {
                lightType = "Spot";
            }
            else if (::g_pLightManager->theLights[i].param1.x == 2.0)
            {
                lightType = "Directional";
            }

            if (::g_pLightManager->theLights[i].param2.x == 0.0)
            {
                on_off = "OFF";
            }
            else if (::g_pLightManager->theLights[i].param2.x == 1.0)
            {
                on_off = "ON";
            }

            lightInfo << "Light " << i + 1 << " " << lightType << " " << on_off << "\n";
            lightInfo << "x: " << ::g_pLightManager->theLights[i].position.x << " ";
            lightInfo << "y: " << ::g_pLightManager->theLights[i].position.y << " ";
            lightInfo << "z: " << ::g_pLightManager->theLights[i].position.z << "\n";
            lightInfo << "Linear: " << ::g_pLightManager->theLights[i].atten.y << " ";
            lightInfo << "Quadratic: " << ::g_pLightManager->theLights[i].atten.z << "\n\n";
        }

        lightInfo.close();

        std::cout << "Saved Lights!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Save" << std::endl;
    }

    return;
}

void LoadLights()
{
    if (MessageBox(NULL, L"Would you like to load the light locations?", L"LOAD", MB_YESNO) == IDYES)
    {
        std::ifstream lightInfo("lightInfo.txt");

        for (int i = 0; i < cLightManager::NUMBEROFLIGHTS; i++)
        {
            float x;
            float y;
            float z;
            float linear;
            float quadratic;
            std::string lightType;
            std::string on_off;
            std::string discard;

            lightInfo >> discard    // "Light"
                >> discard    // Light #
                >> lightType
                >> on_off
                >> discard    // "x:"
                >> x          // x val
                >> discard    // "y:"
                >> y          // y val
                >> discard    // "z:"
                >> z          // z val
                >> discard    // "Linear:"
                >> linear     // Linear value
                >> discard    // "Quardratic
                >> quadratic;  // Quadratic value

            ::g_pLightManager->theLights[i].position.x = x;
            ::g_pLightManager->theLights[i].position.y = y;
            ::g_pLightManager->theLights[i].position.z = z;
            ::g_pLightManager->theLights[i].atten.y = linear;
            ::g_pLightManager->theLights[i].atten.z = quadratic;

            if (lightType == "Point")
            {
                ::g_pLightManager->theLights[i].param1.x = 0.0f;
            }
            else if (lightType == "Spot")
            {
                ::g_pLightManager->theLights[i].param1.x = 1.0f;
            }
            else if (lightType == "Directional")
            {
                ::g_pLightManager->theLights[i].param1.x = 2.0f;
            }

            if (on_off == "OFF")
            {
                ::g_pLightManager->theLights[i].param2.x = 0.0f;
            }
            else if (on_off == "ON")
            {
                ::g_pLightManager->theLights[i].param2.x = 1.0f;
            }
        }

        std::cout << "Loaded Lights!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Load" << std::endl;
    }

    return;
}

void SetCameraPosition()
{
    std::ifstream cameraInfo("cameraInfo.txt");

    float x;
    float y;
    float z;
    float turnLR;
    float turnUD;
    std::string discard;

    cameraInfo >> discard // "Camera"
        >> discard        // "x:"
        >> x              // x value
        >> discard        // "y:"
        >> y              // y value
        >> discard        // "z:"
        >> z              // z value
        >> discard        // "Rotation-Left-Right:"
        >> turnLR         // Left Right value
        >> discard        // "Rotation-Up-Down:"
        >> turnUD;        // Up Down value

    ::g_pFlyCamera->setEyeLocation(x, y, z);
    ::g_pFlyCamera->rotateLeftRight_Yaw_NoScaling(glm::radians(turnLR));
    ::g_pFlyCamera->pitchUpDown(turnUD);

    std::cout << "Camera Position Loaded!" << std::endl;

    return;
}

void ModelsToVAO(const GLuint program)
{
    std::ifstream loadMeshes("loadMeshes.txt");

    //std::string discard;
    //std::string name;
    std::string path;
    //std::string isWireFrame;

    while (!loadMeshes.eof())
    {
        loadMeshes >> path;           // Path value

        sModelDrawInfo mesh;
        ::g_pMeshManager->LoadModelIntoVAO(path, mesh, program);
        std::cout << mesh.numberOfVertices << " vertices loaded" << std::endl;
    }
}

void DrawMeshes()
{
    std::ifstream drawMeshes("drawMeshes.txt");

    std::string discard;
    std::string name;
    std::string path;
    std::string isWireFrame;
    float transparency;
    std::string texture;
    float texScale;

    bool wireframe;

    float r;
    float g;
    float b;

    while (!drawMeshes.eof())
    {
        drawMeshes >> discard // "Name:"
            >> name           // Name value
            >> discard        // "Path:"
            >> path           // Path value
            >> discard        // "isWireframe"
            >> isWireFrame    // isWireFrame value
            >> discard        // "Colour:"
            >> r              // r value
            >> g              // g value
            >> b              // b value
            >> discard        // "Transparency:"
            >> transparency   // transparency value
            >> discard        // "Texture:"
            >> texture        // texture value
            >> texScale;      // texture scale

        if (drawMeshes.eof())
        {
            break;
        }

        if (isWireFrame == "true")
        {
            wireframe = true;
        }
        else
        {
            wireframe = false;
        }

        sMesh* pMesh = new sMesh();
        pMesh->modelFileName = path;
        pMesh->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
        pMesh->bIsWireframe = wireframe;
        pMesh->uniformScale = 0.1f;
        pMesh->objectColourRGBA = glm::vec4((r / 255.0f), (g / 255.0f), (b / 255.0f), 1.0f);
        pMesh->uniqueFriendlyName = name;
        pMesh->alphaTransparency = transparency;
        pMesh->textures[0] = texture;
        pMesh->textureScale = 1/texScale;
        pMesh->bModulateTexture = false;

        if (pMesh->modelFileName == "assets/models/toy_soldier_xyz_N_uv.ply")
        {
            //pMesh->bDoNotLight = true;
        }

        ::g_vecMeshesToDraw.push_back(pMesh);
    }
}

void LoadTextures()
{
    ::g_pTextures->SetBasePath("assets/textures");
    ::g_pTextures->Create2DTextureFromBMPFile("UV_Test_750x750.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("Grey_Brick_Wall_Texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wall_texture_01.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wall_texture_02.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wood_floor_texture_01.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("ceiling_texture_01.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("chandelier_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("window_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("window_cutout.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("door_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("wall_light_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("light_switch_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("cabinet_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("grass_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("low_poly_tree_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("dirt_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("vase_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("scratches_texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("green_square.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("red_square.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("blue_square.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("yellow_square.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("purple_square.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("cyan_square.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("orange_square.bmp");

    // Load the space skybox
    std::string errorString;
    ::g_pTextures->SetBasePath("assets/textures/skyboxes");
    if (::g_pTextures->CreateCubeTextureFromBMPFiles("Space",
        "DarkStormyLeft2048.bmp",
        "DarkStormyRight2048.bmp",
        "DarkStormyUp2048.bmp",
        "DarkStormyDown2048.bmp",
        "DarkStormyFront2048.bmp",
        "DarkStormyBack2048.bmp", true, errorString))
    {
        std::cout << "Loaded space skybox" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load space skybox because: " << errorString << std::endl;
    }

    // Load an "interior" skybox
    ::g_pTextures->SetBasePath("assets/textures/skyboxes");
    if (::g_pTextures->CreateCubeTextureFromBMPFiles("Interior",
        "wall_texture_011.bmp",
        "wall_texture_011.bmp",
        "ceiling_texture_01.bmp",
        "wood_floor_texture_01.bmp",
        "wall_texture_011.bmp",
        "wall_texture_011.bmp", true, errorString))
    {
        std::cout << "Loaded interior skybox" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load interior skybox because: " << errorString << std::endl;
    }

    return;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float CAMERA_MOVE_SPEED = 0.1f;

    // Save the mesh locations, rotations, etc.
    if (mods == GLFW_MOD_SHIFT)
    {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            // Save state to file

            SaveMeshes();

        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            // Load state from file

            LoadMeshes();

        }
    }

    // Save the light locations, rotations, etc.
    if (mods == GLFW_MOD_CONTROL)
    {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            // Save state to file

            SaveLights();

        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            // Load state from file

            LoadLights();

        }
    }

    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
    {
        // check if you are out of bounds
        if (::g_selectedLightIndex > 0)
        {
            ::g_selectedLightIndex--;
        }
        else
        {
            ::g_selectedLightIndex = cLightManager::NUMBEROFLIGHTS - 1;
        }
    }
    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
    {
        ::g_selectedLightIndex++;
        if (::g_selectedLightIndex >= cLightManager::NUMBEROFLIGHTS)
        {
            ::g_selectedLightIndex = 0;
        }
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        if (g_shoot_timer > 10)
        {
            sMesh* PlayerMesh = g_pFindMeshByFriendlyName("Player");

            glm::vec2 direction = glm::vec2(g_pFlyCamera->getTarget().x, g_pFlyCamera->getTarget().z);
            g_ShootBullet(PlayerMesh, direction);
            g_shoot_timer = 0;
        }
    }

    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    {
        //back in meshes
        if (::g_selectedMeshIndex > 0)
        {
            ::g_selectedMeshIndex--;
        }
        else
        {
            g_selectedMeshIndex = g_vecMeshesToDraw.size() - 1;
        }
    }
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    {
        //forward in meshes
        ::g_selectedMeshIndex++;
        if (::g_selectedMeshIndex >= g_vecMeshesToDraw.size())
        {
            ::g_selectedMeshIndex = 0;
        }
    }

    return;
}

// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float getRandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

glm::vec3 getRandom_vec3(glm::vec3 min, glm::vec3 max)
{
    return glm::vec3(
        getRandomFloat(min.x, max.x),
        getRandomFloat(min.y, max.y),
        getRandomFloat(min.z, max.z));
}


int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Callback for keyboard, but for "typing"
    // Like it captures the press and release and repeat
    glfwSetKeyCallback(window, key_callback);

    // 
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowFocusCallback(window, cursor_enter_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    cShaderManager* pShaderManager = new cShaderManager();

    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "assets/shaders/vertex01.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "assets/shaders/fragment01.glsl";

    if ( ! pShaderManager->createProgramFromFile("shader01",
                                                 vertexShader, fragmentShader))
    {
        std::cout << "Error: " << pShaderManager->getLastError() << std::endl;
    }
    else
    {
        std::cout << "Shader built OK" << std::endl;
    }

    const GLuint program = pShaderManager->getIDFromFriendlyName("shader01");

    glUseProgram(program);

    // Loading the TYPES of models I can draw...

//    cVAOManager* pMeshManager = new cVAOManager();
    ::g_pMeshManager = new cVAOManager();

    ModelsToVAO(program);

    ::g_pPhysicEngine = new cPhysics();

    AddModelsToScene();

    // For triangle meshes, let the physics object "know" about the VAO manager
    ::g_pPhysicEngine->setVAOManager(::g_pMeshManager);


    // This also adds physics objects to the phsyics system
    //AddModelsToScene(::g_pMeshManager, program);
    
    ::g_pFlyCamera = new cBasicFlyCamera();

    glUseProgram(program);

    // Enable depth buffering (z buffering)
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
    glEnable(GL_DEPTH_TEST);

    double currentFrameTime = glfwGetTime();
    double lastFrameTime = glfwGetTime();

    SetCameraPosition();

    // Set up the lights
    ::g_pLightManager = new cLightManager();
    // Called only once
    ::g_pLightManager->loadUniformLocations(program);

    // 1 
    ::g_pLightManager->theLights[0].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[0].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // 2 
    ::g_pLightManager->theLights[1].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[1].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 3 
    ::g_pLightManager->theLights[2].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[2].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 4 
    ::g_pLightManager->theLights[3].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[3].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 5 
    ::g_pLightManager->theLights[4].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[4].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 6 
    ::g_pLightManager->theLights[5].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[5].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 7 
    ::g_pLightManager->theLights[6].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[6].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 8 
    ::g_pLightManager->theLights[7].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[7].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 9 
    ::g_pLightManager->theLights[8].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[8].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 10 
    ::g_pLightManager->theLights[9].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[9].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 11
    ::g_pLightManager->theLights[10].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[10].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 12
    ::g_pLightManager->theLights[11].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[11].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 13
    ::g_pLightManager->theLights[12].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[12].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 14
    ::g_pLightManager->theLights[13].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[13].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 15
    ::g_pLightManager->theLights[14].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[14].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 16
    ::g_pLightManager->theLights[15].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[15].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 17
    ::g_pLightManager->theLights[16].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[16].specular = glm::vec4(0.94f, 0.84f, 0.71f, 1.0f);

    // 18

    ::g_pLightManager->theLights[17].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[17].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    ::g_pLightManager->theLights[17].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[17].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[17].param1.z = 15.0f;   //  z = outer angle

    //19

    ::g_pLightManager->theLights[18].diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    ::g_pLightManager->theLights[18].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    ::g_pLightManager->theLights[18].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[18].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[18].param1.z = 15.0f;   //  z = outer angle

    // 20 Directional
    ::g_pLightManager->theLights[19].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[19].diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    ::g_pLightManager->theLights[19].atten.y = 0.01f;
    ::g_pLightManager->theLights[19].atten.z = 0.001f;

    ::g_pLightManager->theLights[19].param1.x = 2.0f;    // Directional light (see shader)
    ::g_pLightManager->theLights[19].param2.x = 1.0f;    // Turn on (see shader)

    ::g_pLightManager->theLights[19].direction = glm::vec4(-0.25f, -1.0f, -0.25f, 1.0f);
    ::g_pLightManager->theLights[19].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[19].param1.z = 10.0f;   //  z = outer angle

    LoadLights();

    ::g_pTextures = new cBasicTextureManager();

    LoadTextures();

    GLint iMaxCombinedTextureInmageUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &iMaxCombinedTextureInmageUnits);
    std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = " << iMaxCombinedTextureInmageUnits << std::endl;

    // data returns one value, the maximum number of components of the inputs read by the fragment shader, 
    // which must be at least 128.
    GLint iMaxFragmentInputComponents = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &iMaxFragmentInputComponents);
    std::cout << "GL_MAX_FRAGMENT_INPUT_COMPONENTS = " << iMaxFragmentInputComponents << std::endl;
    
    // data returns one value, the maximum number of individual floating - point, integer, or boolean values 
    // that can be held in uniform variable storage for a fragment shader.The value must be at least 1024. 
    GLint iMaxFragmentUniformComponents = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &iMaxFragmentUniformComponents);
    std::cout << "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS = " << iMaxFragmentUniformComponents << std::endl;
        
    //  Turn on the blend operation
    glEnable(GL_BLEND);
    // Do alpha channel transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Is the default (cull back facing polygons)
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // ******************************************* Enamy Behaviour Tree ******************************************* //

    std::vector<Enemy*> g_vecEnemies;

    sMesh* pPlayer_Mesh = g_pFindMeshByFriendlyName("Player");

    sMesh* pEnemy1_Mesh = g_pFindMeshByFriendlyName("Enemy1");
    Enemy* pEnemy1 = new Enemy(0.25f, pPlayer_Mesh, pEnemy1_Mesh);
    pEnemy1->buildBehaviourTree_Enemy1();
    g_vecEnemies.push_back(pEnemy1);

    sMesh* pEnemy2_Mesh = g_pFindMeshByFriendlyName("Enemy2");
    Enemy* pEnemy2 = new Enemy(0.25f, pPlayer_Mesh, pEnemy2_Mesh);
    pEnemy2->buildBehaviourTree_Enemy2();
    g_vecEnemies.push_back(pEnemy2);

    sMesh* pEnemy3_Mesh = g_pFindMeshByFriendlyName("Enemy3");
    Enemy* pEnemy3 = new Enemy(0.25f, pPlayer_Mesh, pEnemy3_Mesh);
    pEnemy3->buildBehaviourTree_Enemy3();
    g_vecEnemies.push_back(pEnemy3);

    sMesh* pEnemy4_Mesh = g_pFindMeshByFriendlyName("Enemy4");
    Enemy* pEnemy4 = new Enemy(0.5f, pPlayer_Mesh, pEnemy4_Mesh);
    pEnemy4->buildBehaviourTree_Enemy4();
    g_vecEnemies.push_back(pEnemy4);

    // ******************************************* MAIN ******************************************* //

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        // Calculate elapsed time
        // We'll enhance this
        currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glm::mat4 matProjection = glm::mat4(1.0f);
        glm::mat4 matView = glm::mat4(1.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        // *************************** GAME LOGIC *************************** //

        g_shoot_timer++;
        g_spawn_timer++;

        sMesh* Player = g_pFindMeshByFriendlyName("Player");

        // Snap player to camera
        if (Player)
        {
            Player->positionXYZ.x = g_pFlyCamera->getEyeLocation().x + g_pFlyCamera->getTarget().x * 20;
            Player->positionXYZ.z = g_pFlyCamera->getEyeLocation().z + g_pFlyCamera->getTarget().z * 20;

            // Rotate player mesh
            Player->rotationEulerXYZ.y = -(atan(g_pFlyCamera->getTarget().z / g_pFlyCamera->getTarget().x) * 56);
            if (g_pFlyCamera->getTarget().x < 0)
            {
                Player->rotationEulerXYZ.y += 180;
            }
        }

        // Execute enemy trees and check for player collision
        for (Enemy* pCurrentEnemy : g_vecEnemies)
        {
            if (pCurrentEnemy->selectorOrSequence())
            {
                pCurrentEnemy->executeBehaviourTreeSelector();
            }
            else
            {
                pCurrentEnemy->executeBehaviourTreeSequence();
            }

            if (abs(glm::distance(pCurrentEnemy->getPosition(), Player->positionXYZ)) < 7.5f) // Enemy close enough to player
            {
                // Reset enemies killed, player and enemy positions
                g_enemies_killed = 0;
                g_pFlyCamera->setEyeLocation(0.0f, 12.0f, 0.0f);

                for (Enemy* pCurrentEnemy : g_vecEnemies)
                {
                    pCurrentEnemy->resetPosition();
                }
            }
        }

        // Move and remove bullets
        int i = 0;
        for (sBullet* pCurrentBullet : g_vecBullets)
        {
            pCurrentBullet->pAssociatedMesh->positionXYZ.x += pCurrentBullet->direction.x * pCurrentBullet->speed;
            pCurrentBullet->pAssociatedMesh->positionXYZ.z += pCurrentBullet->direction.y * pCurrentBullet->speed;

            pCurrentBullet->runtime++;

            // Travelled long enough
            if (pCurrentBullet->runtime > 300)
            {
                // Find bullet in vector
                int j = g_vecMeshesToDraw.size() - 1;
                for (std::vector<sMesh*>::iterator it = g_vecMeshesToDraw.end() - 1; it != g_vecMeshesToDraw.begin(); it--)
                {
                    sMesh* pMesh = *it;

                    if (pMesh->uniqueFriendlyName == pCurrentBullet->pAssociatedMesh->uniqueFriendlyName)
                    {
                        pMesh->positionXYZ.y -= 10.0f;
                        pMesh->bIsVisible = false;
                        g_vecMeshesToDraw.erase(g_vecMeshesToDraw.begin() + j);
                        delete pMesh;
                        break;
                    }

                    j--;
                }

                g_vecBullets.erase(g_vecBullets.begin() + i);
                continue;
            }

            if (pCurrentBullet->pSoldierMesh->uniqueFriendlyName == "Player") // Player bullet colliding with enemy
            {
                int enemy_index = 0;
                for (Enemy* pCurrentEnemy : g_vecEnemies)
                {
                    if (abs(glm::distance(pCurrentBullet->pAssociatedMesh->positionXYZ, pCurrentEnemy->getPosition())) < 5.0f) // Hit!
                    {
                        // Find bullet in vector
                        int j = g_vecMeshesToDraw.size() - 1;
                        for (std::vector<sMesh*>::iterator it = g_vecMeshesToDraw.end() - 1; it != g_vecMeshesToDraw.begin(); it--)
                        {
                            sMesh* pMesh = *it;

                            if (pMesh->uniqueFriendlyName == pCurrentBullet->pAssociatedMesh->uniqueFriendlyName)
                            {
                                pMesh->positionXYZ.y -= 10.0f;
                                pMesh->bIsVisible = false;
                                g_vecMeshesToDraw.erase(g_vecMeshesToDraw.begin() + j);
                                delete pMesh;
                                break;
                            }

                            j--;
                        }

                        g_vecBullets.erase(g_vecBullets.begin() + i);

                        // Find enemy in vector
                        int k = 0;
                        for (std::vector<sMesh*>::iterator it = g_vecMeshesToDraw.begin(); it != g_vecMeshesToDraw.end(); it++)
                        {
                            sMesh* pMesh = *it;

                            if (pMesh->uniqueFriendlyName == pCurrentEnemy->getName())
                            {
                                g_vecMeshesToDraw.erase(g_vecMeshesToDraw.begin() + k);
                                delete pMesh;
                                break;
                            }

                            k++;
                        }

                        g_vecEnemies.erase(g_vecEnemies.begin() + enemy_index);

                        g_enemies_killed++;
                        continue;
                    }

                    enemy_index++;
                }
            }
            else if (Player) // Enemy bullet colliding with player
            {
                if (abs(glm::distance(pCurrentBullet->pAssociatedMesh->positionXYZ, Player->positionXYZ)) < 5.0f) // Hit!
                {
                    // Find bullet in vector
                    int j = g_vecMeshesToDraw.size() - 1;
                    for (std::vector<sMesh*>::iterator it = g_vecMeshesToDraw.end() - 1; it != g_vecMeshesToDraw.begin(); it--)
                    {
                        sMesh* pMesh = *it;

                        if (pMesh->uniqueFriendlyName == pCurrentBullet->pAssociatedMesh->uniqueFriendlyName)
                        {
                            pMesh->positionXYZ.y -= 10.0f;
                            pMesh->bIsVisible = false;
                            g_vecMeshesToDraw.erase(g_vecMeshesToDraw.begin() + j);
                            delete pMesh;
                            break;
                        }

                        j--;
                    }

                    g_vecBullets.erase(g_vecBullets.begin() + i);

                    // Reset enemies killed, player and enemy positions
                    g_enemies_killed = 0;
                    g_pFlyCamera->setEyeLocation(0.0f, 12.0f, 0.0f);

                    for (Enemy* pCurrentEnemy : g_vecEnemies)
                    {
                        pCurrentEnemy->resetPosition();
                    }

                    continue;
                }
            }

            i++;
        }

        // Spawn random enemy at random position
        if (g_spawn_timer > 150 && g_vecEnemies.size() < 8)
        {
            std::string textureName;
            float enemySpeed = 0;
            int enemyType = rand() % 4;

            if (enemyType == 0)
            {
                textureName = "red_square.bmp";
                enemySpeed = 0.2f;
            }
            else if (enemyType == 1)
            {
                textureName = "blue_square.bmp";
                enemySpeed = 0.2f;
            }
            else if (enemyType == 2)
            {
                textureName = "yellow_square.bmp";
                enemySpeed = 0.2f;
            }
            else if (enemyType == 3)
            {
                textureName = "purple_square.bmp";
                enemySpeed = 0.3f;
            }

            float randomX = getRandomFloat(-110, 110);
            float randomZ = getRandomFloat(-110, 110);

            while (abs(randomX) < 90 && abs(randomZ) < 90)
            {
                randomX = getRandomFloat(-110, 110);
                randomZ = getRandomFloat(-110, 110);
            }

            sMesh* pEnemyMesh = new sMesh();
            pEnemyMesh->modelFileName = "assets/models/toy_soldier_xyz_N_uv.ply";
            pEnemyMesh->uniformScale = 0.1f;
            pEnemyMesh->bIsWireframe = false;
            pEnemyMesh->textures[0] = textureName;
            pEnemyMesh->textureScale = 1.0f;
            pEnemyMesh->positionXYZ = glm::vec3(randomX, 7.4f, randomZ);
            pEnemyMesh->uniqueFriendlyName = "Enemy" + std::to_string(g_enemies_spawned);
            pEnemyMesh->objectColourRGBA = glm::vec4(255, 255, 255, 1.0f);
            g_vecMeshesToDraw.push_back(pEnemyMesh);

            sMesh* pPlayer_Mesh = g_pFindMeshByFriendlyName("Player");

            Enemy* pEnemy = new Enemy(enemySpeed, pPlayer_Mesh, pEnemyMesh);
            if (enemyType == 0)
            {
                pEnemy->buildBehaviourTree_Enemy1();
            }
            else if (enemyType == 1)
            {
                pEnemy->buildBehaviourTree_Enemy2();
            }
            else if (enemyType == 2)
            {
                pEnemy->buildBehaviourTree_Enemy3();
            }
            else if (enemyType == 3)
            {
                pEnemy->buildBehaviourTree_Enemy4();
            }
            
            g_vecEnemies.push_back(pEnemy);

            g_enemies_spawned++;
            g_spawn_timer = 0;
        }
        else if (g_vecEnemies.size() >= 8)
        {
            g_spawn_timer = 0;
        }

        // **************************************************

        // Render normally

        // Point the output to the regular framebuffer (the screen)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //        glm::mat4 m, p, v, mvp;

        matView = glm::lookAt(::g_pFlyCamera->getEyeLocation(),
            ::g_pFlyCamera->getTargetLocation(),
            upVector);

        matProjection = glm::perspective(0.6f,           // FOV
            ratio,          // Aspect ratio of screen
            0.1f,           // Near plane (as far from the camera as possible)
            1000.0f);       // Far plane (as near to the camera as possible)

        DrawSkyBox(program, ::g_pFlyCamera->getEyeLocation(), "Space", matProjection, matView);
        RenderScene(program, matProjection, matView, ratio, ::g_pFlyCamera->getEyeLocation());

        // **************************************************

        // Handle async IO stuff
        handleKeyboardAsync(window);
        handleMouseAsync(window);

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::string lightOn;

        if (::g_pLightManager->theLights[g_selectedLightIndex].param2.x == true)
        {
            lightOn = "ON";
        }
        else
        {
            lightOn = "OFF";
        }

        std::stringstream ssTitle;
        ssTitle << "Enemies Killed: " << g_enemies_killed;

        glfwSetWindowTitle(window, ssTitle.str().c_str());

    }// End of the draw loop


    // Delete everything
    delete ::g_pFlyCamera;
    delete ::g_pPhysicEngine;

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void AddModelsToScene(void)
{
    // Load some models to draw

    DrawMeshes();
  
    LoadMeshes();

    for (int i = 0; i < 600; i++)
    {
        float xCoord = getRandomFloat( -500, 500 );
        float yCoord = getRandomFloat(-2, 0);
        float zCoord = getRandomFloat(-500, 500);

        float scale = getRandomFloat(3, 8);

        // Place trees
        sMesh* pTree = new sMesh();
        pTree->modelFileName = "assets/models/low_poly_tree_xyz_N_uv.ply";
        pTree->positionXYZ = glm::vec3(xCoord, yCoord, zCoord);
        pTree->uniformScale = scale;
        pTree->objectColourRGBA = glm::vec4(220/255, 188/255, 140/255, 1.0f);
        pTree->uniqueFriendlyName = "Tree" + std::to_string(i);
        pTree->alphaTransparency = 1.0;
        pTree->textureScale = 1.0;
        pTree->textures[0] = "low_poly_tree_texture.bmp";
        pTree->bModulateTexture = false;

        if ((xCoord < 150 && zCoord < 150) &&
            (xCoord > -150 && zCoord > -150))
        {
            delete pTree;
        }
        else
        {
            ::g_vecMeshesToDraw.push_back(pTree);
        }
    }

    return;
}

void g_ShootBullet(sMesh* pSoldierMesh, glm::vec2 moveXZ)
{
    g_total_bullet_count++;

    sMesh* pBulletMesh = new sMesh();
    pBulletMesh->modelFileName = "assets/models/Sphere_radius_1_xyz_N_uv.ply";
    pBulletMesh->uniqueFriendlyName = "Bullet" + std::to_string(g_total_bullet_count);
    pBulletMesh->positionXYZ = pSoldierMesh->positionXYZ;
    pBulletMesh->uniformScale = 0.5f;
    if (pSoldierMesh->uniqueFriendlyName == "Player")
    {
        pBulletMesh->textures[0] = "vase_texture.bmp";
    }
    else
    {
        pBulletMesh->textures[0] = "red_square.bmp";
    }

    g_vecMeshesToDraw.push_back(pBulletMesh);

    sBullet* pBullet = new sBullet();
    pBullet->pAssociatedMesh = pBulletMesh;
    pBullet->pSoldierMesh = pSoldierMesh;
    pBullet->speed = 1.0f;
    pBullet->direction = glm::normalize(moveXZ);

    g_vecBullets.push_back(pBullet);

    return;
}