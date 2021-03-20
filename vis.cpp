#include "camera.h"
#include "FrameBuffer3D.h"
#include "Mesh.h"
#include "shader.h"
#include "vis.h"

// #define GLFW_INCLUDE_NONE before including GLFW, or include glad bfore including glfw.
#include <GLFW/glfw3.h>

#define DEBUG // used to draw debug triangle, increase point size, ...

const int WIDTH = 1024, HEIGHT = 800; // screen dimensions

// ----------------------------------------------------------------------------------------
// TODO:
// - use vec3 for dimensions
// - set camera position
// - integrate window size to class
// ----------------------------------------------------------------------------------------

void renderTestTriangle (Shader *shader)
{
	float vertices[] =
	{
		0, 0, -5,
		-1, -1, -5,
		+1, -1, -5
	};
	unsigned int VAOTriangle, VBOTriangle;
	glGenVertexArrays(1, &VAOTriangle);
	glBindVertexArray(VAOTriangle);

	glGenBuffers(1, &VBOTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, VBOTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	shader->setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// unbind the VAO so other VAO calls won't accidentally modify this VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Vis::Vis ()
{
	init();
}

int Vis::init ()
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

	// load and compile the vertex and fragment shaders
	m_density = new Shader("density.vs", "density.fs");
	m_density->use();
	m_density->setFloat("screenWidth", raster_dim[0]);
	m_density->setFloat("screenHeight", raster_dim[1]);
	m_density->setFloat("bufferHeight", raster_dim[2]);

	m_marchingCubes = new Shader("marchingCubes.vs", "marchingCubes.fs", "marchingCubes.gs");
	m_marchingCubes->use();
	m_marchingCubes->setFloat("bufferwidth", raster_dim[0]);
	m_marchingCubes->setFloat("bufferheight", raster_dim[1]);
	m_marchingCubes->setFloat("bufferlength", raster_dim[2]);

	m_shader = new Shader("simpleShader.vs", "simpleShader.fs");

	// init camera to be able to move around the scene
	m_cam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	m_frameBuffer = new FrameBuffer3D(raster_dim); // holds 3D texture to generate density

	// setup the initial mesh for the rocks
	std::vector<float> vectorArrayRocksCoords;
	for (int i = 0; i < raster_dim[0]; i++)
	{
		for (int j = 0; j < raster_dim[1]; j++)
		{
			for (int k = 0; k < raster_dim[2]; k++)
			{
				vectorArrayRocksCoords.push_back(i);
				vectorArrayRocksCoords.push_back(j);
				vectorArrayRocksCoords.push_back(k);
			}
		}
	}
	m_meshRocks = new Mesh(vectorArrayRocksCoords.data(), count_voxel);

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

	// create the viewport
	//glViewport(0, 0, WIDTH, HEIGHT);

	// unbind the VAO so other VAO calls won't accidentally modify this VAO
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// set wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, (m_showLines) ? GL_LINE : GL_FILL);

	/*#ifdef DEBUG
		glPointSize(5.0);
	#endif*/

	return 0;
}

int Vis::createWindow ()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set all the required options for GLFW
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		return exitWithError("could not initialize glfw window");
	}

	// Make the window's context current and set callbacks
	glfwMakeContextCurrent(m_window);
	glfwSetWindowPos(m_window, 250, 250);
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

void Vis::display ()
{
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(m_window))
	{
		// per-frame time logic
		float currentFrame = (float)glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(deltaTime);

		// generate terrain by rendering into 3D texture
		// ------------------------------------------------------------
		m_frameBuffer->use();
		m_density->use();
		m_density->setFloat("yOffset", yOffset);
		for (int i = 0; i < raster_dim[1]; i++)
		{
			// Create one layer
			m_density->setInt("layer", i);
			// Each layer is made up of a rectangle of vertices
			m_meshTriangle->draw(GL_TRIANGLES);

			m_frameBuffer->setLayer(i + 1);
		}

		// render 3D texture to screen
		// ------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);

		glClearColor(.0f, .0f, .0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// pass projection matrix and camera/view transform to shader
		glm::mat4 projection = m_cam->GetProjectionMatrix();
		glm::mat4 view = m_cam->GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		#ifdef DEBUG
			m_shader->use();
			m_shader->setMat4("projection", projection);
			m_shader->setMat4("view", view);
			m_shader->setMat4("model", model);
			renderTestTriangle(m_shader);
		#endif

		m_marchingCubes->use();
		m_marchingCubes->setMat4("projection", projection);
		m_marchingCubes->setMat4("view", view);
		m_marchingCubes->setMat4("model", model);

		m_meshRocks->draw(GL_POINTS);
		//std::cout << "paint voxel: " << count_voxel << " from texture: " << m_frameBuffer->getTexture() << std::endl;
		//glActiveTexture(GL_TEXTURE0 + m_frameBuffer->getTexture());
		//glBindTexture(GL_TEXTURE_3D, m_frameBuffer->getTexture());
		//glDrawArrays(GL_POINTS, 0, count_voxel);

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
void Vis::mouse_callback (GLFWwindow* window, double xpos, double ypos)
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Vis::scroll_callback (GLFWwindow* window, double xoffset, double yoffset)
{
	m_cam->ProcessMouseScroll((float)yoffset);
}

// The callback function receives the keyboard key, platform-specific scancode, key action and modifier bits.
// ----------------------------------------------------------------------
void Vis::key_callback (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_P) // toggle wireframe view
		{
			glPolygonMode(GL_FRONT_AND_BACK, (m_showLines ^= true) ? GL_LINE : GL_FILL);
			std::cout << "wireframe mode: " << m_showLines << std::endl;
		}
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Vis::processInput (float delta)
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
		yOffset += 0.1f;
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		yOffset -= 0.1f;
}