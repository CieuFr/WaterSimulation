#ifndef COMPUTE_RENDERER_H
#define COMPUTE_RENDERER_H


#include "base_renderer.hpp"


#include "../utils/quad.hpp"





class ComputeRenderer : public BaseRenderer
{
public:

    ComputeRenderer() : BaseRenderer() {}

    ~ComputeRenderer() {
        glDeleteTextures(1, &texture);
        glDeleteProgram(screenQuadShader->ID);
        glDeleteProgram(computeShader->ID);
    }


    // timing 
    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f; // time of last frame

   
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

    Shader* screenQuadShader = NULL;
    ComputeShader* computeShader = NULL;

    // query limitations
    // -----------------
    int max_compute_work_group_count[3];
    int max_compute_work_group_size[3];
    int max_compute_work_group_invocations;

    Quad* quad = NULL;
    unsigned int texture;

    int fCounter = 0;

    //IMGUI VARIABLES
    bool drawTriangle = true;
    float roughness = 0.5f;
    float metallic = 0.5f;

    const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;

    void init() override;
    void render() override;
    void handleEvents(GLFWwindow* window, float deltaTime) override;
    void displayUI() override;
    void handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset) override;
    void handleMouseEvents(GLFWwindow* window, double xposIn, double yposIn) override;
  

private:
   
};
#endif

