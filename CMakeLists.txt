cmake_minimum_required(VERSION 3.10)
project(IronQuill)

set(CMAKE_CXX_STANDARD 17)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui)
set(IMGUI_SOURCES
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)


set(IMGUI_FILE_DIALOG_DIR ${CMAKE_SOURCE_DIR}/lib/ImGuiFileDialog)
set(IMGUI_FILE_DIALOG_SOURCES
	${IMGUI_FILE_DIALOG_DIR}/ImGuiFileDialog.cpp
)

add_executable(IronQuill src/IronQuill.cpp src/IronQuill.h ${IMGUI_SOURCES} ${IMGUI_FILE_DIALOG_SOURCES})

target_include_directories(IronQuill PRIVATE
	${IMGUI_DIR}
	${IMGUI_DIR}/backends

	${IMGUI_FILE_DIALOG_DIR}
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(IronQuill PRIVATE OpenGL::GL glfw)