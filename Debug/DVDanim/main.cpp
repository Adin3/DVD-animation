#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Shader shader);
void borderColision(float &velX, float &velY, glm::vec3 pos, Shader shader);
void randomColor(Shader shader);
float randomNumber(int min, int max);
void getDeltaTime();

float oldCurrentTime;
float deltaTime;

int main()
{
	//glfw Init
	if (!glfwInit()) {
		std::cout << "glfw nu a putut fi initializat" << std::endl; return -1;
	}

	//opengl version 3.3 core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window create
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DVD", NULL, NULL);
	if (!window) {
		std::cout << "window nu a putut fi creat" << std::endl; 
		glfwTerminate(); return -1;
	}

	//declare current context
	glfwMakeContextCurrent(window);

	//GLad Init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "glad nu a putut fi initializat" << std::endl; return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader("shaders/vertexShader.vs", "shaders/fragmentShader.fs");

	//the points
	float vertices[] = {
		//Coord				
		2.0f,  1.0f,  0.0f,     1.0f, 1.0f, //0
		2.0f, -1.0f,  0.0f,     1.0f, 0.0f, //1
	   -2.0f, -1.0f,  0.0f,     0.0f, 0.0f, //2
	   -2.0f,  1.0f,  0.0f,     0.0f, 1.0f, //3
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("img/dvd.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << " failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader.use(); 
	shader.setInt("inTexture", 0);
	glUniform4f(glGetUniformLocation(shader.ID, "color"), 1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec3 pos = glm::vec3(randomNumber(20, 10),randomNumber(20, 10), 0.0f);
	float velY = 0.5f;
	float velX = 0.5f;
	//THE LOOP
	while (!glfwWindowShouldClose(window))
	{
		//Input
		processInput(window,shader);
		borderColision(velX, velY, pos, shader);
		getDeltaTime();

		//Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		pos.x = (pos.x + velX * deltaTime);
		pos.y = (pos.y + velY * deltaTime);
		glm::mat4 trans = glm::mat4(1.0);
		trans = glm::translate(trans, pos);
		trans = glm::scale(trans, glm::vec3(0.2, 0.2, 0.0));
		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		std::cout << deltaTime << std::endl;
		//Buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	//Eliminate
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
}

//the functions
void processInput(GLFWwindow* window, Shader shader)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		randomColor(shader);
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void randomColor(Shader shader)
{
	glUniform4f(glGetUniformLocation(shader.ID, "color"), randomNumber(10, 100), randomNumber(10, 100),
		randomNumber(10, 100), 1.0f);
}

float randomNumber(int min, int max)
{
	float rn = rand() % max + min;
	return rn / 100;
}

void borderColision(float &velX, float &velY, glm::vec3 pos, Shader shader)
{
	if (pos.x >= 1.0f - (pos.x / 2) - 0.1) {
		velX = -velX;
		randomColor(shader);
	}
	if (pos.x <= -1.0f + ((-pos.x) / 2) + 0.1) {
		velX = -velX;
		randomColor(shader);
	}
	if (pos.y >= 1.0f - (pos.y / 2) + 0.2) {
		velY = -velY;
		randomColor(shader);
	}
	if (pos.y <= -1.0f + ((-pos.y) / 2) - 0.2) {
		velY = -velY;
		randomColor(shader);
	}
}

void getDeltaTime()
{
	float currentTime = glfwGetTime();
	deltaTime = currentTime - oldCurrentTime;
	oldCurrentTime = currentTime;
}
