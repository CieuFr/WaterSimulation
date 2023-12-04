
#ifndef TEXTURE_DEBBUGER_H
#define TEXTURE_DEBBUGER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>




class TextureDebugger
{
public:
    GLuint debugVAO, debugVBO;
   
    TextureDebugger() {

        GLfloat debugVertices[] = {

       -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
       -1.0f,  -1.0f,0.0f, 0.0f, 0.0f,
       1.0f,  -1.0f, 0.0f,1.0f, 0.0f,

       -1.0f,  1.0f, 0.0f,0.0f, 1.0f,
       1.0f,  -1.0f,0.0f, 1.0f, 0.0f,
       1.0f,   1.0f, 0.0f,1.0f, 1.0f
        };
     
        glGenVertexArrays(1, &debugVAO);
        glGenBuffers(1, &debugVBO);
        glBindVertexArray(debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(debugVertices), debugVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    ~TextureDebugger() {
        glDeleteVertexArrays(1, &debugVAO);
        glDeleteBuffers(1, &debugVBO);

    }

    void render() {
        glBindVertexArray(debugVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

  

private:
   
   
};
#endif

