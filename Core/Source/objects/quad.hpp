#ifndef QUAD_H
#define QUAD_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "base_object.hpp"


class Quad : BaseObject
{
public:

    GLuint vao, vbo[3], ebo;

    Quad() {
        GLfloat vertices[] = {
        1.0f,  1.0f, 0.0f,  // Vertex 1
       -1.0f,  1.0f, 0.0f,  // Vertex 2
       -1.0f, -1.0f, 0.0f,  // Vertex 3
        1.0f, -1.0f, 0.0f   // Vertex 4
        };

        GLfloat uvs[] = {
            1.0f, 1.0f,  // UV for Vertex 1
            0.0f, 1.0f,  // UV for Vertex 2
            0.0f, 0.0f,  // UV for Vertex 3
            1.0f, 0.0f   // UV for Vertex 4
        };

        GLfloat normals[] = {
            0.0f, 0.0f, 1.0f,  // Normal for Vertex 1
            0.0f, 0.0f, 1.0f,  // Normal for Vertex 2
            0.0f, 0.0f, 1.0f,  // Normal for Vertex 3
            0.0f, 0.0f, 1.0f   // Normal for Vertex 4
        };

        GLuint indices[] = {
            0, 1, 2,  // Triangle 1
            0, 2, 3   // Triangle 2
        };

  
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(3, vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    ~Quad() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo[0]);
     }
    
    void render() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }


private:
   
   
};
#endif

