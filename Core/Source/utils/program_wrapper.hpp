#pragma once
#ifndef __PROGRAM_WRAPPER__
#define __PROGRAM_WRAPPER__

#include "victor_utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include "glm/gtc/type_ptr.hpp"

class ProgramWrapper 
{


	public:
	ProgramWrapper() {};
	~ProgramWrapper() {};

	//====================================================
	// PATH : 
	// path[0] -> vertexShader
	// path[1] -> fragmentShader
	//====================================================

	ProgramWrapper( std::string * path ) {
		if (!createProgram(path)) {
			std::cout << "Creating Program Failed \n";
		}
	}

	bool createProgram( std::string* path )
	{
		std::ifstream test(path[0]);
		if (!test)
		{
			std::cout << "The file doesn't exist" << std::endl;
			std::cout << path[0] << std::endl;
		}
		const std::string vertexShaderStr = readFile( path[0] );
		const std::string fragShaderStr	  = readFile( path[1] );

		// Création des shaders
		const GLuint aVertexShader	 = glCreateShader( GL_VERTEX_SHADER );
		const GLuint aFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Récupération des locations des shaders
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

		// Création des shaders
		glShaderSource( aVertexShader, 1, &vSrc, NULL );
		glShaderSource( aFragmentShader, 1, &fSrc, NULL );

		// Compilation des shaders
		glCompileShader( aVertexShader );
		glCompileShader( aFragmentShader );

		// Code Cf. Tp 1 pour vérifier si les shaders compilent
		GLint compiled;
		glGetShaderiv( aVertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( aVertexShader, sizeof( log ), NULL, log );
			glDeleteShader( aVertexShader );
			glDeleteShader( aFragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		// Initialisation du Program
		programId = glCreateProgram();

		// Attache des shaders
		glAttachShader( programId, aVertexShader );
		glAttachShader( programId, aFragmentShader );

		// Link du programme
		glLinkProgram( programId );
		GLint linked;
		glGetProgramiv( programId, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( programId, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		// Deletion des shaders
		glDeleteShader( aVertexShader );
		glDeleteShader( aFragmentShader );

		return true;
	}

	bool createProgramOnlyFS( std::string path )
	{
	
		const std::string fragShaderStr	  = readFile( path );

		// Création des shaders

		const GLuint aFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Récupération des locations des shaders
	
		const GLchar * fSrc = fragShaderStr.c_str();

		// Création des shaders

		glShaderSource( aFragmentShader, 1, &fSrc, NULL );

		// Compilation des shaders
		
		glCompileShader( aFragmentShader );

		// Code Cf. Tp 1 pour vérifier si les shaders compilent
		GLint compiled;
		glGetShaderiv( aFragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( aFragmentShader, sizeof( log ), NULL, log );
			glDeleteShader( aFragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		// Initialisation du Program
		programId = glCreateProgram();

		// Attache des shaders
		glAttachShader( programId, aFragmentShader );

		// Link du programme
		glLinkProgram( programId );
		GLint linked;
		glGetProgramiv( programId, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( programId, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}
		// Deletion des shaders
		glDeleteShader( aFragmentShader );

		return true;
	}

	void useProgram() { glUseProgram( programId );}

	void setUniform( const std::string & name, GLfloat value )
	{
		GLint location = glGetUniformLocation( programId,name.c_str() );
		glProgramUniform1f( programId, location, value );
	}

	void setUniform( const std::string & name, GLint value )
	{
		GLint location = glGetUniformLocation( programId, name.c_str() );
		glProgramUniform1i( programId, location, value );
	}

	void setUniform( const std::string & name, const glm::vec2 & value )
	{
		GLint location = glGetUniformLocation( programId, name.c_str() );
		glProgramUniform2f( programId, location, value.x, value.y );
	}

	void setUniform( const std::string & name, const glm::vec3 & value )
	{
		GLint location = glGetUniformLocation( programId, name.c_str() );
		glProgramUniform3f( programId, location, value.x, value.y, value.z );
	}

	void setUniform( const std::string & name, const glm::mat3 & value )
	{
		GLint location = glGetUniformLocation( programId, name.c_str() );
		glProgramUniformMatrix3fv( programId, location, 1, GL_FALSE, glm::value_ptr( value ) );
	}

	void setUniform( const std::string & name, const glm::mat4 & value )
	{
		GLint location = glGetUniformLocation( programId, name.c_str() );
		glProgramUniformMatrix4fv( programId, location, 1, GL_FALSE, glm::value_ptr( value ) );
	}


	GLuint getProgramId() { return programId;}

	private:

	GLuint programId; 
		
};



#endif 
