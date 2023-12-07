#ifndef WATER_H
#define WATER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "base_object.hpp"


class Water : BaseObject
{
public:

    unsigned int waterVAO = 0;
    unsigned int waterVBO;
	unsigned int waterVBOPos;
	unsigned int waterVBOUvs;

	GLint detail = 256;
	GLfloat* waterMesh = NULL;
	// physics
	float waveSpeed = 2.0f;
	float posDamping = 1.0f;
	float velDamping = 0.3f;
	float alpha = 0.5f;
	float time = 1.0f;
	int numX;
	int numZ;
	float spacing;
	int numCells;
	float* heights;
	float* bodyHeights;
	float* prevHeights;
	float* velocities;
	// mesh
	float* positions;
	float* uvs;
	float cx;
	float cz;
	float* index;



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

	WaterSurface(int sizeX, int sizeZ,int  depth, float p_spacing) {
		std::cout << "sizeY : " << sizeZ << std::endl;

		spacing = p_spacing;

		numX = (int)glm::floor(static_cast<float>(sizeX) / spacing) + 1;
		std::cout << "numX : " << numX << std::endl;

		numZ = (int)glm::floor(static_cast<float>(sizeZ) / spacing) + 1;
		
		numCells = numX * numZ;
		heights = new float[numCells];
		bodyHeights = new float[numCells];
		prevHeights = new float[numCells];
		velocities = new float[numCells];
		for (int i = 0; i < numCells; i++) {
			heights[i] = depth;
			velocities[i] = 0;
		}	
		positions = new float[numCells * 3];
		uvs = new float[numCells * 2];
		cx = glm::floor(numX / 2.0f);
		cz = glm::floor(numZ / 2.0f);

		for (float i = 0; i < numX; i++) {
			for (float j = 0; j < numZ; j++) {
				positions[3 * (int)(i * numZ + j)] = (i - cx) * spacing;
				positions[3 * (int)(i * numZ + j) + 2] = (j - cz) * spacing;

				uvs[2 * (int)(i * numZ + j)] = i / numX;
				uvs[2 * (int)(i * numZ + j) + 1] = j / numZ;
			}
		}

		index = new float[(numX - 1) * (numZ - 1) * 2 * 3];
		int pos = 0;

		for (int i = 0; i < numX - 1; i++) {
			for (int j = 0; j < numZ - 1; j++) {
				int id0 = i * numZ + j;
				int id1 = i * numZ + j + 1;
				int id2 = (i + 1) * numZ + j + 1;
				int id3 = (i + 1) * numZ + j;

				index[pos++] = id0;
				index[pos++] = id1;
				index[pos++] = id2;

				index[pos++] = id0;
				index[pos++] = id2;
				index[pos++] = id3;
			}
		}

		glGenVertexArrays(1, &waterVAO);
		glGenBuffers(1, &waterVBOPos);
		glGenBuffers(1, &waterVBOUvs);
		glBindVertexArray(waterVAO);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBOPos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numCells * 3, positions, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBOUvs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numCells * 2 , uvs, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glBindVertexArray(0);
	}

    ~WaterSurface() {
        glDeleteVertexArrays(1, &waterVAO);
        glDeleteBuffers(1, &waterVBO);
     }


	bool Water::setupVertices() {
		glGenVertexArrays(1, &vertices_vao);
		glBindVertexArray(vertices_vao);

		glGenBuffers(1, &vertices_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);

		std::vector<HeightFieldVertex> tmp(field_size * field_size, HeightFieldVertex());
		for (int j = 0; j < field_size; ++j) {
			for (int i = 0; i < field_size; ++i) {
				int dx = j * field_size + i;
				tmp[dx].tex.set(i, j);
			}
		}

		for (int j = 0; j < field_size - 1; ++j) {
			for (int i = 0; i < field_size - 1; ++i) {
				int a = (j + 0) * field_size + (i + 0);
				int b = (j + 0) * field_size + (i + 1);
				int c = (j + 1) * field_size + (i + 1);
				int d = (j + 1) * field_size + (i + 0);
				vertices.push_back(tmp[a]);
				vertices.push_back(tmp[b]);
				vertices.push_back(tmp[c]);

				vertices.push_back(tmp[a]);
				vertices.push_back(tmp[c]);
				vertices.push_back(tmp[d]);
			}
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(HeightFieldVertex) * vertices.size(), &vertices[0].tex, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // tex
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(HeightFieldVertex), (GLvoid*)0);
		return true;
	}
  
	void simulateCoupling() {

		cx = glm::floor(numX / 2.0f);
		cz = glm::floor(numZ / 2.0f);
		float h1 = 1.0 / spacing;
	}

	void simulateSurface() {
		float dt = 1.0 / 30.0;
		waveSpeed = glm::min(waveSpeed, 0.5f * spacing / dt);
		float c = waveSpeed * waveSpeed / spacing / spacing;
		float pd = glm::min(posDamping * dt, 1.0f);
		float vd = glm::max(0.0f, 1.0f - velDamping * dt);
		for (int i = 0; i < numX; i++) {
			for (int j = 0; j < numZ; j++) {
				int id = i * numZ * j;
				float h = heights[id];
				float sumH = 0.0f;
				sumH += i > 0 ? heights[id - numZ] : h;
				sumH += i < numX - 1 ? heights[id + numZ] : h;
				sumH += j > 0 ? heights[id - 1] : h;
				sumH += j < numZ - 1 ? heights[id + 1] : h;
				velocities[id] += dt * c * (sumH - 4.0 * h);
				heights[id] += (0.25 * sumH - h) * pd; 
			}
		}

		for (int i = 0; i < numCells; i++) {
			velocities[i] *= vd;		// velocity damping
			heights[i] += velocities[i] * dt;
		}

		
	}

	void dropOnWater(float x, float y) {

		heights[25] = heights[25 * 25];
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

