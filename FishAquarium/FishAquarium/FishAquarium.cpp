#include <glad/glad.h>
#include <glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glfw3.h>
#include <GL/glut.h>
#include <glm.hpp>
#include <GL/freeglut.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#include <filesystem>
#include <iostream>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int texture1, texture2;
unsigned int base_VBO, base_VAO;
unsigned int top_VBO, top_VAO;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, (float)deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Texture(std::string strExePath)
{
    glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load((strExePath + "\\sand.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	data = stbi_load((strExePath + "\\water1.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void drawCube()
{
	float top_cube[] = {
		
		//back
		-1.5f, -0.5f, -2.5f,    0.0f, 0.0f,   // 1  left down
		 1.5f, -0.5f, -2.5f,    1.0f, 0.0f,   // 2  right down
		 1.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 3  right top

		 1.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 3
		-1.5f,  3.0f, -2.5f,    0.0f, 1.0f,   // 4  left top
		-1.5f, -0.5f, -2.5f,    0.0f, 0.0f,   // 1

		//front
		-1.5f,  0.1f,  2.5f,    0.0f, 0.0f,   // 9  left down
		 1.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 6  right down
		 1.5f,  3.0f,  2.5f,    1.0f, 1.0f,   // 7  right top

		 1.5f,  3.0f,  2.5f,    1.0f, 1.0f,   // 7
		-1.5f,  3.0f,  2.5f,    0.0f, 1.0f,   // 8  left top
		-1.5f,  0.1f,  2.5f,    0.0f, 0.0f,   // 9

		//left
		-1.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 8
		-1.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 4
		-1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 1

		-1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 1
		-1.5f,  0.1f,  2.5f,    0.0f, 0.0f,   // 9
		-1.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 8

		//right
		 1.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 7
		 1.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 3
		 1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 2

		 1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 2
		 1.5f, -0.5f,  2.5f,    0.0f, 0.0f,   // 6
		 1.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 7

		 //base
		-1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 1
		 1.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 2
		 1.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 6

		 1.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 6
		-1.5f,  0.1f,  2.5f,    0.0f, 0.0f,   // 9
		-1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 1

		//top
		-1.5f,  3.0f, -2.5f,    0.0f, 1.0f,   // 4
		 1.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 3
		 1.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 7

		 1.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 7
		-1.5f,  3.0f,  2.5f,    0.0f, 0.0f,   // 8
		-1.5f,  3.0f, -2.5f,    0.0f, 1.0f    // 4
	};

	float  bottom_cube[] = {

		//back
		-1.5f, -1.0f, -2.5f,    0.0f, 0.0f,   // 1  left down
		 1.5f, -1.0f, -2.5f,    1.0f, 0.0f,   // 2  right down
		 1.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3  right top

		 1.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		-1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4  left top
		-1.5f, -1.0f, -2.5f,    0.0f, 0.0f,   // 1

		//front
		-1.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5  left down
		 1.5f, -1.0f,  2.5f,    1.0f, 0.0f,   // 6  right down
		 1.5f, -0.5f,  2.5f,    1.0f, 1.0f,   // 7  right top

		 1.5f, -0.5f,  2.5f,    1.0f, 1.0f,   // 7
		-1.5f,  0.1f,  2.5f,    0.0f, 1.0f,   // 9  left top
		-1.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5

		//left
		-1.5f,  0.1f,  2.5f,    1.0f, 0.0f,   // 9
		-1.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 4
		-1.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1

		-1.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1
		-1.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5
		-1.5f,  0.1f,  2.5f,    1.0f, 0.0f,   // 9

		//right
		 1.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7
		 1.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		 1.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 2

		 1.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 2
		 1.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 6
		 1.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7

		 //base
		-1.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1
		 1.5f, -1.0f, -2.5f,    1.0f, 1.0f,   // 2
		 1.5f, -1.0f,  2.5f,    1.0f, 0.0f,   // 6

		 1.5f, -1.0f,  2.5f,    1.0f, 0.0f,   // 6
		-1.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5
		-1.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1

		//top
		-1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
		 1.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		 1.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7

		 1.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7
		-1.5f,  0.1f,  2.5f,    0.0f, 0.0f,   // 9
		-1.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
	};

	//base cube
	glGenVertexArrays(1, &base_VAO);
	glGenBuffers(1, &base_VBO);
	glBindVertexArray(base_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, base_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bottom_cube), bottom_cube, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	//top cube
	glGenVertexArrays(1, &top_VAO);
	glGenBuffers(1, &top_VBO);
	glBindVertexArray(top_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, top_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(top_cube), top_cube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void RenderFunction(Shader& baseShader, Shader& topShader)
{
	glm::vec3 cubePositions[] = {
	   glm::vec3(0.0f, -0.5f, 0.0f),
	   glm::vec3(0.0f, -0.5f, 0.0f),
	};

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	//pt mutare manuala
	//glm::mat4 view = camera.GetViewMatrix();   
	glm::mat4 view;
	float radius = 10.0f;
	float camx = sin(glfwGetTime()) * radius;
	float camz = cos(glfwGetTime()) * radius;
	view = glm::lookAt(glm::vec3(camx, 0.0f, camz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 model = glm::mat4(1.0f);
	float angle = 0;
    model = glm::translate(model, cubePositions[0]);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	
	//al doilea cub
	topShader.use();
	topShader.setMat4("model", model);
	topShader.setMat4("view", view);
	topShader.setMat4("projection", projection);

	glBindVertexArray(top_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	//primul cub
    baseShader.use();
	baseShader.setMat4("model", model);
    baseShader.setMat4("projection", projection);
	baseShader.setMat4("view", view);

	glBindVertexArray(base_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::string strFullExeFileName = argv[0];
	std::string strExePath;
	const size_t last_slash_idx = strFullExeFileName.rfind('\\');
	if (std::string::npos != last_slash_idx) {
		strExePath = strFullExeFileName.substr(0, last_slash_idx);
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	drawCube();

	Shader baseShader("baseShader.vs", "baseShader.fs");
	Shader topShader("topShader.vs", "topShader.fs");

	Texture(strExePath);
	baseShader.use();
	baseShader.setInt("texture1", 0);
	
	topShader.use();
	topShader.setInt("cube", 0);
	topShader.setInt("texture1", 0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		RenderFunction(baseShader, topShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &base_VAO);
	glDeleteVertexArrays(1, &top_VAO);
	glDeleteBuffers(1, &base_VBO);
	glDeleteBuffers(1, &top_VAO);

	glfwTerminate();
	return 0;
}
