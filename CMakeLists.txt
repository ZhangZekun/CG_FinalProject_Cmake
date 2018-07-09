cmake_minimum_required(VERSION 3.1.0)

project(CG_FinalProject)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories("${PROJECT_SOURCE_DIR}/include")
link_directories("${PROJECT_SOURCE_DIR}/lib")

set(SRCS 
	glad.c
	imgui.cpp
	Camera.cpp 
	hw4_std_image.cpp 
	task1.cpp 
	task2.cpp
	imgui_impl_glfw_gl3.cpp
	imgui_draw.cpp
	imgui_demo.cpp
	hw7_main.cpp
	loadModel.cpp
)

add_executable(CG_PRO ${SRCS})
target_link_libraries(CG_PRO glfw3.lib)
target_link_libraries(CG_PRO opengl32.lib)
target_link_libraries(CG_PRO assimpd.lib)