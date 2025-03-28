#pragma once
#include "Core.h"
#include "Helper.h"

class Application
{
public:
	Application();
	~Application();

	void Run();
private:
	uint WindowWidth = 600;
	uint WindowHeight = WindowWidth;
	uint ViewportWidth = WindowWidth; // Keeping the viewport square
	GLFWwindow* Window = nullptr;
	const float MinHeading = 0.f;
	const float MaxHeading = 359.f;
	float CurrentHeading = MinHeading;
	glm::vec4 ClearColor = glm::vec4(0.25f, 0.3f, 0.3f, 1.0f);
	std::shared_ptr<VertexArray> RectVAO;
	std::shared_ptr<VertexBuffer> RectVB;
	std::shared_ptr<IndexBuffer> RectIB;
	std::shared_ptr<VertexBufferLayout> RectVBL;
	std::shared_ptr<Shader> DrawRectShader;
	std::shared_ptr<Texture> CompassBackground;
	std::shared_ptr<Texture> CompassForeground;

	void CreateWindow();
	void InitUI();
	void BeginUIFrame();
	void EndUIFrame();
	void FrameBufferSizeCallback(GLFWwindow* Window, int Width, int Height);
	void RenderUI(GLFWwindow* Window);
	void ClearWindow();
	void Draw();
	void LoadRenderData();
	void SetViewport();
};