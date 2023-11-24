#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "define.hpp"

class Mesh
{
  public:
	Mesh() {};
	~Mesh() {};

	std::vector<glm::vec3>		  vectorPositions;
	std::vector<glm::vec3>		  vectorColors;
	std::vector<unsigned int> vectorIndices;
	glm::mat4					  transformationMatrice = glm::mat4();
	GLuint					  VBOVertices			= GL_INVALID_VALUE;
	GLuint					  VBOColors				= GL_INVALID_VALUE;
	GLuint					  VAO					= GL_INVALID_VALUE;
	GLuint					  EBO					= GL_INVALID_VALUE;

	void cleanUp() {
		glDeleteBuffers(1, &VBOVertices);
		glDeleteBuffers(1, &VBOColors);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
	}

};


class BasicObjectDrawer
{
  public: 
	GLuint drawQuad()
	{
		int _indices[ 6 ] = { 0, 1, 2, 2, 1, 3 };
		// Les sommets du rectangle
		float _vertices[] = {
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
		};

		unsigned int quadVAO, _vbo, _ebo;

		glCreateBuffers( 1, &_vbo );
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _vbo, sizeof( _vertices ), _vertices, GL_STATIC_DRAW );

		glNamedBufferData( _ebo, sizeof( _indices ), _indices, GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &quadVAO );

		glEnableVertexArrayAttrib( quadVAO, 0 );
		glEnableVertexArrayAttrib( quadVAO, 1 );

		glVertexArrayAttribFormat( quadVAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof( float ) );

		glVertexArrayAttribBinding( quadVAO, 0, 0 );
		glVertexArrayAttribBinding( quadVAO, 1, 0 );

		glVertexArrayVertexBuffer( quadVAO, 0, _vbo, 0, 5 * sizeof( float ) );

		glVertexArrayElementBuffer( quadVAO, _ebo );

		return quadVAO;
	}

	Mesh createCube()
	{
		Mesh cube;
		cube.vectorPositions.push_back( glm::vec3( 0.5f, 0.5f, -0.5f ) );
		cube.vectorPositions.push_back( glm::vec3( 0.5f, -0.5f, -0.5f ) );
		cube.vectorPositions.push_back( glm::vec3( -0.5f, -0.5f, -0.5f ) );
		cube.vectorPositions.push_back( glm::vec3( -0.5f, 0.5f, -0.5f ) );
		cube.vectorPositions.push_back( glm::vec3( 0.5f, 0.5f, 0.5f ) );
		cube.vectorPositions.push_back( glm::vec3( 0.5f, -0.5f, 0.5f ) );
		cube.vectorPositions.push_back( glm::vec3( -0.5f, -0.5f, 0.5f ) );
		cube.vectorPositions.push_back( glm::vec3( -0.5f, 0.5f, 0.5f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		cube.vectorColors.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
		for ( int i = 0; i < 4; i++ )
		{
			cube.vectorIndices.push_back( i );
			cube.vectorIndices.push_back( ( i + 1 ) % 4 );
			cube.vectorIndices.push_back( i + 4 );
			cube.vectorIndices.push_back( ( ( i + 1 ) % 4 ) + 4 );
			cube.vectorIndices.push_back( ( i + 1 ) % 4 );
			cube.vectorIndices.push_back( i + 4 );
		}

		// BAS
		cube.vectorIndices.push_back( 0 );
		cube.vectorIndices.push_back( 1 );
		cube.vectorIndices.push_back( 2 );
		cube.vectorIndices.push_back( 0 );
		cube.vectorIndices.push_back( 2 );
		cube.vectorIndices.push_back( 3 );

		// HAUT
		cube.vectorIndices.push_back( 4 );
		cube.vectorIndices.push_back( 5 );
		cube.vectorIndices.push_back( 6 );
		cube.vectorIndices.push_back( 4 );
		cube.vectorIndices.push_back( 6 );
		cube.vectorIndices.push_back( 7 );

		glCreateBuffers( 1, &cube.VBOVertices );
		glCreateBuffers( 1, &cube.EBO );
		glCreateBuffers( 1, &cube.VBOColors );

		glNamedBufferData( cube.VBOVertices,
						   cube.vectorPositions.size() * sizeof( glm::vec3 ),
						   cube.vectorPositions.data(),
						   GL_STATIC_DRAW );

		glNamedBufferData(
			cube.VBOColors, cube.vectorColors.size() * sizeof( glm::vec3 ), cube.vectorColors.data(), GL_STATIC_DRAW );

		glNamedBufferData(
			cube.EBO, cube.vectorIndices.size() * sizeof( int ), cube.vectorIndices.data(), GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &cube.VAO );

		glEnableVertexArrayAttrib( cube.VAO, 0 );
		glVertexArrayAttribFormat( cube.VAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( cube.VAO, 0, cube.VBOVertices, 0, sizeof( glm::vec3 ) );
		glVertexArrayAttribBinding( cube.VAO, 0, 0 );

		glEnableVertexArrayAttrib( cube.VAO, 1 );
		glVertexArrayAttribFormat( cube.VAO, 1, 3, GL_FLOAT, GL_TRUE, 0 );
		glVertexArrayVertexBuffer( cube.VAO, 1, cube.VBOColors, 0, sizeof( glm::vec3 ) );
		glVertexArrayAttribBinding( cube.VAO, 1, 1 );

		glVertexArrayElementBuffer( cube.VAO, cube.EBO );

		cube.transformationMatrice = Mat4f( 1.0f );
		//cube.transformationMatrice = glm::scale( cube.transformationMatrice, glm::vec( 0.8f, 0.8f, 0.8f ) );

		return cube;
	}

};