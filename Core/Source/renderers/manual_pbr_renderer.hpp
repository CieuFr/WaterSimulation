#ifndef MANUAL_PBR_RENDERER_H
#define MANUAL_PBR_RENDERER_H


#include "base_renderer.hpp"

#include "../objects/skybox.hpp"
#include "../objects/sphere.hpp"
#include "../objects/cube.hpp"
#include "../objects/quad.hpp"

#include <glm/gtx/intersect.hpp>

#include"../utils/Framebuffer.h"

#include "../objects/heightfield.hpp"

#include "../objects/water.hpp"
#include "../objects/physics_scene.hpp"



struct MeshGPUQuad {
    glm::vec3 albedo;
    float metallic;
    float roughness;
    glm::vec3 vertices[4];
    glm::vec3 normals[4];
    glm::vec2 uvs[4];
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    int nb_vertices;
};



struct MeshGPUSphere {
    glm::vec3 albedo;
    float metallic;
    float roughness;
    glm::vec3 vertices[100];
    glm::vec3 normals[100];
    glm::vec2 uvs[100];
    int indices[500];
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    int nb_vertices;
    int nb_indices;

};

class ManualPBRRenderer : public BaseRenderer
{
public:

    ManualPBRRenderer() : BaseRenderer(){}

    ~ManualPBRRenderer(){   
        glDeleteProgram(lightingShader->ID);
        glDeleteProgram(lightCubeShader->ID);
        glDeleteProgram(skyboxShader->ID);
        glDeleteProgram(waterShader->ID);
    }

    //water parameter
    const GLint sizeOfWater = 256;
    const GLfloat TEXELSIZE = 1.0 / sizeOfWater;
    bool userHasClicked = false;
    bool clickEnded = false;
    bool rightClickPressed = false;
    float dropX;
    float dropY;
    bool objectSelected = false;
    bool selectionRayLaunched = false;

    int nbClick = 0;

    GLuint inVAO, inVBO;
   

    // settings
   unsigned int SCR_WIDTH = 800;
   unsigned int SCR_HEIGHT = 600;

    // camera
    Camera camera;
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    glm::vec3 lightPositions[4] = {
      glm::vec3(-10.0f,  10.0f, 10.0f),
      glm::vec3(10.0f,  10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[4] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 model = MAT4F_ID;
    glm::mat4 view = MAT4F_ID;
    // build and compile our shader zprogram
   // ------------------------------------
    const std::string shaderFolder = "src/shaders/";
    Shader* lightingShader = NULL;
    Shader* lightCubeShader = NULL;
    Shader* skyboxShader = NULL;
    Shader* waterShader = NULL;
  
    Shader* debugTextureShader = NULL;
    Shader* wallShader = NULL;
    Shader* normalShader = NULL;

    FrameBuffer* fboClass;

    HeightField* heightfield = NULL;

    Cube* cube = NULL;
    Skybox* skybox = NULL;
    Sphere* sphere  = NULL;
    PhysicsScene* physicScene = NULL;
    Water* waterwater = NULL;

    
    Quad* defferedQuad = NULL;

    Quad* back = NULL;
    Quad* right = NULL;
    Quad* left = NULL;
    Quad* top = NULL;
    Quad* bot = NULL;

    Mat4f modelBack = MAT4F_ID;
    Mat4f modelRight = MAT4F_ID;
    Mat4f modelLeft = MAT4F_ID;
    Mat4f modelTop = MAT4F_ID;
    Mat4f modelBot = MAT4F_ID;

    GLuint waterHeightFBO[2];
    GLuint waterHeightTexture[2];


    Vec3f colorBack = Vec3f(1.f, 1.0f, 1.0f);
    Vec3f colorRight = Vec3f(0.f, 1.0f, 0.0f);
    Vec3f colorLeft = Vec3f(1.f, 0.0f, 0.0f);
    Vec3f colorTop = Vec3f(1.f, 1.0f, 1.0f);
    Vec3f colorBot = Vec3f(1.f, 1.0f, 1.0f);


    // RAYTRACED 
    std::vector<MeshGPUQuad> rayTracedMeshes;
    std::vector<MeshGPUSphere> rayTracedMeshesSphere;

    float nb_meshes_gpu;


    short toggle = 1;

    //IMGUI VARIABLES
    bool drawTriangle = true;
    float roughness = 0.5f;
    float metallic = 0.5f;

    void init() override;
    void render() override;
    void handleEvents(GLFWwindow* window, float deltaTime) override;
    void displayUI() override;
    void handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset) override;
    void handleMouseMoveEvents(GLFWwindow* window, double xposIn, double yposIn) override;
    void handleMouseClickEvents(GLFWwindow* window, int button, int action, int mods) override;
    void handleWindowResize(GLFWwindow* window, int width, int height) override;
  
    void renderCornellBox();
    void initCornellBox();

    void initWater();
    void renderWater();

    void renderSphere();

    void initRayTracedObjects();

    bool isClickOnWater();

    float lastTime = 0;
    int nbFrames = 0;
    float deltaTime = 0.f;
    void updateDeltaTime();

    void dragObject();
    Vec3f getRayFromClick(float x, float y);

    struct QuadData {
        Mat4f model;
        Vec3f color;
    };
  

    std::vector<QuadData> quadData;


private:
   
};
#endif

