#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>

#include "ModelViewerCamera.h"
#include "FlyThroughCamera.h"
#include "camera.h"
#include "shader.h"
#include "window.h"
#include "texture.h"
#include <corecrt_math_defines.h>
#include <vector>


boolean isModelViewer = true;
bool isAutoRotate = false;
int light_iter = 1;
boolean isLightOn = true;
bool isSelfRotateRing = false;
float ringRotationAngle = 0.0f;

float cone_scalar_factor = 1.0f;
float torus_scalar_factor = 1.0f;
float sphere_scalar_factor = 1.0f;
float mars_scalar_factor = 1.0f;

const float radius = 0.5f;
const float height = 1.0f;
const int segments_cone = 128;

std::vector<float> coneVertices;

void generateConeVertices() {
	coneVertices.push_back(0.0f); // x
	coneVertices.push_back(0.0f); // y
	coneVertices.push_back(0.0f); // z
	coneVertices.push_back(1.0f); // r
	coneVertices.push_back(1.0f); // g
	coneVertices.push_back(1.0f); // b
	coneVertices.push_back(0.0f); // nx
	coneVertices.push_back(-1.0f); // ny
	coneVertices.push_back(0.0f); // nz
	coneVertices.push_back(0.5f); // u
	coneVertices.push_back(0.5f); // v
	//bottom
	for (int i = 0; i <= segments_cone; ++i) {
		float theta = 2.0f * M_PI * i / segments_cone;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		coneVertices.push_back(x); // x
		coneVertices.push_back(0.0f); // y
		coneVertices.push_back(z); // z
		coneVertices.push_back(0.0f); // r
		coneVertices.push_back(1.0f); // g
		coneVertices.push_back(0.0f); // b
		coneVertices.push_back(0.0f); // nx
		coneVertices.push_back(-1.0f); // ny
		coneVertices.push_back(0.0f); // nz
		coneVertices.push_back((x / radius + 1.0f) * 0.5f); // u
		coneVertices.push_back((z / radius + 1.0f) * 0.5f); // v
	}

	// side points
	for (int i = 0; i <= segments_cone; ++i) {
		float theta = 2.0f * M_PI * i / segments_cone;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		float nx = cos(theta);
		float nz = sin(theta);
		float ny = radius / height;

		// bottom
		coneVertices.push_back(x);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(z);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(1.0f);
		coneVertices.push_back(nx);
		coneVertices.push_back(ny);
		coneVertices.push_back(nz);
		coneVertices.push_back((float)i / segments_cone); // u
		coneVertices.push_back(0.0f); // v

		// top
		coneVertices.push_back(0.0f);
		coneVertices.push_back(height);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(1.0f);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(nx);
		coneVertices.push_back(ny);
		coneVertices.push_back(nz);
		coneVertices.push_back((float)i / segments_cone); // u
		coneVertices.push_back(1.0f); // v
	}
}

// torus coefficient
const float majorRadius = 0.4f;  
const float minorRadius = 0.1f;  
const int segmentsMajor = 64;    
const int segmentsMinor = 32; 

std::vector<float> torusVertices;
std::vector<unsigned int> torusIndices;

void generateTorusVertices() {
	for (int i = 0; i <= segmentsMajor; ++i) {
		float theta = 2.0f * M_PI * i / segmentsMajor; 
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (int j = 0; j <= segmentsMinor; ++j) {
			float phi = 2.0f * M_PI * j / segmentsMinor;
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
			float y = (majorRadius + minorRadius * cosPhi) * sinTheta;
			float z = minorRadius * sinPhi;

			float nx = cosPhi * cosTheta;
			float ny = cosPhi * sinTheta;
			float nz = sinPhi;

			float length = sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;

			float u = (float)i / segmentsMajor;
			float v = (float)j / segmentsMinor;


			torusVertices.push_back(x);
			torusVertices.push_back(y);
			torusVertices.push_back(z);
			torusVertices.push_back(r);
			torusVertices.push_back(g);
			torusVertices.push_back(b);
			torusVertices.push_back(nx);
			torusVertices.push_back(ny);
			torusVertices.push_back(nz);
			torusVertices.push_back(u);
			torusVertices.push_back(v);
		}
	}

	for (int i = 0; i < segmentsMajor; ++i) {
		for (int j = 0; j < segmentsMinor; ++j) {
			unsigned int first = i * (segmentsMinor + 1) + j;
			unsigned int second = first + segmentsMinor + 1;

			torusIndices.push_back(first);
			torusIndices.push_back(second);
			torusIndices.push_back(first + 1);

			torusIndices.push_back(second);
			torusIndices.push_back(second + 1);
			torusIndices.push_back(first + 1);
		}
	}
}



