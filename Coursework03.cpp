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
bool isSelfRotateRing = false;
float ringRotationAngle = 0.0f; // 定义旋转角度变量

// 圆锥参数
const float radius = 0.5f; // 底面半径
const float height = 1.0f; // 高度
const int segments_cone = 128;   // 圆的分段数

std::vector<float> coneVertices;

// 生成底面和侧面顶点
void generateConeVertices() {
	// 底面圆心点（用于绘制三角形扇）
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
	// 底面顶点
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

	// 侧面顶点
	for (int i = 0; i <= segments_cone; ++i) {
		float theta = 2.0f * M_PI * i / segments_cone;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		float nx = cos(theta);
		float nz = sin(theta);
		float ny = radius / height;

		// 底部顶点
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

		// 锥顶点
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

// 圆环参数
const float majorRadius = 0.4f;  // 主半径（中心到圆心的距离）
const float minorRadius = 0.1f;  // 次半径（圆环粗细）
const int segmentsMajor = 64;    // 主环分段数
const int segmentsMinor = 32;    // 次环分段数

std::vector<float> torusVertices;
std::vector<unsigned int> torusIndices;

// 生成圆环顶点和索引
void generateTorusVertices() {
	for (int i = 0; i <= segmentsMajor; ++i) {
		float theta = 2.0f * M_PI * i / segmentsMajor; // 主环角度
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (int j = 0; j <= segmentsMinor; ++j) {
			float phi = 2.0f * M_PI * j / segmentsMinor; // 次环角度
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			// 顶点位置
			float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
			float y = (majorRadius + minorRadius * cosPhi) * sinTheta;
			float z = minorRadius * sinPhi;

			// 法线
			float nx = cosPhi * cosTheta;
			float ny = cosPhi * sinTheta;
			float nz = sinPhi;

			// 归一化法线
			float length = sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			// 顶点颜色
			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;

			// UV坐标
			float u = (float)i / segmentsMajor;
			float v = (float)j / segmentsMinor;


			// 添加顶点数据
			torusVertices.push_back(x);  // 位置
			torusVertices.push_back(y);
			torusVertices.push_back(z);
			torusVertices.push_back(r);  // 颜色
			torusVertices.push_back(g);
			torusVertices.push_back(b);
			torusVertices.push_back(nx); // 法线
			torusVertices.push_back(ny);
			torusVertices.push_back(nz);
			torusVertices.push_back(u);  // UV
			torusVertices.push_back(v);
		}
	}

	// 生成索引数据
	for (int i = 0; i < segmentsMajor; ++i) {
		for (int j = 0; j < segmentsMinor; ++j) {
			unsigned int first = i * (segmentsMinor + 1) + j;
			unsigned int second = first + segmentsMinor + 1;

			// 三角形 1
			torusIndices.push_back(first);
			torusIndices.push_back(second);
			torusIndices.push_back(first + 1);

			// 三角形 2
			torusIndices.push_back(second);
			torusIndices.push_back(second + 1);
			torusIndices.push_back(first + 1);
		}
	}
}



// 球体参数
const float sphereRadius = 0.1f; // 半径
const int segmentsLatitude = 36; // 纬度分段数
const int segmentsLongitude = 36; // 经度分段数

std::vector<float> sphereVertices; // 顶点数据
std::vector<unsigned int> sphereIndices; // 索引数据

// 生成球体顶点数据
void generateSphereVertices() {
	for (int lat = 0; lat <= segmentsLatitude; ++lat) {
		float theta = M_PI * lat / segmentsLatitude; // 纬度角，从 0 到 π
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= segmentsLongitude; ++lon) {
			float phi = 2.0f * M_PI * lon / segmentsLongitude; // 经度角，从 0 到 2π
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			// 顶点坐标
			float x = sphereRadius * sinTheta * cosPhi;
			float y = sphereRadius * cosTheta;
			float z = sphereRadius * sinTheta * sinPhi;

			// 法线
			float nx = sinTheta * cosPhi;
			float ny = cosTheta;
			float nz = sinTheta * sinPhi;
			/*float nx = x;
			float ny = y;
			float nz = z;*/
				
			// 法线归一化
			float length = sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			// 顶点颜色
			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;
			// UV坐标
			float u = (float)lon / segmentsLongitude;
			float v = (float)lat / segmentsLatitude;
			 
			glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

			// 添加顶点
			sphereVertices.push_back(x);  // 位置
			sphereVertices.push_back(y);
			sphereVertices.push_back(z);
			sphereVertices.push_back(r);  // 颜色
			sphereVertices.push_back(g);
			sphereVertices.push_back(b);
			//sphereVertices.push_back(nx); // 法线
			//sphereVertices.push_back(ny);
			//sphereVertices.push_back(nz);
			sphereVertices.push_back(normal.x); // 法线
			sphereVertices.push_back(normal.y);
			sphereVertices.push_back(normal.z);
			sphereVertices.push_back(u);  // UV
			sphereVertices.push_back(v);
		}
	}

	// 生成索引数据
	for (int lat = 0; lat < segmentsLatitude; ++lat) {
		for (int lon = 0; lon < segmentsLongitude; ++lon) {
			unsigned int first = lat * (segmentsLongitude + 1) + lon;
			unsigned int second = first + segmentsLongitude + 1;

			// 三角形 1
			sphereIndices.push_back(first);
			sphereIndices.push_back(second);
			sphereIndices.push_back(first + 1);

			// 三角形 2
			sphereIndices.push_back(second);
			sphereIndices.push_back(second + 1);
			sphereIndices.push_back(first + 1);
		}
	}
}

// 球体参数
const float sphereRadius_1 = 0.1f; // 半径
const int segmentsLatitude_1 = 36; // 纬度分段数
const int segmentsLongitude_1 = 36; // 经度分段数

std::vector<float> sphereVertices_1; // 顶点数据
std::vector<unsigned int> sphereIndices_1; // 索引数据

// 生成球体顶点数据
void generateSphereVertices_1() {
	for (int lat = 0; lat <= segmentsLatitude_1; ++lat) {
		float theta = M_PI * lat / segmentsLatitude_1; // 纬度角，从 0 到 π
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= segmentsLongitude_1; ++lon) {
			float phi = 2.0f * M_PI * lon / segmentsLongitude_1; // 经度角，从 0 到 2π
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			// 顶点坐标
			float x = sphereRadius_1 * sinTheta * cosPhi;
			float y = sphereRadius_1 * cosTheta;
			float z = sphereRadius_1 * sinTheta * sinPhi;

			// 法线
			float nx = sinTheta * cosPhi;
			float ny = cosTheta;
			float nz = sinTheta * sinPhi;

			// 归一化法线
			float length = sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			// 顶点颜色
			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;
			// UV坐标
			float u = (float)lon / segmentsLongitude_1;
			float v = (float)lat / segmentsLatitude_1;


			// 添加顶点
			sphereVertices_1.push_back(x);  // 位置
			sphereVertices_1.push_back(y);
			sphereVertices_1.push_back(z);
			sphereVertices_1.push_back(r);  // 颜色
			sphereVertices_1.push_back(g);
			sphereVertices_1.push_back(b);
			sphereVertices_1.push_back(nx); // 法线
			sphereVertices_1.push_back(ny);
			sphereVertices_1.push_back(nz);
			sphereVertices_1.push_back(u);  // UV
			sphereVertices_1.push_back(v);
		}
	}

	// 生成索引数据
	for (int lat = 0; lat < segmentsLatitude_1; ++lat) {
		for (int lon = 0; lon < segmentsLongitude_1; ++lon) {
			unsigned int first = lat * (segmentsLongitude_1 + 1) + lon;
			unsigned int second = first + segmentsLongitude_1 + 1;

			// 三角形 1
			sphereIndices_1.push_back(first);
			sphereIndices_1.push_back(second);
			sphereIndices_1.push_back(first + 1);

			// 三角形 2
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
float deltaTime = 0.016f; // 每帧间隔时间

void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		lightDirection = Camera.Front;
		lightPos = Camera.Position;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		// Change between the three types of light
		light_iter = light_iter % 3;
		light_iter++;
	}
	//std::cout << "The camera is: " << (int) isModelViewer << std::endl;
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
		isAutoRotate = !isAutoRotate;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		isSelfRotateRing = !isSelfRotateRing;
	}


	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		isModelViewer = !isModelViewer;  // 切换相机模式

		//// 当切换相机模式时，将相机回到中心
		Camera.Position = glm::vec3(0.0f, 0.0f,3.0f);  // 你可以设置为你想要的初始位置
		Camera.Yaw = -90.0f;  // 设置初始的Yaw角
		Camera.Pitch = 0.0f;  // 设置初始的Pitch角
		Camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));  // 设置相机的前方方向

		// 重新计算Right和Up向量
		Camera.Right = glm::normalize(glm::cross(Camera.Front, Camera.WorldUp));
		Camera.Up = glm::normalize(glm::cross(Camera.Right, Camera.Front));
		//InitCamera(Camera);
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

