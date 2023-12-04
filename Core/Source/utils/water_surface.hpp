#ifndef WATER_SURFACE_H
#define WATER_SURFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


class WaterSurface
{
public:

    unsigned int waterVAO = 0;
    unsigned int waterVBO;
	GLint detail = 256;
	GLfloat* waterMesh = NULL;

    WaterSurface() {
		
		createWaterMesh(waterMesh, detail);
		glGenVertexArrays(1, &waterVAO);
		glGenBuffers(1, &waterVBO);
		glBindVertexArray(waterVAO);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * detail * detail * 30, waterMesh, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);  
    }

    ~WaterSurface() {
        glDeleteVertexArrays(1, &waterVAO);
        glDeleteBuffers(1, &waterVBO);
     }
    
    void render() {
		glBindVertexArray(waterVAO);
		glDrawArrays(GL_TRIANGLES, 0, detail * detail * 6);
		glBindVertexArray(0);
    }


private:

	inline GLfloat denormalize(GLfloat var) {
		return (var * 2.0 - 1.0);
	}

	inline void createWaterMesh(GLfloat*& mesh, GLint detail)
	{
		mesh = new GLfloat[30 * detail * detail];
		GLfloat detailsize = 1.0 / (GLfloat)detail;
		for (int y = 0; y < detail; y++) {
			for (int x = 0; x < detail; x++) {
				int index = (detail * y + x) * 30;
				mesh[index] = denormalize(detailsize * (GLfloat)x);
				mesh[index + 1] = denormalize(detailsize * (GLfloat)y);
				mesh[index + 2] = 0.0;
				mesh[index + 3] = detailsize * (GLfloat)x;
				mesh[index + 4] = detailsize * (GLfloat)y;

				mesh[index + 5] = denormalize(detailsize * (GLfloat)(x + 1));
				mesh[index + 6] = denormalize(detailsize * (GLfloat)y);
				mesh[index + 7] = 0.0;
				mesh[index + 8] = detailsize * (GLfloat)(x + 1);
				mesh[index + 9] = detailsize * (GLfloat)y;

				mesh[index + 10] = denormalize(detailsize * (GLfloat)x);
				mesh[index + 11] = denormalize(detailsize * (GLfloat)(y + 1));
				mesh[index + 12] = 0.0;
				mesh[index + 13] = detailsize * (GLfloat)x;
				mesh[index + 14] = detailsize * (GLfloat)(y + 1);

				mesh[index + 15] = denormalize(detailsize * (GLfloat)x);
				mesh[index + 16] = denormalize(detailsize * (GLfloat)(y + 1));
				mesh[index + 17] = 0.0;
				mesh[index + 18] = detailsize * (GLfloat)x;
				mesh[index + 19] = detailsize * (GLfloat)(y + 1);

				mesh[index + 20] = denormalize(detailsize * (GLfloat)(x + 1));
				mesh[index + 21] = denormalize(detailsize * (GLfloat)(y + 1));
				mesh[index + 22] = 0.0;
				mesh[index + 23] = detailsize * (GLfloat)(x + 1);
				mesh[index + 24] = detailsize * (GLfloat)(y + 1);

				mesh[index + 25] = denormalize(detailsize * (GLfloat)(x + 1));
				mesh[index + 26] = denormalize(detailsize * (GLfloat)y);
				mesh[index + 27] = 0.0;
				mesh[index + 28] = detailsize * (GLfloat)(x + 1);
				mesh[index + 29] = detailsize * (GLfloat)y;
			}
		}
	}

   
   
};
#endif

