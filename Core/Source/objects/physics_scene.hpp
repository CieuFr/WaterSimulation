
#ifndef PHYSICS_SCENE_H
#define PHYSICS_SCENE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "water.hpp"
#include "base_object.hpp"
#include "sphere.hpp"


class PhysicSphere : public Sphere {
public : 
	float mass = 1.0f;
	Vec3f pos = Vec3f(0, 0, 0);
	float radius = 1.0f;
	Mat4f model = MAT4F_ID;
	//GLuint geometryTexture[4]; // pos // normal // uv / indices

	PhysicSphere(float p_radius, float p_mass, Vec3f p_pos) : mass(p_mass), pos(p_pos) {
		float geometryRadius = glm::length(positions[0]);
		radius = geometryRadius * p_radius;
		model = glm::scale(model, Vec3f(radius, radius, radius));
		model = glm::translate(model, Vec3f(pos));

		//glGenTextures(4, geometryTexture);
		//// pos
		//glBindTexture(GL_TEXTURE_2D, geometryTexture[0]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 100, 100, 0, GL_RGB32F, GL_FLOAT, &positions[0]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//// normal
		//glBindTexture(GL_TEXTURE_2D, geometryTexture[1]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 100, 100, 0, GL_RGB32F, GL_FLOAT, &normals[0]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//// uvs
		//glBindTexture(GL_TEXTURE_2D, geometryTexture[2]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 100, 100, 0, GL_RG32F, GL_FLOAT, &uv[0]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//// uvs
		//glBindTexture(GL_TEXTURE_2D, geometryTexture[2]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 100, 100, 0, GL_RED, GL_UNSIGNED_INT, &indices[0]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			

		//glBindTexture(GL_TEXTURE_2D, 0);

	}


};

class PhysicsScene
{
public:

	glm::vec3 gravity = glm::vec3(0.0f, -10.0f, 0.0f);
	float dt = 1.0f / 30.0f;
	glm::vec3 tankSize = glm::vec3(8.f, 5.f, 8.f);
	float tankBorder = 0.03f;
	float waterHeight = 0.8f;
	float waterSpacing = 0.2f;
	bool paused = false;
	Water* water = NULL;
	bool isObjectSelected = false;
	std::vector<PhysicSphere*> physicalObjects;
	int indexSphereSelected = 0;

	PhysicsScene() {
		water = new Water(tankSize.x, tankSize.z, waterHeight, waterSpacing);
	};

     ~PhysicsScene() = default;

	 
	 void addSphere(float radius, float mass, Vec3f pos) {
		 physicalObjects.push_back(new PhysicSphere(radius, mass, pos));
	 }

	 bool selectObject(glm::vec3& rayOrigin,glm::vec3& rayDirection) {
		 for (int i = 0; i < physicalObjects.size(); i++) {
			 glm::vec3 worldPosSphere = physicalObjects[i]->pos;
			 float sphereRadiusWorld = physicalObjects[i]->radius;
			 glm::vec3 point;
			 glm::vec3 normal;
			 if (glm::intersectRaySphere(rayOrigin, rayDirection, worldPosSphere, sphereRadiusWorld*sphereRadiusWorld, point,normal)) {
				 std::cout << "sphere selected" << std::endl;
				 indexSphereSelected = i;
				 isObjectSelected = true;
				 return true;
			 }
		 }
		 std::cout << "nothing hit" << std::endl;
		 return false;
	 }


	 Vec3f getSelectedObjectWorldPos() {
		 return physicalObjects[indexSphereSelected]->model * Vec4f(physicalObjects[indexSphereSelected]->pos,1);
	 }

	 Vec3f getSelectedObjectRadius() {
		 return physicalObjects[indexSphereSelected]->model * Vec4f(physicalObjects[indexSphereSelected]->pos, 1);
	 }

	 Mat4f getSelectedObjectModel() {
		 return physicalObjects[indexSphereSelected]->model;
	 }

	 bool raySphereIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
		 const glm::vec3& sphereCenter, float sphereRadius) {
		 glm::vec3 oc = rayOrigin - sphereCenter;
		 float a = glm::dot(rayDirection, rayDirection);
		 float b = 2.0f * glm::dot(oc, rayDirection);
		 float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
		 float discriminant = b * b - 4 * a * c;

		 if (discriminant < 0) {
			 return false;
		 }
		 else {
			 return true;
		 }
	 }

	 void moveObject(Vec3f newCenter) {
		 if (isObjectSelected) {
			 if (newCenter.x < -tankSize.x/2 + physicalObjects[indexSphereSelected]->radius) {
				 newCenter.x = -tankSize.x / 2 + physicalObjects[indexSphereSelected]->radius;
			 }
			 if (newCenter.y < -tankSize.y / 2 + physicalObjects[indexSphereSelected]->radius) {
				 newCenter.y = -tankSize.y / 2 + physicalObjects[indexSphereSelected]->radius;
			 }
			 if (newCenter.z < -tankSize.z / 2 + physicalObjects[indexSphereSelected]->radius) {
				 newCenter.z = -tankSize.z / 2 + physicalObjects[indexSphereSelected]->radius;
			 }
			 if (newCenter.x > tankSize.x / 2 - physicalObjects[indexSphereSelected]->radius) {
				 newCenter.x = tankSize.x / 2 - physicalObjects[indexSphereSelected]->radius;
			 }
			 if (newCenter.y > tankSize.y / 2 - physicalObjects[indexSphereSelected]->radius) {
				 newCenter.y = tankSize.y / 2 - physicalObjects[indexSphereSelected]->radius;
			 }
			 if (newCenter.z > tankSize.z / 2 - physicalObjects[indexSphereSelected]->radius) {
				 newCenter.z = tankSize.z / 2 - physicalObjects[indexSphereSelected]->radius;
			 }
			 
			 Vec3f translation = newCenter - physicalObjects[indexSphereSelected]->pos;
			 physicalObjects[indexSphereSelected]->pos = newCenter;
			 physicalObjects[indexSphereSelected]->model = glm::translate(physicalObjects[indexSphereSelected]->model, Vec3f(translation));
		 }
	 }

	 void simulateCoupling() {

		 float h1 = 1.0 / water->spacing;
		 float h2 = water->spacing * water->spacing;

		 // pour chaque objet physique de la scene 
		 for (int i = 0; i < physicalObjects.size(); i++) {
			 PhysicSphere object = *physicalObjects[i];
			 Vec3f pos = object.pos;
			 float br = object.radius;

			 float x0 = glm::max(0.f, (water->cx + floor((pos.x - br) * h1)));
			 float x1 = glm::min((float)water->numX - 1, water->cx + floor((pos.x + br) * h1));
			 float z0 = glm::max(0.f, water->cz + floor((pos.z - br) * h1));
			 float z1 = glm::min((float)water->numZ - 1, water->cz + floor((pos.z + br) * h1));

		 }

	 }

	 void render() {
		 for (int i = 0; i < physicalObjects.size(); i++) {
			physicalObjects[i]->render();
		 }

	 }
 
private:
   
   
};
#endif

