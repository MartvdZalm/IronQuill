#ifndef IRON_QUILL
#define IRON_QUILL

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "ImGuiFileDialog.h"

class Options
{
public:
    bool ShowMainMenuBar = true;
};

class Document
{

};

class IronQuill
{
	GLFWwindow* window;
	Options* options;
	ImVector<Document> documents;

public:
	IronQuill();
	~IronQuill();

	int CreateWindow();
	void CleanUp();
	void ProgramLoop();
	void TextEditor();
	void ShowMainMenuBar();
	void OpenFile(const std::string& filename);
};

#endif