const float sphereRadius = 0.1f;
const int segmentsLatitude = 36;
const int segmentsLongitude = 36;

std::vector<float> sphereVertices;
std::vector<unsigned int> sphereIndices;

void generateSphereVertices() {
	for (int lat = 0; lat <= segmentsLatitude; ++lat) {
		float theta = M_PI * lat / segmentsLatitude;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= segmentsLongitude; ++lon) {
			float phi = 2.0f * M_PI * lon / segmentsLongitude;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = sphereRadius * sinTheta * cosPhi;
			float y = sphereRadius * cosTheta;
			float z = sphereRadius * sinTheta * sinPhi;

			float nx = sinTheta * cosPhi;
			float ny = cosTheta;
			float nz = sinTheta * sinPhi;

				
			float length = sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;
			float u = (float)lon / segmentsLongitude;
			float v = (float)lat / segmentsLatitude;
			 
			glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

			sphereVertices.push_back(x);
			sphereVertices.push_back(y);
			sphereVertices.push_back(z);
			sphereVertices.push_back(r);
			sphereVertices.push_back(g);
			sphereVertices.push_back(b);
			
			sphereVertices.push_back(normal.x);
			sphereVertices.push_back(normal.y);
			sphereVertices.push_back(normal.z);
			sphereVertices.push_back(u);
			sphereVertices.push_back(v);
		}
	}

	for (int lat = 0; lat < segmentsLatitude; ++lat) {
		for (int lon = 0; lon < segmentsLongitude; ++lon) {
			unsigned int first = lat * (segmentsLongitude + 1) + lon;
			unsigned int second = first + segmentsLongitude + 1;

			// Triangle 1
			sphereIndices.push_back(first);
			sphereIndices.push_back(second);
			sphereIndices.push_back(first + 1);

			// Triangle 2
			sphereIndices.push_back(second);
			sphereIndices.push_back(second + 1);
			sphereIndices.push_back(first + 1);
		}
	}
}

// mars sphere
const float sphereRadius_1 = 0.1f;
const int segmentsLatitude_1 = 36;
const int segmentsLongitude_1 = 36;

std::vector<float> sphereVertices_1;
std::vector<unsigned int> sphereIndices_1;


void generateSphereVertices_1() {
	for (int lat = 0; lat <= segmentsLatitude_1; ++lat) {
		float theta = M_PI * lat / segmentsLatitude_1;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= segmentsLongitude_1; ++lon) {
			float phi = 2.0f * M_PI * lon / segmentsLongitude_1; 
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = sphereRadius_1 * sinTheta * cosPhi;
			float y = sphereRadius_1 * cosTheta;
			float z = sphereRadius_1 * sinTheta * sinPhi;

			float nx = sinTheta * cosPhi;
			float ny = cosTheta;
			float nz = sinTheta * sinPhi;

			float length = sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;

			float u = (float)lon / segmentsLongitude_1;
			float v = (float)lat / segmentsLatitude_1;


			sphereVertices_1.push_back(x);
			sphereVertices_1.push_back(y);
			sphereVertices_1.push_back(z);
			sphereVertices_1.push_back(r);
			sphereVertices_1.push_back(g);
			sphereVertices_1.push_back(b);
			sphereVertices_1.push_back(nx);
			sphereVertices_1.push_back(ny);
			sphereVertices_1.push_back(nz);
			sphereVertices_1.push_back(u);
			sphereVertices_1.push_back(v);
		}
	}

	// Add indices
	for (int lat = 0; lat < segmentsLatitude_1; ++lat) {
		for (int lon = 0; lon < segmentsLongitude_1; ++lon) {
			unsigned int first = lat * (segmentsLongitude_1 + 1) + lon;
			unsigned int second = first + segmentsLongitude_1 + 1;

			// Triangle 1
			sphereIndices_1.push_back(first);
			sphereIndices_1.push_back(second);
			sphereIndices_1.push_back(first + 1);

			// Triangle 2
			sphereIndices_1.push_back(second);
			sphereIndices_1.push_back(second + 1);
			sphereIndices_1.push_back(first + 1);
		}
	}
}


