#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "ModelShader.h"
#include "model.h"
#include<string>
#include<math.h>
#include <iostream>
#include<algorithm>
void hw7_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void hw7_processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void renderScene(const CubeShader &shader, Shader& ModelShader, Model& ourModel);
void renderCube();
void initPlaneVAO();
void getMapVBOAndTexture(unsigned int& depthMapFBO, unsigned int& depthMap, const unsigned int SHADOW_WIDTH, const unsigned int SHADOW_HEIGHT);
//void renderQuad();

// 设置
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 600;
unsigned int curWidth = SCR_WIDTH;
unsigned int curHeight = SCR_HEIGHT;
float aspectRatio = float(curWidth) / float(curHeight);
// 摄影机
float cameraPos[] = { 0.0f, 3.0f, 6.0f };
Camera camera(glm::vec3(cameraPos[0], cameraPos[1], cameraPos[2]));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// 控制第一人称视角，或者自由移动摄像机
bool cursorInCenter = false;
// 用于实现摄像机运行速度一致
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int planeVAO;
unsigned int planeVBO;

unsigned int woodTexture;
unsigned int wall_red_Texture;
unsigned int wall_grey_Texture;
unsigned int sky_Texture;
int main()
{
	// 初始化和创建窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, hw7_framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// 使用glad加载opengl的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	// 创建一个自定义Shader，读入着色器文件
	CubeShader lightShader("task6_verShader_lamp.txt", "task6_fraShader_lamp.txt");
	CubeShader sceneShader("hw7_shadow_map.vs", "hw7_shadow_map.fs");
	CubeShader getDepthMapShader("hw7_get_depth_map.vs", "hw7_get_depth_map.fs");
	CubeShader skyShader("skyShader.vs", "skyShader.fs");

	Shader ModelShader("1.model_loading.vs", "1.model_loading.fs");		
	Model ourModel("./YHY_JZ_DGM_05_LST_YSZND/YHY_JZ_DGM_05_LST_YSZND.obj");
	//生成Plane的Buffer对象，并设置Plane的VAO
	initPlaneVAO();
	
	woodTexture = loadTexture("./3.jpg");
	sky_Texture = loadTexture("./sk3.jpg");
	wall_red_Texture = loadTexture("./wall_red.jpg");
	wall_grey_Texture = loadTexture("./wall_grey.jpg");
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO, depthMap;
	//生成Texture对象，并设置Map的FBO
	getMapVBOAndTexture(depthMapFBO, depthMap, SHADOW_WIDTH, SHADOW_HEIGHT);

	//设置shader属性
	sceneShader.use();
	sceneShader.setInt("diffuseTexture", 0);
	sceneShader.setInt("shadowMap", 1);

	bool orthoProjection = true;
	// 初始化ImGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();
	glm::vec3 lightPos(-40.0f, 40.0f, 20.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//使用户可以关闭窗口
		hw7_processInput(window);
		ImGui_ImplGlfwGL3_NewFrame();
		//功能选项
		ImGui::Begin("Menu");
		ImGui::Checkbox("Orthogonal/Perspective projection", &orthoProjection);
		ImGui::End();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 100.5f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		if (orthoProjection) {
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		}
		else {
			lightProjection = glm::perspective(85.0f, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 3.0f, 120.0f);
		}
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		getDepthMapShader.use();
		getDepthMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		renderScene(getDepthMapShader, ModelShader, ourModel);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//render sky
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sky_Texture);
		skyShader.use();
		glm::mat4 skyProjection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 skyView = camera.GetViewMatrix();
		glm::mat4 skyModel;
		//		skyModel = glm::scale(skyModel, glm::vec3(0.2f));
		skyModel = glm::translate(skyModel, camera.Position);
		skyShader.setMat4("model", skyModel);
		skyShader.setMat4("projection", skyProjection);
		skyShader.setMat4("view", skyView);
		renderCube();

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		sceneShader.use();
		glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		sceneShader.setMat4("projection", projection);
		sceneShader.setMat4("view", view);
		// set light uniforms
		sceneShader.setVec3("viewPos", camera.Position);
		sceneShader.setVec3("lightPos", lightPos);
		sceneShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(sceneShader, ModelShader, ourModel);

		// 3. render the lamp
		lightShader.use();
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::translate(model, lightPos);
		glm::vec3 lightColor = glm::vec3(1.0f);
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		lightShader.setMat4("model", model);
		lightShader.setVec3("lightColor", lightColor);
		renderCube();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		// 所谓双重缓存，替换掉旧的缓存，呈现新界面
		glfwSwapBuffers(window);
	}

	// 关闭Imgui以及销毁VAO_TRI和VBO_TRI的缓存
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	//关闭窗口。
	glfwTerminate();
	return 0;
}


