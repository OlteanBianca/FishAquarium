#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stdlib.h>
//#include <stb_image.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#define GLM_FORCE_CTOR_INIT 
#include <glm.hpp>
#include <GL/freeglut.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include <filesystem>
#include <iostream>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 700;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

Camera camera(glm::vec3(0.0f, 1.0f, 12.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<std::pair<unsigned int, std::string>>cubeTextures = { {1,"topTexture"},{2,"middleTexture"},{3,"baseTexture"} };
std::vector<std::pair<unsigned int, std::string>>fishTextures = { {4,"fish1"},{5,"fish2"},{6,"fish3"},{7,"fish4"},{8,"fish5"} };
unsigned int bubblesTexture;
std::vector<std::pair<unsigned int, std::string>>decoTextures = { {10,"grass"},{11,"blueLeaf"},{12,"yellowLeaf"},{13,"redCoralReef"},{14,"sweetGrass"}, {15,"blueCoralReef"},{16,"pinkCoralReef"} };
unsigned int rocksTexture, rockTexture2;

unsigned int base_VBO, base_VAO;
unsigned int middle_VBO, middle_VAO;
unsigned int top_VBO, top_VAO;
unsigned int fish_VBO, fish_VAO;
unsigned int grass_VBO, grass_VAO;
unsigned int bubbles_VBO, bubbles_VAO;
unsigned int rock_VBO, rock_VAO;
unsigned int depthMapFBO,depthMap;

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

void Texture(std::string strExePath, unsigned int& texture)
{
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(strExePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void GenerateVertexAndBind(unsigned int& VAO, unsigned int& VBO, int size)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	if (size == 8)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}
}

void DrawFish()
{
	float fish[] = {

		//head
		 0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  
		 0.3f,  0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b 
		 0.3f,  0.0f,  0.2f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // l 

		 0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  
		 0.3f, -0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c 
		 0.3f,  0.0f,  0.2f, 0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // l 

		 0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  	
		 0.3f,  0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b
		 0.3f,  0.0f, -0.2f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // m 

		 0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  
		 0.3f, -0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c
		 0.3f,  0.0f, -0.2f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // m 

		//body
		 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d   
		 0.3f,  0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b  
		 0.3f,  0.0f,  0.2f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // l 

		 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d  
		 0.3f, -0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c  
		 0.3f,  0.0f,  0.2f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // l 

		 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d   
		 0.3f,  0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b
		 0.3f,  0.0f, -0.2f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // m 

		 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d  
		 0.3f, -0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c
		 0.3f,  0.0f, -0.2f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // m 

		 //tail
		 1.5f,   0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // e   
		 1.0f,   0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // d  
		 1.3f,  0.04f,  0.0f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // f 

		 1.0f,   0.0f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d   
		 1.5f,  -0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // g 
		 1.3f, -0.04f,  0.0f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // n 

		 //top fin  
		 0.8f,  0.4f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // h  
		 0.3f,  0.2f,  0.0f, 0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b  
		 0.6f,  0.2f,  0.0f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // i 

		 //bottom fin 
		 0.8f, -0.4f,  0.0f, 0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // j   
		 0.3f, -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // c   
		 0.6f, -0.2f,  0.0f,0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // k 
	};

	GenerateVertexAndBind(fish_VAO, fish_VBO, 8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fish), fish, GL_STATIC_DRAW);

}

void DrawDeco()
{
	float grass[] = {
		-0.5f,  0.5f,  0.0f,  0.0f, 0.0f, //a
		-0.5f, -0.5f,  0.0f,  0.0f, 1.0f, //b
		 0.5f, -0.5f,  0.0f,  1.0f, 1.0f, //c

		-0.5f,  0.5f,  0.0f,  0.0f, 0.0f, //a
		 0.5f, -0.5f,  0.0f,  1.0f, 1.0f, //c
		 0.5f,  0.5f,  0.0f,  1.0f, 0.0f  //d
	};

	float bubbles[] =
	{
		-0.10f,  0.10f,  0.0f,  0.0f, 0.0f,
		-0.10f, -0.10f,  0.0f,  0.0f, 1.0f,
		 0.10f, -0.10f,  0.0f,  1.0f, 1.0f,

		-0.10f,  0.10f,  0.0f,  0.0f, 0.0f,
		 0.10f, -0.10f,  0.0f,  1.0f, 1.0f,
		 0.10f,  0.10f,  0.0f,  1.0f, 0.0f
	};

	GenerateVertexAndBind(grass_VAO, grass_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grass), &grass, GL_STATIC_DRAW);

	GenerateVertexAndBind(bubbles_VAO, bubbles_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bubbles), &bubbles, GL_STATIC_DRAW);
}