glm::vec3 cube_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lightDirection = glm::vec3(0.1f, -.81f, -.61f);
glm::vec3 lightPos = glm::vec3(2.f, 6.f, 7.f);
SCamera Camera;
float deltaTime = 0.016f;

void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isLightOn)
	{
		lightDirection = Camera.Front;
		lightPos = Camera.Position;
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && isLightOn)
	{
		light_iter = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && isLightOn)
	{
		light_iter = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && isLightOn)
	{
		light_iter = 3;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		isLightOn = false;
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		isLightOn = true;
	}
	bool cam_changed = false;
	float x = 0.f, y = 0.f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && isModelViewer)
	{
		x = .5f;
		y = 0.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && isModelViewer)
	{
		cam_dist += 0.005f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && isModelViewer)
	{
		cam_dist -= 0.005f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && isModelViewer)
	{
		x = -.5f;
		y = 0.f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && isModelViewer)
	{
		x = 0.f;
		y = .5f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && isModelViewer)
	{
		x = 0.f;
		y = -.5f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !isModelViewer)
	{
		MoveCamera(Camera, SCamera::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !isModelViewer)
	{
		MoveCamera(Camera, SCamera::BACKWARD, deltaTime);

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !isModelViewer)
	{
		MoveCamera(Camera, SCamera::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !isModelViewer)
	{
		MoveCamera(Camera, SCamera::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		isAutoRotate = true;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		isAutoRotate = false;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && isModelViewer)
	{
		cone_scalar_factor += 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && isModelViewer)
	{
		cone_scalar_factor -= 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && isModelViewer)
	{
		torus_scalar_factor += 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && isModelViewer)
	{
		torus_scalar_factor -= 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && isModelViewer)
	{
		sphere_scalar_factor += 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && isModelViewer)
	{
		sphere_scalar_factor -= 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && isModelViewer)
	{
		mars_scalar_factor += 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && isModelViewer)
	{
		mars_scalar_factor -= 0.005f;
	}




	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		isModelViewer = true;

		// Change Camera to the default start state
		Camera.Position = glm::vec3(0.0f, 0.0f,3.0f);
		Camera.Yaw = -90.0f;  
		Camera.Pitch = 0.0f;
		Camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));

		
		Camera.Right = glm::normalize(glm::cross(Camera.Front, Camera.WorldUp));
		Camera.Up = glm::normalize(glm::cross(Camera.Right, Camera.Front));
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		isModelViewer = false;

		// Change Camera to the default start state
		Camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
		Camera.Yaw = -90.0f;
		Camera.Pitch = 0.0f;
		Camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));


		Camera.Right = glm::normalize(glm::cross(Camera.Front, Camera.WorldUp));
		Camera.Up = glm::normalize(glm::cross(Camera.Right, Camera.Front));
	}
	if (cam_changed && isModelViewer)
	{
		MoveAndOrientCamera(Camera, cube_pos, cam_dist, x, y);
	}
	
}
bool firstMouse = true;
float prevMouseX;
float prevMouseY;

void processMouse(GLFWwindow* window, double x, double y)
{
	if (firstMouse)
	{
		prevMouseX = x;
		prevMouseY = y;
		firstMouse = false;
	}

	float dX = x - prevMouseX;
	float dY = y - prevMouseY;

	prevMouseX = x;
	prevMouseY = y;

	OrientCamera(Camera, dX, dY, deltaTime);
}