//根据按键，让摄像机进行移动
void hw7_processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBack(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		cursorInCenter = !cursorInCenter;
		if (cursorInCenter) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}


//改变视口大小。
void hw7_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	curWidth = width;
	curHeight = height;
	aspectRatio = float(curWidth) / float(curHeight);
	//	firstMouse = true;
	glViewport(0, 0, curWidth, curHeight);
}


// 计算x, y 偏移量，并让摄像机根据偏移量进行旋转。
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!cursorInCenter)
		return;
	if (firstMouse)
	{
		lastX = curWidth / 2.0f;
		lastY = curHeight / 2.0f;
		xpos = lastX;
		ypos = lastY;
		//		std::cout << "move:  " << lastX << " " << lastY << std::endl;
		firstMouse = false;
	}
	//	float xoffset = xpos - lastX; !!!!
	float xoffset = lastX - xpos;  //因为向左滑动摄像机，即做逆时针运行，yaw值应该增加。
	float yoffset = lastY - ypos;  //向上滑动摄像机，即做逆时针运动，pitch值应该增加。
	camera.ProcessMouseMovement(xoffset, yoffset);
	if (cursorInCenter) {
		glfwSetCursorPos(window, curWidth / 2.0f, curHeight / 2.0f);
		lastX = curWidth / 2.0f;
		lastY = curHeight / 2.0f;
	}
	else {
		lastX = xpos;
		lastY = ypos;
	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	std::cout << "scroll: " << xoffset << " " << yoffset << std::endl;
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(const char *path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, components;
	unsigned char *data = stbi_load(path, &width, &height, &components, 0);
	if (data) 
	{
		GLenum format;
		if (components == 1)
			format = GL_RED;
		else if (components == 3)
			format = GL_RGB;
		else if (components == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load pitcher" << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}


void renderScene(const CubeShader &shader,  Shader& modelShader, Model& ourModel)
{
	glm::mat4 model;
	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//cubes
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wall_red_Texture);
	//左上
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5, 3, 3));
	model = glm::translate(model, glm::vec3(-10.0f, 1.f, 1.5f));
	shader.setMat4("model", model);
	renderCube();
	//左下
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5, 3, 3));
	model = glm::translate(model, glm::vec3(-10.0f, 1.0f, -1.5f));
	shader.setMat4("model", model);
	renderCube();


	//右上
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5, 3, 3));
	model = glm::translate(model, glm::vec3(10.0f, 1.f, 1.5f));
	shader.setMat4("model", model);
	renderCube();
	//右下
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5, 3, 3));
	model = glm::translate(model, glm::vec3(10.0f, 1.0f, -1.5f));
	shader.setMat4("model", model);
	renderCube();


	//cubes
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wall_grey_Texture);
	//上左
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(2.0, 3, 0.5));
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, -15.0f));
	shader.setMat4("model", model);
	renderCube();

	//上右
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(2.0, 3, 0.5));
	model = glm::translate(model, glm::vec3(1.5f, 1.0f, -15.0f));
	shader.setMat4("model", model);
	renderCube();

	//下左
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(2.0, 3, 0.5));
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 15.0f));
	shader.setMat4("model", model);
	renderCube();

	//下右
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(2.0, 3, 0.5));
	model = glm::translate(model, glm::vec3(1.5f, 1.0f, 15.0f));
	shader.setMat4("model", model);
	renderCube();

	// render the loaded model
	modelShader.use();
	glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	modelShader.setMat4("projection", projection);
	modelShader.setMat4("view", view);
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(15.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, 270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
//	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
	modelShader.setMat4("model", model);
	ourModel.Draw(modelShader);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left 		
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void initPlaneVAO() {
	float planeVertices[] = {
		// positions            // normals         // texcoords
		25.0f, 0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, 0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, 0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		25.0f, 0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, 0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		25.0f, 0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};
	// plane VAO
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
}


void getMapVBOAndTexture(unsigned int& depthMapFBO, unsigned int& depthMap, const unsigned int SHADOW_WIDTH, const unsigned int SHADOW_HEIGHT) {
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}