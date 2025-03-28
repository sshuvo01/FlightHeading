#include "Core.h"
#include "Helper.h"
#include <memory>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const uint SCR_WIDTH = 600;
const uint SCR_HEIGHT = 600;
float sliderValue = 0.0f;  // Default slider value

void RenderUI(GLFWwindow* window) 
{
    ImGui::Begin("Control Panel");

    ImGui::Text("Heading (degree):");
    ImGui::SliderFloat("##Heading", &sliderValue, 0.0f, 359.0f, "%.1f");  
    ImGui::Spacing();

    if (ImGui::Button("Exit", ImVec2(100, 30))) 
    {  // Exit button
        glfwSetWindowShouldClose(window, true); 
    }

    ImGui::End(); 
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif



    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    
    

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& IO = ImGui::GetIO(); (void)IO;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    // monke
    std::shared_ptr<VertexArray> VAO = std::make_shared<VertexArray>();
    VAO->Bind();
    
    float ScreenRect[] =
    {
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    const int ScreenRectCount = sizeof(ScreenRect) / sizeof(float);

    std::shared_ptr<VertexBuffer> VB = std::make_shared<VertexBuffer>(ScreenRect, sizeof(float) * ScreenRectCount);
    std::shared_ptr<VertexBufferLayout> VBL = std::make_shared<VertexBufferLayout>();
    VBL->Push(2);//position
    VBL->Push(2);//tex coord

    VAO->AddBuffer(*VB.get(), *VBL.get());
    //VB->Unbind();
    std::shared_ptr<Shader> DrawRectShader = std::make_shared<Shader>("res/shaders/DrawRect.vert", "res/shaders/DrawRect.Frag");
    std::shared_ptr<Texture> CompassBackground = std::make_shared<Texture>("res/textures/CompassBackground.png");
    std::shared_ptr<Texture> CompassForeground = std::make_shared<Texture>("res/textures/CompassForeground.png");
    // 
    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT));

    DrawRectShader->Bind();
    VAO->Bind();

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // gui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        CompassBackground->Bind(0);
        DrawRectShader->SetUniform1i("rectTexture", 0);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));

        CompassForeground->Bind(0);
        DrawRectShader->SetUniform1i("rectTexture", 0);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));

        /*ImGui::Begin("Monke");
        ImGui::Text("mmm monke");
        ImGui::End();*/

        RenderUI(window);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}