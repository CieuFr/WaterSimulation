#ifndef TEST_RELFECTION_H
#define TEST_RELFECTION_H


#include "base_renderer.hpp"

#include "../utils/skybox.hpp"
#include "../utils/sphere.hpp"
#include "../utils/cube.hpp"
#include "../utils/quad.hpp"



class TestReflection : public BaseRenderer
{
public:

    TestReflection() : BaseRenderer(){}

    ~TestReflection(){   
        glDeleteProgram(colorShader->ID);
        glDeleteProgram(reflectionShader->ID);
        glDeleteProgram(skyboxShader->ID);
      
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
    Shader* reflectionShader = NULL;
    Shader* colorShader = NULL;
    Shader* skyboxShader = NULL;


    Sphere* sphere  = NULL;
    Quad* back = NULL;
    Quad* right = NULL;
    Quad* left = NULL;
    Quad* top = NULL;
    Quad* bot = NULL;

    Skybox* skybox = NULL;


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



private:
   
};
#endif

