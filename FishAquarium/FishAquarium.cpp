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

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 700;

Camera camera(glm::vec3(0.0f, 1.0f, 12.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int texture1, texture2, texture3, texture4, texture5, texture6, texture7;
unsigned int base_VBO, base_VAO;
unsigned int top_VBO, top_VAO;
unsigned int fish_VBO, fish_VAO;

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
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(strExePath.c_str(), &width, &height, &nrChannels, 0);
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

void DrawFish()
{
	float fish[] = {

		//head
		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // a  
		 0.3f,  0.2f,  0.0f,    1.0f, 0.0f,   // b 
		 0.3f,  0.0f,  0.2f,    1.0f, 1.0f,   // l 

		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // a  
		 0.3f, -0.2f,  0.0f,    1.0f, 0.0f,   // c 
		 0.3f,  0.0f,  0.2f,    1.0f, 1.0f,   // l 

		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // a  	
		 0.3f,  0.2f,  0.0f,    1.0f, 0.0f,   // b
		 0.3f,  0.0f, -0.2f,    1.0f, 1.0f,   // m 

		 0.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // a  
		 0.3f, -0.2f,  0.0f,    1.0f, 0.0f,   // c
		 0.3f,  0.0f, -0.2f,    1.0f, 1.0f,   // m 

		//body
		 1.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // d   
		 0.3f,  0.2f,  0.0f,    1.0f, 0.0f,   // b  
		 0.3f,  0.0f,  0.2f,    1.0f, 1.0f,   // l 

		 1.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // d  
		 0.3f, -0.2f,  0.0f,    1.0f, 0.0f,   // c  
		 0.3f,  0.0f,  0.2f,    1.0f, 1.0f,   // l 

		 1.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // d   
		 0.3f,  0.2f,  0.0f,    1.0f, 0.0f,   // b
		 0.3f,  0.0f, -0.2f,    1.0f, 1.0f,   // m 

		 1.0f,  0.0f,  0.0f,    0.0f, 0.0f,   // d  
		 0.3f, -0.2f,  0.0f,    1.0f, 0.0f,   // c
		 0.3f,  0.0f, -0.2f,    1.0f, 1.0f,   // m 

		 //tail
		 1.5f,   0.2f,  0.0f,    0.0f, 0.0f,   // e   
		 1.0f,   0.0f,  0.0f,    1.0f, 0.0f,   // d  
		 1.3f,  0.04f,  0.0f,    1.0f, 1.0f,   // f 

		 1.0f,   0.0f,  0.0f,    0.0f, 0.0f,   // d   
		 1.5f,  -0.2f,  0.0f,    1.0f, 0.0f,   // g 
		 1.3f, -0.04f,  0.0f,    1.0f, 1.0f,   // n 

		 //top fin  
		 0.8f,  0.4f,  0.0f,    0.0f, 0.0f,   // h  
		 0.3f,  0.2f,  0.0f,    1.0f, 0.0f,   // b  
		 0.6f,  0.2f,  0.0f,    1.0f, 1.0f,   // i 

		 //bottom fin 
		 0.8f, -0.4f,  0.0f,    0.0f, 0.0f,   // j   
		 0.3f, -0.2f,  0.0f,    1.0f, 0.0f,   // c   
		 0.6f, -0.2f,  0.0f,    1.0f, 1.0f,   // k 
	};

	glGenVertexArrays(1, &fish_VAO);
	glGenBuffers(1, &fish_VBO);
	glBindVertexArray(fish_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, fish_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fish), fish, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void DrawAquarium()
{
	float top_cube[] = {

		//back
		-4.5f, -0.5f, -2.5f,    0.0f, 0.0f,   // 4  left down
		 4.5f, -0.5f, -2.5f,    1.0f, 0.0f,   // 3  right down
		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12 right top

		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12
		-4.5f,  3.0f, -2.5f,    0.0f, 1.0f,   // 13 left top
		-4.5f, -0.5f, -2.5f,    0.0f, 0.0f,   // 4

		//front
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9  left down
		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7  right down
		 4.5f,  3.0f,  2.5f,    1.0f, 1.0f,   // 11 right top

		 4.5f,  3.0f,  2.5f,    1.0f, 1.0f,   // 11
		-4.5f,  3.0f,  2.5f,    0.0f, 1.0f,   // 10 left top
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9

		//left
		-4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 10
		-4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 13
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4

		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9
		-4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 10

		//right
		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11
		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12
		 4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 3

		 4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 3
		 4.5f, -0.5f,  2.5f,    0.0f, 0.0f,   // 7
		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11

		 //base
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
		 4.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7

		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4

		//top
		-4.5f,  3.0f, -2.5f,    0.0f, 1.0f,   // 13
		 4.5f,  3.0f, -2.5f,    1.0f, 1.0f,   // 12
		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11

		 4.5f,  3.0f,  2.5f,    1.0f, 0.0f,   // 11
		-4.5f,  3.0f,  2.5f,    0.0f, 0.0f,   // 10
		-4.5f,  3.0f, -2.5f,    0.0f, 1.0f    // 13
	};

	float  bottom_cube[] = {

		//back
		-4.5f, -1.0f, -2.5f,    0.0f, 0.0f,   // 1  left down
		 4.5f, -1.0f, -2.5f,    1.0f, 0.0f,   // 2  right down
		 4.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3  right top

		 4.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4  left top
		-4.5f, -1.0f, -2.5f,    0.0f, 0.0f,   // 1

		//front
		-4.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5  left down
		 4.5f, -1.0f,  2.5f,    1.0f, 0.0f,   // 6  right down
		 4.5f, -0.5f,  2.5f,    1.0f, 1.0f,   // 7  right top

		 4.5f, -0.5f,  2.5f,    1.0f, 1.0f,   // 7
		-4.5f,  0.2f,  2.5f,    0.0f, 1.0f,   // 9  left top
		-4.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5

		//left
		-4.5f,  0.2f,  2.5f,    1.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 4
		-4.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1

		-4.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1
		-4.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5
		-4.5f,  0.2f,  2.5f,    1.0f, 0.0f,   // 9

		//right
		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7
		 4.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		 4.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 2

		 4.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 2
		 4.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 6
		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7

		 //base
		-4.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1
		 4.5f, -1.0f, -2.5f,    1.0f, 1.0f,   // 2
		 4.5f, -1.0f,  2.5f,    1.0f, 0.0f,   // 6

		 4.5f, -1.0f,  2.5f,    1.0f, 0.0f,   // 6
		-4.5f, -1.0f,  2.5f,    0.0f, 0.0f,   // 5
		-4.5f, -1.0f, -2.5f,    0.0f, 1.0f,   // 1

		//top
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
		 4.5f, -0.5f, -2.5f,    1.0f, 1.0f,   // 3
		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7

		 4.5f, -0.5f,  2.5f,    1.0f, 0.0f,   // 7
		-4.5f,  0.2f,  2.5f,    0.0f, 0.0f,   // 9
		-4.5f, -0.5f, -2.5f,    0.0f, 1.0f,   // 4
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

void RenderFish(float& Increment, float& Movement, Shader& shader, unsigned int texture, float y, float z)
{
	glm::mat4 model = glm::mat4(1.0f);
	Movement = Movement - Increment;

	glm::vec3 new_pos(Movement, y, z);
	model = glm::translate(model, new_pos);

	if ((new_pos.x <= -3.0 || new_pos.x >= 3.0))   Increment *= -1;
	if (Increment < 0) model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	shader.use();
	shader.setMat4("model", model);

	glBindVertexArray(fish_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void RenderFunction(Shader& shader)
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

	//al doilea cub
	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	glBindVertexArray(top_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);

	//primul cub
	shader.use();
	shader.setMat4("model", model);

	glBindVertexArray(base_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	//1st fish
	static float fish1_Increment = 0.003;
	static float fish1_Movement = 1.9;
	RenderFish(fish1_Increment, fish1_Movement, shader, texture5, 1.3, 0.0);

	//2nd fish
	static float fish2_Increment = 0.003;
	static float fish2_Movement = -1.9;
	RenderFish(fish2_Increment, fish2_Movement, shader, texture4, 2.0, -1.5);

	//3rd fish
	static float fish3_Increment = 0.001;
	static float fish3_Movement = -1.5;
	RenderFish(fish3_Increment, fish3_Movement, shader, texture3, 1.0, 1.5);

	//4th fish
	static float fish4_Increment = 0.001;
	static float fish4_Movement = 2.5;
	RenderFish(fish4_Increment, fish4_Movement, shader, texture6, 0.5, -1.5);

	//5th fish
	static float fish5_Increment = 0.002;
	static float fish5_Movement = -1.2;
	RenderFish(fish5_Increment, fish5_Movement, shader, texture7, 2.0, 1.5);
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
	DrawAquarium();
	DrawFish();

	Shader baseShader("baseShader.vs", "baseShader.fs");

	Texture(strExePath + "\\sand.jpg", texture1);
	Texture(strExePath + "\\water1.jpg", texture2);
	Texture(strExePath + "\\scales.jpg", texture3);
	Texture(strExePath + "\\scales2.jpg", texture4);
	Texture(strExePath + "\\scales3.jpg", texture5);
	Texture(strExePath + "\\scales4.jpg", texture6);
	Texture(strExePath + "\\scales5.png", texture7);

	baseShader.use();
	baseShader.setInt("texture1", 0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		RenderFunction(baseShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &base_VAO);
	glDeleteVertexArrays(1, &top_VAO);
	glDeleteVertexArrays(1, &fish_VAO);
	glDeleteBuffers(1, &base_VBO);
	glDeleteBuffers(1, &top_VAO);
	glDeleteBuffers(1, &fish_VAO);

	glfwTerminate();
	return 0;
}