//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//	if (key == GLFW_KEY_Y) {
//		isAutoRotate = !isAutoRotate; // 切换自动旋转状态
//	}
//}

int main(int argc, char** argv)
{
	GLFWwindow* window = CreateWindow_1(800, 600, "Model Viewer Camera");

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	int numMultiSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &numMultiSamples);
	std::cout << "Max number of samples is " << numMultiSamples << std::endl;

	unsigned int shaderProgram = LoadShader("mvp.vert", "col.frag");

	InitCamera(Camera);
	//Camera.Pitch = -8.5f;
	//Camera.Yaw = -12.f;
	MoveAndOrientCamera(Camera, cube_pos, cam_dist, 0.f, -20.f);
	GLuint cone = setup_texture("cone.bmp");
	GLuint ringball = setup_texture("texture01.bmp");
	GLuint mars = setup_texture("mars.bmp");


// generate cone vertex
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

	// 绑定球体顶点
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

	// 绑定球体顶点
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
		


		glm::mat4 co_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f));
		glm::mat4 co_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // 假设绕 Y 轴旋转
		glm::mat4 co_transformation = co_translation * co_rotation;

		/*glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, cube_pos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		*/
		// Render the cone
		glm::mat4 modelCone = glm::mat4(1.f);

		modelCone = glm::translate(modelCone, cube_pos);
		modelCone = glm::rotate(modelCone, glm::radians(-30.0f), glm::vec3(.0f, .0f, 1.0f));  // 同样旋转
		//modelCone = glm::rotate(modelCone, glm::radians(-40.0f), glm::vec3(1.0f, 1.f, .0f));  // 同样旋转

		//modelCone = modelCone * co_transformation;

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelCone));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cone);
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments_cone + 2);
		glDrawArrays(GL_TRIANGLE_STRIP, segments_cone + 2, (segments_cone + 1) * 2);
		glBindVertexArray(0);

		// print the vertices of modelRing
		glm::mat4 modelRing = glm::mat4(1.f);

		//modelRing = glm::rotate(modelRing, glm::radians(-30.0f), glm::vec3(1.f, 0.f, 0.f));  // 同样的旋转
		modelRing = glm::translate(modelRing, glm::vec3(.25f, .5f, 0.f)); // Translate the ring above the cone
		modelRing = glm::rotate(modelRing, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelRing = glm::rotate(modelRing, glm::radians(-30.0f), glm::vec3(.0f, 1.f, 0.0f));  // 同样旋转
		//modelRing = glm::rotate(modelRing, glm::radians(-50.0f), glm::vec3(1.0f, 1.0f, .0f));  // 同样旋转
		//modelRing = glm::translate(modelRing, glm::vec3(.1f, -.3f, .0f)); // Translate the ring above the cone


		//modelRing = modelRing * co_transformation;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelRing));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ringball);
		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, torusIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);



		// 设置模型矩阵
		glm::mat4 modelSphere = glm::mat4(1.f);
		modelSphere = glm::translate(modelSphere, glm::vec3(0.1f, .7f, 0.f));

		//modelSphere = modelSphere * co_transformation;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere));

		// 绑定 VAO 并绘制球体
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ringball);
		glBindVertexArray(VAO[2]);
		glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// 设置模型矩阵
		glm::mat4 modelSphere_1 = glm::mat4(1.f);
		modelSphere_1 = glm::translate(modelSphere_1, glm::vec3(.0f, 1.f, .5f));
		//modelSphere_1 = modelSphere_1 * co_transformation;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere_1));

		// 绑定 VAO 并绘制球体
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mars);
		glBindVertexArray(VAO[3]);
		glDrawElements(GL_TRIANGLES, sphereIndices_1.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glm::mat4 view = glm::mat4(1.f);
		//view = glm::translate(view, -glm::vec3(0.f, 0.f, 3.f));
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)800 / (float)600, .5f, 100.f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		//std::cout << "The auto state is: " << (int) isAutoRotate << std::endl;
		if (isAutoRotate && isModelViewer) {
			MoveAndOrientCamera(Camera, cube_pos, cam_dist, -.01f, 0.f);
		}
		std::cout << "The selfrotate state is: " << (int)isSelfRotateRing << std::endl;
		if (isSelfRotateRing) {
			ringRotationAngle += 0.01f; // 每帧增加旋转角度
			modelRing = glm::rotate(modelRing, ringRotationAngle, glm::vec3(0.f, 1.f, 0.f));

		}
		//modelRing = glm::rotate(modelRing, ringRotationAngle, glm::vec3(0.f, 1.f, 0.f));


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

