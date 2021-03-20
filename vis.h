#pragma once
#ifndef VIS_H
#define VIS_H

struct GLFWwindow;
class Shader;
class Camera;
class FrameBuffer3D;
class Mesh;

#include <glm/glm.hpp>
#include <iostream>

class Vis
{
private:
	GLFWwindow *m_window;
	Camera *m_cam;

	// shaders
	Shader *m_density;
	Shader *m_marchingCubes;
	Shader *m_shader;

	bool m_showLines = false;				// toogle between polygon fill and wireframe lines using key 'P'

	// mouse
	float m_lastX, m_lastY = 0;				// last stored mouse location

	// timing
	float lastFrame = 0.0f;

	// rendering
	FrameBuffer3D *m_frameBuffer;
	Mesh *m_meshTriangle;
	Mesh* m_meshRocks;

	int exitWithError (std::string code);
	int init ();
	int createWindow ();

	// opengl callbacks
	void mouse_callback (GLFWwindow* window, double xpos, double ypos);
	void scroll_callback (GLFWwindow* window, double xoffset, double yoffset);
	void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods);
	void processInput(float delta);

	// marching cubes
	//int raster_dim[3] = { 96, 256, 96 };
	int raster_dim[3] = { 96, 96, 96 };
	int count_voxel = raster_dim[0] * raster_dim[1] * raster_dim[2];
	float yOffset = 0;

public:
	Vis ();
	void display();
};
#endif // VIS_H