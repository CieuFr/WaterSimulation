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

	// physics
	float waveSpeed = 2.0f;
	float posDamping = 1.5f;
	float velDamping = 0.3f;
	float alpha = 0.5f;
	float time = 0.0f;
	int numX;
	int numZ;
	int sizea;
	int sizeb;

	float spacing;
	int numCells;
	std::vector<float> heights;
	std::vector<float> bodyHeights;
	std::vector<float> prevHeights;
	std::vector<float> velocities;

	/* used to process the height field and extract some usefull data */
	GLuint process_fbo;         /* we use a separate FBO to perform the processing step so we have some space for extra attachments */
	GLuint tex_out_norm;        /* the GL_RGB32F texture that will keep our calculated normals */
	GLuint tex_out_pos;         /* the GL_RGB32F texture that will keep our positions */
	GLuint tex_out_texcoord;    /* the GL_RG32F texture that will keep our texcoords */
	Shader* waterMovement;       /* the program we use to calculate things like normals, etc.. */
	Shader* normalShader;           /* the program we use to calculate the positions */

	GLuint field_fbo[2];
	GLuint tex_u0;              /* height value */
	GLuint tex_u1;              /* height value */
	GLuint tex_v0;              /* velocity at which u diffuses */
	GLuint tex_v1;              /* velocity at which u diffuses */
	short toggle = 1;

	// mesh
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> vertexTexCoords;
	std::vector<glm::uvec3> vertexIndices;
	GLuint waterVAO = 0;
	GLuint waterVBO_pos = 0;
	GLuint waterVBO_normal = 0;
	GLuint waterVBO_tex = 0;
	GLuint water_ibo = 0;

	Quad* defferedQuad = NULL;
	Shader* dropOnWater = NULL;
	GLuint waterHeightFBO[2];
	GLuint waterHeightTexture[2];
	GLuint waterVelocityTexture[2];

	GLenum _drawBuffers[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};

	float cx;
	float cz;

	Water() {
	}

	Water(float sizeX, float sizeZ, float  depth, float p_spacing) {
		defferedQuad = new Quad();
		sizea = sizeX;
		sizeb = sizeZ;

		spacing = p_spacing;
		numX = glm::floor(sizeX / spacing) + 1;
		numZ = glm::floor(sizeZ / spacing) + 1;

		numCells = numX * numZ;

		heights.reserve(numCells);
		bodyHeights.reserve(numCells);
		prevHeights.reserve(numCells);
		velocities.reserve(numCells);

		for (int i = 0; i < numCells; i++) {
			heights[i] = depth;
			velocities[i] = 0;
		}

		for (int i = 0; i < numCells; i++) vertexPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		for (int i = 0; i < numCells; i++) vertexNormals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		for (int i = 0; i < numCells; i++) vertexTexCoords.push_back(glm::vec2(0.0f, 0.0f));

		cx = glm::floor(numX / 2.0f);
		cz = glm::floor(numZ / 2.0f);

		for (float i = 0; i < numX; i++) {
			for (float j = 0; j < numZ; j++) {
				int id = i * numZ + j;
				vertexPositions[id].x = (i - cx) * spacing;
				vertexPositions[id].z = (j - cz) * spacing;
				vertexTexCoords[id].x =  (i / numX);
				vertexTexCoords[id].y = 1 - (j / numZ);
				/*std::cout <<"id : " << id << std::endl;
				std::cout << "x : " << vertexTexCoords[id].x << std::endl;
				std::cout << "y : " << vertexTexCoords[id].y << std::endl;*/

			}
		}

		for (int i = 0; i < numX - 1; i++) {
			for (int j = 0; j < numZ - 1; j++) {
				int id0 = i * numZ + j;
				int id1 = i * numZ + j + 1;
				int id2 = (i + 1) * numZ + j + 1;
				int id3 = (i + 1) * numZ + j;

				vertexIndices.push_back(glm::uvec3(id0, id1, id2));
				vertexIndices.push_back(glm::uvec3(id0, id2, id3));
			}
		}

		int vertexBufferSize = sizeof(glm::vec3) * vertexPositions.size(); // Gather the size of the buffer from the CPU-side vector

		glCreateBuffers(1, &waterVBO_pos); // Generate a GPU buffer to store the positions of the vertices

		glNamedBufferStorage(waterVBO_pos, vertexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT); // Create a data store on the GPU
		glNamedBufferSubData(waterVBO_pos, 0, vertexBufferSize, vertexPositions.data()); // Fill the data store from a CPU array

		glCreateBuffers(1, &waterVBO_normal); // Same for normal
		glNamedBufferStorage(waterVBO_normal, vertexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferSubData(waterVBO_normal, 0, vertexBufferSize, vertexNormals.data());

		glCreateBuffers(1, &waterVBO_tex); // Same for texture coordinates
		int texCoordBufferSize = sizeof(glm::vec2) * vertexTexCoords.size();
		glNamedBufferStorage(waterVBO_tex, texCoordBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferSubData(waterVBO_tex, 0, texCoordBufferSize, vertexTexCoords.data());

		glCreateBuffers(1, &water_ibo); // Same for the index buffer, that stores the list of indices of the triangles forming the mesh
		int indexBufferSize = sizeof(glm::uvec3) * vertexIndices.size();
		glNamedBufferStorage(water_ibo, indexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferSubData(water_ibo, 0, indexBufferSize, vertexIndices.data());

		glCreateVertexArrays(1, &waterVAO); // Create a single handle that joins together attributes (vertex positions, normals) and connectivity (triangles indices)
		glBindVertexArray(waterVAO);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO_pos);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO_tex);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO_normal);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, water_ibo);
		glBindVertexArray(0); // Desactive the VAO just created. Will be activated at rendering time. 

		setUpTextures();
	}

	~Water() {
		vertexPositions.clear();
		vertexNormals.clear();
		vertexTexCoords.clear();
		vertexIndices.clear();
		if (waterVAO) {
			glDeleteVertexArrays(1, &waterVAO);
			waterVAO = 0;
		}
		if (waterVBO_pos) {
			glDeleteBuffers(1, &waterVBO_pos);
			waterVBO_pos = 0;
		}
		if (waterVBO_normal) {
			glDeleteBuffers(1, &waterVBO_normal);
			waterVBO_normal = 0;
		}
		if (waterVBO_tex) {
			glDeleteBuffers(1, &waterVBO_tex);
			waterVBO_tex = 0;
		}
		if (water_ibo) {
			glDeleteBuffers(1, &water_ibo);
			water_ibo = 0;
		}
	}


	void render() {
		glBindVertexArray(waterVAO); // Activate the VAO storing geometry data
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei> (vertexIndices.size() * 3), GL_UNSIGNED_INT, 0);
		// Call for rendering: stream the current GPU geometry through the current GPU program
	}


	bool setUpTextures() {

		glGenTextures(1, &tex_out_norm);
		glBindTexture(GL_TEXTURE_2D, tex_out_norm);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, numX, numZ, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &tex_out_pos);
		glBindTexture(GL_TEXTURE_2D, tex_out_pos);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, numX, numZ, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &tex_out_texcoord);
		glBindTexture(GL_TEXTURE_2D, tex_out_texcoord);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, numX, numZ, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenFramebuffers(1, &process_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, process_fbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_out_pos, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_out_norm, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex_out_texcoord, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Error: process framebuffer not complete.\n");
			return false;
		}


		waterMovement = new Shader{ "Source/shaders/screen_space_quad.vs", "Source/shaders/water_movement.fs" };
		dropOnWater = new Shader{ "Source/shaders/screen_space_quad.vs", "Source/shaders/drop_on_water.fs" };
		normalShader = new Shader{ "Source/shaders/screen_space_quad.vs", "Source/shaders/normal_water.fs" };

		dropOnWater->use();
		dropOnWater->setInt("u_height", 0);
		dropOnWater->setInt("u_velocity", 1);

		waterMovement->use();
		waterMovement->setInt("u_height", 0);
		waterMovement->setInt("u_velocity", 1);

		

		waterMovement->setFloat("u_waveSpeed", waveSpeed);
		waterMovement->setFloat("u_pd", posDamping);
		waterMovement->setFloat("u_vd", velDamping);
		float c = waveSpeed * waveSpeed / spacing / spacing;
		waterMovement->setFloat("u_c", c);
		


		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int upper = 50;
		int lower = 30;
		std::vector<float>					u;
		std::vector<float>					v;

		for (unsigned int i = 0; i < numX*numZ; i++)
		{
			for (int j = 0; j < numX; ++j) {
				for (int i = 0; i < numZ; ++i) {
					if (i > lower && i < upper && j > lower && j < upper) {
						u.push_back(0.f);
					}
					else {
						u.push_back(0.f);
					}
					v.push_back(0.f);
					
				}
			}
			
		}

		glCreateFramebuffers(2, waterHeightFBO);

		// The texture we're going to render to
		glCreateTextures(GL_TEXTURE_2D, 2, waterHeightTexture);
		glCreateTextures(GL_TEXTURE_2D, 2, waterVelocityTexture);

		for (size_t i = 0; i < 2; i++)
		{
			glTextureStorage2D(waterHeightTexture[i], 1, GL_R32F, numX, numZ);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTextureSubImage2D(waterHeightTexture[i], 0, 0, 0, numX, numZ, GL_RED, GL_FLOAT, &u[0]);

			glTextureStorage2D(waterVelocityTexture[i], 1, GL_R32F, numX, numZ);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTextureSubImage2D(waterVelocityTexture[i], 0, 0, 0, numX, numZ, GL_RED, GL_FLOAT, &v[0]);

			glNamedFramebufferTexture(waterHeightFBO[i], _drawBuffers[0], waterHeightTexture[i], 0);
			glNamedFramebufferTexture(waterHeightFBO[i], _drawBuffers[1], waterVelocityTexture[i], 0);

			glNamedFramebufferDrawBuffers(waterHeightFBO[i], 2, _drawBuffers);
		}

	/*	float* data = new float[numX * numZ];
		glGetTextureImage(waterHeightFBO[0], 0, GL_RED, GL_FLOAT, numX * numZ * sizeof(float), data);
		for (int i = 0; i < numX * numZ; i++)
		{
			std::cout << " | " << data[i] << " | ";

		}*/

		
	}


	void update(float dt) {
		waterMovement->use();
	
		glBindFramebuffer(GL_FRAMEBUFFER, waterHeightFBO[toggle]);
		glBindTextureUnit(0, waterHeightTexture[1 - toggle]);
		glBindTextureUnit(1, waterVelocityTexture[1 - toggle]);

		waterMovement->setFloat("u_dx", spacing/ sizea);
		waterMovement->setFloat("u_dz", spacing / sizeb);
		waterMovement->setFloat("u_dt", dt);
		waveSpeed = glm::min(waveSpeed, 0.5f * spacing / dt);
		waterMovement->setFloat("u_waveSpeed", waveSpeed);
		float l_pd = glm::min(posDamping * dt, 1.0f);
		float l_vd = glm::max(0.0f, 1.0f - (velDamping * dt));

		waterMovement->setFloat("u_pd", l_pd);
		waterMovement->setFloat("u_vd", l_vd);
		defferedQuad->render();
		toggle = 1 - toggle;
	}

	void waterDrop(float dropX, float dropY ) {

		dropOnWater->use();
		glBindFramebuffer(GL_FRAMEBUFFER, waterHeightFBO[toggle]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterHeightTexture[1 - toggle]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waterVelocityTexture[1 - toggle]);
		dropOnWater->setVec2("center", dropX, dropY);
		defferedQuad->render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		toggle = 1 - toggle;
	}


	

};
#endif

