#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>

#include "ModelViewerCamera.h"
#include "camera.h"
#include "shader.h"
#include "window.h"
#include <corecrt_math_defines.h>
#include <vector>

// 圆锥参数
const float radius = 0.5f; // 底面半径
const float height = 1.0f; // 高度
const int segments_cone = 36;   // 圆的分段数

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
	}
}

const float innerRadius = 0.3f;
const float outerRadius = 0.5f;
const int segments_ring = 10000;
const int segments_circle = 36; // 圆形截面的分段数

std::vector<float> ringVertices;

void generateRingVertices() {
	for (int i = 0; i < segments_ring; ++i) {
		float theta = 2.0f * M_PI * i / segments_ring;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (int j = 0; j < segments_circle; ++j) {
			float phi = 2.0f * M_PI * j / segments_circle;
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			float x = (outerRadius - innerRadius) / 2.0f * cosPhi;
			float y = (outerRadius - innerRadius) / 2.0f * sinPhi;
			float z = 0.0f;

			// 旋转截面圆
			float rotatedX = (innerRadius + outerRadius) / 2.0f * cosTheta + x * cosTheta - z * sinTheta;
			float rotatedY = (innerRadius + outerRadius) / 2.0f * sinTheta + x * sinTheta + z * cosTheta;

			// 计算法线
			float nx = cosPhi * cosTheta;
			float ny = sinPhi;
			float nz = cosPhi * sinTheta;

			ringVertices.push_back(rotatedX); // x
			ringVertices.push_back(y);        // y
			ringVertices.push_back(rotatedY); // z
			ringVertices.push_back(0.0f);     // r
			ringVertices.push_back(1.0f);     // g
			ringVertices.push_back(0.0f);     // b
			ringVertices.push_back(nx);       // nx
			ringVertices.push_back(ny);       // ny
			ringVertices.push_back(nz);       // nz
		}
	}
}



// 球体参数
const float sphereRadius = innerRadius - radius; // 球体半径
const int segments_sphere = 36; // 球体分段数

std::vector<float> sphereVertices;

// 生成球体顶点
void generateSphereVertices() {
	for (int i = 0; i <= segments_sphere; ++i) {
		float theta = i * M_PI / segments_sphere;
		for (int j = 0; j <= segments_sphere; ++j) {
			float phi = j * 2 * M_PI / segments_sphere;
			float x = sphereRadius * sin(theta) * cos(phi);
			float y = sphereRadius * cos(theta);
			float z = sphereRadius * sin(theta) * sin(phi);

			// 计算法线
			float nx = x / sphereRadius;
			float ny = y / sphereRadius;
			float nz = z / sphereRadius;

			sphereVertices.push_back(x);
			sphereVertices.push_back(y);
			sphereVertices.push_back(z);
			sphereVertices.push_back(1.0f); // r
			sphereVertices.push_back(1.0f); // g
			sphereVertices.push_back(0.0f); // b
			sphereVertices.push_back(nx);   // nx
			sphereVertices.push_back(ny);   // ny
			sphereVertices.push_back(nz);   // nz
		}
	}
}


glm::vec3 cube_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lightDirection = glm::vec3(0.1f, -.81f, -.61f);
glm::vec3 lightPos = glm::vec3(2.f, 6.f, 7.f);

SCamera Camera;


void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		lightDirection = Camera.Front;
	}

	bool cam_changed = false;
	float x = 0.f, y = 0.f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		x = .5f;
		y = 0.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		cam_dist += 0.005f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam_dist -= 0.005f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		x = -.5f;
		y = 0.f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		x = 0.f;
		y = .5f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		x = 0.f;
		y = -.5f;
		cam_changed = true;
	}
	if (cam_changed)
	{
		MoveAndOrientCamera(Camera, cube_pos, cam_dist, x, y);
	}

}

int main(int argc, char** argv)
{
	GLFWwindow* window = CreateWindow(800, 600, "Model Viewer Camera");

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	unsigned int shaderProgram = LoadShader("mvp.vert", "col.frag");

	InitCamera(Camera);
	MoveAndOrientCamera(Camera, cube_pos, cam_dist, 0.f, 0.f);

// generate cone vertex
	generateConeVertices();
	generateRingVertices();
	generateSphereVertices();


	unsigned int VAO[3];
	glGenVertexArrays(1, &VAO[0]);
	glGenVertexArrays(1, &VAO[1]);
	glGenVertexArrays(1, &VAO[2]);
	unsigned int VBO[3];
	glGenBuffers(1, &VBO[0]);
	glGenBuffers(1, &VBO[1]);
	glGenBuffers(1, &VBO[2]);


	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coneVertices.size(), &coneVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ringVertices.size(), &ringVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 绑定球体顶点
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);

	while (!glfwWindowShouldClose(window))
	{

		processKeyboard(window);

		glClearColor(.0f, .0f, .0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUniform3f(glGetUniformLocation(shaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "lightColour"), 0.f, 0.f, 1.f);
		glUniform3f(glGetUniformLocation(shaderProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

		/*glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, cube_pos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		*/
		// Render the cone
		glm::mat4 modelCone = glm::mat4(1.f);
		modelCone = glm::translate(modelCone, cube_pos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelCone));

		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments_cone + 2);
		glDrawArrays(GL_TRIANGLE_STRIP, segments_cone + 2, (segments_cone + 1) * 2);
		glBindVertexArray(0);

		// print the vertices of modelRing
		glm::mat4 modelRing = glm::mat4(1.f);
		modelRing = glm::translate(modelRing, glm::vec3(0.0f, .5f, 0.0f)); // Translate the ring above the cone
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelRing));

		glBindVertexArray(VAO[1]);
		for (int i = 0; i < segments_ring; ++i) {
			glDrawArrays(GL_TRIANGLE_STRIP, i * (segments_circle + 1), (segments_circle + 1) * 2);
		}
		glBindVertexArray(0);


		glm::mat4 modelSphere = glm::mat4(1.f);

		// 计算球体的中心位置，使其与圆锥的侧面相切
		float coneSideY = height - sphereRadius / tan(M_PI / segments_cone);
		float coneSideX = radius - sphereRadius;

		// 计算球体的中心位置，使其与环的内侧相切
		float ringInnerX = innerRadius - sphereRadius;

		modelSphere = glm::translate(modelSphere, glm::vec3(ringInnerX, coneSideY, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere));
		glBindVertexArray(VAO[2]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (segments_sphere + 1) * (segments_sphere + 1));
		glBindVertexArray(0);


		glm::mat4 view = glm::mat4(1.f);
		//view = glm::translate(view, -glm::vec3(0.f, 0.f, 3.f));
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)800 / (float)600, .5f, 100.f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

