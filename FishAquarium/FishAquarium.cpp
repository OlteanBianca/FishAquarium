#include <glad/glad.h>
#include <glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stdlib.h>
#include <stb_image.h>
#include <GL/glut.h>

#define GLM_FORCE_CTOR_INIT 
#include <glm.hpp>
#include <GL/freeglut.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 900;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

Camera camera(glm::vec3(0.0f, 1.0f, 12.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<std::pair<unsigned int, std::string>> cubeTextures = {
	{1,"cube_top.jpg"},
	{2,"cube_middle.jpg"},
	{3,"cube_base.jpg"},
	{4,"cube_support.jpg"},
	{5,"cube_table.png"}
};

std::vector<std::pair<unsigned int, std::string>> fishTextures = {
	{6,"fish1.jpg"},
	{7,"fish2.jpg"},
	{8,"fish3.jpg"},
	{9,"fish4.jpg"},
	{10,"fish5.png"}
};

std::vector<std::pair<unsigned int, std::string>> plantsTextures = {
	{10,"deco_grass.png"},
	{11,"deco_blueLeaf.png"},
	{12,"deco_yellowLeaf.png"},
	{13,"deco_redCoralReef.png"},
	{14,"deco_sweetGrass.png"},
	{15,"deco_blueCoralReef.png"},
	{16,"deco_pinkCoralReef.png"},
	{16,"deco_greenLeaf.png"},
	{17,"deco_leaves.png"},
	{27,"deco_plant.png"},
	{28,"deco_plant2.png"},
	{29,"deco_plant3.png"},
	{30,"deco_plant4.png"},
};

std::vector<std::pair<unsigned int, std::string>> decoTextures = {
	{17,"rock.jpg"},
	{18,"rock2.png"},
	{19,"bubbles.png"},
	{20,"lamp.jpeg"},
	{31,"castle.png"},
};

std::vector<std::pair<unsigned int, std::string>> faces =
{
	{21,"room_ceiling_floor.jpg"},
	{22,"room_ceiling_floor.jpg"},
	{23,"room_backwall.png"},
	{24,"room_window.png"},
	{25,"room_windows.png"},
	{26,"room_door.png"},
};

unsigned int back_and_front_faces_VBO, back_and_front_faces_VAO;
unsigned int left_and_right_faces_VBO, left_and_right_faces_VAO;
unsigned int base_and_top_faces_VBO, base_and_top_faces_VAO;
unsigned int base_VBO, base_VAO;
unsigned int middle_VBO, middle_VAO;
unsigned int top_VBO, top_VAO;
unsigned int support_VBO, support_VAO;
unsigned int column_VBO, column_VAO;
unsigned int table_VBO, table_VAO;
unsigned int tableSupport_VBO, tableSupport_VAO;
unsigned int fish_VBO, fish_VAO;
unsigned int grass_VBO, grass_VAO;
unsigned int bubbles_VBO, bubbles_VAO;
unsigned int rock_VBO, rock_VAO;
unsigned int lamp_VBO, lamp_VAO;
unsigned int light_VBO, light_VAO;
unsigned int depthMapFBO, depthMap;
unsigned int castle_VBO, castle_VAO;

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

void Texture(std::pair<unsigned int, std::string>& texture)
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load(("Texture\\" + texture.second).c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glGenTextures(1, &texture.first);
		glBindTexture(GL_TEXTURE_2D, texture.first);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture " << texture.second << std::endl;
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

void DrawDeco()
{
	float fish[] = {

		//head
		 0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  
		 0.3f,  0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b 
		 0.3f,  0.0f,  0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // l 

		 0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  
		 0.3f, -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c 
		 0.3f,  0.0f,  0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // l 

		 0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  	
		 0.3f,  0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b
		 0.3f,  0.0f, -0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // m 

		 0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // a  
		 0.3f, -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c
		 0.3f,  0.0f, -0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // m 

		//body
		 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d   
		 0.3f,  0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b  
		 0.3f,  0.0f,  0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // l 

		 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d  
		 0.3f, -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c  
		 0.3f,  0.0f,  0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // l 

		 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d   
		 0.3f,  0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b
		 0.3f,  0.0f, -0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // m 

		 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d  
		 0.3f, -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c
		 0.3f,  0.0f, -0.2f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // m 

		 //tail
		 1.5f,   0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // e   
		 1.0f,   0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // d  
		 1.3f,  0.04f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // f 

		 1.0f,   0.0f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // d   
		 1.5f,  -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // g 
		 1.3f, -0.04f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // n 

		 //top fin  
		 0.8f,  0.4f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // h  
		 0.3f,  0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // b  
		 0.6f,  0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // i 

		 //bottom fin 
		 0.8f, -0.4f,  0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,   // j   
		 0.3f, -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // c   
		 0.6f, -0.2f,  0.0f,  0.0f, 1.0f, 0.0f,     1.0f, 1.0f,   // k 
	};

	float grass[] = {
		-0.5f,  0.5f,  0.0f,  0.0f, 0.0f, //a
		-0.5f, -0.5f,  0.0f,  0.0f, 1.0f, //b
		 0.5f, -0.5f,  0.0f,  1.0f, 1.0f, //c

		-0.5f,  0.5f,  0.0f,  0.0f, 0.0f, //a
		 0.5f, -0.5f,  0.0f,  1.0f, 1.0f, //c
		 0.5f,  0.5f,  0.0f,  1.0f, 0.0f  //d
	};

	float bubbles[] = {
		-0.10f,  0.10f,  0.0f,  0.0f, 0.0f,
		-0.10f, -0.10f,  0.0f,  0.0f, 1.0f,
		 0.10f, -0.10f,  0.0f,  1.0f, 1.0f,

		-0.10f,  0.10f,  0.0f,  0.0f, 0.0f,
		 0.10f, -0.10f,  0.0f,  1.0f, 1.0f,
		 0.10f,  0.10f,  0.0f,  1.0f, 0.0f
	};

	float castle[] = {
		-1.7f,  1.7f,  0.0f,  0.0f, 0.0f, //a
		-1.7f, -1.5f,  0.0f,  0.0f, 1.0f, //b
		 1.7f, -1.5f,  0.0f,  1.0f, 1.0f, //c

		-1.7f,  1.7f,  0.0f,  0.0f, 0.0f, //a
		 1.7f, -1.5f,  0.0f,  1.0f, 1.0f, //c
		 1.7f,  1.7f,  0.0f,  1.0f, 0.0f  //d
	};

	float rocks[] = {
		0.1f, -0.45f,  0.1f,   0.0f, 0.0f,   // a  
		0.4f, -0.65f,  0.1f,   1.0f, 0.0f,   // b 
		0.4f, -0.45f,  0.3f,   1.0f, 1.0f,   // l 

		0.1f, -0.45f,  0.1f,   0.0f, 0.0f,   // a  
		0.4f, -0.65f,  0.1f,   1.0f, 0.0f,   // c 
		0.4f, -0.45f,  0.3f,   1.0f, 1.0f,   // l 

		0.1f, -0.45f,  0.1f,   0.0f, 0.0f,   // a  	
		0.4f, -0.25f,  0.1f,   1.0f, 0.0f,   // b
		0.4f, -0.45f, -0.1f,   1.0f, 1.0f,   // m 

		0.1f, -0.45f,  0.1f,   0.0f, 0.0f,   // a  
		0.4f, -0.65f,  0.1f,   1.0f, 0.0f,   // c
		0.4f, -0.45f, -0.1f,   1.0f, 1.0f,   // m 

		0.4f, -0.45f,  0.3f,   0.0f, 1.0f,   // l 
		0.4f, -0.25f,  0.1f,   1.0f, 0.0f,   // b
		0.4f, -0.45f, -0.1f,   1.0f, 1.0f,   // m 

		0.4f, -0.45f, -0.1f,   0.0f, 1.0f,   // m 
		0.4f, -0.65f,  0.1f,   1.0f, 0.0f,   // c
		0.4f, -0.45f,  0.3f,   1.0f, 1.0f,   // l
	};

	float lamp[] = {

		//front
		4.5f, 3.0f, 1.8f,  0.0f, 0.0f,   // 11  top right
		4.2f, 3.0f, 1.8f,  1.0f, 0.0f,   // 14 
		4.5f, 2.9f, 1.8f,  1.0f, 1.0f,   // 15 

		4.2f, 2.9f, 1.8f,  0.0f, 0.0f,   // 20 
		4.2f, 3.0f, 1.8f,  1.0f, 0.0f,   // 14 
		4.5f, 2.9f, 1.8f,  1.0f, 1.0f,   // 15 

		//back
		4.5f, 3.0f, -1.8f,  0.0f, 0.0f,   // 16  
		4.2f, 3.0f, -1.8f,  1.0f, 0.0f,   // 19 
		4.5f, 2.9f, -1.8f,  1.0f, 1.0f,   // 18 

		4.2f, 2.9f, -1.8f,  0.0f, 0.0f,   // 21 
		4.2f, 3.0f, -1.8f,  1.0f, 0.0f,   // 19 
		4.5f, 2.9f, -1.8f,  1.0f, 1.0f,   // 18 

		//right
		4.5f, 3.0f,  1.8f,  0.0f, 0.0f,   // 11 
		4.5f, 3.0f, -1.8f,  1.0f, 0.0f,   // 16 
		4.5f, 2.9f,  1.8f,  1.0f, 1.0f,   // 15

		4.5f, 2.9f, -1.8f,  0.0f, 0.0f,   // 18 
		4.5f, 3.0f, -1.8f,  1.0f, 0.0f,   // 16 
		4.5f, 2.9f,  1.8f,  1.0f, 1.0f,   // 15

		//top
		4.5f, 3.0f,  1.8f,  0.0f, 0.0f,   // 11 
		4.2f, 3.0f,  1.8f,  1.0f, 0.0f,   // 14 
		4.5f, 3.0f, -1.8f,  1.0f, 1.0f,   // 16 

		4.2f, 3.0f, -1.8f,  0.0f, 0.0f,   // 19 
		4.2f, 3.0f,  1.8f,  1.0f, 0.0f,   // 14 
		4.5f, 3.0f, -1.8f,  1.0f, 1.0f,   // 16

		//base
		4.5f, 2.9f,  1.8f,  0.0f, 0.0f,   // 15 
		4.2f, 2.9f,  1.8f,  1.0f, 0.0f,   // 20 
		4.5f, 2.9f, -1.8f,  1.0f, 1.0f,   // 18 

		4.2f, 2.9f, -1.8f,  0.0f, 0.0f,   // 21 
		4.2f, 2.9f,  1.8f,  1.0f, 0.0f,   // 20 
		4.5f, 2.9f, -1.8f,  1.0f, 1.0f,   // 18

		//left
		4.2f, 3.0f,  1.8f,  0.0f, 0.0f,   // 14 
		4.2f, 3.0f, -1.8f,  1.0f, 0.0f,   // 19 
		4.2f, 2.9f, -1.8f,  1.0f, 1.0f,   // 21

		4.2f, 2.9f, -1.8f,  0.0f, 0.0f,   // 21 
		4.2f, 3.0f,  1.8f,  1.0f, 0.0f,   // 14 
		4.2f, 2.9f,  1.8f,  1.0f, 1.0f,   // 20
	};

	//fish
	GenerateVertexAndBind(fish_VAO, fish_VBO, 8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fish), fish, GL_STATIC_DRAW);

	//grass
	GenerateVertexAndBind(grass_VAO, grass_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grass), &grass, GL_STATIC_DRAW);

	//bubbles
	GenerateVertexAndBind(bubbles_VAO, bubbles_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bubbles), &bubbles, GL_STATIC_DRAW);

	//castle
	GenerateVertexAndBind(castle_VAO, castle_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(castle), &castle, GL_STATIC_DRAW);

	//rocks
	GenerateVertexAndBind(rock_VAO, rock_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rocks), rocks, GL_STATIC_DRAW);

	//lamp
	GenerateVertexAndBind(lamp_VAO, lamp_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lamp), lamp, GL_STATIC_DRAW);
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

	float bottom_cube[] = {

		//back
		-4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 1  left down
		 4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 2  right down
		 4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3  right top

		 4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3
		-4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 4  left top
		-4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 1

		//front
		-4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 5  left down
		 4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 6  right down
		 4.5f, -0.5f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 7  right top

		 4.5f, -0.5f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 7
		-4.5f,  0.2f,  2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 9  left top
		-4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 5

		//left
		-4.5f,  0.2f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 4
		-4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 1

		-4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 1
		-4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 5
		-4.5f,  0.2f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 9

		//right
		 4.5f, -0.5f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 7
		 4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3
		 4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 2

		 4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 2
		 4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 6
		 4.5f, -0.5f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 7

		 //base
		-4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 1
		 4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 2
		 4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 6

		 4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 6
		-4.5f, -1.0f,  2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 5
		-4.5f, -1.0f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 1

		//top
		-4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 4
		 4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // 3
		 4.5f, -0.5f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 7

		 4.5f, -0.5f,  2.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 7
		-4.5f,  0.2f,  2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,   // 4
	};

	float support_cube[] = {
		//back
		-5.5f, -1.5f, -3.5f,   0.0f, 0.0f,   // 1  left down
		 5.5f, -1.5f, -3.5f,   1.0f, 0.0f,   // 2  right down
		 5.5f, -1.0f, -3.5f,   1.0f, 1.0f,   // 3  right top

		 5.5f, -1.0f, -3.5f,   1.0f, 1.0f,   // 3
		-5.5f, -1.0f, -3.5f,   0.0f, 1.0f,   // 4  left top
		-5.5f, -1.5f, -3.5f,   0.0f, 0.0f,   // 1

		//front
		-5.5f, -1.5f,  3.5f,   0.0f, 0.0f,   // 5  left down
		 5.5f, -1.5f,  3.5f,   1.0f, 0.0f,   // 6  right down
		 5.5f, -1.0f,  3.5f,   1.0f, 1.0f,   // 7  right top

		 5.5f, -1.0f,  3.5f,   1.0f, 1.0f,   // 7
		-5.5f, -1.0f,  3.5f,   0.0f, 1.0f,   // 9  left top
		-5.5f, -1.5f,  3.5f,   0.0f, 0.0f,   // 5

		//left
		-5.5f, -1.0f,  3.5f,   1.0f, 0.0f,   // 9
		-5.5f, -1.0f, -3.5f,   1.0f, 1.0f,   // 4
		-5.5f, -1.5f, -3.5f,   0.0f, 1.0f,   // 1

		-5.5f, -1.5f, -3.5f,   0.0f, 1.0f,   // 1
		-5.5f, -1.5f,  3.5f,   0.0f, 0.0f,   // 5
		-5.5f, -1.0f,  3.5f,   1.0f, 0.0f,   // 9

		//right
		 5.5f, -1.0f,  3.5f,   1.0f, 0.0f,   // 7
		 5.5f, -1.0f, -3.5f,   1.0f, 1.0f,   // 3
		 5.5f, -1.5f, -3.5f,   0.0f, 1.0f,   // 2

		 5.5f, -1.5f, -3.5f,   0.0f, 1.0f,   // 2
		 5.5f, -1.5f,  3.5f,   0.0f, 0.0f,   // 6
		 5.5f, -1.0f,  3.5f,   1.0f, 0.0f,   // 7

		 //base
		-5.5f, -1.5f, -3.5f,   0.0f, 1.0f,   // 1
		 5.5f, -1.5f, -3.5f,   1.0f, 1.0f,   // 2
		 5.5f, -1.5f,  3.5f,   1.0f, 0.0f,   // 6

		 5.5f, -1.5f,  3.5f,   1.0f, 0.0f,   // 6
		-5.5f, -1.5f,  3.5f,   0.0f, 0.0f,   // 5
		-5.5f, -1.5f, -3.5f,   0.0f, 1.0f,   // 1

		//top
		-5.5f, -1.0f, -3.5f,   0.0f, 1.0f,   // 4
		 5.5f, -1.0f, -3.5f,   1.0f, 1.0f,   // 3
		 5.5f, -1.0f,  3.5f,   1.0f, 0.0f,   // 7

		 5.5f, -1.0f,  3.5f,   1.0f, 0.0f,   // 7
		-5.5f, -1.0f,  3.5f,   0.0f, 0.0f,   // 9
		-5.5f, -1.0f, -3.5f,   0.0f, 1.0f,   // 4
	};

	float column[] = {

		//back
		-0.2f, -2.2f, -0.2f,    0.0f, 0.0f,   // 4  left down
		 0.2f, -2.2f, -0.2f,    1.0f, 0.0f,   // 3  right down
		 0.2f,  2.2f, -0.2f,    1.0f, 1.0f,   // 12 right top

		 0.2f,  2.2f, -0.2f,    1.0f, 1.0f,   // 12
		-0.2f,  2.2f, -0.2f,    0.0f, 1.0f,   // 13 left top
		-0.2f, -2.2f, -0.2f,    0.0f, 0.0f,   // 4

		//front
		-0.2f, -2.2f,  0.2f,    0.0f, 0.0f,   // 9  left down
		 0.2f, -2.2f,  0.2f,    1.0f, 0.0f,   // 7  right down
		 0.2f,  2.2f,  0.2f,    1.0f, 1.0f,   // 11 right top

		 0.2f,  2.2f,  0.2f,    1.0f, 1.0f,   // 11
		-0.2f,  2.2f,  0.2f,    0.0f, 1.0f,   // 10 left top
		-0.2f, -2.2f,  0.2f,    0.0f, 0.0f,   // 9

		//left
		-0.2f,  2.2f,  0.2f,    1.0f, 0.0f,   // 10
		-0.2f,  2.2f, -0.2f,    1.0f, 1.0f,   // 13
		-0.2f, -2.2f, -0.2f,    0.0f, 1.0f,   // 4

		-0.2f, -2.2f, -0.2f,    0.0f, 1.0f,   // 4
		-0.2f, -2.2f,  0.2f,    0.0f, 0.0f,   // 9
		-0.2f,  2.2f,  0.2f,    1.0f, 0.0f,   // 10

		//right
		 0.2f,  2.2f,  0.2f,    1.0f, 0.0f,   // 11
		 0.2f,  2.2f, -0.2f,    1.0f, 1.0f,   // 12
		 0.2f, -2.2f, -0.2f,    0.0f, 1.0f,   // 3

		 0.2f, -2.2f, -0.2f,    0.0f, 1.0f,   // 3
		 0.2f, -2.2f,  0.2f,    0.0f, 0.0f,   // 7
		 0.2f,  2.2f,  0.2f,    1.0f, 0.0f,   // 11
	};

	float table[] = {

		//back
		-6.5f, -2.5f, -4.5f,   0.0f, 0.0f,   // 1  left down
		 6.5f, -1.5f, -4.5f,   1.0f, 0.0f,   // 2  right down
		 6.5f, -2.5f, -4.5f,   1.0f, 1.0f,   // 3  right top

		 6.5f, -2.5f, -4.5f,   1.0f, 1.0f,   // 3
		-6.5f, -2.5f, -4.5f,   0.0f, 1.0f,   // 4  left top
		-6.5f, -1.5f, -4.5f,   0.0f, 0.0f,   // 1

		//front
		-6.5f, -1.5f,  4.5f,   0.0f, 0.0f,   // 5  left down
		 6.5f, -1.5f,  4.5f,   1.0f, 0.0f,   // 6  right down
		 6.5f, -2.5f,  4.5f,   1.0f, 1.0f,   // 7  right top

		 6.5f, -2.5f,  4.5f,   1.0f, 1.0f,   // 7
		-6.5f, -2.5f,  4.5f,   0.0f, 1.0f,   // 9  left top
		-6.5f, -1.5f,  4.5f,   0.0f, 0.0f,   // 5

		//left
		-6.5f, -2.5f,  4.5f,   1.0f, 0.0f,   // 9
		-6.5f, -2.5f, -4.5f,   1.0f, 1.0f,   // 4
		-6.5f, -1.5f, -4.5f,   0.0f, 1.0f,   // 1

		-6.5f, -1.5f, -4.5f,   0.0f, 1.0f,   // 1
		-6.5f, -1.5f,  4.5f,   0.0f, 0.0f,   // 5
		-6.5f, -2.5f,  4.5f,   1.0f, 0.0f,   // 9

		//right
		 6.5f, -2.5f,  4.5f,   1.0f, 0.0f,   // 7
		 6.5f, -2.5f, -4.5f,   1.0f, 1.0f,   // 3
		 6.5f, -1.5f, -4.5f,   0.0f, 1.0f,   // 2

		 6.5f, -1.5f, -4.5f,   0.0f, 1.0f,   // 2
		 6.5f, -1.5f,  4.5f,   0.0f, 0.0f,   // 6
		 6.5f, -2.5f,  4.5f,   1.0f, 0.0f,   // 7

		 //top
		-6.5f, -1.5f, -4.5f,   0.0f, 1.0f,   // 1
		 6.5f, -1.5f, -4.5f,   1.0f, 1.0f,   // 2
		 6.5f, -1.5f,  4.5f,   1.0f, 0.0f,   // 6

		 6.5f, -1.5f,  4.5f,   1.0f, 0.0f,   // 6
		-6.5f, -1.5f,  4.5f,   0.0f, 0.0f,   // 5
		-6.5f, -1.5f, -4.5f,   0.0f, 1.0f,   // 1

		//base
		-6.5f, -2.5f, -4.5f,   0.0f, 1.0f,   // 4
		 6.5f, -2.5f, -4.5f,   1.0f, 1.0f,   // 3
		 6.5f, -2.5f,  4.5f,   1.0f, 0.0f,   // 7

		 6.5f, -2.5f,  4.5f,   1.0f, 0.0f,   // 7
		-6.5f, -2.5f,  4.5f,   0.0f, 0.0f,   // 9
		-6.5f, -2.5f, -4.5f,   0.0f, 1.0f,   // 4
	};

	float table_support[] = {

		//back
		-5.0f, -5.0f, -3.5f,    0.0f, 0.0f,   // 4  left down
		 5.0f, -5.0f, -3.5f,    1.0f, 0.0f,   // 3  right down
		 6.0f, -1.5f, -4.0f,    1.0f, 1.0f,   // 12 right top

		 6.0f, -1.5f, -4.0f,    1.0f, 1.0f,   // 12
		-6.0f, -1.5f, -4.0f,    0.0f, 1.0f,   // 13 left top
		-5.0f, -5.0f, -3.5f,    0.0f, 0.0f,   // 4

		//front
		-5.0f, -5.0f,  3.5f,    0.0f, 0.0f,   // 9  left down
		 5.0f, -5.0f,  3.5f,    1.0f, 0.0f,   // 7  right down
		 6.0f, -1.5f,  4.0f,    1.0f, 1.0f,   // 11 right top

		 6.0f, -1.5f,  4.0f,    1.0f, 1.0f,   // 11
		-6.0f, -1.5f,  4.0f,    0.0f, 1.0f,   // 10 left top
		-5.0f, -5.0f,  3.5f,    0.0f, 0.0f,   // 9

		//left
		-6.0f, -1.5f,  4.0f,    1.0f, 0.0f,   // 10
		-6.0f, -1.5f, -4.0f,    1.0f, 1.0f,   // 13
		-5.0f, -5.0f, -3.5f,    0.0f, 1.0f,   // 4

		-5.0f, -5.0f, -3.5f,    0.0f, 1.0f,   // 4
		-5.0f, -5.0f,  3.5f,    0.0f, 0.0f,   // 9
		-6.0f, -1.5f,  4.0f,    1.0f, 0.0f,   // 10

		//right
		 6.0f, -1.5f,  4.0f,    1.0f, 0.0f,   // 11
		 6.0f, -1.5f, -4.0f,    1.0f, 1.0f,   // 12
		 5.0f, -5.0f, -3.5f,    0.0f, 1.0f,   // 3

		 5.0f, -5.0f, -3.5f,    0.0f, 1.0f,   // 3
		 5.0f, -5.0f,  3.5f,    0.0f, 0.0f,   // 7
		 6.0f, -1.5f,  4.0f,    1.0f, 0.0f,   // 11
	};

	float back_and_front_faces[] = {

		-15.0f,  10.0f,  0.0f,   0.0f, 0.0f, //a
		-15.0f,  -5.0f,  0.0f,   0.0f, 1.0f, //b
		 15.0f,  -5.0f,  0.0f,   1.0f, 1.0f, //c

		-15.0f,  10.0f,  0.0f,   0.0f, 0.0f, //a
		 15.0f,  -5.0f,  0.0f,   1.0f, 1.0f, //c
		 15.0f,  10.0f,  0.0f,   1.0f, 0.0f  //d
	};

	float left_and_right_faces[] = {

		0.0f,  10.0f,  30.0f,   0.0f, 0.0f, //a
		0.0f,  -5.0f,  30.0f,   0.0f, 1.0f, //b
		0.0f,  -5.0f,  -3.0f,   1.0f, 1.0f, //c

		0.0f,  10.0f,  30.0f,   0.0f, 0.0f, //a
		0.0f,  -5.0f,  -3.0f,   1.0f, 1.0f, //c
		0.0f,  10.0f,  -3.0f,   1.0f, 0.0f  //d
	};

	float base_and_top_faces[] = {

		-15.0f,  0.0f, -3.0f,   0.0f, 0.0f,  //a
		-15.0f,  0.0f, 30.0f,   0.0f, 1.0f,  //b
		 15.0f,  0.0f, 30.0f,   1.0f, 1.0f,  //c

		-15.0f,  0.0f, -3.0f,   0.0f, 0.0f,  //a
		 15.0f,  0.0f, 30.0f,   1.0f, 1.0f,  //c
		 15.0f,  0.0f, -3.0f,   1.0f, 0.0f   //d
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

	//support cube
	GenerateVertexAndBind(support_VAO, support_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(support_cube), support_cube, GL_STATIC_DRAW);

	//column
	GenerateVertexAndBind(column_VAO, column_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(column), column, GL_STATIC_DRAW);

	//table
	GenerateVertexAndBind(table_VAO, table_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(table), table, GL_STATIC_DRAW);

	//table support
	GenerateVertexAndBind(tableSupport_VAO, tableSupport_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(table_support), table_support, GL_STATIC_DRAW);

	//back_and_front_faces
	GenerateVertexAndBind(back_and_front_faces_VAO, back_and_front_faces_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(back_and_front_faces), back_and_front_faces, GL_STATIC_DRAW);

	//left_and_right_faces
	GenerateVertexAndBind(left_and_right_faces_VAO, left_and_right_faces_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(left_and_right_faces), left_and_right_faces, GL_STATIC_DRAW);

	//base_and_top_faces
	GenerateVertexAndBind(base_and_top_faces_VAO, base_and_top_faces_VBO, 5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(base_and_top_faces), base_and_top_faces, GL_STATIC_DRAW);
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

	if (Movement >= 2.35) Movement = position.y;

	decoShader.setMat4("model", model);
	Rotate(decoShader, model, texture, 200, 1);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void RenderFish(Shader& shader, Shader& decoShader)
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 bubbles = glm::mat4(1.0f);

	static glm::mat4 models[] = {
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f)
	};

	static glm::vec2 incrementAndMovement[] = {
		glm::vec2(0.021,  1.9),
		glm::vec2(0.018, -1.9),
		glm::vec2(0.019, -1.5),
		glm::vec2(0.019,  2.5),
		glm::vec2(0.020, -1.2)
	};

	glm::vec3 fishPosition[] = {
		glm::vec3(incrementAndMovement[0].y - incrementAndMovement[0].x, 1.3f,  0.0f),
		glm::vec3(incrementAndMovement[1].y - incrementAndMovement[1].x, 2.0f, -1.5f),
		glm::vec3(incrementAndMovement[2].y - incrementAndMovement[2].x, 1.0f,  1.5f),
		glm::vec3(incrementAndMovement[3].y - incrementAndMovement[3].x, 0.5f, -1.5f),
		glm::vec3(incrementAndMovement[4].y - incrementAndMovement[4].x, 2.0f,  1.5f)
	};

	static std::vector<float> bubbleMovement{
		fishPosition[0].y,
		fishPosition[1].y,
		fishPosition[2].y,
		fishPosition[3].y,
		fishPosition[4].y
	};

	static std::vector<float> bubbleIncrement{ 0.003, 0.003, 0.007, 0.008, 0.005 };

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
		DrawBubbles(decoShader, decoTextures[2].first, fishPosition[index], bubbleMovement[index], bubbleIncrement[index]);
	}
}

