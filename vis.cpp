#define DEBUG		// used to draw debug triangle, increase point size, ...

#define DRAW_QUADS
//#define DRAW_CUBES
//#define DRAW_MESHES
#define RENDER_LIGHT_SOURCE
#define ENABLE_VSM

#include "camera.h"
#include "frameBuffer3D.h"
#include "mesh.h"
#include "OBJ-Loader/OBJ_Loader.h"
#include "particleSystem.h"
#include "shader.h"
#include "textureLoader.h"
#include "textRenderer.h"
#include "utility.h"
#include "vis.h"

#ifdef DEBUG
#include "glm/gtx/string_cast.hpp" // glm::to_string
#endif

// #define GLFW_INCLUDE_NONE before including GLFW, or include glad bfore including glfw.
#include <GLFW/glfw3.h>

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

unsigned int VAOTriangle = 0, VBOTriangle;
void renderTestTriangle(Shader* shader, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f))
{
	if (VAOTriangle == 0)
	{
		float vertices[] =
		{
			 0,  0, -5,
			-1, -1, -5,
			 1, -1, -5
		};
		glGenVertexArrays(1, &VAOTriangle);
		glBindVertexArray(VAOTriangle);

		glGenBuffers(1, &VBOTriangle);
		glBindBuffer(GL_ARRAY_BUFFER, VBOTriangle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setMat4("model", model);
	//shader->setVec3("color", color);

	glBindVertexArray(VAOTriangle);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// unbind the VAO so other VAO calls won't accidentally modify this VAO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unsigned int VAOQuad = 0, VBOQuad;
void renderQuad()
{
	if (VAOQuad == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);

		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);

		float quadVertices[] =
		{
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &VAOQuad);
		glGenBuffers(1, &VBOQuad);
		glBindVertexArray(VAOQuad);
		glBindBuffer(GL_ARRAY_BUFFER, VBOQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(VAOQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

unsigned int VAOPlane = 0, VBOPlane;
void renderPlane()
{
	if (VAOPlane == 0)
	{
		float planeVertices[] =
		{
			// positions            // normals         // texcoords
			 15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,  15.0f,  0.0f,
			-15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,   0.0f, 15.0f,

			 15.0f, -0.5f,  15.0f,  0.0f, 1.0f, 0.0f,  15.0f,  0.0f,
			-15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,   0.0f, 15.0f,
			 15.0f, -0.5f, -15.0f,  0.0f, 1.0f, 0.0f,  15.0f, 15.0f
		};
		glGenVertexArrays(1, &VAOPlane);
		glBindVertexArray(VAOPlane);

		glGenBuffers(1, &VBOPlane);
		glBindBuffer(GL_ARRAY_BUFFER, VBOPlane);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(VAOPlane);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

unsigned int VAOCube = 0, VBOCube;
void renderCube()
{
	// initialize (if necessary)
	if (VAOCube == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
		};
		glGenVertexArrays(1, &VAOCube);
		glBindVertexArray(VAOCube);

		glGenBuffers(1, &VBOCube);
		glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(VAOCube);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void drawErrors()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << err << std::endl;
	}
}

void Vis::renderText(float delta)
{
	float xOff = 10.0f;
	float yOff = .0f;
	float dY = 20.0f;

	// assure that GL_BLEND is active and polygon mode is set to full
	// TODO: move this to seperate function
	bool activateBlend = !glIsEnabled(GL_BLEND);
	if (activateBlend) glEnable(GL_BLEND);

	// set wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	yOff = .0f;
	m_textRenderer->RenderText(std::to_string(1.0f/delta), WIDTH - 180.0f, yOff += dY, 0.8f, glm::vec3(0.0f, 1.0f, 0.0f));

	xOff = 10.0f;
	yOff = 25.0f;
	m_textRenderer->RenderText("W, A, S, D        move", xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("Cursor Up, Down   yOffset: " + floatToString(m_yOffset), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("Q, E              height scale: " + floatToString(m_heightScale), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("Page Up, Down     displacement steps: " + std::to_string(m_normalSteps), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("+, -              refinement steps:   " + std::to_string(m_refinementSteps), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));

	xOff = WIDTH - 460.0f;
	yOff = 25.0f;
	m_textRenderer->RenderText("R                 auto rotate:   " + std::string(m_rotate ? "on" : "off"), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("P                 wireframe:     " + std::string(m_showLines ? "on" : "off"), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("1, 2              texture:       " + std::string(m_texturesCurrent == &m_texturesBrick ? "brick" : "wood"), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("Space, Backspace  update rate:   " + std::to_string((int)m_particleSystem->updateRate), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));
	m_textRenderer->RenderText("NUM 2, 8          tes. factor:   " + std::to_string((int)m_tesFac), xOff, yOff += dY, 0.5f, glm::vec3(1.0f));

	if (activateBlend) glDisable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showLines) ? GL_LINE : GL_FILL);
}

Mesh* noobPot;
void renderScene(const Shader* shader, int pass)
{
	// cubes, quads or teapots
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 transform;
	float margin = 0.f;
	#if defined(DRAW_QUADS)
		margin = 2.5f;
	#elif defined(DRAW_CUBES)
		margin = 2.f;
	#elif defined(DRAW_MESHES)
		margin = 5.f;
	#endif
	for (int idx = 0; idx < 3; ++idx)
	{
		transform = glm::translate(model, glm::vec3(idx * margin - margin, 1.0f, 0.0f));
		if (idx == 1) transform = glm::rotate(transform, 1.57f, glm::vec3(1.0f, 0.0f, 0.0f));
		else if (idx == 2) transform = glm::rotate(transform, 0.707f, glm::vec3(1.0f, 1.0f, 0.0f));
		if (idx == 1) transform = glm::scale(transform, glm::vec3(0.5f));
		shader->setMat4("model", transform);
		//m_depth->setVec3("color", glm::vec3(idx == 0, idx == 1, idx == 2));
		#if defined(DRAW_CUBES)
			renderCube();
		#elif defined(DRAW_QUADS)
			renderQuad();
		#elif defined(DRAW_MESHES)
			noobPot->draw(GL_TRIANGLES);
		#endif
	}
	// floor
	if (pass != 0)
	{
		shader->setMat4("model", model);
		renderPlane();
	}
}

Vis::Vis()
{
	init();

	m_textRenderer = new TextRenderer(WIDTH, HEIGHT);
	m_textRenderer->Load("fonts/ocraext.ttf", 36);

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices); // 32
}

int Vis::init()
{
	// initialize glfw library
	if (!glfwInit())
		return exitWithError("could not initialize glfw");

	// create openGL window
	if (createWindow() != 0)
		return exitWithError("could not create openGl window");

	// initialize glad with the pointer to the corresponding functions for the drivers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return exitWithError("Failed to initialize GLAD");
	}

	// load textures
	m_texturesBrick = loadTextureBatch("textures/bricks"); // loads bricks.jpg, bricks_normal.jpg and bricks_disp.jpg
	m_texturesWood = loadTextureBatch("textures/wood");
	m_texturesCurrent = &m_texturesBrick;
	m_textureWood = loadTexture("textures/wood.jpg");

	// create depth texture
	glGenTextures(1, &m_depthMap);
	//std::cout << "depthMap: " << depthMap << std::endl;
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	#ifdef ENABLE_VSM
		// R and G with 32 bit floats: stores mean and variance
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// framebuffer for rendering depthMap
	glGenFramebuffers(1, &m_depthMapFBO);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	#ifdef ENABLE_VSM
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_depthMap, 0);
	#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// load models
	objl::Loader loader;
	if (loader.LoadFile("models/noobpot.obj") == false)
	{
		fprintf(stderr, "error loading model!\n");
	}
	std::vector<float> vertices(loader.LoadedVertices.size() * 3);
	size_t idx = 0;
	for (const objl::Vertex& vertex : loader.LoadedVertices)
	{
		vertices[idx++] = vertex.Position.X;
		vertices[idx++] = vertex.Position.Y;
		vertices[idx++] = vertex.Position.Z;
	}
	m_noobPot = new Mesh(vertices.data(), vertices.size() / 3);
	// TODO: move everything into Vis class
	noobPot = m_noobPot;

	// load and compile shaders
	m_density = new Shader("shaders/density");
	m_density->use();
	m_density->setFloat("bufferHeight", m_buffer_dim.y);

	m_marchingCubes = new Shader("shaders/marchingCubes");
	m_marchingCubes->use();
	m_marchingCubes->setVec3("stepDimension", glm::vec3(1 / m_buffer_dim.x, 1 / m_buffer_dim.y, 1 / m_buffer_dim.z));

	m_displacement = new Shader("shaders/displacement");
	m_displacement->use();
	m_displacement->setInt("diffuseMap", (*m_texturesCurrent)[0] - 1);
	m_displacement->setInt("normalMap", (*m_texturesCurrent)[1] - 1);
	m_displacement->setInt("depthMap", (*m_texturesCurrent)[2] - 1);
	m_displacement->setVec3("lightPos", m_lightPos);

	m_depth = new Shader("shaders/depthShader");

	m_lighting = new Shader("shaders/lightingShader");
	m_lighting->use();
	m_lighting->setInt("diffuseTexture", 0);
	m_lighting->setInt("shadowMap", 1);

	m_terrain = new Shader("shaders/terrain");

	m_shader = new Shader("shaders/simpleShader");

	// init camera to be able to move around the scene
	m_cam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	// set up vertex data (and buffer(s)) and configure vertex attributes
	m_frameBuffer = new FrameBuffer3D(m_buffer_dim); // holds 3D texture to generate density

	// setup the initial mesh for the rocks
	std::vector<float> vectorArrayRocksCoords;
	for (float i = 0; i < m_buffer_dim.x; i++)
	{
		for (float j = 0; j < m_buffer_dim.y; j++)
		{
			for (float k = 0; k < m_buffer_dim.z; k++)
			{
				vectorArrayRocksCoords.push_back(i);
				vectorArrayRocksCoords.push_back(j);
				vectorArrayRocksCoords.push_back(k);
			}
		}
	}
	m_meshRocks = new Mesh(vectorArrayRocksCoords.data(), m_count_voxel);

	float quadSliceCorners[] =
	{
		-1, -1, 0,
		 1, -1, 0,
		 1,  1, 0,
		 1,  1, 0,
		-1,  1, 0,
		-1, -1, 0
	};
	m_meshTriangle = new Mesh(quadSliceCorners, 6);

	m_particleSystem = new ParticleSystem();

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, (m_showLines) ? GL_LINE : GL_FILL);

	// set background color
	glClearColor(.0f, .0f, .0f, 1.0f);

	return 0;
}

int Vis::createWindow()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set all the required options for GLFW
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "SPG", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		return exitWithError("could not initialize glfw window");
	}

	// Make the window's context current and set callbacks
	glfwMakeContextCurrent(m_window);
	glfwSetWindowPos(m_window, 250, 25);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // enables mouse capture
	// --------------------------------------------------------------------------------------------------------
	// problem:
	// https://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback
	// You cannot use regular methods as callbacks, as GLFW is a C library and doesn’t know about objects and this pointers.
	// If you wish to receive callbacks to a C++ object, use static methods or regular functions as callbacks
	//
	// 1. use static callback
	//glfwSetFramebufferSizeCallback(_window, &Vis::framebuffer_size_callback);
	//	--> drawback: cannot access member variables
	// --------------------------------------------------------------------------------------------------------
	// 2. declare lambda function and bound to callback, providing the implementations:
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	});
	//	--> can be a lot of code thus not nice encapsulated
	// --------------------------------------------------------------------------------------------------------
	// 3. declare lamda function and delegate to implementation
	glfwSetWindowUserPointer(m_window, this); // set window handle to this

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos);
	});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->click_callback(window, button, action, mods);
	});

	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
	});

	return 0;
}

