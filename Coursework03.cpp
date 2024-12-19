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

// Բ׶����
const float radius = 0.5f; // ����뾶
const float height = 1.0f; // �߶�
const int segments_cone = 128;   // Բ�ķֶ���

std::vector<float> coneVertices;

// ���ɵ���Ͳ��涥��
void generateConeVertices() {
	// ����Բ�ĵ㣨���ڻ����������ȣ�
	coneVertices.push_back(0.0f); // x
	coneVertices.push_back(0.0f); // y
	coneVertices.push_back(0.0f); // z
	coneVertices.push_back(1.0f); // r
	coneVertices.push_back(1.0f); // g
	coneVertices.push_back(1.0f); // b
	coneVertices.push_back(0.0f); // nx
	coneVertices.push_back(-1.0f); // ny
	coneVertices.push_back(0.0f); // nz

	// ���涥��
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

	// ���涥��
	for (int i = 0; i <= segments_cone; ++i) {
		float theta = 2.0f * M_PI * i / segments_cone;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		float nx = cos(theta);
		float nz = sin(theta);
		float ny = radius / height;

		// �ײ�����
		coneVertices.push_back(x);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(z);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(0.0f);
		coneVertices.push_back(1.0f);
		coneVertices.push_back(nx);
		coneVertices.push_back(ny);
		coneVertices.push_back(nz);

		// ׶����
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

// Բ������
const float majorRadius = 0.4f;  // ���뾶�����ĵ�Բ�ĵľ��룩
const float minorRadius = 0.1f;  // �ΰ뾶��Բ����ϸ��
const int segmentsMajor = 64;    // �����ֶ���
const int segmentsMinor = 32;    // �λ��ֶ���

std::vector<float> torusVertices;
std::vector<unsigned int> torusIndices;

// ����Բ�����������
void generateTorusVertices() {
	for (int i = 0; i <= segmentsMajor; ++i) {
		float theta = 2.0f * M_PI * i / segmentsMajor; // �����Ƕ�
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (int j = 0; j <= segmentsMinor; ++j) {
			float phi = 2.0f * M_PI * j / segmentsMinor; // �λ��Ƕ�
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			// ����λ��
			float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
			float y = (majorRadius + minorRadius * cosPhi) * sinTheta;
			float z = minorRadius * sinPhi;

			// ����
			float nx = cosPhi * cosTheta;
			float ny = cosPhi * sinTheta;
			float nz = sinPhi;

			// ������ɫ
			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;

			// ��Ӷ�������
			torusVertices.push_back(x);  // λ��
			torusVertices.push_back(y);
			torusVertices.push_back(z);
			torusVertices.push_back(r);  // ��ɫ
			torusVertices.push_back(g);
			torusVertices.push_back(b);
			torusVertices.push_back(nx); // ����
			torusVertices.push_back(ny);
			torusVertices.push_back(nz);
		}
	}

	// ������������
	for (int i = 0; i < segmentsMajor; ++i) {
		for (int j = 0; j < segmentsMinor; ++j) {
			unsigned int first = i * (segmentsMinor + 1) + j;
			unsigned int second = first + segmentsMinor + 1;

			// ������ 1
			torusIndices.push_back(first);
			torusIndices.push_back(second);
			torusIndices.push_back(first + 1);

			// ������ 2
			torusIndices.push_back(second);
			torusIndices.push_back(second + 1);
			torusIndices.push_back(first + 1);
		}
	}
}



// �������
const float sphereRadius = 0.1f; // �뾶
const int segmentsLatitude = 36; // γ�ȷֶ���
const int segmentsLongitude = 36; // ���ȷֶ���

std::vector<float> sphereVertices; // ��������
std::vector<unsigned int> sphereIndices; // ��������

// �������嶥������
void generateSphereVertices() {
	for (int lat = 0; lat <= segmentsLatitude; ++lat) {
		float theta = M_PI * lat / segmentsLatitude; // γ�Ƚǣ��� 0 �� ��
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= segmentsLongitude; ++lon) {
			float phi = 2.0f * M_PI * lon / segmentsLongitude; // ���Ƚǣ��� 0 �� 2��
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			// ��������
			float x = sphereRadius * sinTheta * cosPhi;
			float y = sphereRadius * cosTheta;
			float z = sphereRadius * sinTheta * sinPhi;

			// ����
			float nx = sinTheta * cosPhi;
			float ny = cosTheta;
			float nz = sinTheta * sinPhi;

			// ������ɫ
			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;

			// ��Ӷ���
			sphereVertices.push_back(x);  // λ��
			sphereVertices.push_back(y);
			sphereVertices.push_back(z);
			sphereVertices.push_back(r);  // ��ɫ
			sphereVertices.push_back(g);
			sphereVertices.push_back(b);
			sphereVertices.push_back(nx); // ����
			sphereVertices.push_back(ny);
			sphereVertices.push_back(nz);
		}
	}

	// ������������
	for (int lat = 0; lat < segmentsLatitude; ++lat) {
		for (int lon = 0; lon < segmentsLongitude; ++lon) {
			unsigned int first = lat * (segmentsLongitude + 1) + lon;
			unsigned int second = first + segmentsLongitude + 1;

			// ������ 1
			sphereIndices.push_back(first);
			sphereIndices.push_back(second);
			sphereIndices.push_back(first + 1);

			// ������ 2
			sphereIndices.push_back(second);
			sphereIndices.push_back(second + 1);
			sphereIndices.push_back(first + 1);
		}
	}
}

// �������
const float sphereRadius_1 = 0.1f; // �뾶
const int segmentsLatitude_1 = 36; // γ�ȷֶ���
const int segmentsLongitude_1 = 36; // ���ȷֶ���

std::vector<float> sphereVertices_1; // ��������
std::vector<unsigned int> sphereIndices_1; // ��������

// �������嶥������
void generateSphereVertices_1() {
	for (int lat = 0; lat <= segmentsLatitude_1; ++lat) {
		float theta = M_PI * lat / segmentsLatitude_1; // γ�Ƚǣ��� 0 �� ��
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= segmentsLongitude_1; ++lon) {
			float phi = 2.0f * M_PI * lon / segmentsLongitude_1; // ���Ƚǣ��� 0 �� 2��
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			// ��������
			float x = sphereRadius_1 * sinTheta * cosPhi;
			float y = sphereRadius_1 * cosTheta;
			float z = sphereRadius_1 * sinTheta * sinPhi;

			// ����
			float nx = sinTheta * cosPhi;
			float ny = cosTheta;
			float nz = sinTheta * sinPhi;

			// ������ɫ
			float r = 0.5f + 0.5f * nx;
			float g = 0.5f + 0.5f * ny;
			float b = 0.5f + 0.5f * nz;

			// ��Ӷ���
			sphereVertices_1.push_back(x);  // λ��
			sphereVertices_1.push_back(y);
			sphereVertices_1.push_back(z);
			sphereVertices_1.push_back(r);  // ��ɫ
			sphereVertices_1.push_back(g);
			sphereVertices_1.push_back(b);
			sphereVertices_1.push_back(nx); // ����
			sphereVertices_1.push_back(ny);
			sphereVertices_1.push_back(nz);
		}
	}

	// ������������
	for (int lat = 0; lat < segmentsLatitude_1; ++lat) {
		for (int lon = 0; lon < segmentsLongitude_1; ++lon) {
			unsigned int first = lat * (segmentsLongitude_1 + 1) + lon;
			unsigned int second = first + segmentsLongitude_1 + 1;

			// ������ 1
			sphereIndices_1.push_back(first);
			sphereIndices_1.push_back(second);
			sphereIndices_1.push_back(first + 1);

			// ������ 2
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
	int numMultiSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &numMultiSamples);
	std::cout << "Max number of samples is " << numMultiSamples << std::endl;

	unsigned int shaderProgram = LoadShader("mvp.vert", "col.frag");

	InitCamera(Camera);
	//Camera.Pitch = -8.5f;
	//Camera.Yaw = -12.f;
	MoveAndOrientCamera(Camera, cube_pos, cam_dist, 0.f, 0.f);

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
	glBufferData(GL_ARRAY_BUFFER, torusVertices.size() * sizeof(float), &torusVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, torusIndices.size() * sizeof(unsigned int), &torusIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// �����嶥��
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// �����嶥��
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
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
		modelRing = glm::rotate(modelRing, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelRing));

		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, torusIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);



		// ����ģ�;���
		glm::mat4 modelSphere = glm::mat4(1.f);
		modelSphere = glm::translate(modelSphere, glm::vec3(0.28f, .65f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere));

		// �� VAO ����������
		glBindVertexArray(VAO[2]);
		glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// ����ģ�;���
		glm::mat4 modelSphere_1 = glm::mat4(1.f);
		modelSphere_1 = glm::translate(modelSphere_1, glm::vec3(1.0f, 1.5f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere_1));

		// �� VAO ����������
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
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

