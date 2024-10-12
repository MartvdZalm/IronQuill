#include <stdio.h>
#include "IronQuill.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
    IronQuill ironQuill;
    
    if (!ironQuill.CreateWindow())
    {
        return 1;
    }

    ironQuill.ProgramLoop();
    ironQuill.CleanUp();

    return 0;
}

IronQuill::IronQuill()
{
    options = new Options();
}
IronQuill::~IronQuill() {}

int IronQuill::CreateWindow()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        return false;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1280, 720, "Iron Quill", nullptr, nullptr);
    if (window == nullptr)
    {
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls

    // Setup ImGui style
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void IronQuill::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void IronQuill::ProgramLoop()
{
    // State
    bool mainWindow = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (options->ShowMainMenuBar) { ShowMainMenuBar(); }
        if (options->ShowDocuments && !documents.empty()) { ShowDocuments(&options->ShowDocuments); }

        if (mainWindow)
        {
            TextEditor();
        }
       
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void IronQuill::TextEditor()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ImGui::MenuItem("Main menu bar", NULL, &options->ShowMainMenuBar);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void IronQuill::ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New File", "CTRL+N")) {}
            if (ImGui::MenuItem("Open File", "CTRL+O"))
            {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".txt,.cpp");
            }
            if (ImGui::MenuItem("Open Folder", NULL)) {}
            if (ImGui::MenuItem("Save", NULL)) {}
            if (ImGui::MenuItem("Save As", NULL)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", NULL)) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            OpenFile(filePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void IronQuill::ShowDocuments(bool* p_open)
{
    // for (int i = 0; i < documents.Size; i++)
    // {
        Document* doc = &documents[0];
        static std::string textBuffer(doc->content, 20000);

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 viewportSize = viewport->Size;

        ImGui::PushID(doc);

        if (doc->fullscreen)
        {
            ImGui::SetNextWindowSize(viewportSize, ImGuiCond_Always);
            ImGui::SetNextWindowPos(viewport->Pos, ImGuiCond_Always);
        }

        ImGui::Begin("Document", p_open, doc->fullscreen ? ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove : 0);

        if (ImGui::Button(doc->fullscreen ? "Exit Fullscreen" : "Enter Fullscreen"))
        {
            doc->fullscreen = !doc->fullscreen;
        }

        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        windowSize.y -= ImGui::GetStyle().ItemSpacing.y;

        if (ImGui::InputTextMultiline("##editor", &textBuffer[0], textBuffer.size() + 1, windowSize, ImGuiInputTextFlags_AllowTabInput))
        {
            doc->content = textBuffer.data();
        }

        ImGui::End();
        ImGui::PopID();
    // }
}

void IronQuill::OpenFile(const std::string& filePath)
{
    Document document;

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size + 1];
    if (file.read(buffer, size))
    {
        buffer[size] = '\0';
        document.fileSize = size;
        document.content = buffer;
    }

    documents.push_back(document);
}