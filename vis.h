#pragma once
#ifndef VIS_H
#define VIS_H

struct GLFWwindow;

class Shader;
class Camera;
class FrameBuffer3D;
class Mesh;
class ParticleSystem;
class TextRenderer;

#include <array>
#include <glm/glm.hpp>
#include <iostream>

class Vis
{
private:
	GLFWwindow *m_window;
	Camera *m_cam;

	// text renderer
	TextRenderer *m_textRenderer;

	// shaders
	Shader *m_density;
	Shader *m_marchingCubes;
	Shader *m_displacement;
	Shader *m_shader;
	Shader* m_depth;
	Shader* m_lighting;
	Shader* m_terrain;

	unsigned int m_depthMapFBO;

	bool m_showLines = false;				// toogle between polygon fill and wireframe lines using key 'P'
	bool m_rotate = false;					// rotates displacement squad if enabled
	float m_quadRotation = 0.0f;			// rotation in degrees

	// mouse
	float m_lastX, m_lastY = 0;				// last stored mouse location

	// timing
	float m_lastFrame = 0.0f;

	// rendering
	FrameBuffer3D *m_frameBuffer;
	Mesh *m_meshTriangle;
	Mesh *m_meshRocks;
	Mesh *m_noobPot;

	// lighting
	/*const*/ glm::vec3 m_lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

	// textures
	std::array<int, 3> m_texturesBrick;
	std::array<int, 3> m_texturesWood;
	std::array<int, 3> *m_texturesCurrent;
	unsigned int m_depthMap = 0;
	unsigned int m_textureWood = 0;

	// particles
	ParticleSystem *m_particleSystem;

	int exitWithError (std::string code);
	int init ();
	int createWindow ();

	// opengl callbacks
	void mouse_callback (GLFWwindow* window, double xpos, double ypos);
	void click_callback(GLFWwindow* window, int button, int action, int mods);
	void scroll_callback (GLFWwindow* window, double xoffset, double yoffset);
	void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods);
	void processInput(float delta);

	// marching cubes
	//glm::vec3 buffer_dim = glm::vec3(96, 256, 96);
	const glm::vec3 m_buffer_dim = glm::vec3(96, 96, 96);
	const int m_count_voxel = (int)(m_buffer_dim.x * m_buffer_dim.y * m_buffer_dim.z);
	float m_yOffset = 0;

	// displacement mapping
	float m_heightScale = 0;
	int m_normalSteps = 10;
	int m_refinementSteps = 5;

	// tesselation
	float m_tesFac = 1.0;

	// text rendering
	void renderText(float delta);

public:
	Vis ();
	void display();
};
#endif // VIS_H