#pragma once
#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Camera;

#include "shader.h"

class ParticleSystem
{
private:
	struct Emitter
	{
		Emitter(glm::vec3 position, float seed)
			: timeAlive(0), seed(seed), position(position)
		{
		}

		float timeAlive;
		float seed;
		glm::vec3 position;
	};

	Shader *m_shader;
	int maxParticles = 1000;
	int particlesPerSecond = 100;
	float minLifeTime = 1;
	float maxLifeTime = 5;
	const glm::vec3 minVelocity = glm::vec3(-2.5f, 0.0f, 0.0f);
	const glm::vec3 maxVelocity = glm::vec3( 2.5f, 5.0f, 0.0f);

	unsigned int VAO, VBO;
	std::vector<Emitter> m_emitters;

	void updateParticleEmitter (Emitter& ps, float deltaTime)
	{
		m_shader->setMat4("model", glm::translate(glm::mat4(1.0), ps.position));
		m_shader->setFloat("time", ps.timeAlive);
		m_shader->setFloat("seed", ps.seed);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, maxParticles);
		ps.timeAlive += deltaTime * updateRate;
	}

public:
	float updateRate = 1.0f; // TODO: make getter and setter?

	ParticleSystem ()
	{
		m_shader = new Shader("shaders/particles");
		m_shader->use();
		m_shader->setVec3("minVelocity", minVelocity);
		m_shader->setVec3("maxVelocity", maxVelocity);
		m_shader->setFloat("minLifeTime", minLifeTime);
		m_shader->setFloat("maxLifeTime", maxLifeTime);
		m_shader->setInt("maxParticles", maxParticles);
		m_shader->setInt("particlesPerSecond", particlesPerSecond);

		float particleVertices[] = { 0.0f, 0.0f, 0.0f };
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particleVertices), particleVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// create default emitter
		createEmitter(glm::vec3(10.0f, .0f, 10.0f));
	}

	void createEmitter (glm::vec3 pos)
	{
		Emitter emit(pos, (float)glfwGetTime());
		m_emitters.push_back(emit);
	}

	void renderParticles (float deltaTime, glm::mat4 projection, glm::mat4 view, Camera *cam)
	{
		m_shader->use();
		m_shader->setMat4("projection", projection);
		m_shader->setMat4("view", view);
		m_shader->setVec3("cameraUp", cam->Up);
		m_shader->setVec3("cameraLeft", -cam->Right);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for (auto& ps : m_emitters)
		{
			updateParticleEmitter(ps, deltaTime);
		}
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
};
#endif // PARTICLE_SYSTEM_H