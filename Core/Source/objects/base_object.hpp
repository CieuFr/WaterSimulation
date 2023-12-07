
#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../utils/define.hpp"


class BaseObject
{
public:

    glm::mat4 modelMatrix = MAT4F_ID;

    glm::vec3 velocity;
    float mass = 1.0f;

    bool isGrabbable = false;
    bool isGrabbed = false;
    
    glm::vec3 albedo = glm::vec3(1.0, 1.0, 1.0);

    virtual ~BaseObject() = default;

    virtual void render() = 0;

    void translate(glm::vec3 translationVector) {
        modelMatrix = glm::translate(modelMatrix, translationVector);
    }

    void scale(glm::vec3 scaleVector) {
        modelMatrix = glm::scale(modelMatrix, scaleVector);
    }

    void rotateX(float degree) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void rotateY(float degree) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void rotateZ(float degree) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void rotate(float degree, glm::vec3 rotationAxis) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(degree), rotationAxis);
    }

private:
   
   
};
#endif

