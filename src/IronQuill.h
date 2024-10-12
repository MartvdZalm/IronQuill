#ifndef IRON_QUILL
#define IRON_QUILL

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "ImGuiFileDialog.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

class Options
{
public:
    bool ShowMainMenuBar = true;
    bool ShowDocuments = true;
};

class Document
{
public:
	std::string filePath;
	std::size_t fileSize;
	char* content;
	bool fullscreen = false;
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
	void ShowDocuments(bool* p_open);
	void OpenFile(const std::string& filePath);
};

#endif