void DrawRocks()
{
	float rocks[] = {
		0.1f, -0.45f, 0.1f, 0.0f, 0.0f,   // a  
		0.4f, -0.65f, 0.1f, 1.0f, 0.0f,   // b 
		0.4f, -0.45f, 0.3f, 1.0f, 1.0f,   // l 

		0.1f, -0.45f, 0.1f, 0.0f, 0.0f,   // a  
		0.4f, -0.65f, 0.1f, 1.0f, 0.0f,   // c 
		0.4f, -0.45f, 0.3f, 1.0f, 1.0f,   // l 

		0.1f, -0.45f, 0.1f, 0.0f, 0.0f,   // a  	
		0.4f, -0.25f, 0.1f, 1.0f, 0.0f,   // b
		0.4f, -0.45f, -0.1f, 1.0f, 1.0f,   // m 

		0.1f, -0.45f, 0.1f, 0.0f, 0.0f,   // a  
		0.4f, -0.65f, 0.1f, 1.0f, 0.0f,   // c
		0.4f, -0.45f, -0.1f, 1.0f, 1.0f,   // m 

		0.4f, -0.45f, 0.3f, 0.0f, 1.0f,   // l 
		0.4f, -0.25f, 0.1f, 1.0f, 0.0f,   // b
		0.4f, -0.45f, -0.1f, 1.0f, 1.0f,   // m 

		0.4f, -0.45f, -0.1f, 0.0f, 1.0f,   // m 
		0.4f, -0.65f, 0.1f, 1.0f, 0.0f,   // c
		0.4f, -0.45f, 0.3f, 1.0f, 1.0f,   // l
	};
	GenerateVertexAndBind(rock_VAO, rock_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rocks), rocks, GL_STATIC_DRAW);

}

