#include "Application.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

Application::Application()
{
    CreateWindow();
    InitUI();
    LoadRenderData();
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void Application::Run()
{
    ASSERT(Window);
    SetViewport();
    DrawRectShader->Bind();
    RectVAO->Bind();

    while (!glfwWindowShouldClose(Window))
    {
        ClearWindow();
        BeginUIFrame();
        Draw();
        RenderUI(Window);
        EndUIFrame();

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
}

void Application::CreateWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, "Heading", NULL, NULL);

    if (!Window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        ASSERTNOENTRY("Something is not right!");
        return;
    }
    glfwMakeContextCurrent(Window);

    glfwSetWindowUserPointer(Window, this);
    glfwSetFramebufferSizeCallback(Window, [](GLFWwindow* InWindow, int Width, int Height) 
    {
        Application* App = static_cast<Application*>(glfwGetWindowUserPointer(InWindow));
        if (App) 
        {
            App->FrameBufferSizeCallback(InWindow, Width, Height);
        }
    });

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        ASSERTNOENTRY("Something is not right!");
        return;
    }
}

void Application::InitUI()
{
    ASSERT(Window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& IO = ImGui::GetIO(); (void)IO;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::BeginUIFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Application::EndUIFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::FrameBufferSizeCallback(GLFWwindow* Window, int Width, int Height)
{
    ViewportWidth = std::min(Width, Height);
    WindowWidth = Width;
    WindowHeight = Height;

    SetViewport();
}

void Application::RenderUI(GLFWwindow* Window)
{
    ImGui::Begin("Control Panel");

    ImGui::Text("Heading (degree):");
    ImGui::SliderFloat("##Heading", &CurrentHeading, MinHeading, MaxHeading, "%.1f");
    ImGui::Spacing();
    
    static const ImVec2 ExitButtonSize = ImVec2(100, 30);
    if (ImGui::Button("Exit", ExitButtonSize))
    {
        glfwSetWindowShouldClose(Window, true);
    }

    ImGui::End();
}

void Application::ClearWindow()
{
    GLCALL(glClearColor(ClearColor[0], ClearColor[1], ClearColor[2], ClearColor[3]));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Application::Draw()
{
    constexpr static const char* rectTexture = "rectTexture";
    constexpr static const char* modelMat = "modelMat";
    constexpr static const int NumVertices = 6;
    constexpr static const glm::mat4 IdentityMat = glm::mat4(1.0f);

    const float HeadingRadians = glm::radians(CurrentHeading);
    const glm::mat4 HeadingMat = glm::rotate(IdentityMat, HeadingRadians, glm::vec3(0.0f, 0.0f, 1.0f));

    CompassBackground->Bind(0);
    DrawRectShader->SetUniform1i(rectTexture, 0);
    DrawRectShader->SetUniformMatrix4f(modelMat, HeadingMat);
    GLCALL(glDrawArrays(GL_TRIANGLES, 0, NumVertices));

    CompassForeground->Bind(0);
    DrawRectShader->SetUniform1i(rectTexture, 0);
    DrawRectShader->SetUniformMatrix4f(modelMat, IdentityMat);
    GLCALL(glDrawArrays(GL_TRIANGLES, 0, NumVertices));
}

void Application::LoadRenderData()
{
    // todo: use index buffer
    float RectVertices[] =
    {
        // Position   // TexCoord
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    RectVAO = std::make_shared<VertexArray>();
    RectVAO->Bind();
    const int RectVertexCount = sizeof(RectVertices) / sizeof(float);
    RectVB = std::make_shared<VertexBuffer>(RectVertices, sizeof(float) * RectVertexCount);
    RectVBL = std::make_shared<VertexBufferLayout>();
    RectVBL->Push(2); // position
    RectVBL->Push(2); // tex coord
    RectVAO->AddBuffer(*RectVB.get(), *RectVBL.get());
    DrawRectShader = std::make_shared<Shader>("res/shaders/DrawRect.vert", "res/shaders/DrawRect.Frag");
    CompassBackground = std::make_shared<Texture>("res/textures/CompassBackground.png");
    CompassForeground = std::make_shared<Texture>("res/textures/CompassForeground.png");
}

void Application::SetViewport()
{
    GLCALL(glViewport(0, 0, ViewportWidth, ViewportWidth));
}