int main(int argc, char** argv)
{
	GLFWwindow* window = CreateWindow_1(800, 600, "Model Viewer Camera");

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	int numMultiSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &numMultiSamples);

	unsigned int shaderProgram = LoadShader("mvp.vert", "col.frag");

	InitCamera(Camera);

	MoveAndOrientCamera(Camera, cube_pos, cam_dist, 0.f, -20.f);
	GLuint cone = setup_texture("cone.bmp");
	GLuint ringball = setup_texture("texture01.bmp");
	GLuint mars = setup_texture("mars.bmp");


	// generate cone vertex for the objects
	generateConeVertices();
	generateTorusVertices();
	generateSphereVertices();
	generateSphereVertices_1();


	unsigned int VAO[4];
	glGenVertexArrays(1, &VAO[0]);
	glGenVertexArrays(1, &VAO[1]);
	glGenVertexArrays(1, &VAO[2]);
	glGenVertexArrays(1, &VAO[3]);

	unsigned int EBO[3];
	glGenBuffers(1, &EBO[0]);
	glGenBuffers(1, &EBO[1]);
	glGenBuffers(1, &EBO[2]);

	unsigned int VBO[4];
	glGenBuffers(1, &VBO[0]);
	glGenBuffers(1, &VBO[1]);
	glGenBuffers(1, &VBO[2]);
	glGenBuffers(1, &VBO[3]);

	// Bind vertices for Cone
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coneVertices.size(), &coneVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Bind vertices for Torus
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, torusVertices.size() * sizeof(float), &torusVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, torusIndices.size() * sizeof(unsigned int), &torusIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Bind vertices for sphere
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Bind vertices for mars
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);


	while (!glfwWindowShouldClose(window))
	{
		//std::cout << "Light iter: " << light_iter << std::endl;
		processKeyboard(window);
		if (!isModelViewer)
		{
			glfwSetCursorPosCallback(window, processMouse);
		}
		
		glClearColor(.902f, .863f, .902f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUniform3f(glGetUniformLocation(shaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "lightColour"), 1.f, 1.f, 1.f);
		glUniform3f(glGetUniformLocation(shaderProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		glUniform1f(glGetUniformLocation(shaderProgram, "light_decider"), light_iter);
		//std::cout << "The light is :" << (int)isLightOn << std::endl;
		glUniform1i(glGetUniformLocation(shaderProgram, "isLightOn"), (int) isLightOn);
		

		// Draw the cone
		glm::mat4 modelCone = glm::mat4(1.f);

		modelCone = glm::rotate(modelCone, glm::radians(-30.0f), glm::vec3(.0f, .0f, 1.0f)); 
		modelCone = glm::scale(modelCone, glm::vec3(cone_scalar_factor, cone_scalar_factor, cone_scalar_factor));
		//modelCone = glm::rotate(modelCone, glm::radians(-40.0f), glm::vec3(.0f, 0.f, .0f));

		//modelCone = modelCone * co_transformation;

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelCone));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cone);
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments_cone + 2);
		glDrawArrays(GL_TRIANGLE_STRIP, segments_cone + 2, (segments_cone + 1) * 2);
		glBindVertexArray(0);

		// Draw Torus(Ring)
		glm::mat4 modelRing = glm::mat4(1.f);

		modelRing = glm::translate(modelRing, glm::vec3(.25f, .5f, 0.f)); // Translate the ring above the cone
		modelRing = glm::rotate(modelRing, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelRing = glm::rotate(modelRing, glm::radians(-30.0f), glm::vec3(.0f, 1.f, 0.0f));
		modelRing = glm::scale(modelRing, glm::vec3(torus_scalar_factor, torus_scalar_factor, torus_scalar_factor));


		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelRing));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ringball);
		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, torusIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		// Draw Sphere
		glm::mat4 modelSphere = glm::mat4(1.f);
		modelSphere = glm::translate(modelSphere, glm::vec3(0.1f, .7f, 0.f));
		modelSphere = glm::scale(modelSphere, glm::vec3(sphere_scalar_factor, sphere_scalar_factor, sphere_scalar_factor));

		//modelSphere = glm::rotate(modelSphere, glm::radians(-50.0f), glm::vec3(.0f, 0.0f, .0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ringball);
		glBindVertexArray(VAO[2]);
		glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Draw Mars
		glm::mat4 modelSphere_1 = glm::mat4(1.f);
		modelSphere_1 = glm::translate(modelSphere_1, glm::vec3(.0f, 1.f, .5f));
		modelSphere_1 = glm::scale(modelSphere_1, glm::vec3(mars_scalar_factor, mars_scalar_factor, mars_scalar_factor));

		//modelSphere_1 = glm::rotate(modelSphere_1, glm::radians(-50.0f), glm::vec3(.0f, 0.0f, .0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere_1));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mars);
		glBindVertexArray(VAO[3]);
		glDrawElements(GL_TRIANGLES, sphereIndices_1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)800 / (float)600, .5f, 100.f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		if (isAutoRotate && isModelViewer) {
			MoveAndOrientCamera(Camera, cube_pos, cam_dist, -.01f, 0.f);
		}
		if (isSelfRotateRing) {
			ringRotationAngle += 0.01f;
			modelRing = glm::rotate(modelRing, ringRotationAngle, glm::vec3(0.f, 1.f, 0.f));

		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