void DrawAquarium()
{
	float top_cube[] = {

		//back
		-4.5f,  2.5f, -2.5f,    0.0f, 0.0f,   // 4  left down
		 4.5f,  2.5f, -2.5f,    1.0f, 0.0f,   // 3  right down
		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12 right top

		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12
		-4.5f,  3.0f, -2.5f,    0.0f, 1.0f,   // 13 left top
		-4.5f,  2.5f, -2.5f,    0.0f, 0.0f,   // 4

		//front
		-4.5f,  2.5f,  2.5f,    0.0f, 0.0f,   // 9  left down
		 4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 7  right down
		 4.5f,  3.0f,  2.5f,    1.0f, 1.0f,   // 11 right top

		 4.5f,  3.0f,  2.5f,    1.0f, 1.0f,   // 11
		-4.5f,  3.0f,  2.5f,    0.0f, 1.0f,   // 10 left top
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9

		//left
		-4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 10
		-4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 13
		-4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 4

		-4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 4
		-4.5f,  2.5f,  2.5f,    0.0f, 0.0f,   // 9
		-4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 10

		//right
		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11
		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12
		 4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 3

		 4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 3
		 4.5f,  2.5f,  2.5f,    0.0f, 0.0f,   // 7
		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11

		 //base
		-4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 4
		 4.5f,  2.5f, -2.5f,    1.0f, 1.0f,   // 3
		 4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 7

		 4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 7
		-4.5f,  2.5f,  2.5f,    0.0f, 0.0f,   // 9
		-4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 4

		//top
		-4.5f,  3.0f, -2.5f,    0.0f, 1.0f,   // 13
		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12
		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11

		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11
		-4.5f,  3.0f,  2.5f,    0.0f, 0.0f,   // 10
		-4.5f,  3.0f, -2.5f,    0.0f, 1.0f    // 13
	};

	float middle_cube[] = {

		//back
		-4.5f, -0.5f, -2.5f,    0.0f, 0.0f,   // 4  left down
		 4.5f, -0.5f, -2.5f,    1.0f, 0.0f,   // 3  right down
		 4.5f,  2.5f, -2.5f,    1.0f, 1.0f,   // 12 right top

		 4.5f,  2.5f, -2.5f,    1.0f, 1.0f,   // 12
		-4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 13 left top
		-4.5f, -0.5f, -2.5f,    0.0f, 0.0f,   // 4

		//front
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9  left down
		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7  right down
		 4.5f,  2.5f,  2.5f,    1.0f, 1.0f,   // 11 right top

		 4.5f,  2.5f,  2.5f,    1.0f, 1.0f,   // 11
		-4.5f,  2.5f,  2.5f,    0.0f, 1.0f,   // 10 left top
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9

		//left
		-4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 10
		-4.5f,  2.5f, -2.5f,    1.0f, 1.0f,   // 13
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4

		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9
		-4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 10

		//right
		 4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 11
		 4.5f,  2.5f, -2.5f,    1.0f, 1.0f,   // 12
		 4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 3

		 4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 3
		 4.5f, -0.5f,  2.5f,    0.0f, 0.0f,   // 7
		 4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 11

		 //base
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
		 4.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7

		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4

		//top
		-4.5f,  2.5f, -2.5f,    0.0f, 1.0f,   // 13
		 4.5f,  2.5f, -2.5f,    1.0f, 1.0f,   // 12
		 4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 11

		 4.5f,  2.5f,  2.5f,    1.0f, 0.0f,   // 11
		-4.5f,  2.5f,  2.5f,    0.0f, 0.0f,   // 10
		-4.5f,  2.5f, -2.5f,    0.0f, 1.0f    // 13
	};

	float  bottom_cube[] = {

		//back
		-4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 1  left down
		 4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 2  right down
		 4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3  right top

		 4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3
		-4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 4  left top
		-4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // 1

		//front
		-4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 5  left down
		 4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 6  right down
		 4.5f, -0.5f,  2.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // 7  right top

		 4.5f, -0.5f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 7
		-4.5f,  0.2f,  2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 9  left top
		-4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // 5

		//left
		-4.5f,  0.2f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 4
		-4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 1

		-4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 1
		-4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 5
		-4.5f,  0.2f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 9

		//right
		 4.5f, -0.5f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 7
		 4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3
		 4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 2

		 4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 2
		 4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 6
		 4.5f, -0.5f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 7

		 //base
		-4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 1
		 4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // 2
		 4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 6

		 4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 6
		-4.5f, -1.0f,  2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 5
		-4.5f, -1.0f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 1

		//top
		-4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,     0.0f, 1.0f,   // 4
		 4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3
		 4.5f, -0.5f,  2.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 7

		 4.5f, -0.5f,  2.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 7
		-4.5f,  0.2f,  2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 4
	};
	//base cube
	GenerateVertexAndBind(base_VAO, base_VBO, 8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bottom_cube), bottom_cube, GL_STATIC_DRAW);

	//middle cube
	GenerateVertexAndBind(middle_VAO, middle_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(middle_cube), middle_cube, GL_STATIC_DRAW);

	//top cube
	GenerateVertexAndBind(top_VAO, top_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(top_cube), top_cube, GL_STATIC_DRAW);
}

