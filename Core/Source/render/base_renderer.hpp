#ifndef BASE_RENDERER_H
#define BASE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "../utils/camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../utils/define.hpp"
#include <iostream>
#include "../utils/shader.hpp"
#include "../utils/compute_shader.hpp"
#include "../utils/victor_utils.hpp"



class BaseRenderer 
{
public:

    BaseRenderer() = default;
    virtual ~BaseRenderer() = default;

    virtual void init() = 0;
    virtual void render() = 0;
    virtual void displayUI() = 0;
    virtual void handleEvents(GLFWwindow* window,float deltaTime) = 0;
    virtual void handleScrollEvents(GLFWwindow* window, double xoffset, double yoffset) = 0;
    virtual void handleMouseEvents(GLFWwindow* window, double xposIn, double yposIn) = 0;


private:
   
};
#endif