void Vis::display()
{
	float currentFrame;
	float deltaTime;
	int idx = 0, pass = 0;
	glm::mat4 projection, view, model, transform;
	glm::mat4 lightProjection, lightView, lightSpace;

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(m_window))
	{
		// per-frame time logic
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;

#ifdef DEBUG
		drawErrors();
#endif

		processInput(deltaTime);

		// pass projection matrix and camera/view transform to shaders
		projection = m_cam->GetProjectionMatrix();
		view = m_cam->GetViewMatrix();
		model = glm::mat4(1.0f);

		if (m_rotate)
		{
			// change light position over time
			//m_lightPos.x = sin(currentFrame * m_cam->MovementSpeed * 0.1f) * 10.0f;
			//m_lightPos.z = cos(currentFrame * m_cam->MovementSpeed * 0.1f) * 10.0f; // rotate around y
			m_lightPos.x = sin(currentFrame) * 3.0f;
			m_lightPos.z = cos(currentFrame) * 2.0f;
			m_lightPos.y = 4.0 + cos(currentFrame) * 1.0f;
		}
		//std::cout << "light position: " << glm::to_string(m_lightPos) << std::endl;

		#ifdef ENABLE_VSM
			lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, NEAR, FAR);
		#else
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
		#endif
		lightView = glm::lookAt(m_lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpace = lightProjection * lightView;

		// shadow mapping -> render scene twice, saving depth to shadow map, then actual geometry
		for (pass = 0; pass < 2; ++pass)
		{
			if (pass == 0) // render scene from light's point of view
			{
				m_depth->use();
				m_depth->setMat4("lightSpace", lightSpace);

				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
				glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
				glClear(GL_COLOR_BUFFER_BIT);

				//glActiveTexture(GL_TEXTURE1);
				//glBindTexture(GL_TEXTURE_2D, m_depthMap);

				renderScene(m_depth, pass);
			}
			else // render scene using actual lighting and shadows
			{
				m_lighting->use();
				m_lighting->setMat4("projection", projection);
				m_lighting->setMat4("view", view);
				// set light uniforms
				m_lighting->setVec3("viewPos", m_cam->Position);
				m_lighting->setMat4("lightSpaceMatrix", lightSpace);
				m_lighting->setVec3("lightPos", m_lightPos);

				glViewport(0, 0, WIDTH, HEIGHT);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_textureWood);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_depthMap);

				renderScene(m_lighting, pass);
			}
		}