void Rotate(Shader& decoShader, glm::mat4& model, unsigned int texture, unsigned int numberOfRotations, unsigned int value)
{
	const float pi = 3.1415;
	for (int i = 0; i < numberOfRotations; i++)
	{
		model = glm::rotate(model, glm::radians(i * value * pi), glm::vec3(0.f, 1.f, 0.f));
		decoShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void DrawBubbles(Shader& decoShader, unsigned int texture, glm::vec3 position, float& Movement, float& Increment)
{
	glBindVertexArray(bubbles_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	Movement += Increment;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position.x, Movement, position.z));

	if (Movement >= 2.85) Movement = position.y;

	//decoShader.use();
	decoShader.setMat4("model", model);
	Rotate(decoShader, model, texture, 200, 1);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void RenderFish(Shader& shader, Shader& decoShader)
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 bubbles = glm::mat4(1.0f);

	static glm::mat4 models[] =
	{
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f)
	};

	static glm::vec2 incrementAndMovement[] = {
		glm::vec2(0.019,1.9),
		glm::vec2(0.014,-1.9),
		glm::vec2(0.013,-1.5),
		glm::vec2(0.014,2.5),
		glm::vec2(0.013,-1.2)
	};

	glm::vec3 fishPosition[] = {
		glm::vec3(incrementAndMovement[0].y - incrementAndMovement[0].x,1.3f,0.0f),
		glm::vec3(incrementAndMovement[1].y - incrementAndMovement[1].x,2.0f,-1.5f),
		glm::vec3(incrementAndMovement[2].y - incrementAndMovement[2].x,1.0f,1.5f),
		glm::vec3(incrementAndMovement[3].y - incrementAndMovement[3].x,0.5f,-1.5f),
		glm::vec3(incrementAndMovement[4].y - incrementAndMovement[4].x,2.0f,1.5f)
	};

	static std::vector<float> bubbleMovement{ fishPosition[0].y,fishPosition[1].y,fishPosition[2].y,fishPosition[3].y,fishPosition[4].y };
	static std::vector<float> bubbleIncrement{ 0.003,0.003,0.007,0.008,0.005 };
	std::vector<bool>createAnotherBubble{ false,false,false,false,false };

	for (int index = 0; index < 5; index++)
	{
		models[index] = glm::translate(model, fishPosition[index]);

		incrementAndMovement[index].y -= incrementAndMovement[index].x;
		fishPosition[index].x -= incrementAndMovement[index].x;

		if ((incrementAndMovement[index].y <= -3.0 || incrementAndMovement[index].y >= 3.0)) {
			incrementAndMovement[index].x *= -1;
		}
		if (incrementAndMovement[index].x < 0) models[index] = glm::rotate(models[index], glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		shader.use();
		shader.setMat4("model", models[index]);

		glBindVertexArray(fish_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fishTextures[index].first);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		decoShader.use();
		DrawBubbles(decoShader, bubblesTexture, fishPosition[index], bubbleMovement[index], bubbleIncrement[index]);
	}
}

void RenderRocks(Shader& shader)
{

	glm::mat4 model = glm::mat4(1.0f);

	static glm::mat4 models[] =
	{
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f)
	};

	glm::vec3 rockPosition[] = {
		glm::vec3(-1.1f,-0.95f,0.1f),
		glm::vec3(1.4f,-0.95f,-1.4f),
		glm::vec3(1.8f,-0.95f,1.4f),
		glm::vec3(3.4f,-0.95f,-1.5f),
		glm::vec3(-3.9f,-0.55f,1.4f),
		glm::vec3(2.9f,-0.95f,2.0f),
		glm::vec3(0.0f,-0.95f,-1.0f),
		glm::vec3(-4.0f,-0.95f,-2.0f),
	};

	for (int index = 0; index < 8; index++)
	{
		models[index] = glm::translate(model, rockPosition[index]);
		models[index] = glm::rotate(models[index], glm::radians(120.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		shader.use();
		shader.setMat4("model", models[index]);

		glBindVertexArray(rock_VAO);
		glActiveTexture(GL_TEXTURE0);
		if (index % 2 == 0)
			glBindTexture(GL_TEXTURE_2D, rocksTexture);
		else
			glBindTexture(GL_TEXTURE_2D, rockTexture2);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

	}
}

void RenderVegetation(Shader& decoShader, glm::mat4& model, glm::vec3 VegetationPosition, unsigned int texture, unsigned int numberOfRotations, unsigned int value)
{
	model = glm::translate(model, VegetationPosition);
	decoShader.setMat4("model", model);
	Rotate(decoShader, model, texture, numberOfRotations, value);
	model = glm::mat4(1.0f);
}

void DrawVegetation(Shader& decoShader, std::pair<unsigned int, std::string>& texture)
{
	glBindVertexArray(grass_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.first);

	glm::mat4 model = glm::mat4(1.0f);

	glm::vec3 Grass[] = {
		glm::vec3(0.0f, -0.2f, 0.0f),
		glm::vec3(2.5f,-0.2f,1.5f),
		glm::vec3(-3.0f,-0.2f,-2.0f),
		glm::vec3(1.0f,-0.2f,-2.0f),
		glm::vec3(-4.0f,0.3f,1.0f),
		glm::vec3(3.2f,-0.2f,-1.0f),
		glm::vec3(-2.0f,0.1f,1.2f),
	};

	glm::vec3 BlueLeafs[] =
	{
		glm::vec3(1.6f,0.0f,-0.3f),
		glm::vec3(-1.5f,0.0f,-1.2f),
		glm::vec3(-3.0f,0.5f,2.0f)
	};

	glm::vec3 YellowLeafs[] =
	{
		glm::vec3(-3.5f,0.1f,-0.5f),
		glm::vec3(3.5f,0.0f,0.75f),
		glm::vec3(-0.5f,0.2f,2.0f)
	};

	glm::vec3 redCoralBubblePositions[] = {
		glm::vec3(0.0f,0.35f,-1.0f),
		glm::vec3(0.3f,0.15f,-1.0f),
		glm::vec3(-0.3f,0.15f,-1.0f)
	};
	glm::vec3 blueCoralBubblePositions[] = {
		glm::vec3(3.0f,0.35f,0.0f),
		glm::vec3(3.0f,0.15f,0.0f),
		glm::vec3(-3.0f,0.15f,0.0f)
	};
	glm::vec3 pinkCoralBubblePositions[] = {
		glm::vec3(-3.0f,0.65f,0.5f),
		glm::vec3(-3.0f,0.45f,0.5f),
		glm::vec3(3.0f,0.45f,0.5f)
	};

	glm::vec3 sweetGrass[] = {
		glm::vec3(4.0f,0.0f,2.0f),
		glm::vec3(4.0f,0.4f,2.0f),
		glm::vec3(4.0f,0.8f,2.0f),
		glm::vec3(4.0f,1.2f,2.0f),

		glm::vec3(2.5f,0.0f,-1.5f),
		glm::vec3(2.5f,0.4f,-1.5f),
		glm::vec3(2.5f,0.8f,-1.5f),
		glm::vec3(2.5f,1.2f,-1.5f),
		glm::vec3(2.5f,1.6f,-1.5f),

		glm::vec3(-2.0f,0.0f,0.0f),
		glm::vec3(-2.0f,0.4f,0.0f),
		glm::vec3(-2.0f,0.8f,0.0f),

	};
	if (texture.second == "grass")
		for (const auto& vector : Grass)
			RenderVegetation(decoShader, model, vector, texture.first, 8, 1);

	if (texture.second == "blueLeaf")
		for (const auto& vector : BlueLeafs)
			RenderVegetation(decoShader, model, vector, texture.first, 5, 1);

	if (texture.second == "yellowLeaf")
		for (const auto& vector : YellowLeafs)
			RenderVegetation(decoShader, model, vector, texture.first, 5, 1);

	if (texture.second == "sweetGrass")
		for (const auto& vector : sweetGrass)
			RenderVegetation(decoShader, model, vector, texture.first, 5, 3);
	if (texture.second == "redCoralReef" || texture.second == "blueCoralReef" || texture.second == "pinkCoralReef")
	{
		static std::vector<float> bubbleMovement{ 0.35f,0.15f,0.15f };
		static std::vector<float> bubbleIncrement{ 0.005,0.005,0.005 };
		if (texture.second == "redCoralReef")
		{
			RenderVegetation(decoShader, model, glm::vec3(0.0f, 0.0f, -1.0f), texture.first, 5, 10);
			for (int index = 0; index < redCoralBubblePositions->length(); ++index)
				DrawBubbles(decoShader, bubblesTexture, redCoralBubblePositions[index], bubbleMovement[index], bubbleIncrement[index]);
		}
		if (texture.second == "blueCoralReef")
		{
			RenderVegetation(decoShader, model, glm::vec3(3.0f, 0.0f, 0.0f), texture.first, 5, 10);
			for (int index = 0; index < blueCoralBubblePositions->length(); ++index)
				DrawBubbles(decoShader, bubblesTexture, blueCoralBubblePositions[index], bubbleMovement[index], bubbleIncrement[index]);
		}
		if (texture.second == "pinkCoralReef")
		{
			RenderVegetation(decoShader, model, glm::vec3(-3.0f, 0.3f, 0.5f), texture.first, 5, 10);
			for (int index = 0; index < pinkCoralBubblePositions->length(); ++index)
				DrawBubbles(decoShader, bubblesTexture, pinkCoralBubblePositions[index], bubbleMovement[index], bubbleIncrement[index]);
		}

	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void CreateAquarium(unsigned int& VAO, std::pair<unsigned int, std::string> texture)
{
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.first);
	if (texture.second == "middleTexture" || texture.second == "topTexture")
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		return;
	}
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void RenderFunction(Shader& cubeShader, Shader& shadow, Shader& decoShader)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	//pt mutare manuala
	glm::mat4 view = camera.GetViewMatrix();
	//glm::mat4 view;
	//float radius = 10.0f;
	//float camx = sin(glfwGetTime()) * radius;
	//float camz = cos(glfwGetTime()) * radius;
	//view = glm::lookAt(glm::vec3(camx, 0.0f, camz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

	cubeShader.use();
	cubeShader.setMat4("model", model);
	cubeShader.setMat4("projection", projection);
	cubeShader.setMat4("view", view);

	//cubul de sus
	CreateAquarium(top_VAO, cubeTextures[0]);

	//cubul din mijloc
	CreateAquarium(middle_VAO, cubeTextures[1]);

	shadow.use();
	shadow.setMat4("model", model);
	shadow.setMat4("projection", projection);
	shadow.setMat4("view", view);

	//cubul de jos
	CreateAquarium(base_VAO, cubeTextures[2]);
	RenderFish(shadow, decoShader);

	decoShader.use();
	decoShader.setMat4("projection", projection);
	decoShader.setMat4("view", view);

	for (int index = 0; index < decoTextures.size(); ++index)
		DrawVegetation(decoShader, decoTextures[index]);

	RenderRocks(decoShader);
}

void DeleteVertexArrayAndBuffer(unsigned int& VAO)
{
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1, &VAO);
}

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fish Aquarium", NULL, NULL);
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::string strFullExeFileName = argv[0];
	std::string strExePath;
	const size_t last_slash_idx = strFullExeFileName.rfind('\\');
	if (std::string::npos != last_slash_idx) {
		strExePath = strFullExeFileName.substr(0, last_slash_idx);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawAquarium();
	DrawFish();
	DrawRocks();
	DrawDeco();

	Texture(strExePath + "\\glass2.jpg", cubeTextures[0].first);
	Texture(strExePath + "\\water1.jpg", cubeTextures[1].first);
	Texture(strExePath + "\\sand.jpg", cubeTextures[2].first);
	Texture(strExePath + "\\scales.jpg", fishTextures[0].first);
	Texture(strExePath + "\\scales2.jpg", fishTextures[1].first);
	Texture(strExePath + "\\scales3.jpg", fishTextures[2].first);
	Texture(strExePath + "\\scales4.jpg", fishTextures[3].first);
	Texture(strExePath + "\\scales5.png", fishTextures[4].first);
	Texture(strExePath + "\\bubbles2.png", bubblesTexture);
	Texture(strExePath + "\\grass.png", decoTextures[0].first);
	Texture(strExePath + "\\blueLeaf.png", decoTextures[1].first);
	Texture(strExePath + "\\yellowLeaf.png", decoTextures[2].first);
	Texture(strExePath + "\\redCoralReef.png", decoTextures[3].first);
	Texture(strExePath + "\\sweetGrass.png", decoTextures[4].first);
	Texture(strExePath + "\\coralReef.png", decoTextures[5].first);
	Texture(strExePath + "\\pinkCoral.png", decoTextures[6].first);

	Texture(strExePath + "\\rock.jpg", rocksTexture);
	Texture(strExePath + "\\rock2.png", rockTexture2);

	Shader cubeShader("topShader.vs", "topShader.fs");
	Shader decoShader("Blending.vs", "Blending.fs");
	Shader shadowMappingShader("ShadowMapping.vs", "ShadowMapping.fs");
	Shader shadowMappingDepthShader("ShadowMappingDepth.vs", "ShadowMappingDepth.fs");

	cubeShader.use();
	cubeShader.setInt("texture1", 0);
	decoShader.use();
	decoShader.setInt("texture1", 0);

	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowMappingShader.use();
	shadowMappingShader.setInt("diffuseTexture", 0);
	shadowMappingShader.setInt("shadowMap", 1);

	glm::vec3 lightPos(7.0f, 3.5f, 0.0f);

	glEnable(GL_CULL_FACE);
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		lightProjection = glm::ortho(-5.0f, 5.0f, 0.2f, 4.5f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, -1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		shadowMappingDepthShader.use();
		shadowMappingDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTextures[2].first);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		RenderFunction(cubeShader, shadowMappingDepthShader, decoShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightPos.x = glm::sin(1.0);
		lightPos.z = glm::cos(1.0);

		shadowMappingShader.use();
		glm::mat4 projection = camera.GetProjectionMatrix();
		glm::mat4 view = camera.GetViewMatrix();
		shadowMappingShader.setMat4("projection", projection);
		shadowMappingShader.setMat4("view", view);

		shadowMappingShader.setVec3("viewPos", camera.GetPosition());
		shadowMappingShader.setVec3("lightPos", lightPos);
		shadowMappingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTextures[2].first);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDisable(GL_CULL_FACE);
		RenderFunction(cubeShader, shadowMappingShader, decoShader);

		processInput(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	DeleteVertexArrayAndBuffer(base_VAO);
	DeleteVertexArrayAndBuffer(top_VAO);
	DeleteVertexArrayAndBuffer(middle_VAO);
	DeleteVertexArrayAndBuffer(fish_VAO);
	DeleteVertexArrayAndBuffer(grass_VAO);
	DeleteVertexArrayAndBuffer(bubbles_VAO);
	DeleteVertexArrayAndBuffer(rock_VAO);

	glfwTerminate();
	return 0;
}
