cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

project ("VulkanRender")

add_executable (${PROJECT_NAME} ${SOURCES})

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(${PROJECT_NAME} 
	glfw 
	GLEW::GLEW 
	glm::glm
	Vulkan::Vulkan
	GPUOpen::VulkanMemoryAllocator
	imgui::imgui
	${Stb_INCLUDE_DIR}
	Freetype::Freetype)