#ifdef RENDER_LIGHT_SOURCE
		// render light source as quad
		transform = glm::translate(model, m_lightPos);
		m_shader->use();
		m_shader->setMat4("projection", projection);
		m_shader->setMat4("view", view);
		m_shader->setVec3("color", glm::vec3(1, 1, 1));
		m_shader->setMat4("model", transform);
		renderQuad();
#endif

		// generate terrain by rendering into 3D texture
		m_frameBuffer->use();
		m_density->use();
		m_density->setFloat("yOffset", m_yOffset); // adjust with cursor up, down
		for (idx = 0; idx < m_buffer_dim.y; idx++)
		{
			// create one layer per step -> each layer is made up of a rectangle of vertices
			m_density->setInt("layer", idx);
			m_meshTriangle->draw(GL_TRIANGLES);

			m_frameBuffer->setLayer(idx);
		}

		glViewport(0, 0, WIDTH, HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render created geometry
		m_marchingCubes->use();
		m_marchingCubes->setMat4("projection", projection);
		m_marchingCubes->setMat4("view", view);
		transform = glm::translate(model, glm::vec3(0.0f, 50.0f, 0.0f));
		m_marchingCubes->setMat4("model", glm::rotate(transform, 1.57f, glm::vec3(1.0f, 0.0f, 0.0f)));

		m_meshRocks->draw(GL_POINTS);

		// render parallax-mapped quad
		m_displacement->use();
		m_displacement->setMat4("projection", projection);
		m_displacement->setMat4("view", view);
		transform = glm::translate(model, glm::vec3(-5.0f, 2.0f, 10.0f));
		if (m_rotate) // rotate the quad to show parallax mapping from multiple directions
			m_quadRotation = glm::radians(currentFrame * -10.0f);
		m_displacement->setMat4("model", glm::rotate(transform, m_quadRotation, glm::vec3(1.0, 0.0, 1.0)));
		m_displacement->setVec3("viewPos", m_cam->Position);
		m_displacement->setFloat("heightScale", m_heightScale);			// adjust with Q, E
		m_displacement->setInt("normalSteps", m_normalSteps);			// adjust with Page up, down
		m_displacement->setInt("refinementSteps", m_refinementSteps);	// adjust with +, -
		for (int texture : *m_texturesCurrent)
		{
			glActiveTexture(GL_TEXTURE0 + texture - 1);
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		renderQuad();

		// particle system
		m_particleSystem->renderParticles(deltaTime, projection, view, m_cam);

		// fun with tesselation
		m_terrain->use();
		m_terrain->setMat4("projection", projection);
		m_terrain->setMat4("view", view);
		transform = glm::translate(model, glm::vec3(0.0f, 7.0f, -12.0f));
		m_terrain->setMat4("model", glm::scale(transform, glm::vec3(5)));
		m_terrain->setFloat("tessellation_factor", m_tesFac);
		// like renderQuad() but with patches
		glBindVertexArray(VAOQuad);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawArrays(GL_PATCHES, 0, 6);
		glBindVertexArray(0);

		// Text HUD
		renderText(deltaTime);

		// swap buffers and poll IO events
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	glfwTerminate();
	//return EXIT_SUCCESS;
}

int Vis::exitWithError(std::string code)
{
	std::cout << "error: " << code << std::endl;
	return EXIT_FAILURE;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Vis::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static bool firstMouse = true;
	if (firstMouse)
	{
		m_lastX = (float)xpos;
		m_lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)(xpos - m_lastX);
	float yoffset = (float)(m_lastY - ypos); // reversed since y-coordinates go from bottom to top

	m_lastX = (float)xpos;
	m_lastY = (float)ypos;

	m_cam->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: This function sets the mouse button callback of the specified window, which is called when a mouse button is pressed or released.
// ----------------------------------------------------------------------
void Vis::click_callback(GLFWwindow* window, int button, int action, int mods)
{
	// shoots raycast to spawn particle emitter
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// TODO: calc depending on cam front dir
		//glm::vec3 pos += -m_cam->Front * glm::vec3(0, 0, 10);
		glm::vec3 pos = m_cam->Position + glm::vec3(0, 0, 10);
		m_particleSystem->createEmitter(pos);
	}
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Vis::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_cam->ProcessMouseScroll((float)yoffset);
}

// The callback function receives the keyboard key, platform-specific scancode, key action and modifier bits.
// ----------------------------------------------------------------------
void Vis::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_P) // toggle wireframe view
		{
			glPolygonMode(GL_FRONT_AND_BACK, (m_showLines ^= true) ? GL_LINE : GL_FILL);
			std::cout << "wireframe mode: " << m_showLines << std::endl;
		}
		else if (key == GLFW_KEY_R) // enables rotate displacement tile
		{
			std::cout << "rotation " << ((m_rotate ^= true) ? "enabled" : "disabled") << std::endl;
		}
		else if (key == GLFW_KEY_1 || key == GLFW_KEY_2) // swaps current textures for displacement tile
		{
			m_texturesCurrent = (key == GLFW_KEY_1) ? &m_texturesBrick : &m_texturesWood;

			m_displacement->use();
			m_displacement->setInt("diffuseMap", (*m_texturesCurrent)[0] - 1);
			m_displacement->setInt("normalMap", (*m_texturesCurrent)[1] - 1);
			m_displacement->setInt("depthMap", (*m_texturesCurrent)[2] - 1);
		}
		else if (key == GLFW_KEY_SPACE)
		{
			m_particleSystem->updateRate++;
		}
		else if (key == GLFW_KEY_BACKSPACE)
		{
			m_particleSystem->updateRate--;
		}
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Vis::processInput(float delta)
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, true);

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		m_cam->ProcessKeyboard(FORWARD, delta);
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		m_cam->ProcessKeyboard(BACKWARD, delta);
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		m_cam->ProcessKeyboard(LEFT, delta);
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		m_cam->ProcessKeyboard(RIGHT, delta);
	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
		m_yOffset += 0.1f;
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		m_yOffset -= 0.1f;
	if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
		m_heightScale = std::max(m_heightScale - 0.005f, -1.0f);
	if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
		m_heightScale = std::min(m_heightScale + 0.005f, 1.0f);
	if (glfwGetKey(m_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		++m_normalSteps;
	if (glfwGetKey(m_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		m_normalSteps = std::max(--m_normalSteps, 1);
	if (glfwGetKey(m_window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		++m_refinementSteps;
	if (glfwGetKey(m_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		m_refinementSteps = std::max(--m_refinementSteps, 1);
	if (glfwGetKey(m_window, GLFW_KEY_KP_8) == GLFW_PRESS)
		m_tesFac += 1.0f;
	if (glfwGetKey(m_window, GLFW_KEY_KP_2) == GLFW_PRESS)
		m_tesFac = std::max(m_tesFac - 1.0f, 1.0f);
}