
/*****
 *
 *	Johnny Mak
 *	Comp 371 - Computer Graphics
 *	Assignment #1 
 *
 *****/

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include "Shader.h"
#include "FileAnalyzer.h"

// GLEW 
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;


// ========== Function Prototypes ========== //

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
GLfloat pushValueLimit(GLuint value);



// ========== Constants and Values ========== //

// Vertex and Fragment Shader locations
const GLchar* VERTEX_SHADER_PATH   = "./vertex.shader";
const GLchar* FRAGMENT_SHADER_PATH = "./fragment.shader";

// Initial Screen dimensions (800x800) 
const GLuint WIDTH  = 800,
			 HEIGHT = 800;

// Length for both Profile and Trajectory Curves (To be used as input)
int profileHeight;
int trajLength;

GLfloat fov = 45.0f; // Initial field of view for the perspective

GLenum drawMode = GL_TRIANGLES; // Default Drawing mode

// GLM Tranformations
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

// Initial Camera Positions
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// Extra variables for camera movement
bool keys[1024];
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


// ************* Transformation Shape Location ************* // 

// const string FILE_LOCATION = "txt/translational_5x5_grid.txt";
// const string FILE_LOCATION = "txt/translational_hollow_box.txt";
// const string FILE_LOCATION = "txt/translational_rectangle.txt";
// const string FILE_LOCATION = "txt/translational_shape.txt";

// const string FILE_LOCATION = "txt/rotational_bowl.txt";
// const string FILE_LOCATION = "txt/rotational_cone.txt";
// const string FILE_LOCATION = "txt/rotational_cylinder.txt";
const string FILE_LOCATION = "txt/rotational_hat.txt";
// const string FILE_LOCATION = "txt/rotational_pyramid.txt";

// ************* ************* ************* ************* //



							// ========== Main Method ========== //

int main() {

	// Initiates GLFW and defines the settings
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


	// Creates Window Object 
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Johnny Mak - Assignment #1", nullptr, nullptr);
	if (window == nullptr) {
		cout << "Failed to create GLFW Windows\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Initiates GLEW (Extension Wrangler) 
	glewExperimental = GLU_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "Failed to initialize GLEW\n";
		return -1;
	}

	// Creates the Viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Enable Z-Buffer
	glEnable(GL_DEPTH_TEST);



	// ========== Creating our shaders ========== //

	Shader ourShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);


	// ========== Creating Our Shapes ========== //

	// Vertices for the Triangle

	FileAnalyzer file(FILE_LOCATION);
	const int vertexSize = file.getVertexSize();

	// file.printVertexArray(); // Use this to print all the vertices

	profileHeight = file.getProfileCurveLength();

	if (file.isTranslational)
		trajLength = file.getTrajectoryCurveLength();
	else
		trajLength = file.getRotationSpan();



	// cout << "Profile Height: " << profileHeight << " " << trajLength << endl;


	vector<GLfloat> vertices;
	for (int i = 0; i < file.getVertexSize(); i++) {
		vertices.push_back(file.getVertex(i));
	}

	// cout << "TEST: " << vertexSize << endl;
	// cout << "TEST: " << vertices.size() << endl;



	vector<GLuint> indices;

	// Generate Indices based on number of Profile and Trajectory Curve Points
	for (int k = 0; k < trajLength; k++) {
		for (int i = 1; i < profileHeight; i++) {

			if (file.isObjectTranslational() && (k < trajLength - 1)) {
				indices.push_back((GLuint)(i - 1 + (profileHeight * k)));
				indices.push_back((GLuint)(i + (profileHeight * k)));
				indices.push_back((GLuint)(i - 1 + profileHeight + (profileHeight * k)));
				indices.push_back((GLuint)(i + (profileHeight * k)));				
				indices.push_back((GLuint)(i + profileHeight + (profileHeight * k))); // 5th
				indices.push_back((GLuint)(i - 1 + profileHeight + (profileHeight * k))); // 6th
				
			}

			else if(!file.isObjectTranslational()) {
				indices.push_back(pushValueLimit((GLuint)(i - 1 + (profileHeight * k))));
				indices.push_back(pushValueLimit((GLuint)(i + (profileHeight * k))));
				indices.push_back(pushValueLimit((GLuint)(i - 1 + profileHeight + (profileHeight * k))));
				indices.push_back(pushValueLimit((GLuint)(i + (profileHeight * k))));
				indices.push_back(pushValueLimit((GLuint)(i + profileHeight + (profileHeight * k)))); // 5th
				indices.push_back(pushValueLimit((GLuint)(i - 1 + profileHeight + (profileHeight * k)))); // 6th
			}	
		}	
	}
	
	// Function to Print all Indices
	/*for (int i = 0; i < indices.size(); i++) {
		if (i % 3 == 0)
			cout << endl;
		cout << indices.at(i) << " ";
	}*/


	// ========== Creating our Vertex Buffer Obj, Vertex Array Obj ========== //

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO); // Binds the VAO

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);	// Copy our vertices to the buffer
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  3 * sizeof(GLfloat), (GLvoid*)0);	// Set the vertex attribute pointers
	glEnableVertexAttribArray(0);

	//Creating the EBO
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);
	

	glBindVertexArray(0); // Unbinds the VAO

	
	model = glm::rotate(model, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // Set the initial model settings

	// Variables to be used in the main loop
	int minimum;
	int difference; 

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable to see polygon lines



						  // =============== Game Loop ================= //

	while (!glfwWindowShouldClose(window)) {

		// Check and call events
		glfwPollEvents();
		glfwSetKeyCallback(window, key_callback);
		glfwSetScrollCallback(window, scroll_callback);
	
		// Camera movement settings
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		do_movement();

		// Resizes the window 
		minimum = min(width, height);
		difference = (width - height) / 2;
		glfwGetFramebufferSize(window, &width, &height);

		if (width > height) 
			glViewport(difference, 0, minimum, minimum);
		else
			glViewport(0, difference, minimum, minimum);
		
		
		// Rendering Commands
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourShader.Use();

		// Projection, View, Model 
		projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc  = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc  = glGetUniformLocation(ourShader.Program, "projection");


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// Drawing the Triangle
		glBindVertexArray(VAO);
		glDrawElements(drawMode, vertices.size() * 2, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Frees up the buffers when done 
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}
// ========== End of Main Method ========== //


// Event handler on key press
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_ENTER) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_P  && action == GLFW_PRESS)
		drawMode = GL_POINTS;

	if (key == GLFW_KEY_L  && action == GLFW_PRESS)
		drawMode = GL_LINES;

	if (key == GLFW_KEY_T  && action == GLFW_PRESS)
		drawMode = GL_TRIANGLES;

	if (key == GLFW_KEY_UP  && action == GLFW_PRESS)
		model = glm::rotate(model, -25.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	if (key == GLFW_KEY_DOWN  && action == GLFW_PRESS)
		model = glm::rotate(model, 25.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) 
		model = glm::rotate(model, -25.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	if (key == GLFW_KEY_RIGHT  && action == GLFW_PRESS) 
		model = glm::rotate(model, 25.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	
	// Smooth camera transitions
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

// Event handler on scroll wheel
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

// Camera movement controls
void do_movement() {

	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// Function to limit the indices for the mesh to connect the end with the front 
GLfloat pushValueLimit(GLuint value) {

	if (value >= (profileHeight * (trajLength )))
		return value - (profileHeight * (trajLength));
	return value;
}

