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
const int segments_cone = 36;   // Բ�ķֶ���

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

const float innerRadius = 0.3f;
const float outerRadius = 0.5f;
const int segments_ring = 10000;
const int segments_circle = 36; // Բ�ν���ķֶ���

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

			// ��ת����Բ
			float rotatedX = (innerRadius + outerRadius) / 2.0f * cosTheta + x * cosTheta - z * sinTheta;
			float rotatedY = (innerRadius + outerRadius) / 2.0f * sinTheta + x * sinTheta + z * cosTheta;

			// ���㷨��
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
		if (i > 0 && i <segments_ring) {
			// �ظ����һ������
			ringVertices.insert(ringVertices.end(), ringVertices.end() - 9, ringVertices.end());
		}
	}
}



const float sphereRadius = 0.25f; // ����뾶
const int segments_lat = 36;    // γ�߷ֶ���
const int segments_lon = 36;    // ���߷ֶ���

std::vector<float> sphereVertices;

// �������嶥��
void generateSphereVertices() {

	float x, y, z;
	float xy; // �뾶Ϊ 1 �� xy ƽ���ϵĳ���
	float nx, ny, nz, lenghthInv = 1.f / radius; // ����
	//float s, t; // ��������

	float sectorStep = 2 * M_PI / segments_lon;
	float stackStep = M_PI / segments_lat;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= segments_lat; ++i) {
		stackAngle = M_PI / 2 - i * stackStep;
		xy = sphereRadius * cos(stackAngle);
		z = sphereRadius * sin(stackAngle);

		for (int j = 0; j <= segments_lon; ++j) {
			sectorAngle = j * sectorStep;

			x = xy * cos(sectorAngle);
			y = xy * sin(sectorAngle);
			nx = x * lenghthInv;
			ny = y * lenghthInv;
			nz = z * lenghthInv;

			sphereVertices.push_back(x);
			sphereVertices.push_back(y);
			sphereVertices.push_back(z);
			sphereVertices.push_back(fabs(x));  // ��ɫ
			sphereVertices.push_back(fabs(y));
			sphereVertices.push_back(fabs(z));
			sphereVertices.push_back(nx);      // ����
			sphereVertices.push_back(ny);
			sphereVertices.push_back(nz);
		}

		// �ظ������յ㣬�����϶
		if (i > 0 && i < segments_lat) {
			// �ظ����һ������
			sphereVertices.insert(sphereVertices.end(), sphereVertices.end() - 9, sphereVertices.end());
		}
	}
	//for (int i = 0; i <= segments_lat; ++i) {

	//	stackAngle = M_PI / 2 - i * stackStep; // �ӱ������ϼ�
	//	xy = sphereRadius * cos(stackAngle); // �� xy ƽ���ϵİ뾶
	//	z = sphereRadius * sin(stackAngle); // z ����

	//	for (int j = 0; j <= segments_lon; ++j) {
	//		sectorAngle = j * sectorStep; // ���߽Ƕ�

	//		// ���㶥������
	//		x = xy * cos(sectorAngle);
	//		y = xy * sin(sectorAngle);

	//		nx = x * lenghthInv;
	//		ny = y * lenghthInv;
	//		nz = z * lenghthInv;

	//		//// ������������
	//		//s = (float)j / segments_lon;
	//		//t = (float)i / segments_lat;

	//		// ����λ��
	//		sphereVertices.push_back(x);
	//		sphereVertices.push_back(y);
	//		sphereVertices.push_back(z);

	//		// ��ɫ����ʹ��λ��ӳ�䣩
	//		sphereVertices.push_back(fabs(x));
	//		sphereVertices.push_back(fabs(y));
	//		sphereVertices.push_back(fabs(z));

	//		sphereVertices.push_back(nx);
	//		sphereVertices.push_back(ny);
	//		sphereVertices.push_back(nz);
	//	}
	//}

	//for (int i = 0; i <= segments_lat; ++i) {
	//	float theta = M_PI * i / segments_lat;
	//	float sinTheta = sin(theta);
	//	float cosTheta = cos(theta);

	//	for (int j = 0; j <= segments_lon; ++j) {
	//		float phi = 2.0f * M_PI * j / segments_lon;
	//		float sinPhi = sin(phi);
	//		float cosPhi = cos(phi);

	//		float x = sphereRadius * sinTheta * cosPhi;
	//		float y = sphereRadius * cosTheta;
	//		float z = sphereRadius * sinTheta * sinPhi;

	//		// ����λ��
	//		sphereVertices.push_back(x);
	//		sphereVertices.push_back(y);
	//		sphereVertices.push_back(z);

	//		// ��ɫ����ʹ��λ��ӳ�䣩
	//		sphereVertices.push_back(fabs(x));
	//		sphereVertices.push_back(fabs(y));
	//		sphereVertices.push_back(fabs(z));

	//		// ����
	//		sphereVertices.push_back(x / sphereRadius);
	//		sphereVertices.push_back(y / sphereRadius);
	//		sphereVertices.push_back(z / sphereRadius);
	//	}
	//}
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

	// �����嶥��
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);
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
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelRing));

		glBindVertexArray(VAO[1]);
		for (int i = 0; i < segments_ring; ++i) {
			glDrawArrays(GL_TRIANGLES, i * (segments_circle + 1), (segments_circle + 1) * 2);
		}
		glBindVertexArray(0);



		// ����ģ�;���
		glm::mat4 modelSphere = glm::mat4(1.f);
		modelSphere = glm::translate(modelSphere, glm::vec3(0.0f, 1.5f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere));

		// �� VAO ����������
		glBindVertexArray(VAO[2]);
		glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size() / 9);
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

