#ifndef MANUAL_PBR_RENDERER_H
#define MANUAL_PBR_RENDERER_H


#include "base_renderer.hpp"

#include "../utils/skybox.hpp"
#include "../utils/sphere.hpp"
#include "../utils/cube.hpp"
#include "../utils/quad.hpp"



class ManualPBRRenderer : public BaseRenderer
{
public:

    ManualPBRRenderer() : BaseRenderer(){}

    ~ManualPBRRenderer(){   
        glDeleteProgram(lightingShader->ID);
        glDeleteProgram(lightCubeShader->ID);
        glDeleteProgram(skyboxShader->ID);
        glDeleteProgram(manualPBR->ID);
    }



    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

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
    Shader* manualPBR = NULL;

    Cube* cube = NULL;
    Skybox* skybox = NULL;
    Sphere* sphere  = NULL;

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

    Vec3f colorBack = Vec3f(1.f, 1.0f, 1.0f);
    Vec3f colorRight = Vec3f(0.f, 1.0f, 0.0f);
    Vec3f colorLeft = Vec3f(1.f, 0.0f, 0.0f);
    Vec3f colorTop = Vec3f(1.f, 1.0f, 1.0f);
    Vec3f colorBot = Vec3f(1.f, 1.0f, 1.0f);



    //IMGUI VARIABLES
    bool drawTriangle = true;
    float roughness = 0.5f;
    float metallic = 0.5f;

    void init() override;
    void render() override;
    void handleEvents(GLFWwindow* window, float deltaTime) override;
    void displayUI() override;
    void handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset) override;
    void handleMouseEvents(GLFWwindow* window, double xposIn, double yposIn) override;
  
    void renderCornellBox();
    void initCornellBox();


    struct QuadData {
        Mat4f model;
        Vec3f color;
    };
  

    std::vector<QuadData> quadData;


private:
   
};
#endif