void RenderRocks(Shader& shader)
{
	glm::mat4 model = glm::mat4(1.0f);

	static glm::mat4 models[] = {
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
		glm::vec3(-1.1f,-0.95f,  0.1f),
		glm::vec3(1.4f, -0.95f, -1.4f),
		glm::vec3(0.8f, -0.95f,  1.4f),
		glm::vec3(3.4f, -0.95f, -1.5f),
		glm::vec3(-3.9f,-0.55f,  1.4f),
		glm::vec3(2.9f, -0.95f,  2.0f),
		glm::vec3(0.0f, -0.95f, -1.0f),
		glm::vec3(-4.0f,-0.95f, -2.0f),
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
			glBindTexture(GL_TEXTURE_2D, decoTextures[0].first);
		else
			glBindTexture(GL_TEXTURE_2D, decoTextures[1].first);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

void RenderLamp_Castle(Shader& shader, unsigned int texture, unsigned int VAO, glm::vec3 position)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	shader.use();
	shader.setMat4("model", model);

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
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
		glm::vec3(0.0f, -0.2f,  0.0f),
		glm::vec3(2.5f, -0.2f,  1.5f),
		glm::vec3(-3.0f,-0.2f, -2.0f),
		glm::vec3(1.0f, -0.2f, -2.0f),
		glm::vec3(-4.0f, 0.3f,  1.0f),
		glm::vec3(3.2f, -0.2f, -1.0f),
		glm::vec3(-2.0f, 0.1f,  1.2f),
	};

	glm::vec3 BlueLeafs[] = {
		glm::vec3(1.6f,  0.0f, -0.3f),
		glm::vec3(-1.5f, 0.0f, -1.2f),
		glm::vec3(-3.0f, 0.5f,  2.0f)
	};

	glm::vec3 YellowLeafs[] = {
		glm::vec3(-3.5f, 0.1f, -0.5f),
		glm::vec3(3.5f,  0.0f, 0.75f),
		glm::vec3(-0.5f, 0.2f,  2.0f)
	};

	glm::vec3 redCoralBubblePositions[] = {
		glm::vec3(0.0f,  -0.15f, -1.0f),
		glm::vec3(2.3f,  -0.15f, -1.0f),
		glm::vec3(-3.3f, -0.15f,  1.0f)
	};

	glm::vec3 blueCoralBubblePositions[] = {
		glm::vec3(3.0f,  0.35f, 0.0f),
		glm::vec3(3.0f,  0.15f, 0.0f),
		glm::vec3(-3.0f, 0.15f, 0.0f)
	};

	glm::vec3 pinkCoralBubblePositions[] = {
		glm::vec3(-3.0f, 0.65f,  0.5f),
		glm::vec3(-1.0f,  0.0f,  0.5f),
		glm::vec3(3.0f,  -0.1f,  0.5f),
		glm::vec3(1.0f,  -0.1f, -2.0f)
	};

	glm::vec3 sweetGrass[] = {
		glm::vec3(4.0f, 0.0f,  2.0f),
		glm::vec3(4.0f, 0.4f,  2.0f),
		glm::vec3(4.0f, 0.8f,  2.0f),
		glm::vec3(4.0f, 1.2f,  2.0f),

		glm::vec3(2.5f, 0.0f, -1.5f),
		glm::vec3(2.5f, 0.4f, -1.5f),
		glm::vec3(2.5f, 0.8f, -1.5f),
		glm::vec3(2.5f, 1.2f, -1.5f),
		glm::vec3(2.5f, 1.6f, -1.5f),

		glm::vec3(-2.0f, 0.0f,  0.0f),
		glm::vec3(-2.0f, 0.4f,  0.0f),
		glm::vec3(-2.0f, 0.8f,  0.0f),

		glm::vec3(-1.0f, 0.0f,  1.0f),
		glm::vec3(-3.0f, 0.0f,  1.0f),
		glm::vec3(2.0f,  0.0f, -1.0f),
		glm::vec3(-3.0f, 0.1f,  2.2f),

		glm::vec3(-4.0f, 0.1f,  2.2f),
		glm::vec3(-4.0f, 0.5f,  2.2f),
		glm::vec3(-4.0f, 0.9f,  2.2f),
		glm::vec3(-4.0f, 1.3f,  2.2f),
	};

	glm::vec3 greenLeafs[] = {
		glm::vec3(-4.0f, 0.1f, -2.0f),
		glm::vec3(4.0f,  0.0f, -2.0f),
		glm::vec3(1.5f,  0.2f,  2.0f)
	};

	glm::vec3 leaves[] = {
		glm::vec3(-3.0f,  0.0f, -2.0f),
		glm::vec3(3.0f, -0.1f, -2.0f),
		glm::vec3(-1.5f,  0.1f,  1.0f),
		glm::vec3(0.5f, -0.1f,  1.3f),
	};

	glm::vec3 plant[] = {
		glm::vec3(0.0, 0.0, -2.0),
		glm::vec3(0.0, 0.4, -2.0),
		glm::vec3(0.0, 0.8, -2.0),
		glm::vec3(0.0, 1.2, -2.0),

		glm::vec3(-1.0, 0.0, -1.0),
		glm::vec3(-1.0, 0.4, -1.0),

		glm::vec3(-2.7, 0.2, 2.0),
		glm::vec3(-2.7, 0.6, 2.0),
		glm::vec3(-2.7, 1.0, 2.0),

		glm::vec3(3.5, -0.1, 2.0),
		glm::vec3(3.5,  0.3, 2.0),
	};

	glm::vec3 plant2[] = {
		glm::vec3(2.0,  0.0, -1.0),
		glm::vec3(1.0,  0.0, -2.0),
		glm::vec3(3.0,  0.0, -2.0),
		glm::vec3(4.0,  0.0, -0.5),
		glm::vec3(-1.0, 0.0,  0.5),
	};

	glm::vec3 plant3[] = {
		glm::vec3(-2.0, -0.2, -1.5),
		glm::vec3(1.9,  -0.2, -2.2),
		glm::vec3(3.9,  -0.2, -2.2),
		glm::vec3(4.1,  -0.2,  0.0),
		glm::vec3(-1.5, -0.2,  0.5),
	};

	glm::vec3 plant4[] = {
		glm::vec3(-3.0, -0.3, -1.5),
		glm::vec3(1.9,  -0.4, -1.2),
		glm::vec3(3.3,  -0.4,  2.0),
		glm::vec3(2.1,  -0.4,  1.0),
		glm::vec3(-2.5, -0.4,  1.5),
	};

	if (texture.second == "deco_plant.png")
		for (const auto& vector : plant)
			RenderVegetation(decoShader, model, vector, texture.first, 5, 2);

	if (texture.second == "deco_plant2.png")
		for (const auto& vector : plant2)
			RenderVegetation(decoShader, model, vector, texture.first, 3, 5);

	if (texture.second == "deco_plant3.png")
		for (const auto& vector : plant3)
			RenderVegetation(decoShader, model, vector, texture.first, 8, 5);

	if (texture.second == "deco_plant4.png")
		for (const auto& vector : plant4)
			RenderVegetation(decoShader, model, vector, texture.first, 5, 5);

	if (texture.second == "deco_grass.png")
		for (const auto& vector : Grass)
			RenderVegetation(decoShader, model, vector, texture.first, 8, 1);

	if (texture.second == "deco_pinkCoralReef.png")
		for (const auto& vector : pinkCoralBubblePositions)
			RenderVegetation(decoShader, model, vector, texture.first, 8, 1);

	if (texture.second == "deco_redCoralReef.png")
		for (const auto& vector : redCoralBubblePositions)
			RenderVegetation(decoShader, model, vector, texture.first, 8, 1);

	if (texture.second == "deco_leaves.png")
		for (const auto& vector : leaves)
			RenderVegetation(decoShader, model, vector, texture.first, 3, 10);

	if (texture.second == "deco_greenLeaf.png")
		for (const auto& vector : greenLeafs)
			RenderVegetation(decoShader, model, vector, texture.first, 3, 4);

	if (texture.second == "deco_blueLeaf.png")
		for (const auto& vector : BlueLeafs)
			RenderVegetation(decoShader, model, vector, texture.first, 5, 1);

	if (texture.second == "deco_yellowLeaf.png")
		for (const auto& vector : YellowLeafs)
			RenderVegetation(decoShader, model, vector, texture.first, 8, 2);

	if (texture.second == "deco_sweetGrass.png")
		for (const auto& vector : sweetGrass)
			RenderVegetation(decoShader, model, vector, texture.first, 5, 3);

	if (texture.second == "deco_redCoralReef.png" || texture.second == "deco_blueCoralReef.png" || texture.second == "deco_pinkCoralReef.png")
	{
		static std::vector<float> bubbleMovement{ 0.35f, 0.15f, 0.15f };
		static std::vector<float> bubbleIncrement{ 0.005, 0.005, 0.005 };
		if (texture.second == "deco_redCoralReef.png")
		{
			RenderVegetation(decoShader, model, glm::vec3(0.0f, 0.0f, -1.0f), texture.first, 5, 10);
			for (int index = 0; index < redCoralBubblePositions->length(); ++index)
				DrawBubbles(decoShader, decoTextures[2].first, redCoralBubblePositions[index], bubbleMovement[index], bubbleIncrement[index]);
		}
		if (texture.second == "deco_blueCoralReef.png")
		{
			RenderVegetation(decoShader, model, glm::vec3(3.0f, 0.0f, 0.0f), texture.first, 5, 10);
			for (int index = 0; index < blueCoralBubblePositions->length(); ++index)
				DrawBubbles(decoShader, decoTextures[2].first, blueCoralBubblePositions[index], bubbleMovement[index], bubbleIncrement[index]);
		}
		if (texture.second == "deco_pinkCoralReef.png")
		{
			RenderVegetation(decoShader, model, glm::vec3(-3.0f, 0.3f, 0.5f), texture.first, 5, 10);
			for (int index = 0; index < pinkCoralBubblePositions->length(); ++index)
				DrawBubbles(decoShader, decoTextures[2].first, pinkCoralBubblePositions[index], bubbleMovement[index], bubbleIncrement[index]);
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
	if (texture.second == "cube_middle.jpg" || texture.second == "cube_top.jpg")
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

void RenderRoom(Shader& shader, glm::vec3 position, unsigned int VAO, std::pair<unsigned int, std::string> texture)
{
	glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, position);
	shader.setMat4("model", model2);
	CreateAquarium(VAO, texture);
}

void RenderFunction(Shader& cubeShader, Shader& shadow, Shader& decoShader)
{
	glClearColor(1.0f, 0.5f, 1.1f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

	glm::vec3 position[] = {
		glm::vec3(0.0,  0.0,   0.0),  //support down
		glm::vec3(0.0,  4.5,   0.0),  //support up
		glm::vec3(5.0,  1.0,   3.0),  //column
		glm::vec3(-5.0, 1.0,   3.0),  //column
		glm::vec3(5.0,  1.0,  -3.0),  //column
		glm::vec3(-5.0, 1.0,  -3.0),  //column

		glm::vec3(0.0,  10.0,   0.0),  //ceiling
		glm::vec3(0.0,  -5.0,   0.0),  //floor
		glm::vec3(0.0,   0.0,  30.0),  //front
		glm::vec3(0.0,   0.0,  -3.0),  //back
		glm::vec3(-15.0, 0.0,   0.0),  //left
		glm::vec3(15.0,  0.0,   0.0),  //right

		glm::vec3(0.0,   0.7,  -2.4),  //castle
	};

	cubeShader.use();
	cubeShader.setMat4("projection", projection);
	cubeShader.setMat4("view", view);

	//room
	RenderRoom(cubeShader, position[6], base_and_top_faces_VAO, faces[0]);
	RenderRoom(cubeShader, position[7], base_and_top_faces_VAO, faces[1]);
	RenderRoom(cubeShader, position[8], back_and_front_faces_VAO, faces[3]);
	RenderRoom(cubeShader, position[9], back_and_front_faces_VAO, faces[2]);
	RenderRoom(cubeShader, position[10], left_and_right_faces_VAO, faces[4]);
	RenderRoom(cubeShader, position[11], left_and_right_faces_VAO, faces[5]);

	cubeShader.setMat4("model", model);

	//top cube
	CreateAquarium(top_VAO, cubeTextures[0]);

	//middle cube
	CreateAquarium(middle_VAO, cubeTextures[1]);

	//table
	CreateAquarium(table_VAO, cubeTextures[4]);

	//table supports
	RenderRoom(cubeShader, glm::vec3(0.0f, 0.0f, 0.0f), tableSupport_VAO, cubeTextures[4]);

	//columns
	RenderRoom(cubeShader, position[0], support_VAO, cubeTextures[3]);
	RenderRoom(cubeShader, position[1], support_VAO, cubeTextures[3]);

	for (int i = 2; i < 6; i++)
		RenderRoom(cubeShader, position[i], column_VAO, cubeTextures[3]);

	shadow.use();
	shadow.setMat4("model", model);
	shadow.setMat4("projection", projection);
	shadow.setMat4("view", view);

	//base cube
	CreateAquarium(base_VAO, cubeTextures[2]);

	RenderFish(shadow, decoShader);

	decoShader.use();
	decoShader.setMat4("projection", projection);
	decoShader.setMat4("view", view);

	for (int index = 0; index < plantsTextures.size(); ++index)
		DrawVegetation(decoShader, plantsTextures[index]);

	RenderRocks(decoShader);
	//lamp
	RenderLamp_Castle(decoShader, decoTextures[3].first, lamp_VAO, position[0]);
	//castle
	RenderLamp_Castle(decoShader, decoTextures[4].first, castle_VAO, position[12]);
}

void DeleteVertexArrayAndBuffer(unsigned int& VAO)
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VAO);
}

void GenerateShadowBuffer()
{
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
}

void RenderShadows(Shader& shadowMappingDepthShader, Shader& cubeShader, Shader& decoShader, Shader& shadowMappingShader)
{
	glm::vec3 lightPos(3.5f, 4.5f, 1.0f);
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;

	float near_plane = 1.0f, far_plane = 10.0f;

	lightProjection = glm::perspective(glm::radians(85.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT / 2.5f, near_plane, far_plane);
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

	glm::mat4 projection = camera.GetProjectionMatrix();
	glm::mat4 view = camera.GetViewMatrix();

	shadowMappingShader.use();
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
}

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef APPLE
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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	DrawAquarium();
	DrawDeco();

	for (auto& i : cubeTextures)
		Texture(i);

	for (auto& i : fishTextures)
		Texture(i);

	for (auto& i : plantsTextures)
		Texture(i);

	for (auto& i : decoTextures)
		Texture(i);

	for (auto& i : faces)
		Texture(i);

	Shader cubeShader("topShader.vs", "topShader.fs");
	Shader decoShader("Blending.vs", "Blending.fs");
	Shader shadowMappingShader("ShadowMapping.vs", "ShadowMapping.fs");
	Shader shadowMappingDepthShader("ShadowMappingDepth.vs", "ShadowMappingDepth.fs");

	cubeShader.use();
	cubeShader.setInt("texture1", 0);
	decoShader.use();
	decoShader.setInt("texture1", 0);

	GenerateShadowBuffer();

	shadowMappingShader.use();
	shadowMappingShader.setInt("diffuseTexture", 0);
	shadowMappingShader.setInt("shadowMap", 1);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		RenderShadows(shadowMappingDepthShader, cubeShader, decoShader, shadowMappingShader);
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
	DeleteVertexArrayAndBuffer(back_and_front_faces_VAO);
	DeleteVertexArrayAndBuffer(left_and_right_faces_VAO);
	DeleteVertexArrayAndBuffer(base_and_top_faces_VAO);
	DeleteVertexArrayAndBuffer(support_VAO);
	DeleteVertexArrayAndBuffer(column_VAO);
	DeleteVertexArrayAndBuffer(table_VAO);
	DeleteVertexArrayAndBuffer(tableSupport_VAO);
	DeleteVertexArrayAndBuffer(lamp_VAO);
	DeleteVertexArrayAndBuffer(light_VAO);
	DeleteVertexArrayAndBuffer(table_VAO);

	glfwTerminate();
	return 0;
}
