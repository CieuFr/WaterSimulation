
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

	Vec3f velocity = Vec3f(0, 0, 0);
	float restitution = 0.1f;

	bool isGrabbed = false;

	PhysicSphere(float p_radius, float p_mass, Vec3f p_pos) : mass(p_mass), pos(p_pos) {
		float geometryRadius = glm::length(positions[0]);
		radius = geometryRadius * p_radius;
		model = glm::scale(model, Vec3f(radius, radius, radius));
		model = glm::translate(model, Vec3f(pos));

	}

	void applyForce(float force, float deltaTime) {
		velocity.y += deltaTime * force / mass;
		velocity *= 0.999;
	}

	void simulate(Vec3f gravity, float deltaTime, Vec3f tankSize, float tankBorder) {
		if (!isGrabbed) {
			Vec3f previousPos = pos;
			velocity += gravity * deltaTime;
			pos += velocity * deltaTime;

			float wx = 0.5 * tankSize.x - radius - 0.5 * tankBorder;
			float wz = 0.5 * tankSize.z - radius - 0.5 * tankBorder;


			if (pos.x < -wx) {
				pos.x = -wx; velocity.x = -restitution * velocity.x;
			}
			if (pos.x > wx) {
				pos.x = wx; velocity.x = -restitution * velocity.x;
			}
			if (pos.z < -wz) {
				pos.z = -wz; velocity.z = -restitution * velocity.z;
			}
			if (pos.z > wz) {
				pos.z = wz; velocity.z = -restitution * velocity.z;
			}
			if (pos.y < - tankSize.y/2 + 2*radius) {
				pos.y = -tankSize.y/2 + 2*radius ; velocity.y = -restitution * velocity.y;
			}
			if (pos.y > tankSize.y /2) {
				pos.y = tankSize.y / 2; velocity.y = -restitution * velocity.y;
			}


			Vec3f translation = pos - previousPos;
			model = glm::translate(model, Vec3f(translation));
			
		}
	}


};

class PhysicsScene
{
public:

	glm::vec3 gravity = glm::vec3(0.0f, -10.0f, 0.0f);
	float dt = 1.0f / 30.0f;
	float time = 0;

	glm::vec3 tankSize = glm::vec3(8.f, 10.f, 8.f);
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
				 physicalObjects[indexSphereSelected]->isGrabbed = true;
				 return true;
			 }
		 }
		 std::cout << "nothing hit" << std::endl;
		 return false;
	 }

	 void unSelectObject() {
		 isObjectSelected = false;
		 physicalObjects[indexSphereSelected]->isGrabbed = false;
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

	 void simulateCoupling(float deltaTime) {

		 float h1 = 1.0 / water->spacing;
		 float h2 = water->spacing * water->spacing;
		 float* data = new float[water->numX * water->numZ];
		 glGetTextureImage(water->waterHeightTexture[1- water->toggle], 0, GL_RED, GL_FLOAT, water->numX * water->numZ * sizeof(float), data);

		 // pour chaque objet physique de la scene 

		 water->prevHeights.assign(water->bodyHeights.begin(), water->bodyHeights.end());
		 std::fill(water->bodyHeights.begin(), water->bodyHeights.end(), 0.0f);

		 for (PhysicSphere* object : physicalObjects) {
			 Vec3f pos = object->pos;
			 float br = object->radius;

			 float x0 = glm::max(0.f, (water->cx + floor((pos.x - br) * h1)));
			 float x1 = glm::min((float)water->numX - 1, water->cx + floor((pos.x + br) * h1));
			 float z0 = glm::max(0.f, water->cz + floor((pos.z - br) * h1));
			 float z1 = glm::min((float)water->numZ - 1, water->cz + floor((pos.z + br) * h1));

			 for (int xi = x0; xi <= x1; xi++) {
				 for (int zi = z0; zi <= z1; zi++) {
					 float x = (xi - water->cx) * water->spacing;
					 float z = (zi - water->cz) * water->spacing;
					 float r2 = (pos.x - x) * (pos.x - x) + (pos.z - z) * (pos.z - z);
					 if (r2 < br * br) {
						 float bodyHalfHeight = sqrt(br * br - r2);
						 float waterHeight = data[xi * water->numZ + zi] + 0.8f;
						 // ICI
						 float bodyMin = glm::max(pos.y - bodyHalfHeight, -tankSize.y / 2 + physicalObjects[indexSphereSelected]->radius);
						 float bodyMax = glm::min(pos.y + bodyHalfHeight, waterHeight);
						 float bodyHeight = glm::max(bodyMax - bodyMin, 0.0f);
						 if (bodyHeight > 0.0) {
							 object->applyForce(-bodyHeight * h2 * gravity.y, deltaTime);
							 water->bodyHeights[xi * water->numZ + zi] += bodyHeight;
						 }
					 }
				 }
			 }
		 }

		 for (int iter = 0; iter < 2; iter++) {
			 for (int xi = 0; xi < water->numX; xi++) {
				 for (int zi = 0; zi < water->numZ; zi++) {
					 int id = xi * water->numZ + zi;

					 int num = xi > 0 && xi < water->numX - 1 ? 2 : 1;
					 num += zi > 0 && zi < water->numZ - 1 ? 2 : 1;
					 float avg = 0.0;
					 if (xi > 0) avg += water->bodyHeights[id - water->numZ];
					 if (xi < water->numX - 1) avg += water->bodyHeights[id + water->numZ];
					 if (zi > 0) avg += water->bodyHeights[id - 1];
					 if (zi < water->numZ - 1) avg += water->bodyHeights[id + 1];
					 avg /= num;
					 water->bodyHeights[id] = avg;
				 }
			 }
		 }
		
		 for (int i = 0; i < water->numCells; i++) {
			 float bodyChange = water->bodyHeights[i] - water->prevHeights[i];
			 water->bodyChange[i] = water->bodyHeights[i];

			 //water->heights[i] += water->alpha * bodyChange;
		 }

		//glTextureSubImage2D(water->waterHeightTexture[1 - water->toggle], 0, 0, 0, water->numX, water->numZ, GL_RED, GL_FLOAT, &water->heights[0]);

		glTextureSubImage2D(water->bodyChangeTexture, 0, 0, 0, water->numX, water->numZ, GL_RED, GL_FLOAT, &water->bodyChange[0]);
		// glTextureSubImage2D(water->waterHeightTexture[0], 0, 0, 0, water->numX, water->numZ, GL_RED, GL_FLOAT, &water->heights);
		 water->heightObjectUpdate();
	

	 }

	 void simulate(float deltaTime) {
		 
		 simulateCoupling(deltaTime);
		 for (size_t i = 0; i < physicalObjects.size(); i++)
		 {
			 physicalObjects[i]->simulate(gravity,deltaTime,tankSize,tankBorder);
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

