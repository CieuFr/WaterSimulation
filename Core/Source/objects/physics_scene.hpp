
#ifndef PHYSICS_SCENE_H
#define PHYSICS_SCENE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "water_surface.hpp"
#include "base_object.hpp"


class PhysicsScene
{
public:

     PhysicsScene() = default;
     ~PhysicsScene() = default;

	 glm::vec3 gravity = glm::vec3(0.0f, -10.0f, 0.0f);
	 float dt =  1.0f / 30.0f;
	 glm::vec3 tankSize = glm::vec3(2.5f, 1.0f, 3.0f);
	 float tankBorder = 0.03f;
	 float waterHeight = 0.8f;
	 float waterSpacing = 0.02f;
	 bool paused = false;
	 WaterSurface* waterSurface = NULL;
	 std::vector<BaseObject> objects;
 
private:
   
   
};
#endif

